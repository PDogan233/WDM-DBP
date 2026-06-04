#ifndef COMMFUNC_H
#define COMMFUNC_H

#include "mat.h"
#include "vec.h"

namespace ZK
{

/*!
  \brief Generate Gray code of blocklength m.
  \ingroup misccommfunc

  The codes are contained as binary codewords {0,1} in the rows of the
  returned matrix.
  See also the \c gray() function in \c math/scalfunc.h.
*/
ZKIC_DLL_EXPORT bmat graycode(int m);

/*!
  \brief Calculate the Hamming distance between \a a and \a b
  \ingroup misccommfunc
*/
 ZKIC_DLL_EXPORT int hammingDistance(const bvec &a, const bvec &b);

/*!
  \brief Calculate the Hamming weight of \a a
  \ingroup misccommfunc
*/
 ZKIC_DLL_EXPORT int weight(const bvec &a);

/*!
 * \brief Compute the water-filling solution
 * \ingroup misccommfunc
 *
 * This function computes the solution of the water-filling problem
 * \f[
 * \max_{p_0,...,p_{n-1}} \sum_{i=0}^{n-1} \log\left(1+p_i\alpha_i\right)
 * \f]
 * subject to
 * \f[
 * \sum_{i=0}^{n-1} p_i \le P
 * \f]
 *
 * \param alpha vector of \f$\alpha_0,...,\alpha_{n-1}\f$ gains (must have
 * strictly positive elements)
 * \param P power constraint
 * \return vector of power allocations \f$p_0,...,p_{n-1}\f$
 *
 * The computational complexity of the method is \f$O(n^2)\f$ at most
 */
 ZKIC_DLL_EXPORT vec waterfilling(const vec& alpha, double P);
 
 /*!
  \brief Compute the Euclidean distance of a vector
  \ingroup misccommfunc
 */
 ZKIC_DLL_EXPORT double euclideanDistance(const vec& v,const vec& h);

} // namespace ZK

#endif // #ifndef COMMFUNC_H
