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


/* return non-zero if magic sequence is detected */
/* warning: reset the read pointer to the beginning of the file */
int detect_magic(void* input, size_t input_size)
{
  int c;
  unsigned char* b_input = (unsigned char*) input;

  for(c=0; c<8; c++) {
    if (b_input[c] != sixpack_magic[c])
    {
      return 0;
    }
  }

  return -1;
}

inline unsigned long update_adler32(unsigned long checksum, const void *buf, int len)
{
  const unsigned char* ptr = (const unsigned char*)buf;
  unsigned long s1 = checksum & 0xffff;
  unsigned long s2 = (checksum >> 16) & 0xffff;

  while(len>0)
  {
    unsigned k = len < 5552 ? len : 5552;
    len -= k;

    while(k >= 8)
    {
      s1 += *ptr++; s2 += s1;
      s1 += *ptr++; s2 += s1;
      s1 += *ptr++; s2 += s1;
      s1 += *ptr++; s2 += s1;
      s1 += *ptr++; s2 += s1;
      s1 += *ptr++; s2 += s1;
      s1 += *ptr++; s2 += s1;
      s1 += *ptr++; s2 += s1;
      k -= 8;
    }

    while(k-- > 0)
    {
      s1 += *ptr++; s2 += s1;
    }
    s1 = s1 % ADLER32_BASE;
    s2 = s2 % ADLER32_BASE;
  }
  return (s2 << 16) + s1;
}

size_t mem_append(void** input, size_t input_size, void* append_data, size_t append_data_size)
{
  unsigned char* b_input;
//  unsigned char* c_input = (unsigned char *) *input;
  size_t total_size = append_data_size + input_size;
  
  b_input = (unsigned char *) erealloc(*input, total_size);

  if (b_input == NULL) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error: Out of memory");
    return (-1);
  }

  if (memcpy(&b_input[input_size], append_data, append_data_size) == NULL) {
    php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error: Out of memory");
    return (-1);
  }
  
//  *input_size = append_data_size + *input_size;
  *input = (void *) b_input;

  return (total_size);
}