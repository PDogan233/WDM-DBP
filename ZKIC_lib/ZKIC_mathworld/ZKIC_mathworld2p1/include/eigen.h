#ifndef EIGEN_H
#define EIGEN_H

#include "mat.h"
#include "tool.h"

namespace ZK
{

/*!
  \ingroup matrixdecomp
  \brief Calculates the eigenvalues and eigenvectors of a symmetric real matrix

  The Eigenvalues \f$\mathbf{d}(d_0, d_1, \ldots, d_{n-1})\f$ and the eigenvectors
  \f$\mathbf{v}_i, \: i=0, \ldots, n-1\f$ of the real and symmetric \f$n \times n\f$
  matrix \f$\mathbf{A}\f$ satisfies
  \f[
  \mathbf{A} \mathbf{v}_i = d_i \mathbf{v}_i\: i=0, \ldots, n-1.
  \f]
  The eigenvectors are the columns of the matrix V.
  True is returned if the calculation was successful. Otherwise false.

  Uses the LAPACK routine DSYEV.
*/
ZKIC_DLL_EXPORT bool eig_sym(const mat &A, vec &d, mat &V);

/*!
  \ingroup matrixdecomp
  \brief Calculates the eigenvalues of a symmetric real matrix

  The Eigenvalues \f$\mathbf{d}(d_0, d_1, \ldots, d_{n-1})\f$ and the eigenvectors
  \f$\mathbf{v}_i, \: i=0, \ldots, n-1\f$ of the real and symmetric \f$n \times n\f$
  matrix \f$\mathbf{A}\f$ satisfies
  \f[
  \mathbf{A} \mathbf{v}_i = d_i \mathbf{v}_i\: i=0, \ldots, n-1.
  \f]
  True is returned if the calculation was successful. Otherwise false.

  Uses the LAPACK routine DSYEV.
*/
ZKIC_DLL_EXPORT bool eig_sym(const mat &A, vec &d);

/*!
  \ingroup matrixdecomp
  \brief Calculates the eigenvalues of a symmetric real matrix

  The Eigenvalues \f$\mathbf{d}(d_0, d_1, \ldots, d_{n-1})\f$ and the eigenvectors
  \f$\mathbf{v}_i, \: i=0, \ldots, n-1\f$ of the real and symmetric \f$n \times n\f$
  matrix \f$\mathbf{A}\f$ satisfies
  \f[
  \mathbf{A} \mathbf{v}_i = d_i \mathbf{v}_i\: i=0, \ldots, n-1.
  \f]

  Uses the LAPACK routine DSYEV.
*/
ZKIC_DLL_EXPORT vec eig_sym(const mat &A);

/*!
  \ingroup matrixdecomp
  \brief Calculates the eigenvalues and eigenvectors of a hermitian complex matrix

  The Eigenvalues \f$\mathbf{d}(d_0, d_1, \ldots, d_{n-1})\f$ and the eigenvectors
  \f$\mathbf{v}_i, \: i=0, \ldots, n-1\f$ of the complex and hermitian \f$n \times n\f$
  matrix \f$\mathbf{A}\f$ satisfies
  \f[
  \mathbf{A} \mathbf{v}_i = d_i \mathbf{v}_i\: i=0, \ldots, n-1.
  \f]
  The eigenvectors are the columns of the matrix V.
  True is returned if the calculation was successful. Otherwise false.

  Uses the LAPACK routine ZHEEV.
*/
ZKIC_DLL_EXPORT bool eig_sym(const cmat &A, vec &d, cmat &V);

/*!
  \ingroup matrixdecomp
  \brief Calculates the eigenvalues of a hermitian complex matrix

  The Eigenvalues \f$\mathbf{d}(d_0, d_1, \ldots, d_{n-1})\f$ and the eigenvectors
  \f$\mathbf{v}_i, \: i=0, \ldots, n-1\f$ of the complex and hermitian \f$n \times n\f$
  matrix \f$\mathbf{A}\f$ satisfies
  \f[
  \mathbf{A} \mathbf{v}_i = d_i \mathbf{v}_i\: i=0, \ldots, n-1.
  \f]
  True is returned if the calculation was successful. Otherwise false.

  Uses the LAPACK routine ZHEEV.
*/
ZKIC_DLL_EXPORT bool eig_sym(const cmat &A, vec &d);

/*!
  \ingroup matrixdecomp
  \brief Calculates the eigenvalues of a hermitian complex matrix

  The Eigenvalues \f$\mathbf{d}(d_0, d_1, \ldots, d_{n-1})\f$ and the eigenvectors
  \f$\mathbf{v}_i, \: i=0, \ldots, n-1\f$ of the complex and hermitian \f$n \times n\f$
  matrix \f$\mathbf{A}\f$ satisfies
  \f[
  \mathbf{A} \mathbf{v}_i = d_i \mathbf{v}_i\: i=0, \ldots, n-1.
  \f]

  Uses the LAPACK routine ZHEEV.
*/
ZKIC_DLL_EXPORT vec eig_sym(const cmat &A);

/*!
  \ingroup matrixdecomp
  \brief Calculates the eigenvalues and eigenvectors of a real non-symmetric matrix

  The Eigenvalues \f$\mathbf{d}(d_0, d_1, \ldots, d_{n-1})\f$ and the eigenvectors
  \f$\mathbf{v}_i, \: i=0, \ldots, n-1\f$ of the real \f$n \times n\f$
  matrix \f$\mathbf{A}\f$ satisfies
  \f[
  \mathbf{A} \mathbf{v}_i = d_i \mathbf{v}_i\: i=0, \ldots, n-1.
  \f]
  The eigenvectors are the columns of the matrix V.
  True is returned if the calculation was successful. Otherwise false.

  Uses the LAPACK routine DGEEV.
*/
ZKIC_DLL_EXPORT bool eig(const mat &A, cvec &d, cmat &V);

/*!
  \ingroup matrixdecomp
  \brief Calculates the eigenvalues of a real non-symmetric matrix

  The Eigenvalues \f$\mathbf{d}(d_0, d_1, \ldots, d_{n-1})\f$ and the eigenvectors
  \f$\mathbf{v}_i, \: i=0, \ldots, n-1\f$ of the real \f$n \times n\f$
  matrix \f$\mathbf{A}\f$ satisfies
  \f[
  \mathbf{A} \mathbf{v}_i = d_i \mathbf{v}_i\: i=0, \ldots, n-1.
  \f]
  True is returned if the calculation was successful. Otherwise false.

  Uses the LAPACK routine DGEEV.
*/
ZKIC_DLL_EXPORT bool eig(const mat &A, cvec &d);

/*!
  \ingroup matrixdecomp
  \brief Calculates the eigenvalues of a real non-symmetric matrix

  The Eigenvalues \f$\mathbf{d}(d_0, d_1, \ldots, d_{n-1})\f$ and the eigenvectors
  \f$\mathbf{v}_i, \: i=0, \ldots, n-1\f$ of the real \f$n \times n\f$
  matrix \f$\mathbf{A}\f$ satisfies
  \f[
  \mathbf{A} \mathbf{v}_i = d_i \mathbf{v}_i\: i=0, \ldots, n-1.
  \f]

  Uses the LAPACK routine DGEEV.
*/
ZKIC_DLL_EXPORT cvec eig(const mat &A);

/*!
  \ingroup matrixdecomp
  \brief Calculates the eigenvalues and eigenvectors of a complex non-hermitian matrix

  The Eigenvalues \f$\mathbf{d}(d_0, d_1, \ldots, d_{n-1})\f$ and the eigenvectors
  \f$\mathbf{v}_i, \: i=0, \ldots, n-1\f$ of the complex \f$n \times n\f$
  matrix \f$\mathbf{A}\f$ satisfies
  \f[
  \mathbf{A} \mathbf{v}_i = d_i \mathbf{v}_i\: i=0, \ldots, n-1.
  \f]
  The eigenvectors are the columns of the matrix V.
  True is returned if the calculation was successful. Otherwise false.

  Uses the LAPACK routine ZGEEV.
*/
ZKIC_DLL_EXPORT bool eig(const cmat &A, cvec &d, cmat &V);

/*!
  \ingroup matrixdecomp
  \brief Calculates the eigenvalues of a complex non-hermitian matrix

  The Eigenvalues \f$\mathbf{d}(d_0, d_1, \ldots, d_{n-1})\f$ and the eigenvectors
  \f$\mathbf{v}_i, \: i=0, \ldots, n-1\f$ of the complex \f$n \times n\f$
  matrix \f$\mathbf{A}\f$ satisfies
  \f[
  \mathbf{A} \mathbf{v}_i = d_i \mathbf{v}_i\: i=0, \ldots, n-1.
  \f]
  True is returned if the calculation was successful. Otherwise false.

  Uses the LAPACK routine ZGEEV.
*/
ZKIC_DLL_EXPORT bool eig(const cmat &A, cvec &d);

/*!
  \ingroup matrixdecomp
  \brief Calculates the eigenvalues of a complex non-hermitian matrix

  The Eigenvalues \f$\mathbf{d}(d_0, d_1, \ldots, d_{n-1})\f$ and the eigenvectors
  \f$\mathbf{v}_i, \: i=0, \ldots, n-1\f$ of the complex \f$n \times n\f$
  matrix \f$\mathbf{A}\f$ satisfies
  \f[
  \mathbf{A} \mathbf{v}_i = d_i \mathbf{v}_i\: i=0, \ldots, n-1.
  \f]

  Uses the LAPACK routine ZGEEV.
*/
ZKIC_DLL_EXPORT cvec eig(const cmat &A);

} // namespace ZK

#endif // #ifndef EIGEN_H
