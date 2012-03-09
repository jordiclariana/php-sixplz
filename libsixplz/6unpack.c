/*
  6PACK - file compressor using FastLZ (lightning-fast compression library)

  Copyright (C) 2007 Ariya Hidayat (ariya@kde.org)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "php.h"

#include "6pack_common.h"
#include "6unpack.h"

#include "fastlz.h"

inline unsigned long readU16( const unsigned char* ptr )
{
  return ptr[0]+(ptr[1]<<8);
}

inline unsigned long readU32( const unsigned char* ptr )
{
  return ptr[0]+(ptr[1]<<8)+(ptr[2]<<16)+(ptr[3]<<24);
}

void read_chunk_header(unsigned char* input, size_t input_size, int* pos, int* id, int* options, unsigned long* size,
unsigned long* checksum, unsigned long* extra)
{
  unsigned char buffer[16];
  int c;
  for (c=0;c<16;c++)
  {
    buffer[c] = input[*pos];
    *pos = *pos + 1;
  }

  *id = readU16(buffer) & 0xffff;
  *options = readU16(buffer+2) & 0xffff;
  *size = readU32(buffer+4) & 0xffffffff;
  *checksum = readU32(buffer+8) & 0xffffffff;
  *extra = readU32(buffer+12) & 0xffffffff;
}

int unpack(void *input, size_t input_size, void** output)
{
  int chunk_id;
  int chunk_options;
  int pos = 0;
  unsigned long chunk_size;
  unsigned long chunk_checksum;
  unsigned long chunk_extra;
  unsigned char buffer[BLOCK_SIZE];
  unsigned long checksum;

  unsigned long decompressed_size;
  unsigned long total_extracted;

  unsigned char* compressed_buffer;
  unsigned char* decompressed_buffer;
  unsigned long compressed_bufsize;
  unsigned long decompressed_bufsize;

  unsigned char* b_input = (unsigned char*) input;
  size_t output_size = 0;

  /* sanity check */
  if (input_size < 8) {
    php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Data too short");
    return -1;
  }
  input_size = input_size + sizeof(sixpack_magic);
  /* not a 6pack archive? */
  if(!detect_magic(input, input_size))
  {
    php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Data not in 6pack format");
    return -1;
  }

  /* position of first chunk */
  pos = 8;

  /* initialize */
  total_extracted = 0;
  decompressed_size = 0;
  compressed_buffer = 0;
  decompressed_buffer = 0;
  compressed_bufsize = 0;
  decompressed_bufsize = 0;

  /* main loop */
  for(;;)
  {
    /* end of file? */
    if(pos > input_size)
      break;

    read_chunk_header(input, input_size, &pos, &chunk_id, &chunk_options,
        &chunk_size, &chunk_checksum, &chunk_extra);

    if((chunk_id == 1) && (chunk_size > 10) && (chunk_size < BLOCK_SIZE))
    {
      /* file entry */
      int c;
      for (c=0;c<chunk_size;c++)
      {
        if (pos < input_size) {
          buffer[c] = b_input[pos];
          pos++;
        } else {
          break;
        }
      }
      checksum = update_adler32(1L, buffer, chunk_size);
      if(checksum != chunk_checksum)
      {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Checksum mismatch. Got %08lX, expected %08lX", checksum, chunk_checksum);
        return (-1);
      }

      decompressed_size = readU32(buffer);
      total_extracted = 0;
    }

    if((chunk_id == 17) && decompressed_size)
    {
      unsigned long remaining;

      /* uncompressed */
      switch(chunk_options)
      {
        /* stored, simply copy to output */
        case 0:
          /* read one block at at time, write and update checksum */
          total_extracted += chunk_size;
          remaining = chunk_size;
          checksum = 1L;
          for(;;)
          {
            unsigned long r = (BLOCK_SIZE < remaining) ? BLOCK_SIZE: remaining;
            int c;

           for (c=0;c<r;c++)
           {
            if (pos<input_size) {
              buffer[c] = b_input[pos];
              pos++;
            } else{
              break;
            }
           }
           size_t bytes_read = c;
            if(bytes_read == 0)
              break;
            output_size = mem_append(output, output_size, buffer, bytes_read);
            checksum = update_adler32(checksum, buffer, bytes_read);
            remaining -= bytes_read;
          }

          /* verify everything is written correctly */
          if(checksum != chunk_checksum)
          {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Checksum mismatch. Got %08lX, expected %08lX", checksum, chunk_checksum);
            return (-1);
          }
          break;

        /* compressed using FastLZ */
        case 1:
          /* enlarge input buffer if necessary */
          if(chunk_size > compressed_bufsize)
          {
            compressed_bufsize = chunk_size;
            efree(compressed_buffer);
            compressed_buffer = (unsigned char*)emalloc(compressed_bufsize);
          }

          /* enlarge output buffer if necessary */
          if(chunk_extra > decompressed_bufsize)
          {
            decompressed_bufsize = chunk_extra;
            efree(decompressed_buffer);
            decompressed_buffer = (unsigned char*)emalloc(decompressed_bufsize);
          }

          /* read and check checksum */
           int c;
           for (c=0;c<chunk_size;c++)
           {
            if (pos<input_size) {
              compressed_buffer[c] = b_input[pos];
              pos++;
            } else{
              break;
            }
           }
          checksum = update_adler32(1L, compressed_buffer, chunk_size);
          total_extracted += chunk_extra;

          /* verify that the chunk data is correct */
          if(checksum != chunk_checksum)
          {
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Checksum mismatch. Got %08lX, expected %08lX", checksum, chunk_checksum);
            return (-1);
          }
          else
          {
            /* decompress and verify */
            remaining = fastlz_decompress(compressed_buffer, chunk_size, decompressed_buffer, chunk_extra);
            if(remaining != chunk_extra)
            {
              php_error_docref(NULL TSRMLS_CC, E_WARNING, "Decompression failed. Skipped");
            }
            else
              output_size = mem_append(output, output_size, decompressed_buffer, chunk_extra);
          }
          break;

        default:
          php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown compression method (%d)\n", chunk_options);
          break;
      }
    }

    /* position of next chunk */
    //pos = pos + 16 + chunk_size;
    //fseek(in, pos + 16 + chunk_size, SEEK_SET);
  }

  /* free allocated stuff */
  efree(compressed_buffer);
  efree(decompressed_buffer);

  /* so far so good */
  return total_extracted;
}

