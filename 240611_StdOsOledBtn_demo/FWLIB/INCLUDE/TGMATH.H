/* tgmath.h: ISO/IEC 9899:1999 (C99), section 7.22 */

/* Copyright (C) ARM Ltd., 2006
 * All rights reserved
 * RCS $Revision$
 * Checkin $Date$
 * Revising $Author: statham $
 */

#ifndef __tgmath_h
#define __tgmath_h
#define __ARMCLIB_VERSION 5060044

#include <math.h>

#define _ARM_GENERIC1(F,x) __generic(x,,, F, F##f, F##l,,,)(x)
#define _ARM_GENERIC2(F,x,y) __generic(x,y,, F, F##f, F##l,,,)(x, y)
#define _ARM_GENERIC1I(F,x,i) __generic(x,,, F, F##f, F##l,,,)(x, i)
#define _ARM_GENERIC3(F,x,y,z) __generic(x,y,z, F, F##f, F##l,,,)(x, y, z)
#define _ARM_GENERIC2I(F,x,y,i) __generic(x,y,, F, F##f, F##l,,,)(x, y, i)

/* only do complex variants for --c99 */
#if  __STDC_VERSION__ >= 199901L
#  include <complex.h>
#  define _ARM_GENERIC1C(F,x) __generic(x,,, F, F##f, F##l, c##F, c##F##f, c##F##l)(x)
#  define _ARM_GENERIC2C(F,x,y) __generic(x,y,, F, F##f, F##l, c##F, c##F##f, c##F##l)(x,y)
#  define _ARM_GENERIC1C_ONLY(F,x) __generic(x,,,,,,F, F##f, F##l)(x)
#else
#  define _ARM_GENERIC1C(F,x) _ARM_GENERIC1(F,x)
#  define _ARM_GENERIC2C(F,x,y) _ARM_GENERIC2(F,x,y)
#endif /* __STDC_VERSION__ */

/* 7.22#4: Functions with both real and complex variants */
#define acos(x) _ARM_GENERIC1C(acos, x)
#define asin(x) _ARM_GENERIC1C(asin, x)
#define atan(x) _ARM_GENERIC1C(atan, x)
#define acosh(x) _ARM_GENERIC1C(acosh, x)
#define asinh(x) _ARM_GENERIC1C(asinh, x)
#define atanh(x) _ARM_GENERIC1C(atanh, x)
#define cos(x) _ARM_GENERIC1C(cos, x)
#define sin(x) _ARM_GENERIC1C(sin, x)
#define tan(x) _ARM_GENERIC1C(tan, x)
#define cosh(x) _ARM_GENERIC1C(cosh, x)
#define sinh(x) _ARM_GENERIC1C(sinh, x)
#define tanh(x) _ARM_GENERIC1C(tanh, x)
#define exp(x) _ARM_GENERIC1C(exp, x)
#define log(x) _ARM_GENERIC1C(log, x)
#define pow(x, y) _ARM_GENERIC2C(pow, x, y)
#define sqrt(x) _ARM_GENERIC1C(sqrt, x)
/* special case - complex version of fabs is cabs (rather than cfabs) */
#if  __STDC_VERSION__ >= 199901L
#  define fabs(x) __generic(x,,, fabs, fabsf, fabsl, cabs, cabsf, cabsl)(x)
#else
#  define fabs(x) _ARM_GENERIC1(fabs, x)
#endif /* __STDC_VERSION__ */

/* 7.22#5: Functions without complex variants */
#define atan2(x, y) _ARM_GENERIC2(atan2, x, y)
#define log10(x) _ARM_GENERIC1(log10, x)
#define cbrt(x) _ARM_GENERIC1(cbrt, x)
#define ceil(x) _ARM_GENERIC1(ceil, x)
#define copysign(x, y) _ARM_GENERIC2(copysign, x, y)
#define erf(x) _ARM_GENERIC1(erf, x)
#define erfc(x) _ARM_GENERIC1(erfc, x)
#define exp2(x) _ARM_GENERIC1(exp2, x)
#define expm1(x) _ARM_GENERIC1(expm1, x)
#define fdim(x, y) _ARM_GENERIC2(fdim, x, y)
#define floor(x) _ARM_GENERIC1(floor, x)
#define fma(x, y, z) _ARM_GENERIC3(fma, x, y, z)
#define fmax(x, y) _ARM_GENERIC2(fmax, x, y)
#define fmin(x, y) _ARM_GENERIC2(fmin, x, y)
#define fmod(x, y) _ARM_GENERIC2(fmod, x, y)
#define frexp(x, i) _ARM_GENERIC1I(frexp, x, i)
#define hypot(x, y) _ARM_GENERIC2(hypot, x, y)
#define ilogb(x) _ARM_GENERIC1(ilogb, x)
#define ldexp(x, i) _ARM_GENERIC1I(ldexp, x, i)
#define lgamma(x) _ARM_GENERIC1(lgamma, x)
#define llrint(x) _ARM_GENERIC1(llrint, x)
#define llround(x) _ARM_GENERIC1(llround, x)
#define log10(x) _ARM_GENERIC1(log10, x)
#define log1p(x) _ARM_GENERIC1(log1p, x)
#define log2(x) _ARM_GENERIC1(log2, x)
#define logb(x) _ARM_GENERIC1(logb, x)
#define lrint(x) _ARM_GENERIC1(lrint, x)
#define lround(x) _ARM_GENERIC1(lround, x)
#define nearbyint(x) _ARM_GENERIC1(nearbyint, x)
#define nextafter(x, y) _ARM_GENERIC2(nextafter, x, y)
#define nexttoward(x, y) _ARM_GENERIC1I(nexttoward, x, y)
#define remainder(x, y) _ARM_GENERIC2(remainder, x, y)
#define remquo(x, y, p) _ARM_GENERIC2I(remquo, x, y, p)
#define rint(x) _ARM_GENERIC1(rint, x)
#define round(x) _ARM_GENERIC1(round, x)
#define scalbn(x, i) _ARM_GENERIC1I(scalbn, x, i)
#define scalbln(x, i) _ARM_GENERIC1I(scalbln, x, i)
#define tgamma(x) _ARM_GENERIC1(tgamma, x)
#define trunc(x) _ARM_GENERIC1(trunc, x)

/* 7.22#6: Functions without real variants */
#if  __STDC_VERSION__ >= 199901L
#  define carg(x) _ARM_GENERIC1C_ONLY(carg, x)
#  define cimag(x) _ARM_GENERIC1C_ONLY(cimag, x)
#  define conj(x) _ARM_GENERIC1C_ONLY(conj, x)
#  define cproj(x) _ARM_GENERIC1C_ONLY(cproj, x)
#  define creal(x) _ARM_GENERIC1C_ONLY(creal, x)
#endif /* __STDC_VERSION__ */

#endif /* included */

/* end of tgmath.h */

