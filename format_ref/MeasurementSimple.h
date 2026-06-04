/******************************************************************************
 * Copyright (c) 2025 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software is strictly prohibited.
 *****************************************************************************/

 /*
 * @file: MeasurementSimple.h
 * @brief: MeasurementSimple class implements plot data procession
 * @author:Wenbo Zhao
 * @version: 2.0
 * @date: 2025-11-22
 * @department: Product Development Department
 * @project: ZKOE Systems Agents Development
 */

 /*
 * Revision History:
 *
 * Version       Date       Author                   Changes
 * --------   ----------   -------    ------------------------------------------------------------------
 *  1.0      2025-11-21   Wenbo Zhao   build the initial version of MeasurementSimple class according to the "ZKOE公司Agents开发交付要求251003V4p0r1"
 *  2.0      2025-11-22   Wenbo Zhao   build the timeField; eyeDiagram; timePower; constellationChart; spectrum
 */
#pragma once
#include <vec.h> 
#include <fstream>
#include <iomanip>
#include<fft.h>
#include<fstreamIO.h>

namespace ZK
{
    
   
    class MeasurementSimple
    {
    public:

        enum MeasMode {
            EyeDiagram,
            TimeField,
            TimePower,
            ConstellationChart,
            Spectrum
        };


        // Agent参数结构体
        struct Parameters {

            std::string bandId;     //波段ID
            long long channelIndex; //信道索引 

            MeasMode measMode;
            double initialT;
            double reSamplingRatio; // 过采样率
            
            std::string saveFile;    // 保存文件
            mode::WriteMode writeMode;
            mode::FileType dataFormat; // 数据保存格式
        };


        // 输入输出信号结构体
        struct Signals
        {
            // 输入信号
            cvec oIn;
            // 输出信号
            vec oOut1;
            cmat oOut2;      // 
        };

        // 静态执行函数 
        static void execute(const Parameters& params, Signals& signals);
        static void eyeDiagramFun(Signals& signals, int samplesPerSym, double sampleRate);
        static void timeFieldFun(Signals& signals, double initialT, double sampleRate);
        static void timePowerFun(Signals& signals, double initialT, double sampleRate);
        static void constellationChart(Signals& signals, int samplesPerSym);
        static void spectrumFun(Signals& signals, double sampleRate);
    };
} // namespace ZK