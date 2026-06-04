/******************************************************************************
 * Copyright (c) 2025 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software is strictly prohibited.
 *****************************************************************************/

 /*
 * @file: MeasurementSimple.cpp
 * @brief: MeasurementSimple class implements ideal power gain for complex optical field
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
 *  1.0      2025-11-06   Wenbo Zhao   build the initial version of MeasurementSimple class according to the "ZKOE公司Agents开发交付要求251003V4p0r1"
 *  2.0      2025-11-22   Wenbo Zhao   build the timeField; eyeDiagram; timePower; constellationChart; spectrum
 */


#include"MeasurementSimple.h"
#include<GlobalValue.h>

namespace ZK
{
    /*
     * 静态成员函数名称： static void execute(const Parameters& params, Signals& signals);
     * 静态成员函数的输入：
     *
     *            params   // 类型 MeasurementSimple::Parameters，包含MeasurementSimple参数（powerGain）
     *            signals  // 类型 MeasurementSimple::Signals，包含输入输出信号（输入光信号oIn、放大后光信号oOut）
     *
     * 静态成员函数的输出： void
     * 静态成员函数实现的功能描述： 对输入光信号做理想的放大
     */
    void MeasurementSimple::execute(const Parameters& params, Signals& signals)
    {

        long long samplesPerSymbol = GlobalValue::getBand(params.bandId).getDefaultSamplesPerSymbol()*params.reSamplingRatio;
        double samplesRate = GlobalValue::getBand(params.bandId).getDefaultSampleRate()*params.reSamplingRatio;

        switch (params.measMode) {
        case MeasMode::EyeDiagram:
            eyeDiagramFun(signals, samplesPerSymbol, samplesRate);
            break;

        case MeasMode::TimeField:
            timeFieldFun(signals, params.initialT, samplesRate);
            break;
            
        case MeasMode::TimePower:
            timePowerFun(signals, params.initialT, samplesRate);
            break;

        case MeasMode::Spectrum:
            spectrumFun(signals, samplesRate);
            break;

        case MeasMode::ConstellationChart:
            constellationChart(signals, samplesPerSymbol);
            break;
        }

        if (params.saveFile != "" && params.saveFile != "noSave") 
        {
            saveData(signals.oOut1, params.saveFile + "_out1", params.dataFormat, params.writeMode);
            saveData(signals.oOut2, params.saveFile + "_out2", params.dataFormat, params.writeMode);
        }

    }

    void MeasurementSimple::eyeDiagramFun(Signals& signals, int samplesPerSym, double sampleRate)
    {
        double deltaT = 1 / sampleRate;
        std::cout << "deltaT" << deltaT << std::endl;
        std::cout<<"sampleRate"<< sampleRate <<std::endl;
        int num_rows = signals.oIn.length() / samplesPerSym;
        int num_cols = samplesPerSym;
        signals.oOut1.set_length(num_cols);
        signals.oOut2.set_size(num_rows, num_cols);
        for (int i = 0; i < num_cols; i++) {
            signals.oOut1[i] = i * deltaT;
        }
        // 然后，通过循环将输入向量的数据按列填充到矩阵中
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                signals.oOut2(i, j) = signals.oIn(i * num_cols + j);
            }
        }
    }
    
    void MeasurementSimple::timeFieldFun(Signals& signals, double initialT, double sampleRate)
    {
        double deltaT = 1 / sampleRate;
        int num_rows = 1;
        int num_cols = signals.oIn.length();

        signals.oOut1.set_length(num_cols);
        signals.oOut2.set_size(num_rows, num_cols);
        for (int i = 0; i < num_cols; i++) {
            signals.oOut1[i] = initialT + i * deltaT;
        }
        // 然后，通过循环将输入向量的数据按列填充到矩阵中
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                signals.oOut2(i, j) = signals.oIn(i * num_cols + j);
            }
        }
    }

    void MeasurementSimple::spectrumFun(Signals& signals, double sampleRate)
    {
        
        int num_rows = 1;
        int num_cols = signals.oIn.length();
       
        double deltaF = sampleRate / num_cols;
        signals.oOut1.set_length(num_cols);
        

    
        for (int i = 0; i < num_cols; i++) {
            signals.oOut1(i) = (i- num_cols/2) * deltaF;
        }
       
        signals.oOut2.set_size(num_rows, num_cols);
        vec powerFre = fftPowerSpectrum(signals.oIn);
        vec powerShifted;
        powerShifted.set_length(num_cols);
        int shift_amount = num_cols / 2;
        for (int i = 0; i < num_cols; i++) {
            int new_index = (i + shift_amount) % num_cols;
            powerShifted(i) = powerFre(new_index);
        }
        // 然后，通过循环将输入向量的数据按列填充到矩阵中
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                signals.oOut2(i, j) = powerShifted(i * num_cols + j);
            }
        }
    }

    void MeasurementSimple::timePowerFun(Signals& signals, double initialT, double sampleRate)
    {
        double deltaT = 1 / sampleRate;
        int num_rows = 1;
        int num_cols = signals.oIn.length();

        signals.oOut1.set_length(num_cols);
        signals.oOut2.set_size(num_rows, num_cols);
        for (int i = 0; i < num_cols; i++) {
            signals.oOut1[i] = initialT + i * deltaT;
        }
        // 然后，通过循环将输入向量的数据按列填充到矩阵中
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                signals.oOut2(i, j) = std::norm(signals.oIn(i * num_cols + j));
            }
        }
    }
    void MeasurementSimple::constellationChart(Signals& signals, int samplesPerSym)
    {
        int num_rows = 1;
        int num_cols = signals.oIn.length() / samplesPerSym;
        signals.oOut1.set_length(0);   //set 0，用不到横轴
        signals.oOut2.set_size(num_rows, num_cols);
        // 然后，通过循环将输入向量的数据按列填充到矩阵中
        for (int i = 0; i < num_rows; i++) {
            for (int j = 0; j < num_cols; j++) {
                signals.oOut2(i, j) = signals.oIn(i * num_cols + j* samplesPerSym+ samplesPerSym/2);
            }
        }
    }
}//end namespace ZK