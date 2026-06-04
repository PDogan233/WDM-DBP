/******************************************************************************
 * Copyright (c) 2025 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software is strictly prohibited.
 *****************************************************************************/

 /**
  * @file: NRZCoder.cpp
  * @brief: Implementation of NRZCoder class
  * @author: Guo Xuening
  * @version: 1.1
  * @date: 2025-11-18
  * @department: Product Development Department
  * @project: ZKOE Agents Development
  */

  /**
   * Revision History:
   *
   * Version       Date       Author                   Changes
   * --------   ----------   -------    ------------------------------------------------------------------
   *  1.0     2025-11-18   Guo Xuening   Initial version for NRZCoder
   *  1.1     2025-11-20   Guo Xuening   Enhanced parameter validation and file handling support;
   *                                      Changed from dual-path to single-path processing;
   */

#include "NRZCoder.h"
#include <stdexcept> // C++标准库中的异常处理头文件，提供了标准异常类,如std::invalid_argument，用于在参数验证时抛出异常。
#include <cmath> // C++数学库头文件，提供了常用的数学函数，例如std::ceil（向上取整）、std::fmod（浮点数取余）等，在生成脉冲形状时使用。
#include<GlobalValue.h>

namespace ZK
{
    //仿真设置

    /*
     * 静态成员函数名称： static void execute(const Parameters& params, Signals& signals);
     * 静态成员函数的输入：
     *
     *            params   // 类型 NRZCoder::Parameters，包含NRZ编码器参数（上升时间tRise、
     *                     // 符号周期tSymbol、每符号采样点数nSamplePerSym、文件保存路径saveFile、
     *                     // 文件写入模式writemode等）
     *            signals  // 类型 NRZCoder::Signals，包含输入输出信号（输入符号序列eIn、
     *                     // 输出NRZ波形eOut）
     *
     * 静态成员函数的输出： void
     * 静态成员函数实现的功能描述： NRZ编码器的主要执行函数。首先调用validateParameters验证输入参数的有效性，
     *                              检查输入符号是否为空，如为空则设置输出为空并返回，否则调用processSignal
     *                              处理单路信号，完成NRZ脉冲成形的完整处理流程。支持文件输出功能。
     */
    void NRZCoder::execute(const Parameters& params, Signals& signals)
    {

        // simulation setup for reference
        //long long NSamples = signals.eIn.length() * params.reSamplingRatio; //reSamplingRate is versus incoming data's sampling rate
        //double T = GlobalValue::getBand(params.bandId).getClockPeriod();
        //double Ts = T / NSamples; 
        //double F = 1 / Ts; 
        //double Fs = 1 / T; 
        //vec t = "[0: Ts: T - Ts]";      //time bins
        //vec f = "[-F/2: Fs: F/2 - Fs]"; // frequency bins

        // 验证参数
        validateParameters(params,signals);

        // 检查输入信号：验证输入信号是否为空
        if (signals.eIn.length() == 0) {
            signals.eOut.set_size(0);
            return;
        }

        // 处理单路信号
        processSignal(params, signals.eIn, signals.eOut);
        if (params.saveFile != "" && params.saveFile != "noSave")
        {
            saveData(signals.eOut, params.saveFile, params.dataFormat, params.writeMode);
        }
    }

    /*
    * 静态成员函数名称： static void validateParameters(const Parameters& params);
    * 静态成员函数的输入：
    *
    *            params   // 类型 NRZCoder::Parameters，包含NRZ编码器参数（上升时间tRise、
    *                     // 符号周期tSymbol、每符号采样点数nSamplePerSym、文件保存路径saveFile等）
    *
    * 静态成员函数的输出： void
    * 静态成员函数实现的功能描述： 验证NRZ编码器输入参数的合法性。检查上升时间必须大于0以确保梯形脉冲有合理的上升沿，
    *                              符号周期必须大于0以确保时间基准有效，每符号采样点数必须大于0以确保足够的时域分辨率，
    *                              文件路径如果非空则验证其有效性。如参数无效则抛出std::invalid_argument异常，
    *                              确保编码器在有效参数下运行。
    */
    void NRZCoder::validateParameters(const Parameters& params,const Signals& signals)
    {
        // 验证参数的有效性
        // 上升时间必须为正数
        if (params.tRise <= 0) {
            throw std::invalid_argument("NRZ encoder parameter error: tRise must be greater than 0");
        }
        // 符号率必须为正数
        if (params.symbolRate <= 0) {
            throw std::invalid_argument("NRZ encoder parameter error: symbolRate must be greater than 0");
        }
        // 每符号采样点数必须为正数
        if (params.nPerSymbol <= 0) {
            throw std::invalid_argument("NRZ encoder parameter error: nPerSymbol must be greater than 0");
        }
        if (signals.eIn.length() != (GlobalValue::getBand(params.bandId).getClockPeriod() * params.symbolRate))
        {
			throw std::invalid_argument("NRZ encoder input data length error: The length of input symbols does not match the expected number based on symbolRate and clock period");
        }
    }

    /*
     * 静态成员函数名称： static void generatePulseShape(const Parameters& params, vec& pulseShapeData);
     * 静态成员函数的输入：
     *
     *            params           // 类型 NRZCoder::Parameters，包含NRZ编码器参数（上升时间tRise、
     *                             // 符号周期tSymbol、每符号采样点数nSamplePerSym等）
     *            pulseShapeData   // 类型 vec&，生成的脉冲形状数据的引用输出
     *
     * 静态成员函数的输出： void
     * 静态成员函数实现的功能描述： 生成梯形NRZ脉冲形状。首先计算上升沿采样点数n1，然后生成包含上升沿
     *                              、平坦部分和下降沿的完整脉冲形状：上升沿从0线性增加到1，
     *                              平坦部分保持为1，下降沿从1线性下降到0，形成对称的梯形脉冲响应，
     *                              用于后续的卷积运算以生成连续的NRZ波形。
     */
    void NRZCoder::generatePulseShape(const Parameters& params, vec& pulseShapeData)
    {
        long long nSamplePerSym = params.nPerSymbol * params.reSamplingRatio;
	    double tSymbol = 1 / params.symbolRate;
        // 计算上升沿采样点数：tRise/tSymbol：上升时间占符号周期的比例，× nSamplePerSym转换为采样点数
        int n1 = static_cast<int>((std::ceil(params.tRise / tSymbol * nSamplePerSym) -
            std::fmod(std::ceil(params.tRise / tSymbol * nSamplePerSym), 2)) / 2);
        // 生成梯形脉冲形状
        //总长度 = 上升沿长度 + 平坦部分长度 + 下降沿长度 =  2 * n1 + (nSamplePerSym - 2 * n1) + 2 * n1 =  nSamplePerSym + 2 * n1
        int total_length = 2 * n1 + nSamplePerSym;
        pulseShapeData.set_size(total_length);

        // 上升沿
        for (int i = 0; i < 2 * n1; i++) {
            pulseShapeData(i) = static_cast<double>(i) / (2 * n1);
        }

        // 平坦部分
        for (int i = 2 * n1; i < nSamplePerSym; i++) {
            pulseShapeData(i) = 1.0;
        }

        // 下降沿
        for (int i = 0; i < 2 * n1; i++) {
            pulseShapeData(nSamplePerSym + i) = static_cast<double>(2 * n1 - i) / (2 * n1);
        }
    }

    /*
     * 静态成员函数名称： static void generatePulseTrain(const vec& pulseShapeData, int nSymbols, int nSamplePerSym, const vec& symbols, vec& pulseTrain);
     * 静态成员函数的输入：
     *
     *            pulseShapeData  // 类型 vec，脉冲形状数据
     *            nSymbols        // 类型 int，符号总数
     *            nSamplePerSym   // 类型 int，每符号采样点数
     *            symbols         // 类型 vec，输入符号值序列（用于生成冲击序列）
     *            pulseTrain      // 类型 vec&，生成的脉冲序列的引用输出
     *
     * 静态成员函数的输出： void
     * 静态成员函数实现的功能描述： 通过卷积运算生成NRZ脉冲序列。首先创建冲击序列，
     *                              在符号起始位置放置对应的符号值，然后通过线性卷积将脉冲形状与冲击序列卷积，
     *                              生成完整的脉冲序列，最后将卷积超出的部分循环叠加到序列开头以模拟周期性边界条件，
     *                              实现从离散符号到连续波形的转换。
     */
    void NRZCoder::generatePulseTrain(const vec& pulseShapeData, int nSymbols, int nSamplePerSym, const vec& symbols, vec& pulseTrain)
    {
        //  计算总采样点数
        int nSamples = nSamplePerSym * nSymbols;

        // 创建冲击序列
        vec h(nSamples, 0.0);
        // 在每个符号的起始位置（即每隔 nSamplePerSym 个采样点）放置对应的符号值symbols(i)
        for (int i = 0; i < nSymbols; i++) {
            h(i * nSamplePerSym) = symbols(i);// 若有2个符号，每个符号4个采样点，那么冲击序列 h 为：[ symbol0, 0, 0, 0, symbol1, 0, 0, 0 ]
        }

        // 卷积计算
        int conv_length = pulseShapeData.length() + h.length() - 1;
        // 初始化一个长度为 conv_length 的向量 pulse_train1，用于存放卷积结果。
        vec pulse_train1(conv_length, 0.0);

        //  将脉冲形状在时间轴上移动，并在每个冲击序列非零位置（即符号起始位置）乘以符号值，然后叠加。
        for (int i = 0; i < pulseShapeData.length(); i++) {
            for (int j = 0; j < h.length(); j+=nSamplePerSym) {
                pulse_train1(i + j) += pulseShapeData(i) * h(j); //y[n] = Σ x[k] × h[n - k]
            }
        }

        // 取前nSamples个点作为脉冲序列
        pulseTrain = pulse_train1(0, nSamples - 1);

        // 将剩余部分加到前面（模拟周期性）
        int remainingLength = conv_length - nSamples;
        // 由于卷积结果的长度（conv_length）可能大于 nSamples，我们将卷积结果中超出 nSamples 的部分（即剩余部分）加到脉冲序列的开头。
        if (remainingLength > 0) {
            vec remaining_part = pulse_train1(nSamples, nSamples + remainingLength - 1);
            for (int i = 0; i < std::min<int>(remainingLength, nSamples); i++) {
                pulseTrain(i) += remaining_part(i);
            }
        }
    }

    /*
    * 静态成员函数名称： static void processSignal(const Parameters& params, const vec& input_symbols, vec& pulseTrain);
    * 静态成员函数的输入：
    *
    *            params        // 类型 NRZCoder::Parameters，包含NRZ编码器参数（上升时间tRise、
    *                          // 符号周期tSymbol、每符号采样点数nSamplePerSym、文件保存选项等）
    *            input_symbols // 类型 vec，输入符号序列
    *            pulseTrain    // 类型 vec&，生成的脉冲序列的引用输出
    *
    * 静态成员函数的输出： void
    * 静态成员函数实现的功能描述： 处理单路信号的NRZ脉冲成形过程。首先计算输入符号总数，然后调用generatePulseShape
    *                              生成梯形NRZ脉冲形状，最后调用generatePulseTrain通过卷积运算生成连续的脉冲序列，
    *                              完成从离散符号到连续NRZ波形的转换，为后续处理提供基带信号。
    *                              支持文件输出功能，可根据参数设置保存结果。
    */
    void NRZCoder::processSignal(const Parameters& params, const vec& input_symbols, vec& pulseTrain)
    {
        int nSymbols = input_symbols.length();

        // 1. 生成NRZ脉冲形状
        vec pulseShapeData;
        generatePulseShape(params, pulseShapeData);

		long long nSamplePerSym = GlobalValue::getBand(params.bandId).getDefaultSamplesPerSymbol() * params.reSamplingRatio;

        // 2. 生成脉冲序列
        generatePulseTrain(pulseShapeData, nSymbols, nSamplePerSym, input_symbols, pulseTrain);
    }

} //end namespace ZK