#pragma once
#include "fft.h"

namespace ZK
{
	/*!
	* Linear convolution based on FFT
	* Function name: linearConvolutionfft
	* @param x Input signal x
	* @param h Impulse response h
	* @return vec The result of linear convolution
	* Function: Perform linear convolution using FFT for efficient computation
	*/
	vec linearConvolutionfft(const vec& x, const vec& h);

	/*!
	* Linear convolution
	* Function name: linearConvolution
	* @param x Input signal x
	* @param h Impulse response h
	* @return vec The result of linear convolution
	* Function: When the amount of data is small, linear convolution is calculated directly in the time domain;
	  when the amount of data is large, FFT is called for calculation.
	*/
	ZKIC_DLL_EXPORT vec linearConvolution(const vec& x, const vec& h);

	/*!
	* Circular Convolution
	* Function name: circularConvolution
	* @param x Input signal x
	* @param h Impulse response h
	* @return vec The result of circular convolution
	* Function: Perform circular convolution using FFT for efficient computation
	*/
	ZKIC_DLL_EXPORT vec circularConvolution(const vec& x, const vec& h);

	/*!
	* Complex linear convolution based on FFT
	* Function name: linearConvolutionfft
	* @param x Input complex signal x
	* @param h Complex impulse response h
	* @return cvec The result of linear convolution
	* Function: Perform linear convolution using FFT for efficient computation
	*/
	ZKIC_DLL_EXPORT cvec linearConvolutionfft(const cvec& x, const cvec& h);

	/*!
	* Complex linear convolution
	 * Function name: linearConvolution
	* @param x Input complex signal x
	* @param h Complex impulse response h
	* @return cvec The result of linear convolution
	* Function: When the amount of data is small, linear convolution is calculated directly in the time domain;
	*           when the amount of data is large, FFT is called for calculation.
	 */
	ZKIC_DLL_EXPORT cvec linearConvolution(const cvec& x, const cvec& h);

	/*!
	* Complex circular convolution
	* Function name: circularConvolution
	* @param x Input complex signal x
	* @param h Complex impulse response h
	* @return cvec The result of circular convolution
	* Function: Perform circular convolution using FFT for efficient computation
	*/
	ZKIC_DLL_EXPORT cvec circularConvolution(const cvec& x, const cvec& h);

}//end namespace ZK