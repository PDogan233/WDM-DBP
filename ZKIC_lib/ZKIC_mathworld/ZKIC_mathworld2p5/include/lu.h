#ifndef LU_H
#define LU_H

#include "mat.h"
#include "tool.h"

namespace ZK
{


/*! \addtogroup matrixdecomp
 */
//!@{
/*!
  \brief LU factorisation of real matrix

  The LU factorization of the real matrix \f$\mathbf{X}\f$ of size \f$n \times n\f$ is given
  by
  \f[
  \mathbf{X} = \mathbf{P}^T \mathbf{L} \mathbf{U} ,
  \f]
  where \f$\mathbf{L}\f$ and \f$\mathbf{U}\f$ are lower and upper triangular matrices
  and \f$\mathbf{P}\f$ is a permutation matrix.

  The interchange permutation vector \a p is such that \a k and \a p(k) should be
  changed for all \a k. Given this vector a permutation matrix can be constructed using the
  function
  \code
  bmat permutation_matrix(const ivec &p)
  \endcode

  If \a X is an \a n by \a n matrix \a lu(X,L,U,p) computes the LU decomposition.
  \a L is a lower triangular, \a U an upper triangular matrix.
  \a p is the interchange permutation vector such that \a k and \a p(k) should be
  changed for all \a k.

  Returns true is calculation succeeds. False otherwise.
*/
ZKIC_DLL_EXPORT bool lu(const mat &X, mat &L, mat &U, ivec &p);


/*!
  \brief LU factorisation of real matrix

  The LU factorization of the complex matrix \f$\mathbf{X}\f$ of size \f$n \times n\f$ is given
  by
  \f[
  \mathbf{X} = \mathbf{P}^T \mathbf{L} \mathbf{U} ,
  \f]
  where \f$\mathbf{L}\f$ and \f$\mathbf{U}\f$ are lower and upper triangular matrices
  and \f$\mathbf{P}\f$ is a permutation matrix.

  The interchange permutation vector \a p is such that \a k and \a p(k) should be
  changed for all \a k. Given this vector a permutation matrix can be constructed using the
  function
  \code
  bmat permutation_matrix(const ivec &p)
  \endcode

  If \a X is an \a n by \a n matrix \a lu(X,L,U,p) computes the LU decomposition.
  \a L is a lower triangular, \a U an upper triangular matrix.
  \a p is the interchange permutation vector such that elements \a k and row \a p(k) should be
  interchanged.

  Returns true is calculation succeeds. False otherwise.
*/
ZKIC_DLL_EXPORT bool lu(const cmat &X, cmat &L, cmat &U, ivec &p);


//! Makes swapping of vector b according to the interchange permutation vector p.
ZKIC_DLL_EXPORT void interchange_permutations(vec &b, const ivec &p);

//! Make permutation matrix P from the interchange permutation vector p.
ZKIC_DLL_EXPORT bmat permutation_matrix(const ivec &p);
//!@}

} // namespace ZK

#endif // #ifndef LU_H
