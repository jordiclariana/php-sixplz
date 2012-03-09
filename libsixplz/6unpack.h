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

#ifndef SIXUNPACK_H_
#define SIXUNPACK_H_

#if defined(WIN32) || defined(__NT__) || defined(_WIN32) || defined(__WIN32__)
#if defined(__BORLANDC__) || defined(_MSC_VER)
#define inline __inline
#endif
#endif

/* prototypes */
inline unsigned long readU16(const unsigned char* ptr);
inline unsigned long readU32(const unsigned char* ptr);

//void read_chunk_header(FILE* f, int* id, int* options, unsigned long* size,
//unsigned long* checksum, unsigned long* extra);
void read_chunk_header(unsigned char* input, size_t input_size, int* pos, int* id, int* options, unsigned long* size,
unsigned long* checksum, unsigned long* extra);

//int unpack(void *input, size_t input_size, FILE* output);
int unpack(void *input, size_t input_size, void** output);


#endif /* SIXUNPACK_H_ */