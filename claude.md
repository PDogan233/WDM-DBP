# ZKIC_project — Claude 项目总览

> 最后更新: 2026-06-06 (v5.1: DBP 开发完成，15 测试通过，MATLAB 可视化，4 种测试信号含 QPSK)
> 本文档供 Claude Code 在后续会话中快速理解项目背景、架构和规范。每次重大变更后请更新本文档。

---

## 1. 项目背景

**项目目标**：光通信系统仿真平台。当前阶段的核心任务是实现基于 **SSFM (Split-Step Fourier Method)** 的光纤建模算法与 **DBP (Digital Back-Propagation)** 非线性补偿算法。

**开发语言**：C++（工业级、面向对象）

**所属公司**：绍兴智科光电计算有限公司 (ZKOE)

**开发规范依据**：`docs/ZKOE公司Agents开发交付要求251003V4p0r1.md`（已从 PDF 转换为可读 Markdown）

---

## 2. 项目目录结构

```
ZKIC_project/
├── claude.md                  ← 本文档（项目指南）
├── CMakeLists.txt             ← CMake 构建配置（唯一需要维护的构建文件）
├── start_vsc.bat              ← VSCode 启动脚本（自动检测 VS 路径并配置 MSVC 环境）
├── .gitignore                 ← Git 忽略规则
├── .vscode/                   ← VSCode 配置（c_cpp_properties, tasks, launch, settings）
│
├── SSFM_Core.h/.cpp           ← SSFM 共享计算内核（chooseStep, applyNonlinearity, applyDispersion 等）
├── DP_Fiber.h/.cpp             ← 前向双偏振光纤传播 Agent (Manakov SSFM)
├── DP_DBP.h/.cpp               ← 全带数字反向传播 Agent (DBP)
├── DP_DBPTest.cpp              ← 综合测试套件（15 用例 + 4 类信号可视化数据生成，单 main）
├── DP_DBPTest.m                ← MATLAB 可视化（Part A: Fiber 传播, Part B: DBP 补偿+星座图）
├── fiber_WDM.h_old             ← [已归档] 旧版光纤代码
├── fiber_WDM.cpp_old           ← [已归档]
├── fiber_WDM_test.cpp_old      ← [已归档]
│
├── fftw/                      ← FFTW 库（项目内依赖，头文件 + .lib + .dll）
├── ZKIC_lib/                  ← ZKIC 数学库 + GlobalValue（项目内依赖）
│   ├── ZKIC_mathworld/
│   │   └── ZKIC_mathworld2p5/
│   │       ├── include/       ← 头文件（vec.h, mat.h, fft.h, fstreamIO.h 等）
│   │       └── x64/debug/lib/ ← ZKIC_math_world2p5d.lib
│   └── GlobalValueForZD/
│       ├── include/           ← GlobalValue.h
│       └── x64/debug/lib/     ← signals_globalvalue_world1p1d.lib
│
├── format_ref/                ← 公司成熟示范代码（编码规范参考）
│   ├── IQCombiner.h/.cpp
│   ├── MapperMQAM.h/.cpp
│   ├── MeasurementSimple.h/.cpp
│   ├── NRZCoder.h/.cpp
│   ├── PRBSlfsr.h/.cpp
│   └── test4.cpp              ← format_ref 的综合测试入口
│
├── py_ref/                    ← Python 参考实现
│   ├── para.py, tx_DSP.py, channel.py, dbp.py, rx_DSP.py
│   ├── utils.py, visualize.py, main_simu_test.py
│
├── docs/                      ← 文档（交付要求 MD + CHM 手册）
│
└── build/                     ← CMake 构建输出（gitignore，每台电脑本地生成）
```

---

## 3. 公司编码规范（从 format_ref/ 提取）

### 3.1 文件头格式

每个 .h 和 .cpp 文件必须包含：

```cpp
/*******************************************************************************
 * Copyright (c) 2026 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software
 * is strictly prohibited.
 ******************************************************************************/

/**
 * @file: ClassName.h
 * @brief: 简要描述
 * @author: Jiahao Zhang
 * @version: 1.0.0
 * @date: YYYY-MM-DD
 * @department: Research and Development Department
 * @project: ZKIC Agents Development
 */

/**
 * Revision History:
 * * Version    Author          Date          Changes
 * -----------------------------------------------------------------------------
 * 1.0.0      Jiahao Zhang   2026-XX-XX    Initial version
 */
```

### 3.2 命名空间

所有类必须在 `ZK` 命名空间内：

```cpp
namespace ZK {
    class MyClass { ... };
} // namespace ZK
```

### 3.3 类结构模式（Agent 模式）

**核心规则**（来自公司交付要求 V4）：

1. **类不定义构造函数和析构函数**，所有功能由静态成员函数实现
2. **`execute` 签名不可修改**：必须为 `static void execute(const Parameters& params, Signals& signals)`
3. **所有辅助函数也必须是静态的**（静态方法只能调用静态方法）
4. **静态函数间信息传递使用引用**（不使用 `return`）
5. **一个 .cpp/.h 文件包含且仅包含一个 class**
6. **继承时，基类和派生类分别定义在不同的文件中**
7. **派生类的 `Parameters` 继承基类的 `Parameters`**（通过 `struct Parameters : BaseClass::Parameters`）

**Parameters 结构体规则**：

- 结构体名称固定为 `Parameters`
- 通过 `const` 引用传递（只读输入）
- `saveFile` 默认值为 `"noSave"`（不保存），多输出端口时设 `saveFile1`, `saveFile2`...
- 当 `saveFile == "noSave"` 或 `saveFile == ""` 时跳过文件保存

**Signals 结构体规则**：

- 结构体名称固定为 `Signals`
- 通过非 `const` 引用传递（输出，会被修改）
- 信号命名按信号种类 + 端口方向：`eIn`（电输入）、`oIn`/`oOut`（光输入/输出）
- **多端口**：不同端口使用不同变量（如 `oIn1`, `oIn2`），端口变量数 = Agent 端口数
- **同端口多组数据**（如双偏振 X/Y）：使用单个矩阵变量（如 `cmat oIn` = 2 行矩阵，每行一个偏振）

**基本模板**：

```cpp
namespace ZK {

/*
 * Agent 类的功能描述：......
 *
 * Agent 类具体示例执行后的输出：.......
 */

class AgentName {
public:
    struct Parameters {
        // 功能特定参数 ...
        std::string saveFile = "noSave";  // 输出文件保存路径
    };

    struct Signals {
        // 输入/输出信号
    };

    // === 以下语句不可修改 ===
    static void execute(const Parameters& params, Signals& signals);

    // === 辅助函数（须定义为静态） ===
    static void checkParam(const Parameters& params);

private:
    // --- 私有辅助函数（也须为静态）---
};

} // namespace ZK
```

**单端口示例**（`FiberScalar`）：

```cpp
class FiberScalar {
public:
    struct Parameters {
        double alpha;         // Attenuation (dB/m)
        double beta2;         // Dispersion (s^2/m)
        double gamma;         // Nonlinearity (1/W/m)
        std::string saveFile = "noSave";
    };
    struct Signals {
        cvec oIn;             // 输入光信号
        cvec out;             // 输出光信号
    };
    static void execute(const Parameters& params, Signals& signals);
};
```

**多端口示例**（`Mux2`）：

```cpp
class Mux2 {
public:
    struct Parameters {
        std::string saveFile = "noSave";
    };
    struct Signals {
        cvec oIn1;            // 输入光端口 1
        cvec oIn2;            // 输入光端口 2
        cvec out;             // 输出光信号
    };
    static void execute(const Parameters& params, Signals& signals);
};
```

**同端口多数据组（双偏振）示例**（`FiberVector`）：

```cpp
class FiberVector {
public:
    struct Parameters {
        double alpha;         // Attenuation (dB/m)
        double beta2;         // Dispersion (s^2/m)
        double gamma;         // Nonlinearity (1/W/m)
        std::string saveFile = "noSave";
    };
    struct Signals {
        cmat oIn;             // 2行矩阵 [oInX; oInY] — X/Y 偏振复光场
        cmat out;             // 2行矩阵 [outX; outY]
    };
    static void execute(const Parameters& params, Signals& signals);
};
```

**仅有输出的 Agent 示例**（`Laser`）：

```cpp
class Laser {
public:
    struct Parameters {
        double lamda;         // Laser wavelength [m]
        double P;             // Optical output power [dBm]
        double phase;         // Initial phase offset [rad]
        double LW;            // Laser linewidth [Hz]
        double Npoints;       // Number of sampling points
        double deltaT;        // Sampling time interval [s]
        double FO;            // Frequency offset [Hz]
        std::string saveFile = "noSave";
    };
    struct Signals {
        cvec out;             // 输出光场 [W^(1/2)] — 仅输出，无输入
    };
    static void execute(const Parameters& params, Signals& signals);
};
```

**派生类示例**（`LaserDML : public Laser`）：

```cpp
class LaserDML : public Laser {
public:
    struct Parameters : Laser::Parameters {
        double thresholdCurrent;   // Laser threshold current [mA]
        double slopeEfficiency;    // Laser slope efficiency [mW/mA]
        double maxCurrent;         // Maximum operating current [mA]
        double alpha;              // Linewidth enhancement factor
        double kappa;              // Chirp coefficient [Hz/mW]
    };
    struct Signals {
        vec eIn;                   // 输入电驱动信号 (DML 新增输入)
        cvec out;                  // 输出光场 [W^(1/2)]
    };
    static void execute(const Parameters& params, Signals& signals);
};
```

> **注意**：派生类的 `Parameters` 继承基类 `Parameters`，物理参数在不同对象中有独立内存空间。基类 `Laser::Parameters laserPara` 和派生类 `LaserDML::Parameters laserDMLPara` 的同名成员（如 `alpha`）互不影响。

### 3.4 函数文档格式

**All comments in code must use English** (file headers, class-level doc blocks, function-level doc blocks, inline comments).

**类级注释**（在类定义前，`namespace ZK {` 之后）：

```
Agent 类的功能描述：......
Agent 类具体示例执行后的输出：.......
```

**示例**：
```
Agent 类的功能描述：实现激光器输出信号的模拟。根据输入的 Parameters（如波长、输出功率、
采样点数、相位、线宽等），生成对应长度的复数向量 signals.oOut。

Agent 类示例执行后的输出：根据所输入的 Parameters，生成长度为 Npoints 的复数光场向量
signals.oOut。
```

**静态成员函数注释**（在 .cpp 文件中每个函数定义前）：

```
静态成员函数名称：void ClassName::execute(const Parameters& params, Signals& signals)
静态成员函数的输入：params // 类型 ClassName::Parameters，包含的字段（列出关键字段及含义）
                    signals // 类型 ClassName::Signals，输入信号字段
静态成员函数的输出：无返回值 void，输出信号储存在 signals.xxx 中
静态成员函数实现的功能描述：......
```

**示例**：
```
静态成员函数名称：void Laser::execute(const Parameters& params, Signals& signals)

静态成员函数的输入：
  params // 类型 Laser::Parameters，包含激光器的参数（波长 lamda、功率 P、相位 phase、
         采样点数 Npoints、线宽 LW、采样间隔 deltaT、光速 cLight、频率偏移 FO 等）

静态成员函数的输出：无返回值 void，输出 laser 信号储存在 signals.oOut 中

静态成员函数实现的功能描述：根据输入参数 params，生成长度为 params.Npoints 的复数光场向量
signals.oOut。
```

### 3.5 ZK 数学库类型速查

| 类型      | C++ 完整定义                  | 说明             |
| --------- | ----------------------------- | ---------------- |
| `vec`   | `Vec<double>`               | 实数向量         |
| `cvec`  | `Vec<std::complex<double>>` | 复数向量         |
| `ivec`  | `Vec<int>`                  | 整数向量（索引） |
| `bvec`  | `Vec<bin>`                  | 二进制向量       |
| `svec`  | `Vec<short int>`            | 短整数向量       |
| `llvec` | `Vec<long long>`            | 长整数向量       |
| `mat`   | `Mat<double>`               | 实数矩阵         |
| `cmat`  | `Mat<std::complex<double>>` | 复数矩阵         |
| `imat`  | `Mat<int>`                  | 整数矩阵         |
| `bmat`  | `Mat<bin>`                  | 二进制矩阵       |
| `smat`  | `Mat<short int>`            | 短整数矩阵       |
| `bin`   | `bin` (内部 `char`)       | 二进制值 {0,1}   |

**bin 类型运算规则**（GF(2)）：`+`/`-`/`^` = XOR，`*`/`&` = AND，`/`/`|` = OR，`!`/`~` = NOT

**Vec 构造与初始化**：

```cpp
vec v1(100);               // 指定长度
vec v2(100, 0.0);          // 指定长度+初始值
vec v3 = "0 0.7 5 9.3";   // 字符串初始化
ivec v4 = "0:5";           // [0, 1, 2, 3, 4, 5]
vec v5 = "3:2.5:13";       // [3, 5.5, 8, 10.5, 13]
```

**Vec 常用操作**：

| 操作                           | 说明                           |
| ------------------------------ | ------------------------------ |
| `v(i)`                       | 访问元素 i（0 起始）           |
| `v(i1, i2)`                  | 取子向量 [i1, i2]（-1 = 末尾） |
| `v.left(n)` / `v.right(n)` | 取左/右 n 个元素               |
| `v.mid(start, n)`            | 从 start 取 n 个               |
| `v.size()` / `v.length()`  | 元素个数                       |
| `v.zeros()` / `v.ones()`   | 置零/置一                      |
| `concat(v1, v2, ...)`        | 拼接向量（最多 5 个）          |
| `elem_mult(a, b)`            | 逐元素乘法                     |
| `dot(v1, v2)` / `v1 * v2`  | 内积/点积                      |
| `v.set_subvector(i, sub)`    | 替换从 i 开始的子向量          |

**Vec 统计操作**：

| 操作 | 说明 |
|------|------|
| `sum(v)` | 求和 |
| `prod(v)` | 累乘 |
| `cumsum(v)` | 累加和（返回 vec，每个位置是之前的和） |
| `mean(v)` | 均值 |
| `norm(v)` | 欧几里得范数（模长） |

**Mat 基本操作**：

| 操作 | 说明 |
|------|------|
| `m(i, j)` | 访问元素 (row, col)，0 起始 |
| `m(r1, r2, c1, c2)` | 取子矩阵（-1 = 末尾） |
| `m.rows()` / `m.cols()` | 行数 / 列数 |
| `m.get_row(i)` / `m.get_col(j)` | 取第 i 行 / 第 j 列 |
| `m.set_row(i, v)` / `m.set_col(j, v)` | 设置第 i 行 / 第 j 列 |
| `m.swap_cols(i, j)` / `m.copy_col(i, j)` | 交换列 / 复制列 |
| `m.T()` / `m.H()` | 转置 / Hermitian 转置 |
| `m.zeros()` / `m.ones()` | 置零 / 置一 |
| `m.set_size(r, c, copy)` | 改变尺寸（false=不保留） |
| `concat_horizontal(m1, m2)` | 水平拼接 |
| `concat_vertical(m1, m2)` | 垂直拼接 |
| `elem_mult(m1, m2)` | 逐元素乘法 |

头文件按需引入：`<vec.h>`, `<mat.h>`, `<fft.h>`, `<fstreamIO.h>`, `<GlobalValue.h>`, `<commfunc.h>`, `<convolution.h>`, `<random.h>`, `<binary.h>`, `<elem_math.h>`, `<log_exp.h>`, `<trig_hyp.h>`, `<min_max.h>`, `<sort.h>`, `<operators.h>`, `<common_values.h>`, `<converters.h>`, `<error.h>`, `<integration.h>`

### 3.6 文件 I/O

**方式一（交付要求指定格式 — 优先使用）**：

公司交付要求规定输出文件为纯文本格式（扩展名 `.dat`），规则如下：

- **保存开关**：`saveFile == "noSave"` 或 `""` 时跳过保存；否则保存到 `saveFile + ".dat"`
- **数据格式**：所有数据以行向量形式保存；复数拆为实部+虚部交替排列
- **复数行向量** `[a+bj, c+dj]` → 存储为 `a b c d`（一行）
- **矩阵**（n×m）：按行展开为 n 行，复数矩阵每行 2m 个元素（实虚交替）
- **不含列标题、变量名、描述信息**（纯数据）
- **输出文件命名**：大驼峰命名法，反映数据物理意义（如 `CWLaserOutput.dat`、`ModulatedSignalIQ.dat`）

手动实现模板（cvec — 单偏振/单行向量）：

```cpp
bool saveData(const cvec& data, const std::string& saveFile) {
    if (saveFile == "noSave" || saveFile.empty()) return true;
    std::ofstream outFile(saveFile + ".dat");
    if (!outFile.is_open()) return false;
    outFile << std::scientific << std::setprecision(12);
    for (size_t i = 0; i < data.size(); ++i) {
        outFile << data[i].real() << " " << data[i].imag();
        if (i < data.size() - 1) outFile << " ";
    }
    outFile.close();
    return true;
}
```

手动实现模板（cmat — 双偏振/多行矩阵，每行一个偏振态）：

```cpp
bool saveData(const cmat& data, const std::string& saveFile) {
    if (saveFile == "noSave" || saveFile.empty()) return true;
    std::ofstream outFile(saveFile + ".dat");
    if (!outFile.is_open()) return false;
    outFile << std::scientific << std::setprecision(12);
    for (int r = 0; r < data.rows(); ++r) {
        for (int c = 0; c < data.cols(); ++c) {
            outFile << data(r, c).real() << " " << data(r, c).imag();
            if (c < data.cols() - 1) outFile << " ";
        }
        outFile << "\n";  // 每行一个偏振态，换行
    }
    outFile.close();
    return true;
}
```

**方式二（ZKIC 库函数 — 备选）**：

```cpp
saveData(signal, params.saveFile, params.dataFormat, params.writeMode);
// 支持格式：Mat（MATLAB v5）、Zat（ZK 二进制）、Dat、Txt、Csv
// params.saveFile = "" 或 "noSave" 时跳过保存
```

> **注意**：两种方式的跳过条件相同（`"noSave"` / `""`），但方式一按交付要求精确控制数据排列格式，方式二使用 ZKIC 库的固定格式。**与 Python/MATLAB 交互时优先使用方式一**。

### 3.7 其他规范

- 头文件使用 `#pragma once`（不使用 `#ifndef` 守卫）
- **命名规范**：
  - 类名：大驼峰（PascalCase），如 `ClassTest`、`DP_Fiber`
  - 结构体名：大驼峰，参数/信号结构体固定为 `Parameters` / `Signals`
  - 结构体内部成员：小驼峰（camelCase），按信号种类+端口命名，如 `eIn`（电输入）、`oOut1`（光输出端口1）
  - 变量和函数：小驼峰，如 `variableTest`，允许约定俗成的缩写（如 `sqrt`）
  - 文件名：与类名一致（如 `DP_Fiber.h` / `DP_Fiber.cpp` / `DP_FiberTest.cpp`）
- 类成员命名使用驼峰式（如 `bandId`, `channelIndex`）
- 私有成员函数注释在 .cpp 文件中
- **注释语言**：代码中所有注释（文件头、类级/函数级文档块、行内注释）统一使用英文

### 3.8 交付物结构与命名

每个 Agent 的交付包含以下文件：

| 文件 | 说明 |
|------|------|
| `AgentName.h` | 类声明头文件 |
| `AgentName.cpp` | 类实现文件 |
| `AgentNameTest.cpp` | 测试主程序文件（含 main 函数） |
| `debug/` 文件夹 | 编译后的 debug 文件包（含测试数据及结果） |
| `AgentName_tests.docx` | 程序运行测试文档 |
| `AgentNameTest.m` | MATLAB 可视化脚本 |
| 程序原理说明文档 | 理论依据（公式推导、论文、PPT 等） |

**文件夹命名格式**：`AgentNameYYYYMMDDvXpY`（如 `Laser20251003v1p0`，`v1p0` = version 1.0）

**一个 Agent 对应一个 .cpp/.h 文件对**。有继承关系时，基类和派生类分文件定义（如 `Laser.h/.cpp` + `LaserDML.h/.cpp` + 各自测试文件）。

### 3.9 测试与可视化要求

**测试要求**：
- 每个 Agent **不少于 5 个测试用例**
- 测试文档需包含：具体参数、运行结果截图
- 测试主程序中需输出仿真参数和信号摘要信息

**MATLAB 可视化**（`AgentNameTest.m`）：
- 可视化脚本命名与测试文件一致
- **所有输入和输出数据均需可视化**
- **输出数据必须从保存的数据文件读取**（不直接使用内存数据），确保可重复性
- 输入/输出尽量在同一视图或并列视图对比
- 图表需包含：标题、轴标签、图例、性能指标（EVM/BER/SNR 等）
- 时域、频域或调制域需覆盖

**模板**（读取 `.dat` 文件并绘图）：

```matlab
%% Parameters (must match C++ test)
deltaT = 6.25e-12;  % Sampling interval [s] — must match C++ test params
Npoints = 32768;    % Number of points — must match C++ test params

%% Read data from output file
data = load('CWLaserOutput.dat');
if mod(length(data), 2) == 0
    signal_complex = data(1:2:end) + 1j * data(2:2:end);
end
t = (0:length(signal_complex)-1) * deltaT;

figure('Position', [100, 100, 1000, 700]);
subplot(2, 1, 1);
plot(t, abs(signal_complex).^2);
ylabel('Power (W)'); title('Intensity'); grid on;
subplot(2, 1, 2);
plot(t(1:min(1000,end)), real(signal_complex(1:min(1000,end))));
xlabel('Time (s)'); ylabel('Real Part'); title('Waveform'); grid on;
```

---

## 4. Python 参考实现分析（py_ref/）

### 4.1 系统参数 (para.py)

| 参数           | 值            | 说明                 |
| -------------- | ------------- | -------------------- |
| `Nsym`       | 2^15          | 每信道符号数         |
| `M`          | 16            | QAM 阶数             |
| `Rs`         | 32e9          | 符号率               |
| `Nch`        | 3             | WDM 信道数           |
| `DeltaF`     | 50e9          | 信道间隔             |
| `L_span`     | 100e3 m       | 每跨段长度           |
| `Nspans`     | 10            | 跨段数               |
| `alpha_dBpm` | 0.2e-3 dB/m   | 损耗系数             |
| `Dispersion` | 16.5e-6 s/m² | 色散 D               |
| `Dis_S`      | 0.08e3 s/m³  | 色散斜率             |
| `n2`         | 2.6e-20 m²/W | 非线性折射率         |
| `Aeff`       | 80e-12 m²    | 有效面积             |
| `pmd_coeff`  | 0             | PMD 系数（当前关闭） |
| `method`     | 'constant'    | 步长控制方法         |
| `dz`         | 2e3 m         | 默认 SSFM 步长       |

### 4.2 发射端 DSP (tx_DSP.py)

随机比特 → 16-QAM 调制 → RRC 脉冲成形 → 功率归一化 → 频移复用(WDM)

### 4.3 信道传播 (channel.py)

核心算法：**对称 SSFM (Strang Splitting)**

```
for each span:
    for z < L_span:
        choose_step()
        NL(h/2) → Disp(h) → NL(h/2)   // Manakov 方程
    apply_edfa()
```

关键细节：

- **Manakov 因子**: `gamma_manakov = (8/9) * gamma`（随机双折射光纤）
- 非线性相位: `φ = -γ_manakov * P_total * h`
- 色散在频域施加: `H = exp(-α_np*h/2 - j*β(ω)*h)`
- 步长控制: `constant` / `local_error` / `global_error`

### 4.4 DBP (dbp.py)

Python 参考实现支持两种模式：

1. `dbp_fullband_to_subband()`: 提取子带 → 反向 SSFM → 重构全频带
2. `dbp_subband()`: 直接在已下采样的子带上做 DBP

C++ 当前实现：**全带 DBP**（对应 Python 的全带模式，即直接反转 SSFM）：

```
for span in reverse(Nspans):
    remove EDFA gain
    for z < L:
        NL(-h/2) → Disp(-h) → NL(-h/2)    ← h 取负值自动反转符号
```

子带 DBP 留待后续实现。

### 4.5 接收端 DSP (rx_DSP.py)

下变频 → 子带提取 → DBP → 匹配滤波 → 时钟恢复 → 帧同步 → 相位补偿 → 功率归一化 → 16-QAM 解调 → BER

---

## 5. 现有 C++ 代码架构 (v5.0)

旧 `fiber_WDM.*` 已归档为 `*_old`。当前架构包含 3 个模块：

### 5.1 SSFM_Core（共享计算内核，非 Agent）

- 无 Parameters/Signals/execute — 纯工具类
- `computeFiberPhysics()`: D/S/n2/Aeff/λ₀ → β1/β2/β3/γ/α_np
- `buildFrequencyContext()`: 构建 fftshift 频率网格 + 预计算 β(ω)
- `chooseStep()`: 三种自适应步长控制 (constant/local_error/global_error)
- `applyNonlinearity()`: Manakov 非线性相位旋转 (8/9 因子 + 交叉相位调制)
- `applyDispersion()`: 频域色散+损耗 (ZK::fftC2C/ifftC2C 管道)

### 5.2 DP_Fiber（前向光纤传播 Agent）

- Agent 模式：Parameters (物理+数值+采样) + Signals (oIn/out, cmat 2×Nt)
- `execute()`: 对称 SSFM (Strang splitting) — NL(h/2)→Disp(h)→NL(h/2)
- `checkParam()`: 参数合法性验证
- .dat 文件输出（复数实虚交替，无表头）
- 不含 EDFA、PMD

### 5.3 DP_DBP（数字反向传播 Agent）

- 全带 DBP：反向跨段循环 + h<0 自动反转色散/非线性符号
- 与 DP_Fiber 共享 SSFM_Core 内核，仅 h 符号不同
- 支持多跨段 (nSpans)，EDFA 增益移除 (gLin)
- .dat 文件输出

### 5.4 测试

- `DP_DBPTest.cpp`: SSFM_Core 5 测试 + DP_Fiber 5 测试 + DP_DBP 5 往返测试 (**15/15 通过**)
- 4 类测试信号自动生成 .dat 可视化数据：Gaussian 脉冲、单音 CW、双音、**QPSK**
- `DP_DBPTest.m`: MATLAB 可视化脚本（Part A: Fiber 传播, Part B: DBP 补偿 + QPSK 星座图）
- QPSK 参数：100 符号, 80km 光纤, ~6% 色散 ISI + ~89° SPM 旋转 → 星座图清晰展示"干净→失真→恢复"

---

## 6. 第三方库依赖

| 库               | 项目内路径                                     | 关键文件                                                                       |
| ---------------- | ---------------------------------------------- | ------------------------------------------------------------------------------ |
| FFTW 3           | `fftw/`                                      | `fftw3.h`, `libfftw3-3.lib`, `libfftw3-3.dll`                            |
| ZKIC 数学库 v2.5 | `ZKIC_lib/ZKIC_mathworld/ZKIC_mathworld2p5/` | `include/*.h`, `x64/debug/lib/ZKIC_math_world2p5d.lib`                     |
| GlobalValue      | `ZKIC_lib/GlobalValueForZD/`                 | `include/GlobalValue.h`, `x64/debug/lib/signals_globalvalue_world1p1d.lib` |

---

## 7. ZKIC 公司库 API 参考

> 本节详细记录公司两个核心库的 API，供写代码时直接查阅。包含头文件中的实际函数签名和 CHM 手册中的用法说明。

### 7.0 核心原则：公司库优先

> **所有代码必须优先使用 ZKIC 公司库。只有当公司库确实不提供相应功能时，才使用 C++ 标准库或第三方库。**
>
> **关于 FFTW 的角色**：ZKIC 数学库的 DLL（`ZKIC_math_world2p5d.dll`）内部链接了 FFTW3（编译时 `HAVE_FFTW3`），因此运行时 ZKIC DLL 会去加载 `libfftw3-3.dll`。这意味着：
> - 代码中**禁止**直接 `#include <fftw3.h>` 或调用 `fftw_*` 函数 — 应通过 `<fft.h>` 的 `ZK::fftC2C` 等接口
> - `fftw/` 目录**不能删除** — 其中的 `libfftw3-3.dll` 是 ZKIC 数学库 DLL 的运行时依赖
> - CMakeLists.txt 中保留 FFTW 的链接（`libfftw3-3.lib`）和 DLL 复制（POST_BUILD），否则运行时会因找不到 DLL 而报错

**代码审查检查清单**：

- [ ] 复数向量：`ZK::cvec` 而非 `std::vector<std::complex<double>>`
- [ ] 实数向量：`ZK::vec` 而非 `std::vector<double>`
- [ ] FFT/IFFT：`ZK::fftC2C` / `ZK::ifftC2C` 而非原始 FFTW API（ZK 库内部已封装 FFTW）
- [ ] 卷积：`ZK::linearConvolution` 而非手写循环或直接调 FFTW
- [ ] 数学函数：`ZK::exp`/`ZK::log`/`ZK::sin`/`ZK::cos` 等（自动支持 vec/cvec/mat/cmat）
- [ ] 随机数：`ZK::randn`/`ZK::randu`/`ZK::randn_c` 而非 `std::random` 或 `rand()`
- [ ] 文件 I/O：`ZK::saveData`/`ZK::loadData` 而非 `std::ofstream`/`std::ifstream`
- [ ] dB 转换：`ZK::dB`/`ZK::inv_dB` 而非手写 `10*log10(x)`
- [ ] Q 函数/ERF：`ZK::Qfunc`/`ZK::erfc` 而非手写近似
- [ ] 物理常量：`ZK::PHY_C`/`ZK::PHY_H` 等，而非自定义常量或硬编码数字
- [ ] 逐元素运算：`ZK::elem_mult` 而非手写循环
- [ ] 类型转换：`ZK::to_cvec`/`ZK::to_vec` 等，而非手写循环

**替代对照表（写代码时必查）**：

| 类别                | ❌ 禁止使用                                       | ✅ 应使用                                                            | 头文件                    |
| ------------------- | ------------------------------------------------- | -------------------------------------------------------------------- | ------------------------- |
| 复数向量            | `std::vector<std::complex<double>>`             | `ZK::cvec`                                                         | `<vec.h>`               |
| 实数向量            | `std::vector<double>`                           | `ZK::vec`                                                          | `<vec.h>`               |
| 整数向量            | `std::vector<int>`                              | `ZK::ivec`                                                         | `<vec.h>`               |
| FFT（复数）         | `fftw_plan_dft_1d(...)` + `fftw_execute(...)` | `ZK::fftC2C(input)`                                                | `<fft.h>`               |
| IFFT（复数）        | `fftw_plan_dft_1d(..., BACKWARD)` + 手动 /N     | `ZK::ifftC2C(input)`                                               | `<fft.h>`               |
| FFT（实→复）       | `fftw_plan_dft_r2c_1d(...)`                     | `ZK::fftR2C(input)`                                                | `<fft.h>`               |
| 频域色散滤波        | 手写 FFTW + 手动乘 H(ω)                          | `ZK::fftC2C` + `ZK::elem_mult` + `ZK::ifftC2C`                 | `<fft.h>` + `<vec.h>` |
| 线性卷积            | 手写循环 /`fftw` + 乘法 + `ifft`              | `ZK::linearConvolution(x, h)`                                      | `<convolution.h>`       |
| 自相关/互相关       | 手写 FFT 实现                                     | `ZK::autoCorrelation` / `ZK::crossCorrelation`                   | `<fft.h>`               |
| 频谱搬移 (fftshift) | 手写数组索引交换                                  | `ZK::fftShift` / `ZK::fftShiftInPlace`                           | `<fft.h>`               |
| 功率谱              | 手写 `abs(fft).^2`                              | `ZK::fftPowerSpectrum(input, true)`                                | `<fft.h>`               |
| 指数 e^x            | `std::exp(x)` (仅标量)                          | `ZK::exp(v)` (支持 vec/cvec/mat/cmat)                              | `<log_exp.h>`           |
| 自然对数            | `std::log(x)` (仅标量)                          | `ZK::log(v)`                                                       | `<log_exp.h>`           |
| 平方                | `x*x` / 手写循环                                | `ZK::sqr(v)` (实数平方/复数模平方自动区分)                         | `<elem_math.h>`         |
| 绝对值/模           | `std::abs(x)` / 手写循环                        | `ZK::abs(v)`                                                       | `<elem_math.h>`         |
| 平方根              | `std::sqrt(x)`                                  | `ZK::sqrt(v)`                                                      | `<elem_math.h>`         |
| 正弦/余弦           | `std::sin(x)` / `std::cos(x)`                 | `ZK::sin(v)` / `ZK::cos(v)`                                      | `<trig_hyp.h>`          |
| 双曲正切            | `std::tanh(x)`                                  | `ZK::tanh(v)`                                                      | `<trig_hyp.h>`          |
| dB 转换             | `10.0 * std::log10(x)`                          | `ZK::dB(x)` / `ZK::inv_dB(x)`                                    | `<log_exp.h>`           |
| Q 函数              | 手写 `0.5*erfc(x/sqrt(2))`                      | `ZK::Qfunc(x)`                                                     | `<error.h>`             |
| 误差函数            | `std::erf(x)` (仅标量)                          | `ZK::erf(v)` (支持 vec/mat/cvec/cmat)                              | `<error.h>`             |
| 互补误差函数        | `std::erfc(x)` (仅标量)                         | `ZK::erfc(v)`                                                      | `<error.h>`             |
| 随机均匀            | `rand()/(double)RAND_MAX`                       | `ZK::randu()` / `ZK::randu(n)`                                   | `<random.h>`            |
| 随机正态            | Box-Muller 手写                                   | `ZK::randn()` / `ZK::randn(n)`                                   | `<random.h>`            |
| 复高斯噪声          | 手写 I/Q 两路                                     | `ZK::randn_c(n)`                                                   | `<random.h>`            |
| ASE 噪声生成        | `std::normal_distribution`                      | `ZK::Complex_Normal_RNG` 或 `ZK::randn_c(n)`                     | `<random.h>`            |
| 文件保存            | `std::ofstream` + 手写格式                      | `ZK::saveData(data, fileName, format, mode)`                       | `<fstreamIO.h>`         |
| 文件加载            | `std::ifstream` + 手写解析                      | `ZK::loadData<T>(fileName)`                                        | `<fstreamIO.h>`         |
| 逐元素乘法          | `for(i) c[i]=a[i]*b[i]`                         | `ZK::elem_mult(a, b)`                                              | `<vec.h>`               |
| 内积/点积           | 手写循环                                          | `ZK::dot(v1, v2)` 或 `v1 * v2`                                   | `<vec.h>`               |
| 最大值/最小值       | `std::max_element` / 手写                       | `ZK::max(v)` / `ZK::min(v)`                                      | `<min_max.h>`           |
| 排序                | `std::sort`                                     | `ZK::sort(v)`                                                      | `<sort.h>`              |
| 拼接向量            | 手写循环                                          | `ZK::concat(v1, v2, ...)`                                          | `<vec.h>`               |
| 复数的实部/虚部     | `z.real()`/`z.imag()` 逐个取                  | `ZK::real(cv)` / `ZK::imag(cv)` (整向量)                         | `<elem_math.h>`         |
| 复数共轭            | 手写循环                                          | `ZK::conj(cv)`                                                     | `<elem_math.h>`         |
| 类型转换            | 手写循环逐个 cast                                 | `ZK::to_cvec(v)` / `ZK::to_vec(v)` 等                            | `<converters.h>`        |
| 光速常量            | `299792458.0` 硬编码                            | `ZK::PHY_C`                                                        | `<common_values.h>`     |
| 普朗克常数          | `6.626e-34` 硬编码                              | `ZK::PHY_H`                                                        | `<common_values.h>`     |
| 取整                | `(int)std::floor(x)`                            | `ZK::floor_i(x)` / `ZK::round_i(x)`                              | `<converters.h>`        |
| 创建全零/全一向量   | 手写循环                                          | `ZK::zeros(n)` / `ZK::ones(n)` / `v.zeros()`                   | `<specmat.h>`           |
| 线性间隔            | 手写循环                                          | `ZK::linspace(from, to, n)`                                        | `<specmat.h>`           |
| 单位矩阵            | 手写循环                                          | `ZK::eye(n)`                                                       | `<specmat.h>`           |
| 解线性方程 Ax=b     | 手写求逆+乘法                                     | `ZK::ls_solve(A, b)` 或 `ZK::backslash(A, b)`                    | `<ls_solve.h>`          |
| 时间测量            | `std::chrono`                                   | `ZK::Real_Timer` / `ZK::CPU_Timer` / `ZK::tic()`/`ZK::toc()` | `<timing.h>`            |
| 数值积分            | 手写梯形/辛普森                                   | `ZK::quad(f, a, b)` / `ZK::quadl(f, a, b)`                       | `<integration.h>`       |

### 7.1 GlobalValue 系统（`<GlobalValue.h>`）

**用途**：统一管理仿真中的波段（Band）参数和全局信号（ESignal）配置。所有 Agent 通过 `GlobalValue::getBand(bandId)` 获取参数，避免参数散落在各 Agent 中。

#### 7.1.1 Band 结构体（波段参数）

`Band` 是核心参数结构，包含一个波段的所有物理层和链路层参数。通过 `BandBuilder` 构建，通过 `GlobalValue::getBand(bandId)` 读取。

**关键字段分类**：

| 类别         | 字段                     | 类型          | 说明                         |
| ------------ | ------------------------ | ------------- | ---------------------------- |
| 波段基本信息 | `band_ID`              | `string`    | 波段标识符（如 "band1"）     |
|              | `band_name`            | `string`    | 波段名称                     |
|              | `N_chans`              | `long long` | 信道数                       |
|              | `chan_spacing`         | `double`    | 信道间隔 (Hz)                |
|              | `fc_ref`               | `double`    | 参考信道频率 (Hz)            |
| 调制         | `pol_mux_mode`         | `string`    | 偏振复用模式                 |
|              | `modulation_format`    | `string`    | 调制格式 ("QPSK", "16QAM"…) |
|              | `N_pols`               | `long long` | 偏振数                       |
|              | `M_ary`                | `long long` | 调制阶数                     |
| 帧结构       | `N_bits_TS`            | `long long` | 训练序列比特数               |
|              | `N_TXbits_frame`       | `long long` | 每帧传输比特数               |
| 速率         | `Rb_infor`             | `double`    | 信息比特率 (bps)             |
|              | `Rb`                   | `double`    | 总比特率 (bps)               |
| 采样         | `N_per_symbol_default` | `long long` | 每符号默认采样数             |
|              | `sample_rate_default`  | `double`    | 默认采样率 (Hz)              |

**关键 getter 方法**：

```cpp
const Band& band = GlobalValue::getBand("band1");
band.getBandID();             // string
band.getNumChannels();        // long long
band.getChannelSpacing();     // double (Hz)
band.getRefChannelFrequency();// double (Hz)
band.getNumPolarizations();   // long long
band.getModulationOrder();    // long long
band.getInformationBitRate(); // double (bps)
band.getGrossBitRate();       // double (bps)
band.getDefaultSamplesPerSymbol(); // long long
band.getDefaultSampleRate();  // double (Hz)
// 计算某个信道的标准频率:
band.getStdChannelFrequency(channelIndex);  // fc_ref + spacing * (idx - idx_ref)
```

#### 7.1.2 BandBuilder（构建器）

使用链式调用构建 Band 对象：

```cpp
Band band = BandBuilder()
    .setBandIDName("band1", "testBand1")
    .setChannelGrid(3, 50e9, chan_idx, interest_idx)
    .setRefChannel(1, 190.1e12)
    .setModulation("sgl_pol", "QPSK", 1, 4)
    .setRates(1e11, 1.268e11, 1.268e11)
    .setSampling(16, 1.014e12)
    .build();
```

#### 7.1.3 ESignal / ESignalBuilder（全局信号配置）

```cpp
ESignal eSignal = ESignalBuilder()
    .setRefBandInfo("band1", totalBits, nFramesClkPerPol, bandCount)
    .setPropagation("fiberLoss", decimationFactor)  // prop_mode, M_deci
    .addBand(band)
    .build();
```

#### 7.1.4 GlobalValue（全局注册中心）

```cpp
// 注册配置
GlobalValue::registerConfig("taskId", eSignal);
// 选择当前使用的配置
GlobalValue::selectESignal("taskId");
// 获取当前激活的 Band/ESignal
const Band& band = GlobalValue::getBand("band1");
const ESignal& esig = GlobalValue::getESignal();
// 移除配置
GlobalValue::removeESignal("taskId");
```

#### 7.1.5 线程安全

`GlobalValue` 内部使用 `ThreadSafeResources` (CRITICAL_SECTION) + `ScopedLock` 实现线程安全访问。

---

### 7.2 ZKIC 数学库 API（`<vec.h>`, `<fft.h>`, ...）

#### 7.2.1 FFT 模块（`<fft.h>`）

基于 FFTW3，提供线程局部 FFTW plan 缓存（`ThreadLocalFFTCache`）、SIMD 加速的数据拷贝（`vectorizedCopy2FFTW/FromFFTW`）、32 字节对齐内存分配（`AlignedFFTWAllocator`）。所有函数在 `ZK` 命名空间。

| 函数                       | 签名                                                | 说明                         |
| -------------------------- | --------------------------------------------------- | ---------------------------- |
| `fftC2C`                 | `cvec fftC2C(const cvec& inputSignal)`            | 1D 复数 FFT（正向）          |
| `ifftC2C`                | `cvec ifftC2C(const cvec& input)`                 | 1D 复数 IFFT（已归一化）     |
| `fftR2C`                 | `cvec fftR2C(const vec& input)`                   | 实数→复数 FFT（返回全频带） |
| `ifftC2R`                | `vec ifftC2R(const cvec& input)`                  | 复数→实数 IFFT（已归一化）  |
| `fftC2CInPlace`          | `void fftC2CInPlace(cvec& data)`                  | 原地复数 FFT                 |
| `fft2C2C`                | `cmat fft2C2C(const cmat& input)`                 | 2D 复数 FFT                  |
| `ifft2C2C`               | `cmat ifft2C2C(const cmat& input)`                | 2D 复数 IFFT（已归一化）     |
| `fftPowerSpectrum`       | `vec fftPowerSpectrum(const cvec&, bool)`         | 功率谱密度（可选先 FFT）     |
| `fftShift`               | `cvec fftShift(const cvec& input)`                | 频谱搬移（零频移到中心）     |
| `fftShiftInPlace`        | `void fftShiftInPlace(cvec& data)`                | 原地频谱搬移                 |
| `batchDftC2C`            | `std::vector<cvec> batchDftC2C(const Vec<cvec>&)` | 批量 FFT（需等长）           |
| `crossCorrelation(x, y)` | `cvec/vec crossCorrelation(const cvec/vec&, ...)` | 互相关（复数/实数重载）      |
| `autoCorrelation(x)`     | `cvec/vec autoCorrelation(const cvec/vec&)`       | 自相关（复数/实数重载）      |
| `cleanupThreadFFT`       | `void cleanupThreadFFT()`                         | 清理当前线程 FFT 缓存        |

**关键注意**：

- 逆变换 `ifftC2C` 和 `ifftC2R` **已自动除以 N 归一化**
- `fftR2C` 返回完整的 N 点复数频谱（非 Hermitian 对称减半），可直接用于频域滤波
- FFT plan 缓存在每个线程独立，首次调用时自动创建，线程结束时需手动调用 `cleanupThreadFFT()` 或用 `FFTWManager::cleanup()`

**FFTW 初始化**（程序启动时执行一次）：

```cpp
FFTWManager::initialize();  // 初始化线程支持 + planner 线程安全 + 导入 wisdom
// ... 程序运行 ...
FFTWManager::cleanup();     // 导出 wisdom + 清理
```

#### 7.2.2 卷积模块（`<convolution.h>`）

| 函数                          | 签名                                   | 说明                                 |
| ----------------------------- | -------------------------------------- | ------------------------------------ |
| `linearConvolution(x, h)`   | `vec/cvec linearConvolution(...)`    | 线性卷积（自动选择算法），结果 N+M-1 |
| `circularConvolution(x,h)`  | `vec/cvec circularConvolution(...)`  | 循环卷积（基于 FFT），结果 max(N,M)  |
| `linearConvolutionfft(x,h)` | `vec/cvec linearConvolutionfft(...)` | 强制使用 FFT 的线性卷积              |

**算法选择策略**：数据量小 → 直接时域计算；数据量大 → FFT 加速。

#### 7.2.3 逐元素数学（`<elem_math.h>`）

| 函数                                           | 说明               | 支持类型                     |
| ---------------------------------------------- | ------------------ | ---------------------------- |
| `sqr(x)`                                     | 平方/模平方 (复数) | scalar, vec, mat, cvec, cmat |
| `abs(x)`                                     | 绝对值/模          | scalar, vec, mat, cvec, cmat |
| `sqrt(x)`                                    | 平方根             | scalar, vec, mat             |
| `sign(x)`                                    | 符号函数           | scalar, vec, mat             |
| `real(z)`/`imag(z)`/`arg(z)`/`conj(z)` | 复数分量提取       | cvec, cmat                   |
| `fact(n)`                                    | 阶乘 (n≤170)      | int                          |
| `binom(n,k)`                                 | 二项式系数         | int                          |
| `gcd(a,b)`                                   | 最大公约数         | int                          |

#### 7.2.4 对数/指数/dB（`<log_exp.h>`）

| 函数                                    | 说明                           |
| --------------------------------------- | ------------------------------ |
| `exp(x)`                              | e^x（支持 vec/cvec/mat/cmat）  |
| `log(x)` / `log2(x)` / `log10(x)` | 对数（支持 vec/cvec/mat/cmat） |
| `pow(x, y)`                           | x^y                            |
| `pow2(x)` / `pow10(x)`              | 2^x / 10^x                     |
| `dB(x)` / `inv_dB(x)`               | 10·log10 / 10^(x/10)          |
| `trunc_log(x)` / `trunc_exp(x)`     | 截断版（防上溢/下溢）          |
| `int2bits(n)` / `levels2bits(n)`    | 整数→所需比特数               |

#### 7.2.5 三角/双曲函数（`<trig_hyp.h>`）

| 函数        | 说明     | 函数         | 说明       |
| ----------- | -------- | ------------ | ---------- |
| `sin(x)`  | 正弦     | `asin(x)`  | 反正弦     |
| `cos(x)`  | 余弦     | `acos(x)`  | 反余弦     |
| `tan(x)`  | 正切     | `atan(x)`  | 反正切     |
| `sinc(x)` | sinc(x)  |              |            |
| `sinh(x)` | 双曲正弦 | `asinh(x)` | 反双曲正弦 |
| `cosh(x)` | 双曲余弦 | `acosh(x)` | 反双曲余弦 |
| `tanh(x)` | 双曲正切 | `atanh(x)` | 反双曲正切 |

所有函数支持 scalar、vec、mat。双精度。

#### 7.2.6 最大/最小值（`<min_max.h>`）

```cpp
double m = max(v);            // 向量最大值
double m = max(v, index);     // 最大值 + 位置
vec m = max(mat, dim);        // dim=1 → 每列最大; dim=2 → 每行最大
int idx = max_index(v);       // 最大值位置
// min/max/min_index 同理
```

#### 7.2.7 排序（`<sort.h>`）

```cpp
sort(v);                      // 升序排列（Introsort 默认）
sort(v, QUICKSORT);           // 指定排序算法
ivec idx = sort_index(v);     // 返回排序索引（不修改原向量）
```

#### 7.2.8 随机数（`<random.h>`）

**全局种子管理**：

```cpp
GlobalRNG_reset(seed);        // 设置全局种子
RNG_reset(seed);              // 设置当前线程种子
RNG_randomize();              // 随机化种子
```

**常用生成函数**：

```cpp
double u = randu();           // 均匀 [0, 1)
vec u = randu(n);             // 均匀向量
double n = randn();           // 标准正态 N(0,1)
vec n = randn(n);             // 正态向量
cvec cn = randn_c(n);         // 复正态 (实虚独立，各方差 0.5)
int ri = randi(low, high);    // 随机整数
bvec b = randb(n);            // 伯努利 (等概率 0/1)
```

**分布类**（用于需要独立实例的场景）：

| 类                     | 说明       | 构造参数              |
| ---------------------- | ---------- | --------------------- |
| `Uniform_RNG`        | 均匀分布   | (min=0, max=1)        |
| `Normal_RNG`         | 正态分布   | (mean=0, variance=1)  |
| `Complex_Normal_RNG` | 复正态分布 | (mean, variance)      |
| `Bernoulli_RNG`      | 伯努利     | (prob=0.5)            |
| `Exponential_RNG`    | 指数分布   | (lambda=1.0)          |
| `Laplace_RNG`        | 拉普拉斯   | (mean=0, variance=1)  |
| `Rayleigh_RNG`       | 瑞利分布   | (sigma=1.0)           |
| `Rice_RNG`           | 莱斯分布   | (sigma=1.0, v=1.0)    |
| `Gamma_RNG`          | 伽马分布   | (alpha=1.0, beta=1.0) |

#### 7.2.9 误差函数（`<error.h>`）

| 函数          | 支持类型                     | 说明                                      |
| ------------- | ---------------------------- | ----------------------------------------- |
| `erf(x)`    | scalar, vec, mat, cvec, cmat | 误差函数（复数参数支持）                  |
| `erfc(x)`   | scalar, vec, mat             | 互补误差函数 `1-erf(x)`                 |
| `erfinv(x)` | scalar, vec, mat             | 逆误差函数（输入 ∈ [-1,1]）              |
| `Qfunc(x)`  | scalar, vec, mat             | Q 函数（标准正态右尾概率，用于 BER 计算） |

```cpp
double ber = Qfunc(sqrt(2.0 * SNR));  // 理论 BER（QPSK）
vec bers = Qfunc(sqrt(2.0 * SNR_vec)); // 向量化 BER 计算
```

#### 7.2.10 通信辅助（`<commfunc.h>`）

```cpp
bmat graycode(int m);                   // m 位格雷码表 (2^m × m)
int hammingDistance(const bvec&, const bvec&);  // 汉明距离
int weight(const bvec&);                // 汉明重量 (1 的个数)
vec waterfilling(const vec& alpha, double P);   // 注水法功率分配
double euclideanDistance(const vec&, const vec&); // 欧几里得距离
```

#### 7.2.11 线性代数模块一览

所有线性代数模块依赖 LAPACK，位于 ZK 命名空间：

| 模块     | 头文件           | 关键函数                                                                                                      |
| -------- | ---------------- | ------------------------------------------------------------------------------------------------------------- |
| 线性求解 | `<ls_solve.h>` | `ls_solve(A,b)` / `ls_solve_chol(A,b)` / `ls_solve_od(A,b)` / `ls_solve_ud(A,b)` / `backslash(A,b)` |
| Cholesky | `<cholesky.h>` | `chol(X, F)` / `chol(X)`（对称正定）                                                                      |
| LU 分解  | `<lu.h>`       | `lu(X, L, U, p)`                                                                                            |
| QR 分解  | `<qr.h>`       | `qr(X, Q, R)`                                                                                               |
| 特征值   | `<eigen.h>`    | `eig_sym(X, d, V)`                                                                                          |
| SVD      | `<svd.h>`      | `svd(X, U, S, V)`                                                                                           |
| 求逆     | `<inv.h>`      | `inv(X)` / `inv(X, Y)`                                                                                    |
| 行列式   | `<det.h>`      | `det(X)`（返回 double/complex`<double>`）                                                                 |
| 矩阵函数 | `<matfunc.h>`  | `expm(X)`, `logm(X)`, `sqrtm(X)`, `powm(X)`, `cosm(X)`, `sinm(X)`                                 |

**`<ls_solve.h>` 详细 API**（所有均有 `bool` 返回 + 直接返回两种重载，支持 mat/cmat）：

```cpp
// 方阵求解 Ax=b（LU 分解）
vec x = ls_solve(A, b);
// Cholesky 求解（对称/Hermitian 正定，更快）
vec x = ls_solve_chol(A, b);
// 超定方程（最小二乘，m≥n，QR 分解）
vec x = ls_solve_od(A, b);
// 欠定方程（最小范数，m≤n，LQ 分解）
vec x = ls_solve_ud(A, b);
// 通用求解器（仿 MATLAB backslash，自动选择方法）
vec x = backslash(A, b);
// 前向/后向代入
vec x = forward_substitution(L, b);   // Lx = b, L 下三角
vec x = backward_substitution(U, b);  // Ux = b, U 上三角
```

#### 7.2.12 文件 I/O（`<fstreamIO.h>`）

**文件格式**（`mode::FileType`）：`Mat`（MATLAB v5）、`Zat`（ZK 自定义二进制，Magic=ZKIO）、`Dat`、`Txt`（文本，`#` 开头为注释）、`Csv`

**写入模式**（`mode::WriteMode`）：`OverWrite`（覆盖）、`Append`（追加）

**saveData**（支持 `Vec<T>` / `Mat<T>` / 裸指针）：

```cpp
// 保存向量
saveData(signal, "output.mat", mode::Mat, mode::OverWrite);
saveData(signal, "output.csv", mode::Csv);
// 跳过保存的条件：
// params.saveFile == "" 或 params.saveFile == "noSave"
```

**loadData**（自动检测文件类型）：

```cpp
vec data = loadData<double>("input.dat");         // 自动检测 count
vec data = loadData<double>("input.dat", 100);    // 指定 count
mat m = loadDatMat<double>("matrix.dat");          // 加载矩阵
```

**createFolder**：`bool createFolder(const string& folderPath)`

#### 7.2.13 物理常量（`<common_values.h>`）

| 常量       | 值              | 说明       |
| ---------- | --------------- | ---------- |
| `PHY_C`  | 299792458.0     | 光速 (m/s) |
| `PHY_H`  | 6.62607015e-34  | 普朗克常数 |
| `PHY_KB` | 1.380649e-23    | 玻尔兹曼   |
| `PHY_QE` | 1.602176634e-19 | 电子电荷   |
| `MA_PI`  | 3.14159265...   | π         |
| `MA_E`   | 2.71828182...   | e          |

#### 7.2.14 类型转换 + 取整（`<converters.h>`）

**类型转换**（Vec/Mat 之间的完整转换矩阵）：

```cpp
to_bvec(v) / to_svec(v) / to_ivec(v) / to_vec(v) / to_cvec(v)  // Vec<T> → 目标类型
to_cvec(realPart, imagPart)    // 实部+虚部 → cvec
to_bmat(m) / to_smat(m) / to_imat(m) / to_mat(m) / to_cmat(m)  // Mat<T> → 目标类型
```

**取整函数**：

| 函数                          | 说明                                 |
| ----------------------------- | ------------------------------------ |
| `round(x)` / `round_i(x)` | 四舍五入 → vec/ivec（支持 vec/mat） |
| `floor(x)` / `floor_i(x)` | 向下取整 → vec/ivec（支持 vec/mat） |
| `ceil(x)` / `ceil_i(x)`   | 向上取整 → vec/ivec（支持 vec/mat） |

**数值处理**：

```cpp
round_to_zero(x, threshold)  // |x| < threshold → 0（支持 vec/cvec/mat/cmat）
round_to_infty(x, threshold) // |x| > threshold → round(x)（去浮点尾数）
```

**数制转换**：

```cpp
bvec bits = dec2bin(length, index);  // 十进制 → 二进制（指定位宽）
int val = bin2dec(bvec, msb_first);  // 二进制 → 十进制
bvec gray = dec2bin(gray_code(x));   // 格雷码（也可直接用 converters 中的 gray_code()）
```

**角度转换**：`rad_to_deg(x)`, `deg_to_rad(x)`

#### 7.2.15 特殊矩阵生成（`<specmat.h>`）

```cpp
// 基本矩阵
vec  v = zeros(n);            // 全零向量（_b=二进制, _i=int, _c=复数）
vec  v = ones(n);             // 全一向量
mat  m = zeros(r, c);         // 全零矩阵
mat  I = eye(n);              // 单位矩阵
vec  imp = impulse(n);        // 脉冲向量 [1,0,0,...]

// 等间隔
vec x = linspace(from, to, n);          // 线性等分 (默认 100 点)
vec x = linspace_fixed_step(0, 10, 0.5);// 固定步长 (0:0.5:10)
vec x = zigzag_space(t0, t1, K);        // Zig-zag 采样 (1+2^K 点)

// 特殊矩阵
imat H = hadamard(n);         // Hadamard 矩阵 (n = 2^k)
imat J = jacobsthal(p);       // Jacobsthal 矩阵 (p 素数)
imat C = conference(n);       // Conference 矩阵
mat  T = toeplitz(c, r);      // Toeplitz 矩阵 (对称: 省略 r)
cmat T = toeplitz(cvec);      // 复数 Toeplitz

// 查找非零元素
ivec indices = find(bvec);    // 返回 bvec==1 的所有位置

// Householder/Givens 旋转
house(x, v, beta);            // Householder 向量
givens(a, b, c, s);           // Givens 旋转参数
mat G = givens(a, b);         // Givens 旋转矩阵
mat Q = rotation_matrix(dim, p1, p2, angle);  // 平面旋转矩阵

// 便捷构造
vec v = vec_1(a);             // {a}
vec v = vec_2(a, b);          // {a, b}
vec v = vec_3(a, b, c);       // {a, b, c}
mat m = mat_2x2(a,b, c,d);    // 2x2 矩阵 [a b; c d]
mat m = mat_3x3(a,b,c, d,e,f, g,h,i); // 3x3 矩阵
// 还有 mat_1x1, mat_1x2, mat_2x1, mat_1x3, mat_3x1, mat_2x3, mat_3x2
```

#### 7.2.16 贝塞尔函数（`<bessel.h>`）

光纤模式分析中可能用到。支持 scalar 和 vec 输入。

| 函数               | 说明                                    |
| ------------------ | --------------------------------------- |
| `besselj(nu, x)` | 第一类贝塞尔 J_ν（nu 支持 int/double） |
| `bessely(nu, x)` | 第二类贝塞尔 Y_ν（nu 支持 int/double） |
| `besseli(nu, x)` | 修正第一类贝塞尔 I_ν                   |
| `besselk(nu, x)` | 修正第二类贝塞尔 K_ν（nu 仅 int）      |

#### 7.2.17 数值积分（`<integration.h>`）

```cpp
// 自适应 Simpson 积分
double val = quad(functor, a, b, tol);   // functor 方式（默认 tol=eps）
double val = quad(funcPtr, a, b, tol);   // 函数指针方式
// 自适应 Lobatto 积分（更高精度）
double val = quadl(functor, a, b, tol);
double val = quadl(funcPtr, a, b, tol);
```

#### 7.2.18 计时器（`<timing.h>`）

```cpp
// 全局计时
tic();                        // 开始计时
double t = toc();             // 返回已用时间（秒）
toc_print();                  // 打印已用时间
pause(seconds);               // 暂停 n 秒（无参则等待按键）

// 面向对象计时器
Real_Timer timer;             // 挂钟时间（真实时间）
CPU_Timer timer;              // CPU 时间（进程耗用）
timer.tic();                  // 复位 + 开始
double t = timer.toc();       // 返回已用时间
timer.start();                // 开始（不归零）
double t = timer.stop();      // 停止 + 返回
double t = timer.get_time();  // 获取时间（不停止）
timer.reset();                // 归零 + 停止
```

#### 7.2.19 参数解析器（`<parser.h>`）

MATLAB 兼容语法的参数文件解析。在 `format_ref/PRBSlfsr.h` 中有典型使用示例。

```cpp
Parser p("config.txt");                    // 从文件读取
Parser p(argc, argv);                      // 从命令行读取
Parser p("config.txt", argc, argv);        // 合并（命令行覆盖文件）

p.set_silentmode(true);                    // 静默模式（不 echo 变量）
bool exists = p.exist("varname");          // 检查变量是否存在

// 类型安全读取
double d = p.get_double("snr_db");         // 读取 double
int    n = p.get_int("N_syms");            // 读取 int
bool   b = p.get_bool("enable_dbp");       // 读取 bool
string s = p.get_string("mode");           // 读取 string
vec    v = p.get_vec("freqs");             // 读取 vec
ivec   i = p.get_ivec("indices");           // 读取 ivec
mat    m = p.get_mat("matrix");            // 读取 mat

// 泛型模板（支持保留默认值）
my_custom_type var = default_value;
p.get(var, "varname");                     // 找不到保留旧值
```

**配置文件语法**（与 MATLAB m 文件兼容）：

```
snr_db = 12.5;
N_syms = 32768;
freqs = [190.1e12, 190.15e12, 190.2e12];
enable_dbp = true;
mode = "constant";
matrix = [1 2; 3 4];
% 注释以 % 开头
```

#### 7.2.20 快速矩阵运算（`<fastmath.h>`）

```cpp
sub_v_vT_m(m, v);   // m = m - v*v'*m   (要求 v.size() == m.rows())
sub_m_v_vT(m, v);   // m = m - m*v*v'   (要求 v.size() == m.cols())
```

#### 7.2.21 杂项工具（`<misc.h>`）

```cpp
bool is_int(double x);              // x 是否为整数
bool is_even(int x);                // x 是否为偶数
bool big = is_bigendian();          // 系统是否为大端序
std::string ver = itpp_version();   // IT++ 库版本号
// 常量: pi, m_2pi (2*pi), eps (机器精度)
```

#### 7.2.22 混合类型运算符（`<operators.h>`）

提供不同类型之间的算术运算符重载，实现 `vec + int`、`mat * cvec` 等混合运算。无需显式 include（被 `<vec.h>` / `<mat.h>` 间接引入），但写混合类型表达式时需要知道其存在。

**支持的混合运算**：

| 运算 | 示例 |
|------|------|
| `vec` 与标量 | `vec v2 = v + 3.5;` / `vec v2 = 2 * v;` / `v += 5;` |
| `cvec` 与标量 | `cvec cv2 = cv + 2.0;` / `cvec cv2 = 3.0 * cv;` |
| `ivec` 与 double | `vec v = 2.5 + iv;`（结果升级为 vec） |
| `mat` 与标量 | `mat m2 = m + 1.0;` / `m *= 2.0;` |
| `cmat` 与标量 | `cmat cm2 = cm + 3.0;` |
| `vec` 间不同类型 | `vec v = bvec + vec;` / `cvec cv = ivec + cvec;` |
| `mat` 间不同类型 | `mat m = bmat + mat;` / `cmat cm = mat * cmat;` |
| 复数与实数的混合 | `std::complex<double> z = 5 + c;`（int/float → complex 自动升级） |

---

### 7.3 数学库用户手册与头文件对照

> CHM 手册 `docs/数学库用户手册.chm` 是数学库的正式说明文档。以下为"头文件 ↔ 手册模块"对应关系和关键补充信息。

**手册覆盖的 9 大模块**：

1. **信号处理**：卷积 (`<convolution.h>`) + FFT (`<fft.h>`)
2. **基础类型和数据结构**：Vec, Sparse_Vec, bin, Array, Stack, Mat, Sparse_Mat, Circular_Buffer
3. **数值计算**：elem_math, log_exp, matfunc, min_max, operators, sort, trig_hyp
4. **文件 I/O**：fstreamIO（5 种格式的 saveData/loadData）
5. **特殊函数**：Airy, Bessel, Gamma, hyperg, Struve
6. **线性代数**：Cholesky, Eigen, LU, QR, Schur, SVD, ls_solve, det, inv（依赖 LAPACK）
7. **工具函数**：commfunc, common_values, converters, error, fastmath, help_functions, parser, timing
8. **概率与统计**：integration, random

**手册中提及但代码中不常用/已过时的内容**：`Stack`, `Array`, `Circular_Buffer` — 仅在特殊场景使用；`Sparse_Vec`/`Sparse_Mat` — 大规模稀疏数据时使用（密度 > 30% 应使用密集类型）。

**与头文件的关键差异**：

- 手册中描述 Vec 的 `set_size` 用 `false` 表示不保留旧值、`true` 表示保留
- `bin` 类型的 `+` = XOR（加法即异或）、`/` = OR（除法即或运算）——这是 GF(2) 域的标准设计
- `sort_index` 返回排序后的索引（不修改原向量）

### 7.4 关键注意事项（易错点汇总）

> 以下是从头文件和 CHM 手册中提取的容易用错的关键点。

| 类别 | 注意事项 |
|------|---------|
| **FFT 归一化** | `ifftC2C` / `ifftC2R` 已自动除以 N，无需手动归一化 |
| **FFT 实数变换** | `fftR2C` 返回**完整 N 点**复数频谱，而非 FFTW 默认的 `N/2+1` 半频谱。可直接用于频域滤波和 `elem_mult` |
| **批量 FFT** | `batchDftC2C` 要求所有输入 cvec **长度相同** |
| **卷积长度** | 线性卷积结果 = N+M-1；循环卷积结果 = max(N,M)。要循环卷积等价线性，需手动补零到 N+M-1 |
| **复高斯噪声** | `randn_c(n)` 实部和虚部**独立**，**各方差 0.5**（总方差=1）。要产生指定功率的复噪声，乘以 `sqrt(P/2)` |
| **正态分布参数** | `Normal_RNG(mean, variance)` 第二个参数是**方差**不是标准差 |
| **随机种子时机** | `GlobalRNG_reset(seed)` 必须在任何 `Random_Generator` 对象**构造之前**调用（最好在 `main()` 最开头） |
| **set_size** | `set_size(n, false)` = 不保留旧值；`set_size(n, true)` = 保留旧值 |
| **bin 运算** | `+`/`-`/`^` = XOR（非算术加法），`*`/`&` = AND，`/`/`|` = OR |
| **Q 函数** | `Qfunc(x)` = 0.5·erfc(x/√2)，标准正态右尾概率。`Qfunc(sqrt(2*SNR))` = QPSK 理论 BER |
| **dB 函数** | `dB(x)` = 10·log10(x)（功率比），`inv_dB(x)` = 10^(x/10) |
| **文件 I/O 跳过** | `saveFile = ""` 或 `"noSave"` 时跳过保存 |
| **FFTW 依赖** | 代码禁止 `#include <fftw3.h>`，但 `libfftw3-3.dll` 必须在 exe 目录（ZKIC DLL 运行时加载） |
| **LAPACK 依赖** | 线性代数模块（inv/det/chol/lu/qr/svd/eigen/ls_solve）依赖 LAPACK，已编译在 ZKIC 数学库中 |
| **频率轴** | ZKIC 库**不提供** `fftfreq` 等效函数。频域滤波时需手动构造频率网格：`vec f = linspace(-Fs/2, Fs/2 - Fs/N, N)` 配合 `ZK::fftShift` 使用 |

---

## 8. .gitignore 策略

```
同步到 git：
  ✓ *.cpp, *.h, CMakeLists.txt, .vscode/, .gitignore, claude.md
  ✓ py_ref/, format_ref/, docs/  （参考代码和文档）
  ✓ start_vsc.bat
  ✓ fftw/                         （FFTW DLL，ZKIC 数学库运行时依赖，必须提交）

不提交（本地生成 / 手动拷贝）：
  ✗ build/                        （CMake 构建输出）
  ✗ x64/, Win32/                  （旧 VS 构建输出）
  ✗ .vs/, *.sln, *.vcxproj*      （VS 工程文件，CMake 替代）
  ✗ *.dat, *.tmp, __pycache__/
  ✗ ZKIC_lib/                     （ZKIC 库，.lib/.dll 手动复制到每台电脑）
```

> ZKIC_lib 不通过 git 同步。在新电脑上需手动将 ZKIC_lib 目录复制到项目根目录下。CMakeLists.txt 使用 `${CMAKE_CURRENT_SOURCE_DIR}` 相对路径引用，只要目录结构一致即可编译。

---

## 9. 当前进度

- [X] 项目文件替换（旧文件 → fiber_WDM.* 归档）
- [X] VSCode + CMake 开发环境搭建完成
- [X] CMakeLists.txt 配置完毕（自动文件发现 + CONFIGURE_DEPENDS）
- [X] 第三方库（FFTW, ZKIC, GlobalValue）集成到项目内
- [X] start_vsc.bat 多电脑自动适配（vswhere + 常见路径回退）
- [X] ZKIC 公司库 API 文档整理（GlobalValue + 数学库全部头文件 + CHM 手册）
- [X] 技术决策确认（DP_Fiber/DP_DBP 命名、双偏振、参数方式、gitignore 策略）
- [X] SSFM_Core + DP_Fiber + DP_DBP 实现（Agent 模式 + ZK 类型 + Manakov 8/9 修正 + 双偏振）
- [X] DP_DBPTest.cpp 测试套件（15/15 通过，4 类测试信号含 QPSK）
- [X] DP_DBPTest.m MATLAB 可视化（Part A: Fiber + Part B: DBP + QPSK 星座图）
- [X] 参数命名修正（camelCase：lSpan, groupRef, dispersion, disS, aEff, bWdm, nSpans, gLin）
- [X] 注释统一为英文，文件头模板更新
- [ ] 后续（按需）：EDFA Agent、PMD、GlobalValue 迁移、测试文档 _tests.docx、程序原理说明文档

---

## 10. 已决策事项与待确认问题

### 10.1 已决策

| 事项 | 决策 |
|------|------|
| DBP 架构 | **独立 Agent 类**，不与 Fiber 类耦合 |
| 双偏振 | **必须支持** X/Y 双偏振 |
| PMD | **暂不实现**（与 Python 一致，系数=0） |
| EDFA | **暂不实现** |
| 类名 | **`DP_Fiber`** / **`DP_DBP`**（已确认，符合 PascalCase 规范） |
| 参数传递 | **先用独立变量定义**，但命名和结构尽量兼容 GlobalValue 的 Band 字段 |
| ZKIC_lib 同步 | **不通过 git 同步**，新电脑手动复制 ZKIC_lib 目录到项目根目录 |
| FFTW | `fftw/` 必须保留（ZKIC DLL 运行时依赖），代码中禁止直接调 FFTW API |
| SSFM_Core | **非 Agent 工具类**，不含 Parameters/Signals/execute，供 DP_Fiber/DP_DBP 共享 |

### 10.2 仍需确认

1. **GlobalValue 迁移时间线**：当前用独立 Parameters 结构体，后续需迁移到 GlobalValue Band 系统。是否有明确时间线？
2. **EDFA Agent**：DP_DBP 中已有 gLin 参数（默认 1.0=透明），后续是否需要独立 EDFA Agent？
3. **交付物完善**：debug/ 文件夹、_tests.docx 文档、程序原理说明文档——是否现在准备？

---

---

## 附：C++ 项目 VSCode + CMake 环境配置流程（CC 执行手册）

> 当需要为新 C++ 项目或从零配置 VSCode + CMake 环境时，按以下步骤执行。

### A. 前提条件检测

在开始配置前，确认以下条件：

1. 编译器：项目使用 **MSVC**（Visual Studio 2022），x64 平台
2. .lib 文件：确认为 MSVC 编译产物（不能混用 MinGW 的 .lib）
3. IDE：VSCode + 扩展 `C/C++` + `CMake Tools` + `Claude Code`
4. 启动方式：通过 `start_vsc.bat` 启动（自动调用 `vcvars64.bat` 设置 MSVC 环境）

### B. 新建 .vscode/ 四个配置文件

创建以下 4 个文件（放在项目根目录的 `.vscode/` 下）：

#### B.1 `c_cpp_properties.json`

```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                // <<< 按项目实际包含的库添加，示例: >>>
                "${workspaceFolder}/fftw",
                "${workspaceFolder}/ZKIC_lib/ZKIC_mathworld/ZKIC_mathworld2p5/include",
                "${workspaceFolder}/ZKIC_lib/GlobalValueForZD/include"
            ],
            "defines": ["_DEBUG", "_CONSOLE"],
            "windowsSdkVersion": "DETECT_AFTER_CMAKE_CONFIGURE",
            "compilerPath": "DETECT_VIA_WHERE_CL_IN_VCVARS_SHELL",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-msvc-x64",
            "configurationProvider": "ms-vscode.cmake-tools"
        }
    ],
    "version": 4
}
```

- `includePath`：列出所有第三方库 include 目录（项目特定，迁移到新项目时替换）
- `compilerPath`：在 `start_vsc.bat` 启动的终端中执行 `where cl.exe` 获取完整路径后填入（如 `D:/Microsoft Visual Studio/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe`）
- `windowsSdkVersion`：执行一次 `cmake configure` 后，从 CMake 输出中获取 SDK 版本号后填入（如 `10.0.26100.0`）
- 如果 `configurationProvider` 设为 `ms-vscode.cmake-tools`，CMake Tools 会自动接管大部分 IntelliSense 配置，此文件中 `compilerPath` 和 `windowsSdkVersion` 可省略

#### B.2 `tasks.json`

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "CMake: 构建",
            "type": "cmake",
            "command": "build",
            "group": { "kind": "build", "isDefault": true }
        },
        {
            "label": "CMake: 配置",
            "type": "cmake",
            "command": "configure"
        },
        {
            "label": "CMake: 清理并重新构建",
            "type": "cmake",
            "command": "cleanRebuild"
        }
    ]
}
```

#### B.3 `launch.json`

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "C++ 调试 (MSVC)",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${command:cmake.launchTargetPath}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "console": "integratedTerminal",
            "preLaunchTask": "CMake: 构建"
        }
    ]
}
```

关键：`program` 用 `${command:cmake.launchTargetPath}` 自动定位 .exe；`preLaunchTask` 确保调试前自动编译。

#### B.4 `settings.json`

```json
{
    "cmake.generator": "Visual Studio 17 2022",
    "cmake.buildDirectory": "${workspaceFolder}/build",
    "cmake.configureOnOpen": true,
    "cmake.parallelJobs": 8,
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
}
```

### C. 创建/更新 CMakeLists.txt

按以下模板创建（`<<< >>>` 标记的部分为项目特定内容，需根据实际库依赖替换或删除）：

```cmake
cmake_minimum_required(VERSION 3.22)
project(<<<PROJECT_NAME>>> LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动发现根目录源文件（CONFIGURE_DEPENDS 自动感知增删，禁止用 GLOB_RECURSE）
file(GLOB SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# <<< 第三方库路径 — 按项目实际情况增删 >>>
# 全部用 CMAKE_CURRENT_SOURCE_DIR 相对路径，保证多电脑兼容
set(<<<LIBNAME>>>_DIR "${CMAKE_CURRENT_SOURCE_DIR}/<<<lib_dir>>>")
set(<<<LIBNAME>>>_INCLUDE "${<<<LIBNAME>>>_DIR}/include")
set(<<<LIBNAME>>>_LIB "${<<<LIBNAME>>>_DIR}/lib/<<<library>>>.lib")

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${<<<LIBNAME>>>_INCLUDE})

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    "${<<<LIBNAME>>>_LIB}"
)

# <<< 复制 DLL 到输出目录 — 为每个 .dll 添加一行 >>>
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${<<<LIBNAME>>>_DIR}/<<<library>>>.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:_DEBUG> _CONSOLE
    )
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /utf-8)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif()
```

**本项目实际使用的 CMakeLists.txt 参考**（ZKIC 项目）：
- FFTW: `fftw/` → `libfftw3-3.lib` + `libfftw3-3.dll`
- ZKIC 数学库: `ZKIC_lib/ZKIC_mathworld/ZKIC_mathworld2p5/` → `ZKIC_math_world2p5d.lib` + `.dll`
- GlobalValue: `ZKIC_lib/GlobalValueForZD/` → `signals_globalvalue_world1p1d.lib` + `.dll`

关键规则：
- `file(GLOB ... CONFIGURE_DEPENDS)` — 禁止使用 `GLOB_RECURSE`（会扫入子目录旧文件）
- 所有 .lib/.dll 路径使用 `${CMAKE_CURRENT_SOURCE_DIR}` 相对引用
- DLL 通过 `add_custom_command(POST_BUILD)` 复制

### D. 创建 start_vsc.bat

```batch
@echo off
chcp 65001 > nul
setlocal enabledelayedexpansion

echo ============================================
echo   Launching VSCode - <PROJECT_NAME> (MSVC x64)
echo ============================================

rem Step 1: 用 vswhere 自动查找 VS 2022
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VCVARS="

if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2^>nul`) do (
        set "VS_PATH=%%i"
    )
    if defined VS_PATH (
        set "VCVARS=!VS_PATH!\VC\Auxiliary\Build\vcvars64.bat"
    )
)

rem Step 2: 回退到常见路径
if not defined VCVARS (
    for %%d in (
        "D:\Microsoft Visual Studio\Community"
        "C:\Program Files\Microsoft Visual Studio\2022\Community"
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise"
        "C:\Program Files\Microsoft Visual Studio\2022\Professional"
    ) do (
        if exist "%%~d\VC\Auxiliary\Build\vcvars64.bat" (
            set "VCVARS=%%~d\VC\Auxiliary\Build\vcvars64.bat"
        )
    )
)

if not defined VCVARS (
    echo [ERROR] Cannot find Visual Studio 2022 with C++ toolchain.
    pause
    exit /b 1
)

rem Step 3: 设置 MSVC 环境并启动 VSCode
call "%VCVARS%"
code .
```

关键点：`chcp 65001` 解决中文乱码；`vswhere.exe` 自动检测 VS 路径；回退路径列表可自定义。

### E. 配置 .gitignore

```
# Visual Studio 生成文件（CMake 替代）
.vs/
*.sln
*.vcxproj*

# 构建输出（本地生成，不提交）
build/
x64/
Win32/
out/

# 临时文件
*.dat
*.tmp
*.csv
__pycache__/
```

原则：只提交源码（.cpp/.h）、配置（CMakeLists.txt, .vscode/）、项目内第三方库（.lib/.dll）。

> **本项目实际策略见第 8 节**（包含 ZKIC_lib/ 排除、fftw/ 提交等具体决策）。

### F. 注册 MSVC 工具包到 CMake Tools

编辑 `C:\Users\<用户名>\AppData\Local\CMakeTools\cmake-tools-kits.json`，确保存在以下条目。

> 如果该文件或目录不存在，手动创建目录 `CMakeTools` 和文件 `cmake-tools-kits.json`，写入 `[]`（空数组），然后用 VS 2022 条目填充。VS 安装路径和 MSVC 版本号需根据实际安装位置修改。

```json
{
    "name": "Visual Studio 2022 Community",
    "visualStudio": "Visual Studio 17 2022",
    "compilers": {
        "C": "<VS_PATH>/VC/Tools/MSVC/<ver>/bin/Hostx64/x64/cl.exe",
        "CXX": "<VS_PATH>/VC/Tools/MSVC/<ver>/bin/Hostx64/x64/cl.exe"
    },
    "preferredGenerator": {
        "name": "Visual Studio 17 2022"
    }
}
```

### G. 新项目/新电脑配置步骤

#### G.1 从零创建新 C++ 项目

1. 安装 Visual Studio 2022（勾选"使用 C++ 的桌面开发"）
2. 安装 Git for Windows（`git` 需在 PATH 中）
3. 安装 VSCode + 扩展 `C/C++` + `CMake Tools` + `Claude Code`
4. 注册 MSVC 工具包（见 F 节）
5. 创建项目根目录，放入第三方库（如有）
6. **先创建** `start_vsc.bat`（见 D 节模板），然后双击启动 VSCode
7. 在 VSCode 中创建 `.vscode/` 下 4 个配置文件（见 B 节模板）
8. 创建 `CMakeLists.txt`（见 C 节模板）、`.gitignore`（见 E 节）、一个最小 `main.cpp`
9. 按 `F5` 验证是否能编译运行
10. 确认通过后，再逐步添加实际代码和库依赖

#### G.2 从 git 克隆已有项目

1. 安装 Visual Studio 2022 + VSCode + 扩展（同上）
2. 注册 MSVC 工具包（见 F 节）
3. `git clone` 项目
4. 手动复制 `ZKIC_lib/` 目录到项目根目录（该目录不在 git 中）
5. 双击项目自带的 `start_vsc.bat` 启动 VSCode
6. CMake Tools 弹出配置提示 → 选择 VS 2022 工具包 → Yes
7. 按 **F5** 构建并运行

#### G.3 验证构建系统

在正式开发前，创建一个最小 `main.cpp` 来验证工具链：

```cpp
#include <iostream>
int main() {
    std::cout << "Build system OK" << std::endl;
    return 0;
}
```

确认 F5 能编译并输出后，再添加实际代码。

### H. 日常操作速查

| 操作              | 执行                                                   |
| ----------------- | ------------------------------------------------------ |
| 改代码后运行      | **F5**                                                 |
| 增删 .cpp/.h 文件 | **F5**（CONFIGURE_DEPENDS 自动感知）                   |
| 改 CMakeLists.txt | **F5**（CMake Tools 自动 reconfigure）                 |
| 换电脑 / git pull | 打开 → CMake Tools 自动 configure → F5                 |
| 出诡异构建错误    | `Ctrl+Shift+P` → `CMake: Delete Cache and Reconfigure` |

### I. 故障排查

| 症状                                            | 排查动作                                                                 |
| ----------------------------------------------- | ------------------------------------------------------------------------ |
| `error LNK1104: 无法打开文件 xxx.lib`         | 检查 CMakeLists.txt 中 .lib 路径是否正确；确认 .lib 文件存在              |
| `error C1083: 无法打开源文件`                 | `CMake: Delete Cache and Reconfigure`（GLOB 缓存过期）                 |
| 编译通过但运行时报错"找不到 xxx.dll"            | 检查 POST_BUILD 的 `copy_if_different` 是否覆盖了所有 DLL；手动将缺失 DLL 复制到 exe 同目录 |
| IntelliSense 红线但编译通过                     | 更新 `c_cpp_properties.json` 的 includePath；检查 `configurationProvider` |
| 找不到 MSVC 工具包                              | 检查 `cmake-tools-kits.json` 是否已注册 VS 条目                          |
| `没有为此 CMake 项目激活任何工具包`           | VSCode 底部状态栏 → 点击工具包选择器 → 选择 VS 2022                       |
| 中文乱码                                        | .bat 文件加 `chcp 65001`；CMakeLists.txt 加 `/utf-8`                   |
| F5 后找不到 .exe                                | 确认 `launch.json` 使用 `${command:cmake.launchTargetPath}`；检查编译是否成功 |
| CMake 配置报错 "Could not find a toolchain"     | 确认 VS 2022 已安装 C++ 桌面开发工作负载；检查 kit 中 compilerPath 是否正确 |
| 新电脑 git clone 后 F5 报错缺少 .lib/.dll       | 确认已手动复制 ZKIC_lib 目录（不被 git 跟踪）；确认 fftw/ 已提交到 git    |
