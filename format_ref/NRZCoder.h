/******************************************************************************
 * Copyright (c) 2025 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software is strictly prohibited.
 *****************************************************************************/

 /**
  * @file: NRZCoder.h
  * @brief: NRZCoder class implements NRZ pulse shaping for M-QAM signals
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
   *  1.0     2025-11-18    Guo Xuening   Initial version for NRZCoder
   *  1.1     2025-11-20    Guo Xuening   Added file save functionality and WriteMode enum;
   *                                      Changed from dual-path (I/Q) to single-path processing;
   */

#pragma once
#include <vec.h>
#include<fstreamIO.h>

namespace ZK
{
    /*
     * NRZCoder的功能描述：
     *
     *       实现MQAM信号的NRZ脉冲成形功能，将符号序列转换为NRZ波形。
     *       基于pulsetrain.pdf文档中的NRZ信号生成流程，包括：
     *       1. 生成梯形NRZ脉冲形状
     *       2. 通过卷积生成脉冲序列
     *       （当前版本为单路处理）
     */
    class NRZCoder
    {
    public:
        // Agent参数结构体
        struct Parameters
        {
			std::string bandId;        // 波段ID
            long long channelIndex;    //信道索引

            double tRise;              // 上升时间
            double symbolRate;         // 符号率
            double nPerSymbol;         // 每符号采样点数
			double reSamplingRatio;    // 过采样率

            std::string saveFile;      // 文件保存路径
            mode::WriteMode writeMode; // 文件保存模式
            mode::FileType dataFormat; // 数据保存格式
        };

        // 输入输出信号结构体
        struct Signals
        {
            vec eIn;                   // 输入符号序列
            vec eOut;                  // 输出NRZ波形
        };

        // 静态执行函数
        static void execute(const Parameters& params, Signals& signals);

        // 参数验证
        static void validateParameters(const Parameters& params, const Signals& signals);

        // NRZ脉冲形状生成
        static void generatePulseShape(const Parameters& params, vec& pulseShapeData);

        // 脉冲序列生成
        static void generatePulseTrain(const vec& pulseShapeData, int nSymbols, int nSamplePerSym, const vec& symbols, vec& pulseTrain);

        // 信号处理
        static void processSignal(const Parameters& params, const vec& input_symbols, vec& pulseTrain);
    };

} //end namespace ZK