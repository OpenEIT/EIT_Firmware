/* ===-- __divmoddi4.c - configuration header for compiler-rt  -----------------===
 *
 *                     The LLVM Compiler Infrastructure
 *
 * This file is dual licensed under the MIT and the University of Illinois Open
 * Source Licenses. See LICENSE.TXT for details.
 *
 * ===----------------------------------------------------------------------===
 *
 * This file is not part of the interface of this library.
 *
 * This file defines various standard types, most importantly a number of unions
 * used to access parts of larger types.
 *
 * ===----------------------------------------------------------------------===
 */
#include <stdio.h>

int64_t __divmoddi4(int64_t num, int64_t den, int64_t *rem_p)
{
  int minus = 0;
  int64_t v;

  if ( num < 0 ) {
    num = -num;
    minus = 1;
  }
  if ( den < 0 ) {
    den = -den;
    minus ^= 1;
  }

  v = __udivmoddi4(num, den, (uint64_t *)rem_p);
  if ( minus ) {
    v = -v;
    if ( rem_p )
      *rem_p = -(*rem_p);
  }

  return v;
}
