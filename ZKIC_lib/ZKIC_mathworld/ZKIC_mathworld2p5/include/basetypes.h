#ifndef ITTYPES_H
#define ITTYPES_H

//! \cond

// Fixed size integer types for MSVC++
#if defined(_MSC_VER)
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
// Assume that inttypes.h header file is available everywhere
#  include <inttypes.h>
#endif // defined(_MSC_VER)

//! \endcond

#endif // ITTYPES_H
