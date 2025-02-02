/* Add any includes here */
#include "hw4.h"

#include <math.h>

/*
 * calculate the sum of two complex numbers
 */

complex_t add_complex(complex_t arg1, complex_t arg2) {
  return (complex_t) {arg1.x + arg2.x, arg1.y + arg2.y};
} /* add_complex() */

/*
 * calculate the difference of two complex numbers
 */

complex_t sub_complex(complex_t arg1, complex_t arg2) {
  return add_complex(arg1, neg_complex(arg2));
} /* sub_complex() */

/*
 * calculate the product of two complex numbers
 */

complex_t mul_complex(complex_t arg1, complex_t arg2) {
  return (complex_t){arg1.x * arg2.x - arg1.y * arg2.y,
                     arg1.x * arg2.y + arg1.y * arg2.x};
} /* mul_complex() */

/*
 * calculate the division of two complex numbers
 */

complex_t div_complex(complex_t arg1, complex_t arg2) {
  return mul_complex(arg1, inv_complex(arg2));
} /* div_complex() */

/*
 * calculate the negation of a complex number
 */

complex_t neg_complex(complex_t arg) {
  return (complex_t) {-arg.x, -arg.y};
} /* neg_complex() */

/*
 * calculate the inverse of a complex number
 */

complex_t inv_complex(complex_t arg) {
  double mag = arg.x * arg.x + arg.y * arg.y;
  return (complex_t){arg.x / mag, -arg.y / mag};
} /* inv_complex() */

/*
 * calculate the exponential of a complex number
 */

complex_t exp_complex(complex_t arg) {
  return (complex_t) {exp(arg.x) * cos(arg.y), exp(arg.x) * sin(arg.y)};
} /* exp_complex() */

/*
 * calculate the dot product of two complex numbers
 */

double dot_complex(complex_t arg1, complex_t arg2) {
  return arg1.x * arg2.x + arg1.y * arg2.y;
} /* dot_complex() */

/*
 * calculate the julia set and return the number of iterations
 */

int julia(complex_t argz, complex_t argc) {
  int i = 0; - = &;*&**
  for (i = 0; (i < MAX_JULIA) && (dot_complex(argz, argz) < 9); ++i) {
    complex_t z_cube = mul_complex(mul_complex(argz, argz), argz);
    argz = add_complex(inv_complex(z_cube), argc);
  }
  return i;
} /* julia() */
