#ifndef QR_H
#define QR_H

#include "mat.h"
#include "tool.h"

namespace ZK
{


/*! \addtogroup matrixdecomp
 */
//!@{
/*!
  \brief QR factorisation of real matrix

  The QR factorization of the real matrix \f$\mathbf{A}\f$ of size \f$m \times n\f$ is given
  by
  \f[
  \mathbf{A} = \mathbf{Q} \mathbf{R} ,
  \f]
  where \f$\mathbf{Q}\f$ is an \f$m \times m\f$ orthogonal matrix and \f$\mathbf{R}\f$ is an \f$m \times n\f$ upper triangular matrix.

  Returns true is calculation succeeds. False otherwise.
  Uses the LAPACK routine DGEQRF and DORGQR.
*/
ZKIC_DLL_EXPORT bool qr(const mat &A, mat &Q, mat &R);

/*!
 * \brief QR factorisation of real matrix with suppressed evaluation of Q
 *
 * For certain type of applications only the \f$\mathbf{R}\f$ matrix of full
 * QR factorization of the real matrix \f$\mathbf{A}=\mathbf{Q}\mathbf{R}\f$
 * is needed. These situations arise typically in designs of square-root
 * algorithms where it is required that
 * \f$\mathbf{A}^{T}\mathbf{A}=\mathbf{R}^{T}\mathbf{R}\f$. In such cases,
 * evaluation of \f$\mathbf{Q}\f$ can be skipped.
 *
 * Modification of qr(A,Q,R).
 *
 * \author Vasek Smidl
 */
ZKIC_DLL_EXPORT bool qr(const mat &A, mat &R);

/*!
  \brief QR factorisation of real matrix with pivoting

  The QR factorization of the real matrix \f$\mathbf{A}\f$ of size \f$m \times n\f$ is given
  by
  \f[
  \mathbf{A} \mathbf{P} = \mathbf{Q} \mathbf{R} ,
  \f]
  where \f$\mathbf{Q}\f$ is an \f$m \times m\f$ orthogonal matrix, \f$\mathbf{R}\f$ is an \f$m \times n\f$ upper triangular matrix
  and \f$\mathbf{P}\f$ is an \f$n \times n\f$ permutation matrix.

  Returns true is calculation succeeds. False otherwise.
  Uses the LAPACK routines DGEQP3 and DORGQR.
*/
ZKIC_DLL_EXPORT bool qr(const mat &A, mat &Q, mat &R, bmat &P);

/*!
  \brief QR factorisation of a complex matrix

  The QR factorization of the complex matrix \f$\mathbf{A}\f$ of size \f$m \times n\f$ is given
  by
  \f[
  \mathbf{A} = \mathbf{Q} \mathbf{R} ,
  \f]
  where \f$\mathbf{Q}\f$ is an \f$m \times m\f$ unitary matrix and \f$\mathbf{R}\f$ is an \f$m \times n\f$ upper triangular matrix.

  Returns true is calculation succeeds. False otherwise.
  Uses the LAPACK routines ZGEQRF and ZUNGQR.
*/
ZKIC_DLL_EXPORT bool qr(const cmat &A, cmat &Q, cmat &R);

/*!
 * \brief QR factorisation of complex matrix with suppressed evaluation of Q
 *
 * For certain type of applications only the \f$\mathbf{R}\f$ matrix of full
 * QR factorization of the complex matrix
 * \f$\mathbf{A}=\mathbf{Q}\mathbf{R}\f$ is needed. These situations arise
 * typically in designs of square-root algorithms where it is required that
 * \f$\mathbf{A}^{H}\mathbf{A}=\mathbf{R}^{H}\mathbf{R}\f$. In such cases,
 * evaluation of \f$\mathbf{Q}\f$ can be skipped.
 *
 * Modification of qr(A,Q,R).
 *
 * \author Vasek Smidl
 */
ZKIC_DLL_EXPORT bool qr(const cmat &A, cmat &R);

/*!
  \brief QR factorisation of a complex matrix with pivoting

  The QR factorization of the complex matrix \f$\mathbf{A}\f$ of size \f$m \times n\f$ is given
  by
  \f[
  \mathbf{A} \mathbf{P} = \mathbf{Q} \mathbf{R} ,
  \f]
  where \f$\mathbf{Q}\f$ is an \f$m \times m\f$ unitary matrix, \f$\mathbf{R}\f$ is an \f$m \times n\f$ upper triangular matrix
  and \f$\mathbf{P}\f$ is an \f$n \times n\f$ permutation matrix.

  Returns true is calculation succeeds. False otherwise.
  Uses the LAPACK routines ZGEQP3 and ZUNGQR.
*/
ZKIC_DLL_EXPORT bool qr(const cmat &A, cmat &Q, cmat &R, bmat &P);

//!@}


} // namespace ZK

#endif // #ifndef QR_H
