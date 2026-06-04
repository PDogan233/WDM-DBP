/******************************************************************************
 * Copyright (c) 2025 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software is strictly prohibited.
 *****************************************************************************/

 /**
  * @file: MapperMQAM.cpp
  * @brief: Implementation of MapperMQAM class
  * @author:  Guo Xuening
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
   *  1.0     2025-11-13   Guo Xuening   Initial version for MapperMQAM
   *  1.1     2025-11-19   Guo Xuening   Replace bitsPerSymbol with MaryQAM in the Parameters struct.
   */

#include "MapperMQAM.h"
#include <stdexcept>// C++标准异常处理头文件,包含的异常类：std::invalid_argument, std::runtime_error
#include <cmath>// C++数学函数库头文件,包含std::pow(幂运算)等数学运算函数
#include <GlobalValue.h>


namespace ZK
{
    namespace simu_paras
    {
        int Mary;
    }

    std::map<std::string, double> MapperMQAM::mappingTable;

    void MapperMQAM::initialize(const Parameters& params)
    {
        simu_paras::Mary = GlobalValue::getBand(params.bandId).getModulationOrder();
    }

    /*
     * 静态成员函数名称： static void execute(const Parameters& params, Signals& signals);
     * 静态成员函数的输入：
     *
     *            params   // 类型 MapperMQAM::Parameters，包含MQAM编码器参数（调制阶数MaryQAM、
     *                     // 文件保存选项saveFile等）
     *            signals  // 类型 MapperMQAM::Signals，包含输入输出信号（输入比特流eIn、
     *                     // 输出I路符号eOut1、输出Q路符号eOut2）
     *
     * 静态成员函数的输出： void
     * 静态成员函数实现的功能描述： MQAM编码器的主要执行函数。
     *                              首先，检查输入比特流是否为空，如为空则设置输出为空并返回，否则调用mapBitsToSymbols
     *                              执行比特到符号的映射转换，最后根据saveFile参数决定是否保存输出结果，
     *                              完成MQAM编码的完整处理流程
     */
    void MapperMQAM::execute(const Parameters& params, Signals& signals)
    {
        if (simu_paras::Mary < 2)
        {
            initialize(params);
        }
        else
        {
            simu_paras::Mary = params.MaryMQAM;
        }

        // 检查输入信号
        if (signals.eIn.length() == 0) {
            signals.eOut1.set_size(0);
            signals.eOut2.set_size(0);
            return;
        }

        // 执行MQAM调制映射
        if (params.mappingRule == PAMGrayCoding)
        {
            mapBitsToSymbols(params, signals);
        }
        else
        {
            loadConfigFile(params);
            modulate(params, signals);
        }

        // 如果需要保存文件，这里可以添加文件保存逻辑
        if (params.saveFile != "noSave" && params.saveFile != "") {
            saveData(signals.eOut1, params.saveFile + "_out1", params.dataFormat, params.writeMode);
            saveData(signals.eOut1, params.saveFile + "_out2", params.dataFormat, params.writeMode);
        }
    }

    /*
     * 静态成员函数名称： static void calculateBitsPerSymbol(int MaryQAM, int& bitsPerSymbol);
     * 静态成员函数的输入：
     *
     *            MaryQAM        // 类型 int，QAM调制阶数
     *            bitsPerSymbol  // 类型 int&，计算结果每符号比特数的引用输出
     *
     * 静态成员函数的输出： void
     * 静态成员函数实现的功能描述： 根据调制阶数计算每符号比特数。通过计算以2为底的对数得到bitsPerSymbol，
     *                              并验证是否为整数。如果不是有效的2的幂次方，将bitsPerSymbol设置为-1
     */
    void MapperMQAM::calculateBitsPerSymbol(int MaryQAM, int& bitsPerSymbol)
    {
        double log2M = std::log2(MaryQAM);
        bitsPerSymbol = static_cast<int>(log2M);

        // 检查是否为整数
        if (std::abs(log2M - bitsPerSymbol) > 1e-10) {
            bitsPerSymbol = -1; // 不是2的幂次方
        }
    }

    /*
     * 静态成员函数名称： static void calculateSymbolValue(const bvec& bits, int start, int length, int& symbolValue);
     * 静态成员函数的输入：
     *
     *            bits         // 类型 bvec，输入比特序列
     *            start        // 类型 int，比特序列中计算的起始位置
     *            length       // 类型 int，需要计算的比特数
     *            symbolValue  // 类型 int&，计算结果符号值的引用输出
     *
     * 静态成员函数的输出： void
     * 静态成员函数实现的功能描述： 根据Coder_mQAM文档中的公式计算MQAM符号值。对指定长度的比特序列，
     *                              按照公式 A_j = sum_{k=0}^{m/2-1} [2^{(m/2 - (k+1))} × (2×I_k - 1)] 进行计算，
     *                              其中m为每路比特数，I_k为第k个比特值，计算结果通过symbolValue引用参数返回
     */
    void MapperMQAM::calculateSymbolValue(const bvec& bits, int start, int length, int& symbolValue)
    {
        // 根据文档中的公式计算符号值
        // A_j = sum_{k=0}^{m/2-1} [2^{(m/2 - (k+1))} × (2×I_k - 1)]
        symbolValue = 0;
        int m = length;  // 每路的比特数

        //4-QAM
        //假如 I路有2个比特 [b0, b1] = [1, 0]，m=2：
        //k = 0: term = (2×1 - 1) × 2 ^ (2 - 0 - 1) = 1 × 2 = 2
        //k = 1 : term = (2×0 - 1) × 2 ^ (2 - 1 - 1) = -1 × 1 = -1
        //symbolValue = 2 + (-1) = 1
        int previousBit = 0;
        for (int k = 0; k < m; k++) {

            int grayBit = static_cast<int>(bits(start + k));
            int bitValue = previousBit ^ grayBit;
            previousBit = bitValue;
            int term = (2 * bitValue - 1) * static_cast<int>(std::pow(2, m - k - 1));
            symbolValue += term;
        }
    }

    /*
    * 静态成员函数名称： static void mapBitsToSymbols(const Parameters& params, Signals& signals);
    * 静态成员函数的输入：
    *
    *            params   // 类型 MapperMQAM::Parameters，包含MQAM编码器参数（调制阶数MaryQAM等）
    *            signals  // 类型 MapperMQAM::Signals，包含输入输出信号（输入比特流eIn、
    *                     // 输出I路符号eOut1、输出Q路符号eOut2）
    *
    * 静态成员函数的输出： void
    * 静态成员函数实现的功能描述： 执行MQAM比特到符号的核心映射过程。首先根据调制阶数计算每符号比特数，
    *                              验证输入比特数是否为每符号比特数的整数倍，然后按照Coder_mQAM文档描述的
    *                              交替分配方式进行串并转换：将输入比特流交替分配到I路和Q路，
    *                              对每路比特调用calculateSymbolValue计算符号值，生成I/Q两路正交符号输出，
    *                              完成从二进制比特到MQAM符号的完整转换流程
    */
    void MapperMQAM::mapBitsToSymbols(const Parameters& params, Signals& signals)
    {
        // 根据调制阶数计算每符号比特数
        int bitsPerSymbol;
        long long MaryQAM = GlobalValue::getBand(params.bandId).getModulationOrder();

        calculateBitsPerSymbol(simu_paras::Mary, bitsPerSymbol);
        int inputLength = signals.eIn.length();
        int bitsPerBranch = bitsPerSymbol / 2;  // I路和Q路每符号的比特数
        int numSymbols = inputLength / bitsPerSymbol;

        // 检查输入比特数是否能被bitsPerSymbol整除
        if (inputLength % bitsPerSymbol != 0) {
            throw std::runtime_error("MQAM modulation error: The number of input bits is not an integer multiple of bitsPerSymbol");
        }

        // 设置输出向量大小
        signals.eOut1.set_size(numSymbols); // I路输出
        signals.eOut2.set_size(numSymbols);// Q路输出

        // 按照文档描述的交替分配方式进行串并转换和映射
        for (int i = 0; i < numSymbols; i++) {
            int baseIndex = i * bitsPerSymbol;

            // 提取I路比特 (偶数位置: 0,2,4,...)
            bvec iBits(bitsPerBranch);
            for (int j = 0; j < bitsPerBranch; j++) {
                iBits(j) = signals.eIn(baseIndex + 2 * j);
            }

            // 提取Q路比特 (奇数位置: 1,3,5,...)
            bvec qBits(bitsPerBranch);
            for (int j = 0; j < bitsPerBranch; j++) {
                qBits(j) = signals.eIn(baseIndex + 2 * j + 1);
            }

            // 计算I路和Q路符号值
            int iSymbolValue, qSymbolValue;
            calculateSymbolValue(iBits, 0, bitsPerBranch, iSymbolValue);
            calculateSymbolValue(qBits, 0, bitsPerBranch, qSymbolValue);

            signals.eOut1(i) = iSymbolValue;
            signals.eOut2(i) = qSymbolValue;
        }
    }

    // 读取配置文件
    void MapperMQAM::loadConfigFile(const Parameters& params) {

        if (simu_paras::Mary <= 0 || (simu_paras::Mary & (simu_paras::Mary - 1)) != 0) {
            throw std::runtime_error("simu_paras::Mary must be a power of 2");
        }

        std::ifstream file(params.userTableFileName);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open the file" << params.userTableFileName << std::endl;
        }

        mappingTable.clear();
        std::string line;
        int expectedLen = -1;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            std::stringstream ss(line);
            std::string bitString, delimiter;
            double value;

            // 解析格式: "00 : -3.0"
            ss >> bitString;

            // 校验比特位数
            if (expectedLen == -1) expectedLen = bitString.length();
            if (bitString.length() != expectedLen) {
                std::cerr << "Error: The number of bits in this line is incorrect -> " << bitString << std::endl;
                continue;
            }

            // 读取分隔符 (:) 和 数值
            ss >> delimiter >> value;

            // 如果文件里没写冒号，可能是直接读到了数值，这里做个简单的容错
            // 如果 delimiter 不是 ":", 尝试把它转回数值（但这需要 stringstream 回退，
            // 简单起见，我们强制要求配置文件必须有 ":" 或者空格分开）
            // 如果你的文件只是 "00 -3.0"，把上面那行改成 ss >> value 即可。

            mappingTable[bitString] = value;
        }

        if (std::log2(simu_paras::Mary) != expectedLen * 2)
        {
            throw std::runtime_error("READ FILE ERROR: the number of bits from file not same as the setted MaryQAM");
        }
        if (mappingTable.empty()) {
            std::cerr << "Warning: Mapping table is empty" << std::endl;
        }
    }

    /*
    * 静态成员函数名称： std::vector<double> modulate(const Parameters& params, const std::vector<int>& bits)
    * 静态成员函数的输入：
    *
    *
    * 静态成员函数的输出： void
    * 静态成员函数实现的功能描述：
    */
    void MapperMQAM::modulate(const Parameters& params, Signals& signals)
    {
        int bitsPerSymbol;

        calculateBitsPerSymbol(simu_paras::Mary, bitsPerSymbol);
        int inputLength = signals.eIn.length();
        int bitsPerBranch = bitsPerSymbol / 2;  // I路和Q路每符号的比特数
        int numSymbols = inputLength / bitsPerSymbol;

        // 检查输入比特数是否能被bitsPerSymbol整除
        if (inputLength % bitsPerSymbol != 0) {
            throw std::runtime_error("MQAM modulation error: The number of input bits is not an integer multiple of bitsPerSymbol");
        }

        signals.eOut1.set_size(numSymbols); // I路输出
        signals.eOut2.set_size(numSymbols);// Q路输出

        for (int i = 0; i < numSymbols; i++) {
            int baseIndex = i * bitsPerSymbol;

            bvec iBits(bitsPerBranch);
            for (int j = 0; j < bitsPerBranch; j++) {
                iBits(j) = signals.eIn(baseIndex + 2 * j);
            }

            bvec qBits(bitsPerBranch);
            for (int j = 0; j < bitsPerBranch; j++) {
                qBits(j) = signals.eIn(baseIndex + 2 * j + 1);
            }

            std::string key1 = "";
            std::string key2 = "";
            for (int k = 0; k < bitsPerBranch; ++k) {
                key1 += std::to_string((int)iBits[k]);
                key2 += std::to_string((int)qBits[k]);
            }

            if (mappingTable.count(key1)) {
                signals.eOut1[i] = mappingTable[key1];
            }
            else {
                std::cerr << "Warning: Undefined bit combination " << key1 << "，Default output 0.0" << std::endl;
                signals.eOut1[i] = 0.0;
            }

            if (mappingTable.count(key2)) {
                signals.eOut2[i] = mappingTable[key2];
            }
            else {
                std::cerr << "Warning: Undefined bit combination " << key2 << "，Default output 0.0" << std::endl;
                signals.eOut2[i] = 0.0;
            }
        }
    }


} //end namespace ZK