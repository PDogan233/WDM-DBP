/******************************************************************************
 * Copyright (c) 2025 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software is strictly prohibited.
 *****************************************************************************/

 /**
  * @file: MapperMQAM.h
  * @brief: MapperMQAM class implements the modulation function of the M-QAM (Quadrature Amplitude Modulation) signal.
  * @author: Guo Xuening
  * @version: 1.0
  * @date: 2025-11-13
  * @department: Product Development Department
  * @project: ZKOE Agents Development
  */

  /**
   * Revision History:
   *
   * Version       Date       Author                   Changes
   * --------   ----------   -------    ------------------------------------------------------------------
   *  1.0     2025-11-13    Guo Xuening   Initial version for MapperMQAM
   *  1.1     2025-11-19    Guo Xuening   Replace bitsPerSymbol with MaryQAM in the Parameters struct.
   */

#pragma once
#include "commfunc.h"    // 通用功能函数头文件
#include<fstreamIO.h>
#include <map>
namespace ZK
{


    /*
     * MapperMQAM的功能描述：
     *
     *       实现MQAM（多进制正交幅度调制）信号的调制功能，将比特流转换为I/Q两路正交符号。
     *       基于Coder_mQAM文档的核心参数，专注于bit到symbol的转换，不考虑波形形成部分。
     *       支持4-QAM, 16-QAM, 64-QAM, 256-QAM, 1024-QAM, 4096-QAM等高阶调制方式。
     *
     * MapperMQAM类具体实例执行后的输出：
     *
     *       根据输入的Parameters和Signals，进行MQAM调制后输出I路和Q路两路符号向量。
     */
    class MapperMQAM
    {
    public:
        enum MappingRule
        {
            PAMGrayCoding,
            QAMGrayCoding,
            PAMuserTLU,
            QAMuserTLU
        };
        // Agent参数结构体
        struct Parameters
        {
            std::string bandId;          // 波段ID
            long long channelIndex;      // 信道索引

            long long MaryMQAM;                  // M-QAM调制阶数
            MappingRule mappingRule;             // Gray编码选项
            std::string userTableFileName;		 // 配置文件路径

            std::string saveFile;                // 文件保存选项
            mode::WriteMode writeMode;                 // 文件保存模式
            mode::FileType dataFormat;           // 数据保存格式
        };

        // 输入输出信号结构体
        struct Signals
        {
            bvec eIn;      // 输入比特流
            vec eOut1;     // 输出I路符号(同相分量)
            vec eOut2;     // 输出Q路符号(正交分量)
        };

        // 静态执行函数
        static void execute(const Parameters& params, Signals& signals);
        static void calculateBitsPerSymbol(int MaryQAM, int& bitsPerSymbol);

    private:
        // 私有辅助函数
        static std::map<std::string, double> mappingTable;
        static void initialize(const Parameters& params);
        static void loadConfigFile(const Parameters& params);
        static void mapBitsToSymbols(const Parameters& params, Signals& signals);
        static void calculateSymbolValue(const bvec& bits, int start, int length, int& symbolValue);
        static void modulate(const Parameters& params, Signals& signals);
    };

} //end namespace ZK