/******************************************************************************
 * Copyright (c) 2025 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software is strictly prohibited.
 *****************************************************************************/

 /**
  * @file: IQCombiner.cpp
  * @brief: Implementation of IQCombiner class
  * @author: Guo Xuening
  * @version: 1.0
  * @date: 2025-11-20
  * @department: Product Development Department
  * @project: ZKOE Agents Development
  */

  /**
   * Revision History:
   *
   * Version       Date       Author                   Changes
   * --------   ----------   -------    ------------------------------------------------------------------
   *  1.0     2025-11-20   Guo Xuening   Initial version for IQCombiner
   */

#include "IQCombiner.h"
#include <stdexcept>
#include <algorithm>


namespace ZK
{
    /*
     * 静态成员函数名称： static void execute(Signals& signals);
     * 静态成员函数的输入：
     *
     *            signals  // 类型 IQCombiner::Signals，包含输入输出信号（输入I路信号eIn1、
     *                     // 输入Q路信号eIn2、输出复数信号eOut）
     *
     * 静态成员函数的输出： void
     * 静态成员函数实现的功能描述： IQ合成器的主要执行函数。验证输入信号的有效性，
     *                              然后执行I/Q信号合成，生成复数信号
     */
    void IQCombiner::execute(const Parameters& params,Signals& signals)
    {
        // 检查输入信号
        if (signals.eIn1.length() == 0 || signals.eIn2.length() == 0) {
            signals.eOut.set_size(0);
            return;
        }

        // 检查信号长度一致性
        int iLength = signals.eIn1.length();
        int qLength = signals.eIn2.length();

        if (iLength != qLength) {
            throw std::invalid_argument("IQ synthesizer signal error: The lengths of the I-channel and Q-channel signals are inconsistent");
        }

        // 执行I/Q信号合成
        int signalLength = iLength;
        signals.eOut.set_size(signalLength);

        // 复数信号合成：complex = I + j*Q
        for (int i = 0; i < signalLength; i++) {
            signals.eOut(i) = std::complex<double>(signals.eIn1(i), signals.eIn2(i));
        }

        if (params.saveFile != "" && params.saveFile != "noSave") 
        {
            saveData(signals.eOut,params.saveFile, params.dataFormat,params.writeMode);
        }


    }

} //end namespace ZK