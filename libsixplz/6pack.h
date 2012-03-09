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

#ifndef SIXPACK_H_
#define SIXPACK_H_
#undef PATH_SEPARATOR

#if defined(MSDOS) || defined(__MSDOS__) || defined(MSDOS)
#define PATH_SEPARATOR '\\'
#endif

#if defined(WIN32) || defined(__NT__) || defined(_WIN32) || defined(__WIN32__)
#define PATH_SEPARATOR '\\'
#if defined(__BORLANDC__) || defined(_MSC_VER)
#define inline __inline
#endif
#endif

#ifndef PATH_SEPARATOR
#define PATH_SEPARATOR '/'
#endif

/* prototypes */
size_t write_magic(void **input, size_t input_size);
size_t write_chunk_header(void **input, size_t input_size, int id, int options, unsigned long size,
  unsigned long checksum, unsigned long extra);
unsigned long block_compress(const unsigned char* input, unsigned long length, unsigned char* output);
unsigned long pack_compressed(void* input, size_t input_size, int level, void** output);

//unsigned long pack_compressed(void* input, size_t input_size, int level, FILE* f);

#endif /* SIXPACK_H_ */