#include <iostream>
#include <cmath>
#include <string>
#include <random>
#include <iomanip>
#include <GlobalValue.h>
#include <fstreamIO.h>
#include "PRBSlfsr.h"
#include "MapperMQAM.h"
#include "NRZCoder.h"
#include "IQCombiner.h"
#include "MeasurementSimple.h"

// ==============================================================
// 全局 Band 参数定义
// ==============================================================
// ---- Band C ----
std::string         band_Id_c = "band_C";
std::string         band_name_c = "C-Band";
long long           N_chans_c = 5;
double              chan_spacing_c = 75e9;
ZK::Vec<long long>  chan_index_c = "[1 2 3 4 5]";
ZK::Vec<long long>  n_chan_of_interest_c = "[2 3 4]";
long long           n_chan_fc_ref_c = 3;
double              fc_ref_c = 193.1e12;
std::string         pol_mux_mode_c = "DP";
std::string         modulation_format_c = "16QAM";
long long           N_pols_c = 2;
long long           M_ary_c = 16;
long long           n_fec_c = 32768;
long long           k_fec_c = 27306;
double              code_rate_c = 0.8333;
long long           N_bits_delim_c = 10;
long long           N_bits_delim_fix_c = 10;
long long           N_bits_TS_c = 4096;
long long           N_bits_TS_fix_c = 4096;
long long           N_codeswords_frame_c = 10;
long long           N_TXbits_frame_c = 332020;
double              Rb_infor_c = 400e9;
double              Rb_c = 512e9;
double              Rb_fix_c = 512e9;
long long           N_fram_clk_c = 10;
long long           N_bits_infor_clk_c = 273060;
double              T_clk_c = 6.484e-7;
long long           N_bits_infor_total_c = 1000000;
long long           N_codewords_total_c = 40;
long long           N_clks_total_c = 4;
double              T_total_c = 2.6e-6;
long long           N_TXbits_clk_c = 332020;
long long           N_TXbits_frame_clk_c = 332020;
long long           N_Txbits_pad_clk_c = 0;
double              fc_ref_fix_c = 193.1e12;
double              lambda_ref_fix_c = 1552.52;
double              chan_spacing_fix_c = 75e9;
double              Rsym_fix_c = 128e9;
long long           N_syms_TS_c = 512;
long long           N_TXsyms_frame_c = 41502;
long long           N_TXsyms_clk_c = 41502;
long long           N_TXsyms_frame_clk_c = 41502;
long long           N_TXsyms_pad_clk_c = 0;
long long           N_persymbol_default_c = 2;
double              sample_rate_default_c = 256e9;

// ---- Band L ----
std::string         band_Id_L = "band_L";
std::string         band_name_L = "L-Band";
long long           N_chans_L = 3;
double              chan_spacing_L = 100e9;
ZK::Vec<long long>  chan_index_L = "[1 2 3]";
ZK::Vec<long long>  n_chan_of_interest_L = "[1 2]";
long long           n_chan_fc_ref_L = 2;
double              fc_ref_L = 190.1e12;
std::string         pol_mux_mode_L = "DP";
std::string         modulation_format_L = "64QAM";
long long           N_pols_L = 2;
long long           M_ary_L = 64;
long long           n_fec_L = 16384;
long long           k_fec_L = 13653;
double              code_rate_L = 0.8333;
long long           N_bits_delim_L = 8;
long long           N_bits_delim_fix_L = 8;
long long           N_bits_TS_L = 2048;
long long           N_bits_TS_fix_L = 2048;
long long           N_codeswords_frame_L = 5;
long long           N_TXbits_frame_L = 166010;
double              Rb_infor_L = 200e9;
double              Rb_L = 256e9;
double              Rb_fix_L = 256e9;
long long           N_fram_clk_L = 5;
long long           N_bits_infor_clk_L = 136530;
double              T_clk_L = 3e-7;
long long           N_bits_infor_total_L = 500000;
long long           N_codewords_total_L = 20;
long long           N_clks_total_L = 4;
double              T_total_L = 1.3e-6;
long long           N_TXbits_clk_L = 166010;
long long           N_TXbits_frame_clk_L = 166010;
long long           N_Txbits_pad_clk_L = 0;
double              fc_ref_fix_L = 190.1e12;
double              lambda_ref_fix_L = 1577.86;
double              chan_spacing_fix_L = 100e9;
double              Rsym_fix_L = 64e9;
long long           N_syms_delim_L = 16;
long long           N_syms_TS_L = 256;
long long           N_TXsyms_frame_L = 20751;
long long           N_TXsyms_clk_L = 20751;
long long           N_TXsyms_frame_clk_L = 20751;
long long           N_TXsyms_pad_clk_L = 0;
long long           N_persymbol_default_L = 4;
double              sample_rate_default_L = 256e9;

// ---- ESignal ----
long long           N_bands = 2;
std::string         ref_band = "band_C";
long long           N_bits_total_ref_band = 1000000;
long long           N_frames_clk_ref_band_per_pol = 10;
std::string         prop_mode = "SSFM";
double              M_deci = 2.0;

ZK::ESignal eSignal;
ZK::Band bandC, bandL;

// ==============================================================
// 构建 Band 和 ESignal 对象
// ==============================================================
void Band_C_create()
{
    bandC = ZK::BandBuilder()
        .setBandIDName(band_Id_c, band_name_c)
        .setChannelGrid(N_chans_c, chan_spacing_c, chan_index_c, n_chan_of_interest_c)
        .setRefChannel(n_chan_fc_ref_c, fc_ref_c)
        .setModulation(pol_mux_mode_c, modulation_format_c, N_pols_c, M_ary_c)
        .setChanCoding(n_fec_c, k_fec_c, code_rate_c)
        .setFrameBits(N_bits_delim_c, N_bits_TS_c, N_codeswords_frame_c, N_TXbits_frame_c)
        .setRates(Rb_infor_c, Rb_c, Rb_fix_c)
        .setClockInfo(N_fram_clk_c, T_clk_c, T_total_c)
        .setTotalCounts(N_codewords_total_c, N_clks_total_c)
        .setInfoBitsCounts(N_bits_infor_clk_c, N_bits_infor_total_c)
        .setTxBitsCounts(N_TXbits_clk_c, N_TXbits_frame_clk_c, N_Txbits_pad_clk_c)
        .setFixedParams(fc_ref_fix_c, lambda_ref_fix_c, N_bits_delim_fix_c, N_bits_TS_fix_c, chan_spacing_fix_c, Rsym_fix_c)
        .setSymbolStructure(N_syms_TS_c, N_TXsyms_frame_c)
        .setSymbolCounts(N_TXsyms_clk_c, N_TXsyms_frame_clk_c, N_TXsyms_pad_clk_c)
        .setSampling(N_persymbol_default_c, sample_rate_default_c)
        .build();
}

void Band_L_create()
{
    bandL = ZK::BandBuilder()
        .setBandIDName(band_Id_L, band_name_L)
        .setChannelGrid(N_chans_L, chan_spacing_L, chan_index_L, n_chan_of_interest_L)
        .setRefChannel(n_chan_fc_ref_L, fc_ref_L)
        .setModulation(pol_mux_mode_L, modulation_format_L, N_pols_L, M_ary_L)
        .setChanCoding(n_fec_L, k_fec_L, code_rate_L)
        .setFrameBits(N_bits_delim_L, N_bits_TS_L, N_codeswords_frame_L, N_TXbits_frame_L)
        .setRates(Rb_infor_L, Rb_L, Rb_fix_L)
        .setClockInfo(N_fram_clk_L, T_clk_L, T_total_L)
        .setTotalCounts(N_codewords_total_L, N_clks_total_L)
        .setInfoBitsCounts(N_bits_infor_clk_L, N_bits_infor_total_L)
        .setTxBitsCounts(N_TXbits_clk_L, N_TXbits_frame_clk_L, N_Txbits_pad_clk_L)
        .setFixedParams(fc_ref_fix_L, lambda_ref_fix_L, N_bits_delim_fix_L, N_bits_TS_fix_L, chan_spacing_fix_L, Rsym_fix_L)
        .setSymbolStructure(N_syms_TS_L, N_TXsyms_frame_L)
        .setSymbolCounts(N_TXsyms_clk_L, N_TXsyms_frame_clk_L, N_TXsyms_pad_clk_L)
        .setSampling(N_persymbol_default_L, sample_rate_default_L)
        .build();
}

void Esignal_create()
{
    eSignal = ZK::ESignalBuilder()
        .setRefBandInfo(ref_band, N_bits_total_ref_band, N_frames_clk_ref_band_per_pol, N_bands)
        .setPropagation(prop_mode, M_deci)
        .addBand(bandC)
        .addBand(bandL)
        .build();
}

void Globalinit()
{
    std::string taskId = "task_001";
    ZK::GlobalValue::registerConfig(taskId, eSignal);
    ZK::GlobalValue::selectESignal(taskId);
}

// ==============================================================
// 辅助函数：打印信号信息（保持简洁）
// ==============================================================
template <typename T>
void printSignalInfo(const std::string& name, const ZK::Vec<T>& vec, int maxPrint = 10)
{
    std::cout << name << " : length = " << vec.length();
    if (vec.length() > 0) {
        std::cout << ", first " << maxPrint << " elements: [";
        for (int i = 0; i < std::min<int>(maxPrint, vec.length()); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << vec(i);
        }
        if (vec.length() > maxPrint) std::cout << " ...";
        std::cout << "]";
    }
    std::cout << std::endl;
}

void printComplexSignalInfo(const std::string& name, const ZK::cvec& vec, int maxPrint = 5)
{
    std::cout << name << " : length = " << vec.length();
    if (vec.length() > 0) {
        std::cout << ", first " << maxPrint << " elements: [";
        for (int i = 0; i < std::min<int>(maxPrint, vec.length()); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << "(" << vec(i).real() << "," << vec(i).imag() << ")";
        }
        if (vec.length() > maxPrint) std::cout << " ...";
        std::cout << "]";
    }
    std::cout << std::endl;
}

// ==============================================================
// 主测试函数：按顺序测试所有 Agent
// ==============================================================
int main()
{
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "   ZK Agents Sequential Test (Tx Chain)" << std::endl;
    std::cout << std::string(60, '=') << std::endl;

    // 初始化全局 Band
    Band_C_create();
    Band_L_create();
    Esignal_create();
    Globalinit();

    const std::string testBandId = "band_L";
    const long long channelIdx = 1;

    const ZK::Band& band = ZK::GlobalValue::getBand(testBandId);
    double T_clk = band.getClockPeriod();
    double Rsym = band.getFixedSymbolRate();
    long long M_ary = band.getModulationOrder();
    int bitsPerSymbol = static_cast<int>(std::log2(M_ary));

    long long expectedSymbols = static_cast<long long>(T_clk * Rsym);
    long long bitsNeeded = expectedSymbols * bitsPerSymbol;

    std::cout << "\nBand parameters:" << std::endl;
    std::cout << std::left << std::setw(25) << "  T_clk:" << T_clk << " s" << std::endl;
    std::cout << std::left << std::setw(25) << "  Rsym:" << Rsym << " Hz" << std::endl;
    std::cout << std::left << std::setw(25) << "  M_ary:" << M_ary << std::endl;
    std::cout << std::left << std::setw(25) << "  bitsPerSymbol:" << bitsPerSymbol << std::endl;
    std::cout << std::left << std::setw(25) << "  Expected symbols:" << expectedSymbols << std::endl;
    std::cout << std::left << std::setw(25) << "  Bits needed:" << bitsNeeded << std::endl;

    // ==========================================================
    // 2. 测试 PRBSlfsr
    // ==========================================================
    std::cout << "\n--- Testing PRBSlfsr ---" << std::endl;
    ZK::PRBSlfsr::Parameters prbsParams;
    prbsParams.bandId = testBandId;
    prbsParams.channelIndex = channelIdx;
    prbsParams.prbsOrder = 15;
    prbsParams.prbsMode = ZK::PRBSlfsr::ITU_STANDARD;
    prbsParams.randomSeed = 12345;
    prbsParams.operateMode = ZK::PRBSlfsr::Reset;
    prbsParams.SPMode = ZK::PRBSlfsr::Data;
    prbsParams.saveFile = "noSave";
    prbsParams.writeMode = ZK::mode::WriteMode::OverWrite;
    prbsParams.dataFormat = ZK::mode::FileType::Txt;
    prbsParams.loadLogicInfo = "noLoad";
    prbsParams.saveLogicInfo = "noSave";

    std::cout << "PRBSlfsr Parameters:" << std::endl;
    std::cout << std::left << std::setw(20) << "  bandId:" << prbsParams.bandId << std::endl;
    std::cout << std::left << std::setw(20) << "  channelIndex:" << prbsParams.channelIndex << std::endl;
    std::cout << std::left << std::setw(20) << "  prbsOrder:" << prbsParams.prbsOrder << std::endl;
    std::cout << std::left << std::setw(20) << "  prbsMode:" << (prbsParams.prbsMode == ZK::PRBSlfsr::ITU_STANDARD ? "ITU_STANDARD" : "?") << std::endl;
    std::cout << std::left << std::setw(20) << "  randomSeed:" << prbsParams.randomSeed << std::endl;
    std::cout << std::left << std::setw(20) << "  operateMode:" << (prbsParams.operateMode == ZK::PRBSlfsr::Reset ? "Reset" : "?") << std::endl;
    std::cout << std::left << std::setw(20) << "  SPMode:" << (prbsParams.SPMode == ZK::PRBSlfsr::Data ? "Data" : "DataBar") << std::endl;
    std::cout << std::left << std::setw(20) << "  saveFile:" << prbsParams.saveFile << std::endl;
    std::cout << std::left << std::setw(20) << "  writeMode:" << (prbsParams.writeMode == ZK::mode::WriteMode::OverWrite ? "OverWrite" : "Append") << std::endl;
    std::cout << std::left << std::setw(20) << "  dataFormat:" << (prbsParams.dataFormat == ZK::mode::FileType::Txt ? "Txt" : "?") << std::endl;
    std::cout << std::left << std::setw(20) << "  loadLogicInfo:" << prbsParams.loadLogicInfo << std::endl;
    std::cout << std::left << std::setw(20) << "  saveLogicInfo:" << prbsParams.saveLogicInfo << std::endl;

    ZK::PRBSlfsr::Signals prbsSignals;
    try {
        ZK::PRBSlfsr::execute(prbsParams, prbsSignals);
        printSignalInfo("PRBS full output eOut", prbsSignals.eOut);
    }
    catch (const std::exception& e) {
        std::cerr << "PRBSlfsr exception: " << e.what() << std::endl;
        return -1;
    }

    if (prbsSignals.eOut.length() < bitsNeeded) {
        std::cerr << "Error: PRBS generated " << prbsSignals.eOut.length() << " bits, need " << bitsNeeded << std::endl;
        return -1;
    }

    ZK::bvec truncatedBits(bitsNeeded);
    for (long long i = 0; i < bitsNeeded; ++i) truncatedBits(i) = prbsSignals.eOut(i);
    std::cout << "Truncated " << bitsNeeded << " bits for MapperMQAM input." << std::endl;

    // ==========================================================
    // 3. 测试 MapperMQAM
    // ==========================================================
    std::cout << "\n--- Testing MapperMQAM ---" << std::endl;
    ZK::MapperMQAM::Parameters mqamParams;
    mqamParams.bandId = testBandId;
    mqamParams.channelIndex = channelIdx;
    mqamParams.mappingRule = ZK::MapperMQAM::PAMGrayCoding;
    mqamParams.MaryMQAM = band.getModulationOrder();
    mqamParams.userTableFileName = "";
    mqamParams.saveFile = "noSave";
    mqamParams.writeMode = ZK::mode::WriteMode::OverWrite;
    mqamParams.dataFormat = ZK::mode::FileType::Txt;

    std::cout << "MapperMQAM Parameters:" << std::endl;
    std::cout << std::left << std::setw(20) << "  bandId:" << mqamParams.bandId << std::endl;
    std::cout << std::left << std::setw(20) << "  channelIndex:" << mqamParams.channelIndex << std::endl;
    std::cout << std::left << std::setw(20) << "  mappingRule:"
        << (mqamParams.mappingRule == ZK::MapperMQAM::PAMGrayCoding ? "PAMGrayCoding" :
            (mqamParams.mappingRule == ZK::MapperMQAM::QAMGrayCoding ? "QAMGrayCoding" :
                (mqamParams.mappingRule == ZK::MapperMQAM::PAMuserTLU ? "PAMuserTLU" :
                    (mqamParams.mappingRule == ZK::MapperMQAM::QAMuserTLU ? "QAMuserTLU" : "Unknown")))) << std::endl;
    std::cout << std::left << std::setw(20) << "  MaryMQAM:" << mqamParams.MaryMQAM << std::endl;
    std::cout << std::left << std::setw(20) << "  userTableFileName:" << mqamParams.userTableFileName << std::endl;
    std::cout << std::left << std::setw(20) << "  saveFile:" << mqamParams.saveFile << std::endl;
    std::cout << std::left << std::setw(20) << "  writeMode:" << (mqamParams.writeMode == ZK::mode::WriteMode::OverWrite ? "OverWrite" : "Append") << std::endl;
    std::cout << std::left << std::setw(20) << "  dataFormat:" << (mqamParams.dataFormat == ZK::mode::FileType::Txt ? "Txt" : "?") << std::endl;

    ZK::MapperMQAM::Signals mqamSignals;
    mqamSignals.eIn = truncatedBits;
    try {
        ZK::MapperMQAM::execute(mqamParams, mqamSignals);
        if (mqamSignals.eOut1.length() != expectedSymbols) {
            std::cerr << "Warning: MapperMQAM output symbols = " << mqamSignals.eOut1.length()
                << ", expected = " << expectedSymbols << std::endl;
        }
        printSignalInfo("I symbols eOut1", mqamSignals.eOut1);
        printSignalInfo("Q symbols eOut2", mqamSignals.eOut2);
    }
    catch (const std::exception& e) {
        std::cerr << "MapperMQAM exception: " << e.what() << std::endl;
        return -1;
    }

    // ==========================================================
    // 4. 测试 NRZCoder
    // ==========================================================
    std::cout << "\n--- Testing NRZCoder (I channel) ---" << std::endl;
    ZK::NRZCoder::Parameters nrzParams;
    nrzParams.bandId = testBandId;
    nrzParams.channelIndex = channelIdx;
    nrzParams.symbolRate = Rsym;
    nrzParams.nPerSymbol = band.getDefaultSamplesPerSymbol();
    nrzParams.reSamplingRatio = 1.0;
    nrzParams.tRise = 0.1 / Rsym;
    nrzParams.saveFile = "noSave";
    nrzParams.writeMode = ZK::mode::WriteMode::OverWrite;
    nrzParams.dataFormat = ZK::mode::FileType::Txt;

    std::cout << "NRZCoder Parameters:" << std::endl;
    std::cout << std::left << std::setw(20) << "  bandId:" << nrzParams.bandId << std::endl;
    std::cout << std::left << std::setw(20) << "  channelIndex:" << nrzParams.channelIndex << std::endl;
    std::cout << std::left << std::setw(20) << "  symbolRate:" << nrzParams.symbolRate << " Hz" << std::endl;
    std::cout << std::left << std::setw(20) << "  nPerSymbol:" << nrzParams.nPerSymbol << std::endl;
    std::cout << std::left << std::setw(20) << "  reSamplingRatio:" << nrzParams.reSamplingRatio << std::endl;
    std::cout << std::left << std::setw(20) << "  tRise:" << nrzParams.tRise << " s" << std::endl;
    std::cout << std::left << std::setw(20) << "  saveFile:" << nrzParams.saveFile << std::endl;
    std::cout << std::left << std::setw(20) << "  writeMode:" << (nrzParams.writeMode == ZK::mode::WriteMode::OverWrite ? "OverWrite" : "Append") << std::endl;
    std::cout << std::left << std::setw(20) << "  dataFormat:" << (nrzParams.dataFormat == ZK::mode::FileType::Txt ? "Txt" : "?") << std::endl;

    ZK::NRZCoder::Signals nrzSignalsI;
    nrzSignalsI.eIn = mqamSignals.eOut1;
    try {
        ZK::NRZCoder::execute(nrzParams, nrzSignalsI);
        printSignalInfo("NRZ I waveform eOut", nrzSignalsI.eOut);
    }
    catch (const std::exception& e) {
        std::cerr << "NRZCoder I exception: " << e.what() << std::endl;
        return -1;
    }

    std::cout << "\n--- Testing NRZCoder (Q channel) ---" << std::endl;
    ZK::NRZCoder::Signals nrzSignalsQ;
    nrzSignalsQ.eIn = mqamSignals.eOut2;
    try {
        ZK::NRZCoder::execute(nrzParams, nrzSignalsQ);
        printSignalInfo("NRZ Q waveform eOut", nrzSignalsQ.eOut);
    }
    catch (const std::exception& e) {
        std::cerr << "NRZCoder Q exception: " << e.what() << std::endl;
        return -1;
    }

    // ==========================================================
    // 5. 测试 IQCombiner
    // ==========================================================
    std::cout << "\n--- Testing IQCombiner ---" << std::endl;
    ZK::IQCombiner::Parameters iqParams;
    iqParams.saveFile = "noSave";
    iqParams.writeMode = ZK::mode::WriteMode::OverWrite;
    iqParams.dataFormat = ZK::mode::FileType::Txt;

    std::cout << "IQCombiner Parameters:" << std::endl;
    std::cout << std::left << std::setw(20) << "  saveFile:" << iqParams.saveFile << std::endl;
    std::cout << std::left << std::setw(20) << "  writeMode:" << (iqParams.writeMode == ZK::mode::WriteMode::OverWrite ? "OverWrite" : "Append") << std::endl;
    std::cout << std::left << std::setw(20) << "  dataFormat:" << (iqParams.dataFormat == ZK::mode::FileType::Txt ? "Txt" : "?") << std::endl;

    ZK::IQCombiner::Signals iqSignals;
    iqSignals.eIn1 = nrzSignalsI.eOut;
    iqSignals.eIn2 = nrzSignalsQ.eOut;
    try {
        ZK::IQCombiner::execute(iqParams, iqSignals);
        printComplexSignalInfo("Combined complex signal eOut", iqSignals.eOut);
    }
    catch (const std::exception& e) {
        std::cerr << "IQCombiner exception: " << e.what() << std::endl;
        return -1;
    }

    // ==========================================================
    // 6. 测试 MeasurementSimple
    // ==========================================================
    std::cout << "\n--- Testing MeasurementSimple (EyeDiagram) ---" << std::endl;
    ZK::MeasurementSimple::Parameters measParams;
    measParams.bandId = testBandId;
    measParams.channelIndex = channelIdx;
    measParams.measMode = ZK::MeasurementSimple::EyeDiagram;
    measParams.initialT = 0.0;
    measParams.reSamplingRatio = 1.0;
    measParams.saveFile = "noSave";
    measParams.writeMode = ZK::mode::WriteMode::OverWrite;
    measParams.dataFormat = ZK::mode::FileType::Txt;

    std::cout << "MeasurementSimple Parameters:" << std::endl;
    std::cout << std::left << std::setw(20) << "  bandId:" << measParams.bandId << std::endl;
    std::cout << std::left << std::setw(20) << "  channelIndex:" << measParams.channelIndex << std::endl;
    std::cout << std::left << std::setw(20) << "  measMode:"
        << (measParams.measMode == ZK::MeasurementSimple::EyeDiagram ? "EyeDiagram" :
            (measParams.measMode == ZK::MeasurementSimple::TimeField ? "TimeField" :
                (measParams.measMode == ZK::MeasurementSimple::TimePower ? "TimePower" :
                    (measParams.measMode == ZK::MeasurementSimple::ConstellationChart ? "ConstellationChart" :
                        (measParams.measMode == ZK::MeasurementSimple::Spectrum ? "Spectrum" : "Unknown"))))) << std::endl;
    std::cout << std::left << std::setw(20) << "  initialT:" << measParams.initialT << " s" << std::endl;
    std::cout << std::left << std::setw(20) << "  reSamplingRatio:" << measParams.reSamplingRatio << std::endl;
    std::cout << std::left << std::setw(20) << "  saveFile:" << measParams.saveFile << std::endl;
    std::cout << std::left << std::setw(20) << "  writeMode:" << (measParams.writeMode == ZK::mode::WriteMode::OverWrite ? "OverWrite" : "Append") << std::endl;
    std::cout << std::left << std::setw(20) << "  dataFormat:" << (measParams.dataFormat == ZK::mode::FileType::Txt ? "Txt" : "?") << std::endl;

    ZK::MeasurementSimple::Signals measSignals;
    measSignals.oIn = iqSignals.eOut;
    try {
        ZK::MeasurementSimple::execute(measParams, measSignals);
        std::cout << "EyeDiagram output:" << std::endl;
        printSignalInfo("oOut1 (time axis)", measSignals.oOut1);
        std::cout << "oOut2 is a matrix of size " << measSignals.oOut2.rows() << " x " << measSignals.oOut2.cols() << std::endl;
        if (measSignals.oOut2.rows() > 0 && measSignals.oOut2.cols() > 0) {
            std::cout << "First row of oOut2: [";
            for (int j = 0; j < std::min<int>(5, measSignals.oOut2.cols()); ++j)
                std::cout << " " << measSignals.oOut2(0, j);
            std::cout << " ...]" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "MeasurementSimple exception: " << e.what() << std::endl;
        return -1;
    }

    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "   All agents executed sequentially." << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    return 0;
}