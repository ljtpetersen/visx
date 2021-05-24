/* include/jp/def.h
 *
 * This file is part of the VisX project (https://github.com/ljtpetersen/visx).
 * Copyright (c) 2021 James Petersen
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef JP_DEF_H
#define JP_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <float.h>

// Integer definitions
typedef unsigned int uint;
typedef unsigned char uchar;
typedef int8_t int8, byte, i8;
typedef int16_t int16, i16;
typedef int32_t int32, i32;
typedef int64_t int64, i64;
typedef intmax_t intmax, imax;
typedef intptr_t intptr, iptr;
typedef uint8_t uint8, ubyte, u8;
typedef uint16_t uint16, u16;
typedef uint32_t uint32, u32;
typedef uint64_t uint64, u64;
typedef uintmax_t uintmax, umax;
typedef uintptr_t uintptr, uptr;

#define JP_STRMACRO1(X) #X
#define JP_STRMACRO(X) JP_STRMACRO1(X)

#ifdef _WIN32
#define PS_S "\\"
#define PS_C '\\'
#else
#define PS_S "/"
#define PS_C '/'
#endif

#define BOOL_STR(X) ((X) ? "true" : "false")

#define FPTR(RETURN_TYPE, NAME, ...) RETURN_TYPR (*NAME)(__VA_ARGS__)
#define FPTR_VARR(RETURN_TYPR, NAME, ...) RETURN_TYPR (*NAME[])(__VA_ARGS__)
#define FPTR_ARR(RETURN_TYPE, NAME, COUNT, ...) RETURN_TYPE (*NAME[COUNT])(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif

