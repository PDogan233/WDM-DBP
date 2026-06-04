/******************************************************************************
 * Copyright (c) 2025 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software is strictly prohibited.
 *****************************************************************************/

 /**
  * @file: IQCombiner.h
  * @brief: IQCombiner class implements I/Q signal combination for complex signal generation
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
   *  1.0     2025-11-20    Guo Xuening   Initial version for IQCombiner
   */

#pragma once
#include <vec.h>
#include<fstreamIO.h>
namespace ZK
{
    /*
     * IQCombiner的功能描述：
     *
     *       实现I/Q两路信号的合成功能，将独立的I路和Q路实数信号合成为一路复数信号。
     *       复数信号 = I + j*Q
     */
     // Agent参数结构体
    class IQCombiner
    {
    public:
        struct Parameters
        {
            std::string saveFile;        // 文件保存选项
            mode::WriteMode writeMode;         // 文件保存模式
            mode::FileType dataFormat;   // 数据保存格式
        };

        // 输入输出信号结构体
        struct Signals
        {
            vec eIn1;                  // 输入I路信号
            vec eIn2;                  // 输入Q路信号
            cvec eOut;           // 输出复数信号
        };

        // 静态执行函数
        static void execute(const Parameters& params, Signals& signals);
    };

} //end namespace ZK