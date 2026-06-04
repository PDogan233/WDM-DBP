#pragma once

#include "fftw3.h"
#include <unordered_map>
#include "vec.h"
#include "mat.h"
#include <immintrin.h>
#include <atomic>
#include <omp.h>

#ifndef  FFTW_CALCULATE_METHOD
#define FFTW_CALCULATE_METHOD  FFTW_ESTIMATE
#endif // ! FFTW_CALCULATE_METHOD


namespace ZK
{
	//===============================Memory Manager===============================
	struct FFTWDeleter
	{
		void operator()(fftw_complex* p) { if (p) { fftw_free(p); } }
		void operator()(double* p) { if (p) { fftw_free(p); } }
	};

	using fftwComplexPtr = std::unique_ptr<fftw_complex, FFTWDeleter>;
	using fftwDoublePtr = std::unique_ptr<double, FFTWDeleter>;

	//===============================Aligned Memory Allocation===============================
	//Ensure 32-byte memory alignment to optimize AVX2 SIMD access performance
	class AlignedFFTWAllocator
	{
	public:
		static constexpr int ALIGNMENT = 32;// AVX2 requires 32-byte alignment

		static fftwComplexPtr allocateComplex(int count)
		{
			void* ptr = fftw_malloc(sizeof(fftw_complex) * count);
			if (!ptr) {
				throw std::bad_alloc();
			}

			return fftwComplexPtr(reinterpret_cast<fftw_complex*>(ptr));
		}

		static fftwDoublePtr allocateDouble(int count)
		{
			void* ptr = fftw_malloc(sizeof(double) * count);
			if (!ptr) {
				throw std::bad_alloc();
			}
			return fftwDoublePtr(reinterpret_cast<double*>(ptr));
		}
	};

	//===============================FFTW Plan Cache Key===============================
	//Used to uniquely identify different FFT plan configurations
	struct PlanKey
	{
		int size;		// The size of FFT
		int direction;	// Direction of fft: FFTW_FORWARD or FFTW_BACKWARD
		bool is2D;		// Is it 2D fft
		int rows;		// The number of rows in 2D FFT
		int	cols;		// Number of columns in 2D FFT

		bool operator==(const PlanKey& other) const
		{
			return size == other.size && direction == other.direction && is2D == other.is2D && rows == other.rows && cols == other.cols;
		}
	};

	// Planned hash function for unordered_map
	struct PlanKeyHash
	{
		int operator()(const PlanKey& k)const
		{
			return std::hash<int>()(k.size) ^ (std::hash<int>()(k.direction) << 1) ^ (std::hash<bool>()(k.is2D) << 2) ^ (std::hash<int>()(k.rows) << 3) ^ (std::hash<int>()(k.cols) << 4);
		}
	};

	//=================================Thread-local FFT Cache=====================================
	//Each thread can maintain its own FFT plan cache and memory to avoid inter-thread competition
	class ThreadLocalFFTCache
	{
	private:

		thread_local static std::unordered_map<PlanKey, fftw_plan, PlanKeyHash>* planCache;// Thread-local storage: Each thread has an independent plan cache
		// Thread-local workspace to avoid repeated allocation
		thread_local static fftwComplexPtr workSpaceIn;		// Input buffer
		thread_local static fftwComplexPtr workSpaceOut;	// output buffer
		thread_local static fftwDoublePtr workSpaceReal;	// Real number buffer (R2C transformation)
		thread_local static int workSPaceSize;				// Current workspace size
		thread_local static int realSpaceSize;				// Current real workspace size

		
		static std::unordered_map<PlanKey, fftw_plan, PlanKeyHash>& getPlanCache() {
			if (!planCache) {
				planCache = new std::unordered_map<PlanKey, fftw_plan, PlanKeyHash>();
			}
			return *planCache;
		}

	public:

		/*!
		* Get the plan for creating 1D FFT
		* Function name: getPlan1D
		* @param N The size of FFT
		* @param direction FFTW_FORWARD or FFTW_BACKWARD
		* @return fftw_plan The corresponding FFTW plan
		* Function: Create or retrieve a cached 1D FFT plan
		*/
		static fftw_plan getPlan1D(int N, int direction)
		{
			PlanKey key{ N,direction,false,0,0 };

			auto& cache = getPlanCache();
			auto it = cache.find(key);
			if (it != cache.end())
			{
				return it->second;
			}

			ensureWorkSpace(N);

			fftw_plan plan = fftw_plan_dft_1d(N, workSpaceIn.get(), workSpaceOut.get(), direction, FFTW_CALCULATE_METHOD);

			cache[key] = plan;
			return plan;
		}


		/*!
		* Get the created 2D FFT plan
		* Function name: getPlan2D
		* @param rows The number of rows in 2D FFT
		* @param cols The number of columns in 2D FFT
		* @param direction FFTW_FORWARD or FFTW_BACKWARD
		* @return fftw_plan The corresponding FFTW plan
		* Function: Obtain the 2D FFT plan; if it does not exist, create it.
		*/
		static fftw_plan getPlan2D(int rows, int cols, int direction)
		{
			int N = rows * cols;

			PlanKey key{ N,direction,true,rows,cols };

			auto& cache = getPlanCache();
			auto it = cache.find(key);
			if (it != cache.end())
			{
				return it->second;
			}

			ensureWorkSpace(N);

			fftw_plan plan = fftw_plan_dft_2d(rows, cols, workSpaceIn.get(), workSpaceOut.get(), direction, FFTW_CALCULATE_METHOD);

			cache[key] = plan;
			return plan;

		}

		/*!
		* Plan for obtaining the transformation from real numbers to complex numbers
		* Function name: getPlanR2C
		* @param N The size of FFT
		* @return fftw_plan The corresponding FFTW plan
		* Function: Create or retrieve a cached R2C FFT plan
		*/
		static fftw_plan getPlanR2C(int N)
		{
			PlanKey key{ N,FFTW_FORWARD,false,-1,0 };// Use -1 to distinguish R2C

			auto& cache = getPlanCache();
			auto it = cache.find(key);
			if (it != cache.end())
			{
				return it->second;
			}

			ensureWorkSpaceReal(N);

			int NComplex = N / 2 + 1;
			ensureWorkSpace(NComplex);

			fftw_plan plan = fftw_plan_dft_r2c_1d(N, workSpaceReal.get(), workSpaceOut.get(), FFTW_CALCULATE_METHOD);
			cache[key] = plan;
			return plan;
		}

		/*!
		* Plan for obtaining the transformation from complex numbers to real numbers
		* Function name: getPlanC2R
		* @param N The size of FFT
		* @return fftw_plan The corresponding FFTW plan
		* Function: Create or retrieve a cached C2R FFT plan
		*/
		static fftw_plan getPlanC2R(int N)
		{
			PlanKey key{ N,FFTW_BACKWARD,false,-2,0 };// Use -2 to distinguish C2R

			auto& cache = getPlanCache();
			auto it = cache.find(key);
			if (it != cache.end())
			{
				return it->second;
			}
			int NComplex = N / 2 + 1;
			ensureWorkSpace(NComplex);

			if (realSpaceSize < static_cast<int>(N))
			{
				workSpaceReal = AlignedFFTWAllocator::allocateDouble(N);
				realSpaceSize = static_cast<int>(N);
			}

			fftw_plan plan = fftw_plan_dft_c2r_1d(N, workSpaceIn.get(), workSpaceReal.get(), FFTW_CALCULATE_METHOD);
			cache[key] = plan;
			return plan;
		}

		/*!
		* Ensure that the workspace is large enough (dynamically expandable)
		* Function name: ensureWorkSpace
		* @param requiredSize The required size of the workspace
		* @return void
		* Function: Ensure that the complex workspace is large enough
		*/
		static void ensureWorkSpace(int requiredSize)
		{
			if (!workSpaceIn || !workSpaceOut || workSPaceSize < requiredSize)
			{
				try {
					auto newIn = AlignedFFTWAllocator::allocateComplex(requiredSize);
					auto newOut = AlignedFFTWAllocator::allocateComplex(requiredSize);

					workSpaceIn = std::move(newIn);
					workSpaceOut = std::move(newOut);
					workSPaceSize = requiredSize;
				}
				catch (const std::bad_alloc&) {
					throw std::runtime_error("Failed to allocate FFTW workspace memory");
				}
			}
		}

		/*!
		* Ensure that the real number workspace is large enough (with dynamic expansion)
		* Function name: ensureWorkSpaceReal
		* @param requiredSize The required size of the workspace
		* @return void
		* Function:: Ensure that the real number workspace is large enough
		*/
		static void ensureWorkSpaceReal(int requiredSize)
		{
			if (!workSpaceReal || realSpaceSize < requiredSize)
			{
				workSpaceReal = AlignedFFTWAllocator::allocateDouble(requiredSize);
				realSpaceSize = requiredSize;
			}
		}

		//Get the workspace pointer
		static fftw_complex* getWorkSpaceIn() { return workSpaceIn.get(); }
		static fftw_complex* getWorkSpaceOut() { return workSpaceOut.get(); }
		static double* getWorkSpaceReal() { return workSpaceReal.get(); }

		/*!
		* Thread cleanup function
		* Function name: cleanup
		* @param  void
		* @return void
		* Function:: called when the thread ends to release resources
		*/
		static void cleanup()
		{
			if (planCache) {
				for (auto& pair : *planCache)
				{
					if (pair.second) {
						fftw_destroy_plan(pair.second);
					}
				}
				delete planCache;
				planCache = nullptr;
			}

			workSpaceIn.reset();
			workSpaceOut.reset();
			workSpaceReal.reset();
			workSPaceSize = 0;
			realSpaceSize = 0;
		}
	};

	//===============================FFTW Global Initialization Manager===============================
	class FFTWManager
	{
	public:

		/*
		* Initialize the FFTW thread environment
		* Function name: initialize
		* @param void
		* @return void
		* Function: Initialize FFTW threads using the singleton method to ensure it is initialized only once, and import smart data.
		*/
		static void initialize()
		{
			if (!initialized.exchange(true))
			{
				fftw_init_threads();
				// FFTW's planner is not thread-safe by default; Java-side multi-threaded calls
				// can trigger concurrent plan creation and corrupt heap state.
				fftw_make_planner_thread_safe();
				fftw_plan_with_nthreads(1/*omp_get_max_threads()*/);
				fftw_import_system_wisdom();
				const char* wisdomFile = "fftw_wisdom.dat";
				FILE* f = nullptr;
				errno_t err = fopen_s(&f, wisdomFile, "r");
				if (f)
				{
					fftw_import_wisdom_from_file(f);
					fclose(f);
				}
			}
		}

		static void cleanup()
		{
			if (initialized.exchange(false))
			{
				const char* wisdomFile = "fftw_wisdom.dat";
				FILE* f = nullptr;
				errno_t err = fopen_s(&f, wisdomFile, "w");
				if (f)
				{
					fftw_export_wisdom_to_file(f);
					fclose(f);
				}

				fftw_cleanup_threads();
				fftw_cleanup();
			}
		}

	private:
		static std::atomic<bool> initialized;// Atomic variables ensure initialization only once
	};

	//================================SIMD-optimized data copy function==========================================
	/*!
	* vec Data copy to fftw_complex function
	* Function name: vectorizedCopy2FFTW
	* @param src Source complex vector
	* @param dst Destination fftw_complex array
	* @param count Number of elements to copy
	* @return void
	* Function: Use SIMD instructions to accelerate copying from std::complex to fftw_complex
	*/
	inline void vectorizedCopy2FFTW(const cvec& src, fftw_complex* dst, int count)
	{
		const int simdCount = count & ~3ULL;// Handle the part that is a multiple of 4

		// Main loop: process 4 elements in each group
		for (int i = 0;i < simdCount;i += 4)
		{
			if ((i + 8) < count) _mm_prefetch(reinterpret_cast<const char*>(&src[i + 8]), _MM_HINT_T0);


			// Unroll the loop to improve instruction-level parallelism
			for (int j = 0;j < 4;++j)
			{
				dst[i + j][0] = src[i + j].real();
				dst[i + j][1] = src[i + j].imag();
			}

		}
		// Handle the remaining elements
		for (int i = simdCount;i < count;++i)
		{
			dst[i][0] = src[i].real();
			dst[i][1] = src[i].imag();
		}
	}

	/*!
	* nextPow2 function
	* Function name: nextPow2
	* @param n Input size
	* @return size_t The next power of 2 greater than or equal to n
	* Function: Calculate the next power of 2 greater than or equal to n
	*/
	inline size_t nextPow2(size_t n)
	{
		if (n == 0) return 1;
		size_t p = 1;
		while (p < n) p <<= 1;
		return p;
	}
	
	/*!
	* reOrderCorrelation function
	* Function name: reOrderCorrelation
	* @param circleCorr Input circular correlation result vector
	* @param Nx Length of the first input signal
	* @param Ny Length of the second input signal
	* @param Nfft FFT length used in correlation calculation
	* @return Vec<T> Reordered linear correlation result vector
	* Function: Reorder the circular correlation result to obtain the linear correlation result
	*/
	template<typename T>
	Vec<T> reOrderCorrelation(const Vec<T>& circleCorr, size_t Nx, size_t Ny, size_t Nfft)
	{
		size_t Ncorr = Nx + Ny - 1;

		Vec<T> result(Ncorr);

		size_t numNegLags = Ny - 1;
		size_t startNeg = Nfft - numNegLags;

		size_t numPosLags = Nx;

		if (startNeg < Nfft)
		{
			for (size_t i = 0;i < numNegLags;++i)
			{
				result[i] = circleCorr[startNeg + i];
			}
		}

		for (size_t i = 0;i < numPosLags;++i)
		{
			result[numNegLags + i] = circleCorr[i];
		}

		return result;

	}


	/*!
	* vec Data copy to fftw_complex function
	* Function name: vectorizedCopyFromFFTW
	* @param src Source fftw_complex array
	* @param dst Destination complex vector
	* @param count Number of elements to copy
	* @return void
	* Function: Use SIMD instructions to accelerate copying from fftw_complex to std::complex
	*/
	inline void vectorizedCopyFromFFTW(const fftw_complex* src, cvec& dst, int count)
	{
		const int simdCount = count & ~3ULL;

		for (int i = 0;i < simdCount;i += 4)
		{
			if ((i + 8) < count) _mm_prefetch(reinterpret_cast<const char*>(src[i + 8]), _MM_HINT_T0);
			for (int j = 0;j < 4;++j)
			{
				dst[i + j]._Val[0] = src[i + j][0];
				dst[i + j]._Val[1] = src[i + j][1];
			}
		}

		for (int i = simdCount;i < count;++i)
		{
			dst[i]._Val[0] = src[i][0];
			dst[i]._Val[1] = src[i][1];
		}
	}

	//===============================FFT Public Interface Declaration==============================
	//-------------------------------- One-dimensional Fourier transform --------------------------------

	/*
	* One-dimensional Discrete Fourier Transform
	* Function name: fftC2C (Complex to Complex DFT)
	* @param inputSignal Input complex vector (time-domain signal)
	* @return Output complex vector (frequency-domain signal)
	* Function: Perform one-dimensional Fast Fourier Transform from complex to complex
	*/
	ZKIC_DLL_EXPORT cvec fftC2C(const cvec& inputSignal);

	/*
	* Inverse Discrete Fourier Transform
	* Function name: ifftC2C (Complex to Complex IDFT)
	* @param input Input complex vector (frequency domain signal)
	* @return Output complex vector (time domain signal)
	* Function: Perform complex-to-complex inverse fast Fourier transform and normalize
	*/
	ZKIC_DLL_EXPORT cvec ifftC2C(const cvec& input);

	/*
	* FFT from Real Numbers to Complex Numbers
	* Function name: fftR2C (Double to Complex FFT)
	* @param input Input real number vector (time-domain signal)
	* @return Output complex number vector (frequency-domain signal)
	* Function: Perform fast Fourier transform from real numbers to complex numbers, and optimize storage using the symmetry of FFT
	*/
	ZKIC_DLL_EXPORT cvec fftR2C(const vec& input);

	/*
	* IFFT from complex numbers to real numbers
	* Function name: fftC2R (Complex to Double IFFT)
	* @param input Input real number vector (frequency domain signal)
	* @return Output complex number vector (frequency domain signal)
	* Function: Perform fast Fourier transform from real numbers to complex numbers
	*/
	ZKIC_DLL_EXPORT vec ifftC2R(const cvec& input);

	/*
	* In-place FFT transformation
	* Function name: fftC2CInPlace
	* @param data Input and output complex vector (modified in-place)
	* @return void
	* Function: Perform Fourier transformation directly on the original array
	*/
	ZKIC_DLL_EXPORT void fftC2CInPlace(cvec& data);

	// -------------------------------- 2D FFT transform --------------------------------
	/*
	* 2D Discrete Fourier Transform
	* Function name: fft2C2C
	* @param input Input complex matrix (time domain)
	* @return Output complex matrix (frequency domain)
	* Function: Perform 2D Fast Fourier Transform
	*/
	ZKIC_DLL_EXPORT cmat fft2C2C(const cmat& input);

	/*
	* 2D Inverse Discrete Fourier Transform
	* Function name: ifft2C2C
	* @param input Input complex matrix (frequency domain)
	* @return Output complex matrix (time domain)
	* Function: Perform 2D inverse Fourier transform and normalize
	*/
	ZKIC_DLL_EXPORT cmat ifft2C2C(const cmat& input);

	// -------------------------------- Spectrum analysis tool --------------------------------
	/*
	* Power Spectral Density Calculation
	* Function name: fftPowerSpectrum
	* @param input Input complex vector
	* @param isNeedFFT true: Perform FFT first and then calculate the power spectrum; false: Calculate the power spectrum directly
	* @return Power spectral density vector (real number)
	* Function: Calculate the Power Spectral Density (PSD) of the signal, measuring the energy distribution of each frequency component
	*/
	ZKIC_DLL_EXPORT vec fftPowerSpectrum(const cvec& input, bool isNeedFFT = true);

	/*
	* FFT Spectrum Shift
	* Function name: fftShift
	* @param input FFT result (frequency domain)
	* @return Shifted spectrum
	* Function: Move the zero-frequency component to the center of the spectrum to make the spectrum display more intuitive (low frequencies at the center, high frequencies at both ends)
	*/
	ZKIC_DLL_EXPORT cvec fftShift(const cvec& input);

	/*
	* In-place FFT spectrum shifting
	* Function name: fftShiftInPlace
	* @param data Input and output array (modified in place)
	* @return void
	* Function: Perform Fourier transform directly on the original array
	*/
	ZKIC_DLL_EXPORT void fftShiftInPlace(cvec& data);

	//  -------------------------------- batch FFT -------------------------------- 
	/*
	* Batch FFT Processing
	* Function name: batchDftC2C
	* @param inputs A vector of multiple input signals
	* @return The corresponding vector of FFT results
	* Function: Perform FFT on a set of signals in batches, using multi-threaded parallel acceleration
	* Application scenarios:
	* 1. Multi-channel signal processing
	* 2. Sliding window FFT
	* 3. Parallel spectrum analysis
	*/
	ZKIC_DLL_EXPORT std::vector<cvec> batchDftC2C(const Vec<cvec>& inputs);

	// -------------------------------- FFTW Global Initialization and Cleanup -------------------------------- 
	 /*
	* Thread Cleanup Function
	* Function name: cleanupThreadFFT
	* @param void
	* @return void
	* Function: Manually clean up the FFT resources of the current thread
	* Calling time
	* 1. When the thread is about to end
	* 2. When memory needs to be released
	* 3. Before switching to FFT tasks of different sizes
	*/

	ZKIC_DLL_EXPORT void cleanupThreadFFT();

	/*!
	* Cross corrrelation function
	* Function name: crossCorrrelation
	* @param x First input complex vector
	* @param y Second input complex vector
	* @return Cross-correlation result vector
	* Function: Calculate the cross-correlation between two complex vectors using FFT
	*/
	ZKIC_DLL_EXPORT cvec crossCorrelation(const cvec& x, const cvec& y);

	/*!
	* Auto-correlation function
	* Function name: autoCorrelatuon
	* @param x Input complex vector
	* @return Auto-correlation result vector
	* Function: Calculate the auto-correlation of a complex vector using FFT
	*/
	ZKIC_DLL_EXPORT cvec autoCorrelation(const cvec& x);

	/*!
	* Cross corrrelation function
	* Function name: crossCorrelation
	* @param x First input real vector
	* @param y Second input real vector
	* @return Cross-correlation result vector
	* Function: Calculate the cross-correlation between two real vectors using FFT
	*/
	ZKIC_DLL_EXPORT vec crossCorrelation(const vec& x, const vec& y);

	/*!
	* Auto-correlation function
	* Function name: autoCorrelation
	* @param x Input real vector
	* @return Auto-correlation result vector
	* Function: Calculate the auto-correlation of a real vector using FFT
	*/
	ZKIC_DLL_EXPORT vec autoCorrelation(const vec& x);
} //end namespace ZK
