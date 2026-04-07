#pragma once
#include_next <fenv.h>
#ifndef FE_OVERFLOW
#define FE_OVERFLOW 0
#endif
#ifndef FE_DIVBYZERO
#define FE_DIVBYZERO 0
#endif
#ifndef FE_INVALID
#define FE_INVALID 0
#endif
#ifndef FE_INEXACT
#define FE_INEXACT 0
#endif
#ifdef __cplusplus
extern "C" {
#endif
static inline int feenableexcept(int e) { (void)e; return -1; }
static inline int fedisableexcept(int e) { (void)e; return -1; }
#ifdef __cplusplus
}
#endif
