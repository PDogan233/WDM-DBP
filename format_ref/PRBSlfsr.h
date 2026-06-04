#pragma once
/******************************************************************************
 * Copyright (c) 2025 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software is strictly prohibited.
 *****************************************************************************/

 /*
 * @file: PRBSlfsr.h
 * @brief: PRBSlfsr template class implements the full PRBSlfsr functionality
 * @author:Guo Xuening
 * @version: 1.1
 * @date: 2025-10-22
 * @department: Product Development Department
 * @project: ZKOE Systems Agents Development
 */

 /*
 * Revision History:
 *
 * Version       Date       Author                   Changes
 * --------   ----------   -------    ------------------------------------------------------------------
 *  1.1     2025-10-22  Guo Xuening   Modify the format of the Agents according to the "ZKOE公司Agents开发交付要求251003V4p0r1"
 */

//问题与任务汇总：
/*2025.10.24
* 问题：
* 1.去除多余的头文件
* 2.不允许在子类使用类模板函数，要求指定类型
* 3.需将程序分为.h与.cpp文件
* 4.将类名由PRBS修改为PRBSxy
* 5.去除setUpDown等多余的变量
* 6.PRBS阶数增加至31
* 7.此功能不需要多输出，检查修改
* 8.添加随机种子和自定义种子的选择
* 任务:
* 1.具体看一下PRBS原理，做一份ppt
* 2.按照要求修改程序
* 3.一块决定信号和参数设计
* 

* 2025.10.29
* 问题：
* 1.将类名由PRBSxy修改为PRBSlfsr
* * 任务;
* 1.PRBS时钟触发，作为输入（question）
* 2.后续需与赵炆博讨论参数命名
* 3.去除了多余的函数sendSiganl等不属于PRBS的函数
* 4.文件保存后续应该放在一块
* 5.prbsMethod修改为PRBSPolynMode
* 6.customPoly修改为polyCustom
* 7.set函数修改为checkParam函数
*

* 2025.11.4
* 问题：
* 1.有关参数NBitsPerClock和prbsLength参数定义命名的问题，向炆博雪凝确认是否修改
* 任务：
* 1.OperateMode有关截断的问题
* 2.加入WriteMode参数
* 3.在Advanced中有关种子信息的保存，使用参数LoadLogicInfo, SPMode, SaveLogicInfo
* 4.prbsMethod修改为prbsMode
* 5.customSeed修改为randomSeed
*/
#include <vec.h> 
#include <fstream>  //  文件流操作头文件
#include <iomanip>  //  输入输出格式化头文件（固定小数点格式、设置小数位数、设置填充字符、字段宽度等等）
#include <map>      //  关联容器头文件（用于随机数多项式）
#include <chrono>
#include <queue>    //  处理种子序列头文件
#include <algorithm>
#include <fstreamIO.h>

#define DEFAULT_SEED (0)  //默认种子

namespace ZK
{


    /*,
     * PRBSlfsr的功能描述：
     *
     *       实现PRBS生成伪随机比特序列功能，根据输入的Parameters（如PRBS阶数、 序列长度、生成方法等）生成PRBS序列
     *
     * PRBSlfsr类具体实例执行后的输出：
     *
     *       根据输入的Parameters和Signals，输出生成的PRBS序列signals.eOut。
     */
    class PRBSlfsr
    {
    public:

        // PRBS生成方法枚举
        enum PRBS {
            ITU_STANDARD,   //!< ITU-T O.150标准
        };

        // 截断模式枚举
        enum OperateMode {
            Reset           // 非继续，而是Reset即PRBS重新开始
        };

        // 数据输出模式枚举
        enum SPMode {
            Data,           // 直接数据模式，直接输出信号
            DataBar,        // 反码数据模式，输出按位取反后的数据
        };

        // Agent参数结构体
        struct Parameters //修改：去除多余的变量setUpDown，signalLength，并添加了customSeed，可以外界设置种子也可以使用随机种子
                          //修改：添加了参数operateMode，用于设置PRBS序列和clock长度的截断模式，将prbsLength修改为NBitsPerClock，customSeed修改为randomSeed,
                          //添加了保存文件的writeMode, 保存和读取配置文件（现阶段仅为种子文件）的loadLogicInfo, SPMode, saveLogicInfo，以及循环右移时的nshift
        {
			std::string bandId;          // 波段ID
			long long channelIndex;      // 信道索引

            long long prbsOrder;      // PRBS阶数 (2-31) 
            PRBS prbsMode;            // PRBS生成方法 
            long long randomSeed;     //自定义种子
            OperateMode operateMode;  // PRBS序列与clock长度的截断模式
            std::string saveFile;     // 保存文件
            mode::WriteMode writeMode;// 写入模式
            std::string loadLogicInfo;// 读取配置文件
            SPMode SPMode;            // 数据输出模式
            std::string saveLogicInfo;// 保存配置文件
            mode::FileType dataFormat;// 数据保存格式
        };

        // 输入输出信号结构体
		struct Signals        
        {
            // 输出信号：生成的PRBS序列
            bvec eOut;     
        };

        // 静态执行函数  
        static void execute(const Parameters& params, Signals& signals);
        static void checkParam(const Parameters& params);
        static void generatePRBS(const Parameters& params, bvec& prbsOut,long long length);
    };

} // namespace ZK