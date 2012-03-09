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
#include "6pack.h"

#include "fastlz.h"

size_t write_magic(void **input, size_t input_size)
{
  return(mem_append(input, input_size, sixpack_magic, sizeof(sixpack_magic)));
}

size_t write_chunk_header(void **input, size_t input_size, int id, int options, unsigned long size,
  unsigned long checksum, unsigned long extra)
{
  unsigned char buffer[16];

  buffer[0] = id & 255;
  buffer[1] = id >> 8;
  buffer[2] = options & 255;
  buffer[3] = options >> 8;
  buffer[4] = size & 255;
  buffer[5] = (size >> 8) & 255;
  buffer[6] = (size >> 16) & 255;
  buffer[7] = (size >> 24) & 255;
  buffer[8] = checksum & 255;
  buffer[9] = (checksum >> 8) & 255;
  buffer[10] = (checksum >> 16) & 255;
  buffer[11] = (checksum >> 24) & 255;
  buffer[12] = extra & 255;
  buffer[13] = (extra >> 8) & 255;
  buffer[14] = (extra >> 16) & 255;
  buffer[15] = (extra >> 24) & 255;

  return(mem_append(input, input_size, buffer, 16));
}

unsigned long pack_compressed(void* input, size_t input_size, int level, void** output)
{
  unsigned long fsize;
  unsigned long checksum;
  const char* shown_name = "6P";
  unsigned char buffer[BLOCK_SIZE];
  unsigned char result[BLOCK_SIZE*2]; /* FIXME twice is too large */
  unsigned long total_read = 0;
  unsigned long total_compressed;
  int chunk_size;
  size_t output_size = 0;
  size_t input_pos = 0;

  fsize = (unsigned long) input_size;

  if (input_size < 8) {
    php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Data too short");
    return (-1);
  }

  /* already a 6pack archive? */
  if(detect_magic(input, input_size))
  {
    php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Data already compressed");
    return (-1);
  }

  output_size = write_magic(output, output_size);
  /* chunk for File Entry */
  buffer[0] = fsize & 255;
  buffer[1] = (fsize >> 8) & 255;
  buffer[2] = (fsize >> 16) & 255;
  buffer[3] = (fsize >> 24) & 255;
#if 0
  buffer[4] = (fsize >> 32) & 255;
  buffer[5] = (fsize >> 40) & 255;
  buffer[6] = (fsize >> 48) & 255;
  buffer[7] = (fsize >> 56) & 255;
#else
  /* because fsize is only 32-bit */
  buffer[4] = 0;
  buffer[5] = 0;
  buffer[6] = 0;
  buffer[7] = 0;
#endif
  buffer[8] = (strlen(shown_name)+1) & 255;
  buffer[9] = (strlen(shown_name)+1) >> 8;
  checksum = 1L;
  checksum = update_adler32(checksum, buffer, 10);
  checksum = update_adler32(checksum, shown_name, strlen(shown_name)+1);

  output_size = write_chunk_header(output, output_size, 1, 0, 10+strlen(shown_name)+1, checksum, 0);
  output_size = mem_append(output, output_size, buffer, 10);
  output_size = mem_append(output, output_size, (void *) shown_name, strlen(shown_name)+1);
  total_compressed = 16 + 10 + strlen(shown_name)+1;

  for(;;)
  {
    int compress_method = 1;
    size_t bytes_read = 0;

    if (total_read == input_size) {
      break;
    } else if (total_read < input_size) {
      if ((input_size - total_read) >= BLOCK_SIZE) {
        if (memcpy(buffer, &input[total_read], BLOCK_SIZE) == NULL) {
          php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error: Out of memory");
          return (-1);
        }
        bytes_read = BLOCK_SIZE;
      } else {
        if (memcpy(buffer, &input[total_read], (input_size - total_read)) == NULL) {
          php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error: Out of memory");
          return (-1);          
        }
        bytes_read = (input_size - total_read);
      }
    }
    if (bytes_read == 0) {
      break;
    }
    total_read += bytes_read;

    /* too small, don't bother to compress */
    if(bytes_read < 32)
      compress_method = 0;

    /* write to output */
    switch(compress_method)
    {
      /* FastLZ */
      case 1:
        chunk_size = fastlz_compress_level(level, buffer, bytes_read, result);
        checksum = update_adler32(1L, result, chunk_size);
        output_size = write_chunk_header(output, output_size, 17, 1, chunk_size, checksum, bytes_read);
        output_size = mem_append(output, output_size, result, chunk_size);
        total_compressed += 16;
        total_compressed += chunk_size;
        break;

      /* uncompressed, also fallback method */
      case 0:
      default:
        checksum = 1L;
        checksum = update_adler32(checksum, buffer, bytes_read);
        output_size = write_chunk_header(output, output_size, 17, 0, bytes_read, checksum, bytes_read);
        output_size = mem_append(output, output_size, buffer, bytes_read);
        total_compressed += 16;
        total_compressed += bytes_read;
        break;
    }
  }

  if(total_read != fsize)
  {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Read data and real data size mismatch");
    return (-1);
  }

  if (total_compressed != output_size - 8) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Compressed data length different form output size");
    return (-1);
  }

  return total_compressed;
}

