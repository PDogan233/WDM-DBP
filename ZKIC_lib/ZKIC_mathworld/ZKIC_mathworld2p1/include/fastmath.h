#ifndef FASTMATH_H
#define FASTMATH_H

#include "vec.h"
#include "mat.h"
#include "tool.h"

namespace ZK
{

/*!
  \relatesalso Mat
  \brief Calculates m=m-v*v'*m
*/
ZKIC_DLL_EXPORT void sub_v_vT_m(mat &m, const vec &v);

/*!
  \relatesalso Mat
  \brief Calculates m=m-m*v*v'
*/
ZKIC_DLL_EXPORT void sub_m_v_vT(mat &m, const vec &v);

} // namespace ZK

#endif // #ifndef FASTMATH_H
