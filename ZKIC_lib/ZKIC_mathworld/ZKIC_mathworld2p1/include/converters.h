#ifndef CONVERTERS_H
#define CONVERTERS_H

#include "help_functions.h"
#include "misc.h"
#include "tool.h"

namespace ZK
{

//! \addtogroup convertfunc
//@{

// ----------------------------------------------------------------------
// Converters for vectors
// ----------------------------------------------------------------------

/*!
  \relatesalso Vec
  \brief Converts a Vec<T> to bvec
*/
template <class T>
bvec to_bvec(const Vec<T> &v)
{
  bvec temp(v.length());
  for (int i = 0; i < v.length(); ++i) {
    temp(i) = static_cast<bin>(v(i));
  }
  return temp;
}

/*!
  \relatesalso Vec
  \brief Converts a Vec<T> to svec
*/
template <class T>
svec to_svec(const Vec<T> &v)
{
  svec temp(v.length());
  for (int i = 0; i < v.length(); ++i) {
    temp(i) = static_cast<short>(v(i));
  }
  return temp;
}

/*!
  \relatesalso Vec
  \brief Converts a Vec<T> to ivec
*/
template <class T>
ivec to_ivec(const Vec<T> &v)
{
  ivec temp(v.length());
  for (int i = 0; i < v.length(); ++i) {
    temp(i) = static_cast<int>(v(i));
  }
  return temp;
}

/*!
  \relatesalso Vec
  \brief Converts a Vec<T> to vec
*/
template <class T>
vec to_vec(const Vec<T> &v)
{
  vec temp(v.length());
  for (int i = 0; i < v.length(); ++i) {
    temp(i) = static_cast<double>(v(i));
  }
  return temp;
}

/*!
  \relatesalso Vec
  \brief Converts a Vec<T> to cvec
*/
template <class T>
cvec to_cvec(const Vec<T> &v)
{
  cvec temp(v.length());
  for (int i = 0; i < v.length(); ++i) {
    temp(i) = std::complex<double>(static_cast<double>(v(i)), 0.0);
  }
  return temp;
}

//! \cond
template<> inline
cvec to_cvec(const cvec& v)
{
  return v;
}
//! \endcond

/*!
  \relatesalso Vec
  \brief Converts real and imaginary Vec<T> to cvec
*/
template <class T>
cvec to_cvec(const Vec<T> &real, const Vec<T> &imag)
{
  ZKIC_Assert(real.length() == imag.length(),
            "to_cvec(): real and imaginary parts must have the same length");
  cvec temp(real.length());
  for (int i = 0; i < real.length(); ++i) {
    temp(i) = std::complex<double>(static_cast<double>(real(i)),
                                   static_cast<double>(imag(i)));
  }
  return temp;
}

/*!
  \relatesalso Vec
  \brief Converts an int to ivec
*/
ivec to_ivec(int s);

/*!
  \relatesalso Vec
  \brief Converts an double to vec
*/
vec to_vec(double s);

/*!
  \relatesalso Vec
  \brief Converts real and imaginary double to cvec
*/
cvec to_cvec(double real, double imag);

// ----------------------------------------------------------------------
// Converters for matrices
// ----------------------------------------------------------------------

/*!
  \relatesalso Mat
  \brief Converts a Mat<T> to bmat
*/
template <class T>
bmat to_bmat(const Mat<T> &m)
{
  bmat temp(m.rows(), m.cols());
  for (int i = 0; i < temp.rows(); ++i) {
    for (int j = 0; j < temp.cols(); ++j) {
      temp(i, j) = static_cast<bin>(m(i, j));
    }
  }
  return temp;
}

/*!
  \relatesalso Mat
  \brief Converts a Mat<T> to smat
*/
template <class T>
smat to_smat(const Mat<T> &m)
{
  smat temp(m.rows(), m.cols());
  for (int i = 0; i < temp.rows(); ++i) {
    for (int j = 0; j < temp.cols(); ++j) {
      temp(i, j) = static_cast<short>(m(i, j));
    }
  }
  return temp;
}

/*!
  \relatesalso Mat
  \brief Converts a Mat<T> to imat
*/
template <class T>
imat to_imat(const Mat<T> &m)
{
  imat temp(m.rows(), m.cols());
  for (int i = 0; i < temp.rows(); ++i) {
    for (int j = 0; j < temp.cols(); ++j) {
      temp(i, j) = static_cast<int>(m(i, j));
    }
  }
  return temp;
}

/*!
  \relatesalso Mat
  \brief Converts a Mat<T> to mat
*/
template <class T>
mat to_mat(const Mat<T> &m)
{
  mat temp(m.rows(), m.cols());
  for (int i = 0; i < temp.rows(); ++i) {
    for (int j = 0; j < temp.cols(); ++j) {
      temp(i, j) = static_cast<double>(m(i, j));
    }
  }
  return temp;
}

/*!
  \relatesalso Mat
  \brief Converts a Mat<T> to cmat
*/
template <class T>
cmat to_cmat(const Mat<T> &m)
{
  cmat temp(m.rows(), m.cols());
  for (int i = 0; i < temp.rows(); ++i) {
    for (int j = 0; j < temp.cols(); ++j) {
      temp(i, j) = std::complex<double>(static_cast<double>(m(i, j)), 0.0);
    }
  }
  return temp;
}

//! \cond
template<> inline
cmat to_cmat(const cmat& m)
{
  return m;
}
//! \endcond

/*!
  \relatesalso Mat
  \brief Converts real and imaginary Mat<T> to cmat
*/
template <class T>
cmat to_cmat(const Mat<T> &real, const Mat<T> &imag)
{
  ZKIC_Assert((real.rows() == imag.rows())
                  && (real.cols() == imag.cols()),
                  "to_cmat(): real and imag part sizes does not match");
  cmat temp(real.rows(), real.cols());
  for (int i = 0; i < temp.rows(); ++i) {
    for (int j = 0; j < temp.cols(); ++j) {
      temp(i, j) = std::complex<double>(static_cast<double>(real(i, j)),
                                        static_cast<double>(imag(i, j)));
    }
  }
  return temp;
}


/*!
  \brief Convert a decimal int \a index to bvec using \a length bits in the representation
*/
ZKIC_DLL_EXPORT bvec dec2bin(int length, int index);

/*!
  \brief Convert a decimal int \a index to bvec. Value returned in \a v.
*/
ZKIC_DLL_EXPORT void dec2bin(int index, bvec &v);

/*!
  \brief Convert a decimal int \a index to bvec with the first bit as MSB if \a msb_first == true
*/
ZKIC_DLL_EXPORT bvec dec2bin(int index, bool msb_first = true);

/*!
  \brief Convert a bvec to decimal int with the first bit as MSB if \a msb_first == true
*/
ZKIC_DLL_EXPORT int bin2dec(const bvec &inbvec, bool msb_first = true);

/*!
  \brief Convert ivec of octal form to bvec

  Converts from ivec containing {0,1,2,...,7} to bvec containing {0,1}.
  Removes zeros to the left if keepzeros = 0 (default).
  Example: oct2bin("3 5 5 1") returns {1 1 1 0 1 1 0 1 0 0 1}.
*/
ZKIC_DLL_EXPORT bvec oct2bin(const ivec &octalindex, short keepzeros = 0);

/*!
  \brief Convert bvec to octal ivec

  Converts from  bvec containing {0,1} to ivec containing {0,1,2,...,7}.
  Adds zeros to the left if inbits.length() is not a factor of 3.
  Example: bin2oct("1 1 1 0 1 1 0 1 0 0 1") returns {3 5 5 1}.
*/
ZKIC_DLL_EXPORT ivec bin2oct(const bvec &inbits);

//! Convert bvec to polar binary representation as ivec
ZKIC_DLL_EXPORT ivec bin2pol(const bvec &inbvec);

//! Convert binary polar ivec to bvec
ZKIC_DLL_EXPORT bvec pol2bin(const ivec &inpol);

//! Convert radians to degrees
inline double rad_to_deg(double x) { return (180.0 / ZK::pi * x); }
//! Convert degrees to radians
inline double deg_to_rad(double x) { return (ZK::pi / 180.0 * x); }

//! Round to nearest integer, return result in double
ZKIC_DLL_EXPORT double round(double x);
//! Round to nearest integer
ZKIC_DLL_EXPORT vec round(const vec &x);
//! Round to nearest integer
ZKIC_DLL_EXPORT mat round(const mat &x);
//! Round to nearest integer
ZKIC_DLL_EXPORT int round_i(double x);
//! Round to nearest integer and return ivec
ZKIC_DLL_EXPORT ivec round_i(const vec &x);
//! Round to nearest integer and return imat
ZKIC_DLL_EXPORT imat round_i(const mat &x);

//! Round to nearest upper integer
inline vec ceil(const vec &x) { return apply_function<double>(std::ceil, x); }
//! Round to nearest upper integer
inline mat ceil(const mat &x) { return apply_function<double>(std::ceil, x); }
//! The nearest larger integer
inline int ceil_i(double x) { return static_cast<int>(std::ceil(x)); }
//! Round to nearest upper integer
ZKIC_DLL_EXPORT ivec ceil_i(const vec &x);
//! Round to nearest upper integer
ZKIC_DLL_EXPORT imat ceil_i(const mat &x);

//! Round to nearest lower integer
inline vec floor(const vec &x) { return apply_function<double>(std::floor, x); }
//! Round to nearest lower integer
inline mat floor(const mat &x) { return apply_function<double>(std::floor, x); }
//! The nearest smaller integer
inline int floor_i(double x) { return static_cast<int>(std::floor(x)); }
//! Round to nearest lower integer
ZKIC_DLL_EXPORT ivec floor_i(const vec &x);
//! Round to nearest lower integer
ZKIC_DLL_EXPORT imat floor_i(const mat &x);


//! Round \a x to zero if \a abs(x) is smaller than \a threshold
inline double round_to_zero(double x, double threshold = 1e-14)
{
  return ((std::fabs(x) < threshold) ? 0.0 : x);
}

//! Round each part of \a x smaller than \a threshold to zero
inline std::complex<double> round_to_zero(const std::complex<double>& x,
    double threshold = 1e-14)
{
  return std::complex<double>(round_to_zero(x.real(), threshold),
                              round_to_zero(x.imag(), threshold));
}

//! Round each element to zero if element < threshold
inline vec round_to_zero(const vec &x, double threshold = 1e-14)
{
  return apply_function<double>(round_to_zero, x, threshold);
}

//! Round each element to zero if element < threshold
inline mat round_to_zero(const mat &x, double threshold = 1e-14)
{
  return apply_function<double>(round_to_zero, x, threshold);
}

//! Round each element to zero if element < threshold
ZKIC_DLL_EXPORT cvec round_to_zero(const cvec &x, double threshold = 1e-14);

//! Round each element to zero if element < threshold
ZKIC_DLL_EXPORT cmat round_to_zero(const cmat &x, double threshold = 1e-14);

//! Remove trailing digits, found after the decimal point, for numbers greater than threshold
inline double round_to_infty(const double in, const double threshold = 1e9)
{
  return (std::fabs(in)>threshold)?ZK::round(in):in;
}

//! Remove trailing digits, found after the decimal point, for complex numbers whose real and imaginary parts are greater than threshold
inline std::complex<double> round_to_infty(const std::complex<double> &in, const double threshold = 1e9)
{
  return std::complex<double>(round_to_infty(in.real(), threshold),
                              round_to_infty(in.imag(), threshold));
}

//! Remove trailing digits, found after the decimal point, for vectors greater than threshold
inline vec round_to_infty(const vec &in, const double threshold = 1e9)
{
  return apply_function<double>(round_to_infty, in, threshold);
}

//! Remove trailing digits, found after the decimal point, for matrices greater than threshold
inline mat round_to_infty(const mat &in, const double threshold = 1e9)
{
  return apply_function<double>(round_to_infty, in, threshold);
}

//! Remove trailing digits, found after the decimal point, for complex vectors greater than threshold
ZKIC_DLL_EXPORT cvec round_to_infty(const cvec &in, const double threshold = 1e9);

//! Remove trailing digits, found after the decimal point, for complex matrices greater than threshold
ZKIC_DLL_EXPORT cmat round_to_infty(const cmat &in, const double threshold = 1e9);

//! Convert to Gray Code
inline int gray_code(int x) { return x ^(x >> 1); }


/*!
  \brief Convert anything to string

  \param i (Input) The value to be converted to a string
*/
template <typename T>
std::string to_str(const T &i);

/*!
  \brief Convert double to string

  \param[in]  i          The value to be converted to a string
  \param[in]  precision  The number of digits used to represent the
  fractional part
*/
ZKIC_DLL_EXPORT std::string to_str(const double &i, const int precision);

//@}

template <typename T>
std::string to_str(const T &i)
{
  std::ostringstream ss;
  ss.precision(8);
  ss.setf(std::ostringstream::scientific, std::ostringstream::floatfield);
  ss << i;
  return ss.str();
}

//! \cond

// ---------------------------------------------------------------------
// Instantiations
// ---------------------------------------------------------------------

ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec to_bvec(const svec &v);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bvec to_bvec(const ivec &v);

ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec to_svec(const bvec &v);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec to_svec(const ivec &v);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT svec to_svec(const vec &v);

ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec to_ivec(const bvec &v);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec to_ivec(const svec &v);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT ivec to_ivec(const vec &v);

ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec to_vec(const bvec &v);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec to_vec(const svec &v);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT vec to_vec(const ivec &v);

ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec to_cvec(const bvec &v);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec to_cvec(const svec &v);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec to_cvec(const ivec &v);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec to_cvec(const vec &v);

ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec to_cvec(const bvec &real, const bvec &imag);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec to_cvec(const svec &real, const svec &imag);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec to_cvec(const ivec &real, const ivec &imag);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cvec to_cvec(const vec &real, const vec &imag);

ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bmat to_bmat(const smat &m);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT bmat to_bmat(const imat &m);

ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT smat to_smat(const bmat &m);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT smat to_smat(const imat &m);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT smat to_smat(const mat &m);

ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT imat to_imat(const bmat &m);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT imat to_imat(const smat &m);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT imat to_imat(const mat &m);

ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT mat to_mat(const bmat &m);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT mat to_mat(const smat &m);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT mat to_mat(const imat &m);

ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cmat to_cmat(const bmat &m);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cmat to_cmat(const smat &m);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cmat to_cmat(const imat &m);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cmat to_cmat(const mat &m);

ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cmat to_cmat(const bmat &real, const bmat &imag);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cmat to_cmat(const smat &real, const smat &imag);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cmat to_cmat(const imat &real, const imat &imag);
ZKIC_DLL_EXPORT_TEMPLATE template ZKIC_DLL_EXPORT cmat to_cmat(const mat &real, const mat &imag);

//! \endcond

} // namespace ZK

#endif // CONVERTERS_H
