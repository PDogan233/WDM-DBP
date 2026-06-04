#ifndef MISC_H
#define MISC_H

#include <complex>
#include <string>
#include <limits>
#include "tool.h"

namespace std
{

//! Output stream operator for complex numbers
template <class T>
std::ostream& operator<<(std::ostream &os, const std::complex<T> &x)
{
  os << x.real();
  ios::fmtflags saved_format = os.setf(ios::showpos);
  os << x.imag();
  os.setf(saved_format, ios::showpos);
  return os << 'i';
}

//! Input stream operator for complex numbers
template <class T>
std::istream& operator>>(std::istream &is, std::complex<T> &x)
{
  T re, im;
  char c;
  is >> c;
  if (c == '(') {
    is >> re >> c;
    if (c == ',') {
      is >> im >> c;
      if (c == ')') {
        x = complex<T>(re, im);
      }
      else {
        is.setstate(ios_base::failbit);
      }
    }
    else if (c == ')') {
      x = complex<T>(re, T(0));
    }
    else {
      is.setstate(ios_base::failbit);
    }
  }
  else {
    is.putback(c);
    is >> re;
    if (!is.eof() && ((c = static_cast<char>(is.peek())) == '+' || c == '-')) {
      is >> im >> c;
      if (c == 'i') {
        x = complex<T>(re, im);
      }
      else {
        is.setstate(ios_base::failbit);
      }
    }
    else {
      x = complex<T>(re, T(0));
    }
  }
  return is;
}

} // namespace std


//! ZK namespace
namespace ZK
{

//! Constant Pi
const double pi = 3.14159265358979323846;

//! Constant 2*Pi
const double m_2pi = 2 * pi;

//! Constant eps
const double eps = std::numeric_limits<double>::epsilon();

//! \addtogroup miscfunc
//!@{

//! Return true if x is an integer
inline bool is_int(double x)
{
  double dummy;
  return (modf(x, &dummy) == 0.0);
}

//! Return true if x is an even integer
inline bool is_even(int x) { return ((x&1) == 0); }

//! Returns IT++ library version number, e.g. "3.7.1".
ZKIC_DLL_EXPORT std::string itpp_version();

//! Returns true if machine endianness is BIG_ENDIAN
ZKIC_DLL_EXPORT bool is_bigendian();

//! This function is deprecated. Please use is_bigendian() instead.
inline bool check_big_endianness() { return is_bigendian(); }

//!@}

} //namespace ZK


#endif // #ifndef MISC_H
