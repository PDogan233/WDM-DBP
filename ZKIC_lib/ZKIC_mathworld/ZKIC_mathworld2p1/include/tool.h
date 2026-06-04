#pragma once

#include<iostream>
#include<cassert>

#if !defined(ZKIC_SHARED_DLL) && (defined(_WIN32) || defined(__CYGWIN__))
#define ZKIC_SHARED_DLL   
#endif

#if defined(ZKIC_SHARED_DLL) && defined(_MSC_VER)
#define ZKIC_DLL_EXPORT __declspec(dllexport)
#else
#define ZKIC_DLL_EXPORT __declspec(dllimport)
#endif



#if (__GNUC__ >= 4) 
#ifndef	ZKIC_EXPORT_TEMPLATE
#define ZKIC_EXPORT_TEMPLATE extern
#endif
#endif

#ifndef ZKIC_DLL_EXPORT
#define ZKIC_DLL_EXPORT
#endif
#ifndef ZKIC_DLL_EXPORT_TEMPLATE
#define ZKIC_DLL_EXPORT_TEMPLATE
#endif


#define ZKIC_Assert(Y,X) if((Y)==false){assert(Y); std::cout<<X<<std::endl;} 
#define it_info_debug(s) std::cout<<s<<std::endl



inline void it_error(const std::string& msg)
{
	std::cout << msg << std::endl;
}

inline void it_warning(const std::string& msg)
{
	std::cout << msg << std::endl;
}

inline void it_error_if(bool judge,const std::string& mess) 
{
	if (judge)
	{
		std::cout << mess << std::endl;
	}
	
}

