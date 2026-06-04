#ifndef BASEEXPORTS_H
#define BASEEXPORTS_H

#include "tool.h"

namespace ZK
{

//! \cond

#if (defined(_MSC_VER) && defined(ITPP_SHARED_LIB))
//MSVC needs to explicitely instantiate required templates while building the
//shared library. Also, these definitions are marked as imported when library is
//linked with user's code.
template class ZKIC_DLL_EXPORT Array<bool>;
template class ZKIC_DLL_EXPORT Array<std::string>;
template class ZKIC_DLL_EXPORT Array<bmat>;
template class ZKIC_DLL_EXPORT Array<mat>;
template class ZKIC_DLL_EXPORT Array<vec>;
template class ZKIC_DLL_EXPORT Array<ivec>;
template class ZKIC_DLL_EXPORT Array<cvec>;
template class ZKIC_DLL_EXPORT Array<Array<int> >;
template class ZKIC_DLL_EXPORT Array<Vec<unsigned int> >;
template class ZKIC_DLL_EXPORT Array<Array<vec> >;
template class ZKIC_DLL_EXPORT Array<Array<cvec> >;
#endif

//! \endcond

}

#endif
