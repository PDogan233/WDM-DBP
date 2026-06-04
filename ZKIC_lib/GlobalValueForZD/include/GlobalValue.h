#pragma once

#include<vec.h>
#include<unordered_map>
#include<common_values.h>
#include<mutex>
#include<windows.h>

namespace ZK
{
	class BandBuilder;
	class ESignalBuilder;
	
	class ThreadSafeResources 
	{
	public:
		static ThreadSafeResources& getInstance()
		{
			static ThreadSafeResources instance;
			return instance;
		}

		CRITICAL_SECTION& getThreadLock() { return lock_; }
	private:
		ThreadSafeResources() 
		{
			InitializeCriticalSection(&lock_);
		}
		~ThreadSafeResources() 
		{
			DeleteCriticalSection(&lock_);
		}

		ThreadSafeResources(const ThreadSafeResources&) = delete;
		ThreadSafeResources& operator=(const ThreadSafeResources&) = delete;

		CRITICAL_SECTION lock_;
	};
	class ScopedLock 
	{
	public:
		ScopedLock(CRITICAL_SECTION& cs) :cs_(cs)
		{
			EnterCriticalSection(&cs_);
		}
		~ScopedLock() 
		{
			LeaveCriticalSection(&cs_);
		}
	private:
		CRITICAL_SECTION& cs_;

	};


	struct ZKIC_DLL_EXPORT Band
	{
		friend class BandBuilder;
	private:
		// ==========================================
		// Band information
		// ==========================================
		std::string band_ID = "band1";			    //the ID of the band
		std::string band_name = "testBand1";        //the name of the band
		long long N_chans = 4;				        //number of channels in the band
		double chan_spacing = 50e9;			        //channel spacing in Hz
		ZK::Vec<long long> chan_index;	            //the index of channels in the band
		ZK::Vec<long long> n_chan_of_interest;      //the index of channels of interest in the band
		long long n_chan_fc_ref = 1;		        //the index of the reference channel in the band
		double fc_ref = 190.1e12;					//the frequency of the reference channel in Hz
		// ==========================================
		// Modulation 
		// ==========================================
		std::string pol_mux_mode = "sgl_pol";		//the polarization multiplexing mode
		std::string modulation_format = "QPSK";		//the modulation format
		long long N_pols = 1;				        //number of polarizations
		long long M_ary = 4;				        //modulation order
		// ==========================================
		// FEC and Channel coding
		// ==========================================
		long long n = 12600;					    //Total number of bits of the channel codeword
		long long k = 10000;					    //Number of information bits in channel coding codeword
		double code_rate = 0.793650793650794;       //Channel coding rate
		// ==========================================
		// Frame structure
		// ==========================================
		long long N_bits_delim = 12;			    //number of bits in the delimiter
		long long N_bits_delim_fix = 12;			//number of bits in the fixed delimiter
		long long N_bits_TS = 1024;			        //number of bits in the training sequence
		long long N_bits_TS_fix = 1024;			    //number of bits in the fixed training sequence
		long long N_codeswords_frame = 15;	        //number of codewords per frame
		long long N_TXbits_frame = 332020;		    //number of transmitted bits per frame
		// ==========================================
		// BitRate system
		// ==========================================
		double Rb_infor = 1e11;				        //the information bit rate per channel in bps
		double Rb = 1.268e11;						//the gross bit rate per channel in bps
		double Rb_fix = 1.268e11;					//the fixed gross bit rate per channel in bps
		// ==========================================
		// Clock and Simulation Control
		// ==========================================
		long long N_frame_clk = 10;			        //number of frames per clock
		long long N_bits_infor_clk = 1500000;		//number of information bits per clock
		double T_clk = 1.5e-5;                      //clock period in once simulation
		long long N_bits_infor_total = 10500000;    //number of information bits per channel
		long long N_codewords_total = 1050;	        //number of codewords in total
		long long N_clks_total = 7;			        //number of clocks in total
		double T_total = 0.000105;					//total time in once simulation
		long long N_TXbits_clk = 1902040;			//number of transmitted bits per clock
		long long N_TXbits_frame_clk = 1902040;	    //number of transmitted bits per frame per clock
		long long N_Txbits_pad_clk = 0;		        //number of padded transmitted bits per clock
		// ==========================================
		// Symbol domain parameters
		// ==========================================
		double fc_ref_fix = 190.1e12;				//the fixed frequency of the reference channel in Hz
		double lambda_ref_fix = 1.577-06;			//the fixed wavelength of the reference channel in nm
		double chan_spacing_fix = 5e10;             //the fixed channel spacing in THz
		double Rsym_fix = 63401333333.3333;			//the fixed symbol rate in Gbaud
		long long N_syms_TS = 512;			        //number of symbols in the training sequence
		long long N_TXsyms_frame = 95102;		    //number of transmitted symbols per frame
		long long N_TXsyms_clk = 951020;		    //number of transmitted symbols per clock
		long long N_TXsyms_frame_clk = 951020;	    //number of transmitted symbols per frame per clock
		long long N_TXsyms_pad_clk = 0;		        //number of padded transmitted symbols per clock
		// ==========================================
		// Sampling and DSP
		// ==========================================
		long long N_per_symbol_default = 16;        //number of samples per symbol (default)
		double sample_rate_default = 1.014e12;//the sample rate (default) in Hz
	public:
		std::string getBandID() const { return band_ID; }
		std::string getBandName() const { return band_name; }
		long long getNumChannels() const { return N_chans; }
		double getChannelSpacing() const { return chan_spacing; }
		ZK::Vec<long long> getChannelIndices() const { return chan_index; }
		ZK::Vec<long long> getChannelsOfInterest() const { return n_chan_of_interest; }
		long long getRefChannelIndex() const { return n_chan_fc_ref; }
		double getRefChannelFrequency() const { return fc_ref; }
		std::string getPolMuxMode() const { return pol_mux_mode; }
		std::string getModulationFormat() const { return modulation_format; }
		long long getNumPolarizations() const { return N_pols; }
		long long getModulationOrder() const { return M_ary; }
		long long getChanCoding_n() const { return n; }
		long long getChanCoding_k() const { return k; }
		double getChanCodeRate() const { return code_rate; }
		long long getNumBitsDelimiter() const { return N_bits_delim; }
		long long getNumBitsDelimiterFixed() const { return N_bits_delim_fix; }
		long long getNumBitsTrainingSeq() const { return N_bits_TS; }
		long long getNumBitsTrainingSeqFixed() const { return N_bits_TS_fix; }
		long long getNumCodewordsPerFrame() const { return N_codeswords_frame; }
		long long getNumTXBitsPerFrame() const { return N_TXbits_frame; }
		double getInformationBitRate() const { return Rb_infor; }
		double getGrossBitRate() const { return Rb; }
		double getFixedGrossBitRate() const { return Rb_fix; }
		long long getNumFramesPerClock() const { return N_frame_clk; }
		long long getNumInfoBitsPerClock() const { return N_bits_infor_clk; }
		double getClockPeriod() const { return T_clk; }
		long long getNumInfoBits() const { return N_bits_infor_total; }
		long long getNumCodewordsTotal() const { return N_codewords_total; }
		long long getNumClocksTotal() const { return N_clks_total; }
		double getTotalTime() const { return T_total; }
		long long getNumTXBitsPerClock() const { return N_TXbits_clk; }
		long long getNumTXBitsPerFramePerClock() const { return N_TXbits_frame_clk; }
		long long getNumTXBitsPaddedPerClock() const { return N_Txbits_pad_clk; }
		double getFixedRefChannelFrequency() const { return fc_ref_fix; }
		double getFixedRefChannelWavelength() const { return lambda_ref_fix; }
		double getFixedChannelSpacing() const { return chan_spacing_fix; }
		double getFixedSymbolRate() const { return Rsym_fix; }
		long long getNumSymbolsTrainingSeq() const { return N_syms_TS; }
		long long getNumTXSymbolsPerFrame() const { return N_TXsyms_frame; }
		long long getNumTXSymbolsPerClock() const { return N_TXsyms_clk; }
		long long getNumTXSymbolsPerFramePerClock() const { return N_TXsyms_frame_clk; }
		long long getNumTXSymbolsPaddedPerClock() const { return N_TXsyms_pad_clk; }
		long long getDefaultSamplesPerSymbol() const { return N_per_symbol_default; }
		double getDefaultSampleRate() const { return sample_rate_default; }
		double getStdChannelFrequency(long long channelIndex) const { return fc_ref + chan_spacing * (channelIndex - n_chan_fc_ref); }
		double getStdChannelWavelength(long long channelIndex) const
		{
			double freq = getStdChannelFrequency(channelIndex);
			return  (PHY_C / freq);
		}
		double getFixChannelFrequency(long long channelIndex) const { return fc_ref_fix + chan_spacing_fix * (channelIndex - n_chan_fc_ref); }
		double getFixChannelWavelength(long long channelIndex) const
		{
			double freq_fix = getFixChannelFrequency(channelIndex);
			return  (PHY_C / freq_fix);
		}
	};

	struct ZKIC_DLL_EXPORT ESignal
	{
		friend class ESignalBuilder;
	private:
		long long N_bands = 1;						  //number of bands
		std::string ref_band_ID = "band1";            //the reference band
		long long N_infor_bits_total_ref_band = 1e7;  //number of total bits in the reference band
		long long N_frames_clk_ref_band_per_pol = 10;  //number of frame clocks per polarization in the reference band
		std::string prop_mode = "fiberLoss";          //the propagation mode
		double M_deci = 4;                            //the decimation factor
	public:
		std::unordered_map<std::string, Band> bands;  //map of bands

		long long getNumBands() const { return N_bands; }
		std::string getRefBandID() const { return ref_band_ID; }
		long long getNumTotalBitsRefBand() const { return N_infor_bits_total_ref_band; }
		long long getNumFramesClkRefBandPerPol() const { return N_frames_clk_ref_band_per_pol; }
		std::string getPropagationMode() const { return prop_mode; }
		double getDecimationFactor() const { return M_deci; }
	};

	class ZKIC_DLL_EXPORT BandBuilder
	{
	private:
		Band _band;

	public:
		BandBuilder() = default;

		BandBuilder& setBandIDName(const std::string & Id,const std::string& name) {
			_band.band_ID = Id;
			_band.band_name = name;
			return *this;
		}

		BandBuilder& setChannelGrid(long long n_chans, double spacing, const ZK::Vec<long long>& indexes, const ZK::Vec<long long>& interest_indexes) {
			_band.N_chans = n_chans;
			_band.chan_spacing = spacing;
			_band.chan_index = indexes;
			_band.n_chan_of_interest = interest_indexes;
			return *this;
		}

		BandBuilder& setRefChannel(long long index, double freq_thz) {
			_band.n_chan_fc_ref = index;
			_band.fc_ref = freq_thz;
			return *this;
		}

		BandBuilder& setModulation(const std::string& mode, const std::string& format, long long pols, long long m_ary) {
			_band.pol_mux_mode = mode;
			_band.modulation_format = format;
			_band.N_pols = pols;
			_band.M_ary = m_ary;
			return *this;
		}

		BandBuilder& setChanCoding(long long n, long long k, double rate) {
			_band.n = n;
			_band.k = k;
			_band.code_rate = rate;
			return *this;
		}

		BandBuilder& setFrameBits(long long bits_delim, long long bits_ts, long long codewords_frame, long long tx_bits_frame) {
			_band.N_bits_delim = bits_delim;
			_band.N_bits_TS = bits_ts;
			_band.N_codeswords_frame = codewords_frame;
			_band.N_TXbits_frame = tx_bits_frame;
			return *this;
		}

		BandBuilder& setRates(double rb_info, double rb_gross, double rb_fix) {
			_band.Rb_infor = rb_info;
			_band.Rb = rb_gross;
			_band.Rb_fix = rb_fix;
			return *this;
		}

		BandBuilder& setClockInfo(long long n_fram_clk, double t_clk, double t_total) {
			_band.N_frame_clk = n_fram_clk;
			_band.T_clk = t_clk;
			_band.T_total = t_total;
			return *this;
		}

		BandBuilder& setTotalCounts(long long codewords_total, long long clks_total) {
			_band.N_codewords_total = codewords_total;
			_band.N_clks_total = clks_total;
			return *this;
		}

		BandBuilder& setInfoBitsCounts(long long bits_info_clk, long long bits_info_total) {
			_band.N_bits_infor_clk = bits_info_clk;
			_band.N_bits_infor_total = bits_info_total;
			return *this;
		}

		BandBuilder& setTxBitsCounts(long long tx_clk, long long tx_frame_clk, long long tx_pad_clk) {
			_band.N_TXbits_clk = tx_clk;
			_band.N_TXbits_frame_clk = tx_frame_clk;
			_band.N_Txbits_pad_clk = tx_pad_clk;
			return *this;
		}

		BandBuilder& setFixedParams(double freq_fix, double lambda_fix,double bits_delim_fix,double bits_ts_fix, double spacing_fix, double rsym_fix) {
			_band.fc_ref_fix = freq_fix;
			_band.lambda_ref_fix = lambda_fix;
			_band.N_bits_delim_fix = bits_delim_fix;
			_band.N_bits_TS_fix = bits_ts_fix;	
			_band.chan_spacing_fix = spacing_fix;
			_band.Rsym_fix = rsym_fix;
			return *this;
		}

		BandBuilder& setSymbolStructure(long long syms_ts, long long tx_syms_frame) {
			_band.N_syms_TS = syms_ts;
			_band.N_TXsyms_frame = tx_syms_frame;
			return *this;
		}

		BandBuilder& setSymbolCounts(long long syms_clk, long long syms_frame_clk, long long syms_pad_clk) {
			_band.N_TXsyms_clk = syms_clk;
			_band.N_TXsyms_frame_clk = syms_frame_clk;
			_band.N_TXsyms_pad_clk = syms_pad_clk;
			return *this;
		}

		BandBuilder& setSampling(long long per_symbol, double sample_rate) {
			_band.N_per_symbol_default = per_symbol;
			_band.sample_rate_default = sample_rate;
			return *this;
		}
		Band build() const& { return _band; }//copy
		Band build()&& { return std::move(_band); }//move
	};

	class ZKIC_DLL_EXPORT ESignalBuilder
	{
	private:
		ESignal _esignal;
	public:
		ESignalBuilder() = default;

		ESignalBuilder& setRefBandInfo(const std::string& refBandID, long long totalBitsNum,long long nFramesClkPerPol,long long bandCount)
		{
			_esignal.ref_band_ID = refBandID;
			_esignal.N_infor_bits_total_ref_band = totalBitsNum;
			_esignal.N_frames_clk_ref_band_per_pol = nFramesClkPerPol;	
			_esignal.N_bands = bandCount;
			return *this;
		}

		ESignalBuilder& setPropagation(const std::string& mode, double decimation) {
			_esignal.prop_mode = mode;
			_esignal.M_deci = decimation;
			return *this;
		}

		ESignalBuilder& addBand(const Band& band) {
			_esignal.bands[band.getBandID()] = band;
			_esignal.N_bands = static_cast<long long>(_esignal.bands.size());
			return *this;
		}

		ESignal build() const {
			return _esignal;
		}
	};

	class ZKIC_DLL_EXPORT GlobalValue
	{
	public:
		static void registerConfig(const std::string& taskId, const ESignal& eSignal);
		static void selectESignal(const std::string& taskId);
		static void removeESignal(const std::string& taskId);
	public:
		static const Band& getBand(const std::string& bandId);
	    static const ESignal& getESignal();
	private:
		static std::unordered_map<std::string, ESignal> eSignalHouse;
	};
}