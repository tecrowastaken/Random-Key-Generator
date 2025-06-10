#ifndef __MT19937_H__
#define __MT19937_H__

// # MT19937

// Copyright (c) 2003-2005, Jean-Sebastien Roy (js@jeannot.org)

// The rk_random and rk_seed functions algorithms and the original design of
// the Mersenne Twister RNG:

//   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
//   All rights reserved.

//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:

//   1. Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.

//   2. Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.

//   3. The names of its contributors may not be used to endorse or promote
//   products derived from this software without specific prior written
//   permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER
// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Original algorithm for the implementation of rk_interval function from
// Richard J. Wagner's implementation of the Mersenne Twister RNG, optimised by
// Magnus Jonsson.

// Constants used in the rk_double implementation by Isaku Wada.

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once
#include <math.h>
#include <stdint.h>

// #if defined(_WIN32) && !defined (__MINGW32__)
// #define inline __forceinline
// #endif

#ifdef __cpluplus
extern "C" {
#endif

#define RK_STATE_LEN 624

#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL
#define UPPER_MASK 0x80000000UL
#define LOWER_MASK 0x7fffffffUL

typedef struct s_mt19937_state {
  uint32_t key[RK_STATE_LEN];
  int pos;
} mt19937_state;

extern void mt19937_seed(mt19937_state *state, uint32_t seed);

extern void mt19937_gen(mt19937_state *state);

/* Slightly optimized reference implementation of the Mersenne Twister */
static inline uint32_t mt19937_next(mt19937_state *state) {
  uint32_t y;

  if (state->pos == RK_STATE_LEN) {
    // Move to function to help inlining
    mt19937_gen(state);
  }
  y = state->key[state->pos++];

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);

  return y;
}

extern void mt19937_init_by_array(mt19937_state *state, uint32_t *init_key,
                                  int key_length);

static inline uint64_t mt19937_next64(mt19937_state *state) {
  return (uint64_t)mt19937_next(state) << 32 | mt19937_next(state);
}

static inline uint32_t mt19937_next32(mt19937_state *state) {
  return mt19937_next(state);
}

static inline double mt19937_next_double(mt19937_state *state) {
  int32_t a = mt19937_next(state) >> 5, b = mt19937_next(state) >> 6;
  return (a * 67108864.0 + b) / 9007199254740992.0;
}

// void mt19937_jump(mt19937_state *state);

#ifdef __cpluplus
};
#endif

#endif // __MT19937_H__