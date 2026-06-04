#include"PRBSlfsr.h"
#include<GlobalValue.h>
namespace ZK
{
    /*
    * 静态成员函数名称：static void execute(const Parameters& params, Signals& signals);
    *
    * 输入参数：
    *     - params  类型：PRBSlfsr::Parameters
    *       伪随机二进制序列生成参数，包括：
    *       - prbsOrder：PRBS阶数
    *       - prbsLength：序列长度
    *       - signalLength：信号长度
    *       - prbsMode：生成方法
    *       - customPoly：自定义多项式
    *
    *     - signals  类型：PRBSlfsr::Signals
    *       输入输出信号容器，包含：
    *       - eOut：生成的PRBS序列输出
    *
    * 输出：void（无返回值）
    *
    * 功能描述：
    *     PRBS序列生成器的主要执行函数。根据输入的参数配置生成指定特性的伪随机二进制序列。
    *     当prbsLength参数大于0时，生成PRBS序列并存储在signals.eOut中；如果设置了保存文件路径，
    *     则自动将生成的序列数据保存到指定文件。
    */
    void PRBSlfsr::execute(const Parameters& params, Signals& signals)
    {
        checkParam(params);

        long long NBitsPerClock = GlobalValue::getBand(params.bandId).getNumInfoBitsPerClock();

        if (NBitsPerClock > 0) {
            generatePRBS(params, signals.eOut, NBitsPerClock);
        }

        if (params.saveFile != "noSave"&&params.saveFile != "") {
            saveData(signals.eOut, params.saveFile, params.dataFormat, params.writeMode);
        }
    }

        /*
         * 静态成员函数名称：static void set(const Parameters& params);
         *
         * 输入参数：
         *     - params  类型：PRBSlfsr::Parameters
         *       伪随机二进制序列生成参数，包括：
         *       - prbsOrder：PRBS阶数
         *       - prbsLength：序列长度
         *       - signalLength：信号长度
         *       - prbsMode：生成方法
         *       - customPoly：自定义多项式
         *
         * 输出：void（无返回值）
         *
         * 功能描述：
         *     参数验证函数。对输入的PRBS生成参数进行有效性检查，确保参数在合理范围内。
         *     验证内容包括：
         *     - PRBS阶数范围检查（2-31）
         *     - PRBS序列长度正数检查
         *     当参数不合法时，输出错误信息并终止当前处理流程。
         *
         */

    void PRBSlfsr::checkParam(const Parameters& params)
    {
        // 验证输入参数
        if (params.prbsOrder < 2 || params.prbsOrder > 31) {
            throw std::runtime_error("Error: PRBS order must be between 2 and 31");
        }
    }

   /*
    * 静态成员函数名称：static void generatePRBS(const Parameters& params, bvec& prbsOut);
    *
    * 输入参数：
    *     - params  类型：PRBSlfsr::Parameters
    *       伪随机二进制序列生成参数，包括：
    *       - prbsOrder：PRBS阶数（2-31）
    *       - prbsLength：序列长度
    *       - prbsMode：生成方法（标准/自定义）
    *       - customPoly：自定义多项式
    *       - randomSeed：自定义种子值
    *
    *     - prbsOut  类型：bvec
    *       输出PRBS序列的二进制向量
    *
    * 输出：void（无返回值）
    *
    * 功能描述：
    *     生成伪随机比特序列(PRBS)，支持2-31阶的PRBS生成。遵循ITU-T标准多项式，
    *     使用线性反馈移位寄存器(LFSR)算法实现。支持标准多项式和自定义多项式两种生成方式，
    *     提供默认随机种子和用户自定义种子两种初始化选项。
    */
    void PRBSlfsr::generatePRBS(const Parameters& params, bvec& prbsOut,long long length)
    {
        //到2的31次方（要求种子是随机的，支持随机产生或者用户输入两种）
        static const std::map<int, bvec> defaultPolynomials = {  
            {2, bvec("1 1")},
            {3, bvec("1 0 1")},
            {4, bvec("1 0 0 1")},
            {5, bvec("1 0 0 1 0")},
            {6, bvec("1 0 0 0 0 1")},
            {7, bvec("1 0 0 0 0 0 1")},
            {8, bvec("1 0 0 0 1 1 1 0")},
            {9, bvec("1 0 0 0 0 1 0 0 0")},
            {10, bvec("1 0 0 0 0 0 0 1 0 1")},
            {11, bvec("1 0 0 0 0 0 0 0 0 1 0")},
            {12, bvec("1 0 0 0 0 0 1 0 1 0 0 1")},
            {13, bvec("1 0 0 0 0 0 0 0 0 1 1 0 1")},
            {14, bvec("1 0 0 0 1 0 0 0 1 0 0 0 0 1")},
            {15, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 1")},
			{16, bvec("1 0 0 1 0 0 0 0 0 0 0 0 1 0 1 1")},
            {17, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {18, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {19, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {20, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {21, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {22, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {23, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {24, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {25, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {26, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {27, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {28, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {29, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {30, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")},
            {31, bvec("1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0")}
        };

        // 获取生成多项式
        bvec generatorPoly;

        if (params.prbsMode == ITU_STANDARD) {
            // 在defaultPolynomials映射中查找对应阶数的多项式，find()函数检查是否存在该阶数的多项式
            // 如果找到，使用at()函数安全地获取多项式，如果找不到，输出错误信息并退出
            if (defaultPolynomials.find(params.prbsOrder) != defaultPolynomials.end()) {
                generatorPoly = defaultPolynomials.at(params.prbsOrder);
            }
            else {
                std::cerr << "Error: Unsupported PRBS order: " << params.prbsOrder << std::endl;
                return;
            }
        }

        // 创建移位寄存器，长度为PRBS阶数
        bvec shiftRegister(params.prbsOrder);

        unsigned int seed = 0;

        // 需要引入一个clock的计数，用于判断多个时钟周期间的PRBS处理，这里暂时用直接定义clock占位代替
        // 这个参数是跨clock的，需要在每次调用的时候由外部更新
        int clock = 1;

        std::queue<unsigned int> seedQueue;
        if (params.loadLogicInfo.size()>=6&&((params.loadLogicInfo.substr(params.loadLogicInfo.size()-6))!= "noLoad")) {

            std::string filename = params.loadLogicInfo + ".dat";
            std::ifstream inFile(filename);

            if (!inFile.is_open()) {
                std::cerr << "Error: Cannot open file for reading: " << filename << std::endl;
                return;
            }

            std::string line;
            int currentLine = 1;   // 用于匹配 clock

            // 找到 clock 对应的行
            while (std::getline(inFile, line)) {
                if (currentLine == clock) break;
                currentLine++;
            }

            if (inFile.eof() && currentLine != clock) {
                std::cerr << "Error: Not enough lines in file for clock = " << clock << std::endl;
                return;
            }

            inFile.close();

            // 解析该行配置信息
            std::stringstream ss(line);
            std::string token;

            while (ss >> token) {
                if (token == "&") continue;

                // 判断 token 是否为数字
                if (std::all_of(token.begin(), token.end(), ::isdigit)) {
                    seedQueue.push(std::stoul(token));
                }
            }
        }


        // 计算PRBS序列的周期长度，并定义PRBS计数器
        int prbsLength = (1 << params.prbsOrder) - 1;
        int prbsCount = 0;

        
        if (params.loadLogicInfo.size() >= 6 && ((params.loadLogicInfo.substr(params.loadLogicInfo.size() - 6)) != "noLoad")) {
            //如果设置为读取配置参数，则从读取的种子序列中读取
            seed = seedQueue.front();
            seedQueue.pop();
            for (int i = 0;i < params.prbsOrder;++i)
            {
                //将种子的第i个比特位（从最低位开始）赋值给移位寄存器的第i个位置。
                shiftRegister[i] = (seed >> i) % 2;
            }
        }
        else
        {
            seed = static_cast<unsigned int>(params.randomSeed);
            for (int i = 0;i < params.prbsOrder;++i)
            {
                shiftRegister[i] = (seed >> i) % 2;
            }
        }

        //当设置为记录配置参数时，记录当前的种子
        if (params.saveLogicInfo.size() >= 6 && ((params.saveLogicInfo.substr(params.saveLogicInfo.size() - 6)) != "noSave")) {
            std::string filename = params.saveLogicInfo + ".dat";
            std::ofstream outFile(filename, std::ios::app);
            if (!outFile.is_open()) {
                std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
                return;
            }
            //不同种子间用&隔开
            outFile << " & " << seed;
            outFile.close();  
        }


        

        // 检查并防止线性反馈移位寄存器(LFSR)的全零状态
        // feedback = 0 ^ 0 ^ 0 ^ ... ^ 0 = 0
        // 导致寄存器永远保持全零状态，无法生成PRBS序列
        bool allZeros = true;
        for (int i = 0; i < shiftRegister.length(); i++) {
            if (shiftRegister(i) == bin(1)) {
                allZeros = false;
                break;
            }
        }
        if (allZeros) {
            std::cerr << "Error: Seed cannot be all zeros" << std::endl;
            return;
        }

        // 初始化输出序列
        prbsOut.set_length(length);

        // 初始化反馈比特
        bin feedback(0);

        // 生成PRBS
        for (int i = 0; i < length; i++) {
            // 输出当前比特（最高位）
            prbsOut(i) = shiftRegister(params.prbsOrder - 1);

            // 计算反馈比特
            feedback = 0;
            for (int j = 0; j < params.prbsOrder; j++) {
                // 如果生成多项式的第j位为1，则将该位的寄存器值与feedback进行异或
                if (generatorPoly(j) == bin(1)) {
                    //
                    feedback = feedback ^ shiftRegister(j);
                }
            }

            // 移位寄存器：从最高位开始，将每一位的值替换为前一位的值，实现左移一位
            for (int j = params.prbsOrder - 1; j > 0; j--) {
                shiftRegister(j) = shiftRegister(j - 1);
            }
            // 将计算得到的反馈值放入移位寄存器的最低位（索引0）
            shiftRegister(0) = feedback;

            // 检测当前的PRBS序列是否完成一个周期
            prbsCount++; //PRBS计数器
            if (prbsLength == prbsCount) {       //当计数器与周期长度相等，需要更新种子    
                prbsCount = 0; //归零  
                //如果设置为读取配置参数，则从读取的种子序列中读取
                if (params.loadLogicInfo.size() >= 6 && ((params.loadLogicInfo.substr(params.loadLogicInfo.size() - 6)) != "noLoad")) {
                    seed = seedQueue.front();
                    seedQueue.pop();
                    for (int i = 0;i < params.prbsOrder;++i)
                    {
                        //将种子的第i个比特位（从最低位开始）赋值给移位寄存器的第i个位置。
                        shiftRegister[i] = (seed >> i) % 2;
                    }
                }
                
                //randomSeed为定值，seed不需变化

                //当设置为记录配置参数时，记录当前的种子
                if (params.saveLogicInfo.size() >= 6 && ((params.saveLogicInfo.substr(params.saveLogicInfo.size() - 6)) != "noSave")) {
                    std::string filename = params.saveLogicInfo + ".dat";
                    std::ofstream outFile(filename, std::ios::app);
                    if (!outFile.is_open()) {
                        std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
                        return;
                    }
                    //不同种子间用&隔开
                    outFile << " & " << seed;
                    outFile.close();  
                }
            }
        }

        if (params.saveLogicInfo.size() >= 6 && ((params.saveLogicInfo.substr(params.saveLogicInfo.size() - 6)) != "noSave")) {
            std::string filename = params.saveLogicInfo + ".dat";
            std::ofstream outFile(filename, std::ios::app);
            if (!outFile.is_open()) {
                std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
                return;
            }
            //换行
            outFile << "\n";
            outFile.close();
        }
        // Data直接输出
        if (params.SPMode == Data) {
            prbsOut = prbsOut;
        }

        // Databar按位取反
        else if (params.SPMode == DataBar) {
            for (int i = 0; i < prbsOut.length(); ++i) {
                prbsOut[i] = (prbsOut[i] == 0 ? 1 : 0);
            }
        }           
    }

}//end namespace ZK