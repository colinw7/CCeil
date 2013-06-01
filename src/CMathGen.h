#ifndef CMathGen_H
#define CMathGen_H

#include <COSNaN.h>
#include <cerrno>
#include <cmath>
#include <iostream>

namespace CMathGen {
  inline void math_throw(const char *msg) {
    std::cerr << msg << std::endl;
  }

  inline long sign(long x) {
    if      (x > 0) return 1;
    else if (x < 0) return -1;
    else            return 0;
  }

  inline double sqrt(double real) {
     double result;

    if (COSNaN::is_nan(real)) {
      COSNaN::set_nan(&result);
      return real;
    }

    if (real < 0.0)
      math_throw("Negative Value for Sqrt");

    return ::sqrt(real);
  }

  inline bool is_integer(double r) {
    return (long(r) == r);
  }

  inline double pow(double real1, double real2) {
    double real;

    if (COSNaN::is_nan(real1) || COSNaN::is_nan(real2)) {
      COSNaN::set_nan(&real);
      return real;
    }

    if (real1 < 0.0 && ! is_integer(real2))
      math_throw("Non Integer Power of Negative");

    if (real1 == 0.0 && real2 < 0.0)
      math_throw("Zero to Negative Power is Undefined");

    errno = 0;

    if (real2 < 0.0)
      real = 1.0/::pow(real1, -real2);
    else
      real = ::pow(real1, real2);

    if (errno != 0)
      math_throw("Power Failed");

    return real;
  }

  inline double pow(long integer1, long integer2) {
    if (integer1 == 0 && integer2 < 0)
      math_throw("Zero to Negative Power is Undefined");

    errno = 0;

    double real;

    if (integer2 < 0.0)
      real = 1.0/::pow((double) integer1, (double) -integer2);
    else
      real = ::pow((double) integer1, (double) integer2);

    if (errno != 0)
      math_throw("Power Failed");

    return real;
  }

  inline double acos(double x) {
    if      (x >=  1.0) return 0.0;
    else if (x <= -1.0) return M_PI;
    else                return ::acos(x);
  }

  inline double asin(double x) {
    if      (x >=  1.0) return  M_PI/2.0;
    else if (x <= -1.0) return -M_PI/2.0;
    else                return ::asin(x);
  }

  inline double log(double real) {
    double result;

    if (COSNaN::is_nan(real)) {
      COSNaN::set_nan(&result);
      return real;
    }

    if (real <= 0.0)
      math_throw("Negative or Zero Value for Log");

    result = ::log(real);

    return result;
  }

  inline double log10(double real) {
    double result;

    if (COSNaN::is_nan(real)) {
      COSNaN::set_nan(&result);
      return real;
    }

    if (real <= 0.0)
      math_throw("Negative or Zero Value for Log");

    result = ::log10(real);

    return result;
  }

  inline double modulus(double real1, double real2) {
    double result = 0;

    if (COSNaN::is_nan(real1) || COSNaN::is_nan(real2)) {
      COSNaN::set_nan(&result);
      return result;
    }

    if (real2 == 0.0)
      math_throw("Divide by Zero");

    int factor = (int) (real1/real2);

    result = real1 - (real2*factor);

    return result;
  }
}

#endif
