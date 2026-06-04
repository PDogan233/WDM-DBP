# ZKIC_project — Claude 项目总览

> 最后更新: 2026-06-04
> 本文档供 Claude Code 在后续会话中快速理解项目背景、架构和规范。每次重大变更后请更新本文档。

---

## 1. 项目背景

**项目目标**：光通信系统仿真平台。当前阶段的核心任务是实现基于 **SSFM (Split-Step Fourier Method)** 的光纤建模算法与 **DBP (Digital Back-Propagation)** 非线性补偿算法。

**开发语言**：C++（工业级、面向对象）

**所属公司**：绍兴智科光电计算有限公司 (ZKOE)

**开发规范依据**：`docs/ZKOE公司Agents开发交付要求251003V4p0r1.pdf`（加密，需向公司索取密码）

---

## 2. 项目目录结构

```
ZKIC_project/
├── claude.md                  ← 本文档（项目指南）
├── CMakeLists.txt             ← CMake 构建配置（唯一需要维护的构建文件）
├── start_vsc.bat              ← VSCode 启动脚本（配置 MSVC 环境）
├── .gitignore                 ← Git 忽略规则
├── .vscode/                   ← VSCode 配置（c_cpp_properties, tasks, launch, settings）
│
├── fiber_WDM.h                ← [待重构] 光纤 WDM 传播类头文件
├── fiber_WDM.cpp              ← [待重构] 光纤 WDM 传播类实现
├── fiber_WDM_test.cpp         ← [待重构] 测试程序
│
├── format_ref/                ← 公司成熟示范代码（编码规范参考）
│   ├── IQCombiner.h/.cpp      ← I/Q 合成器
│   ├── MapperMQAM.h/.cpp      ← M-QAM 调制器
│   ├── MeasurementSimple.h/.cpp ← 测量/可视化工具
│   ├── NRZCoder.h/.cpp        ← NRZ 脉冲成形
│   └── PRBSlfsr.h/.cpp        ← PRBS 伪随机序列生成器
│
├── py_ref/                    ← Python 参考实现
│   ├── para.py                ← 系统参数定义
│   ├── tx_DSP.py              ← 发射端 DSP
│   ├── channel.py             ← 信道传播（SSFM 光纤 + EDFA）
│   ├── dbp.py                 ← DBP 数字反向传播
│   ├── rx_DSP.py              ← 接收端 DSP
│   ├── utils.py               ← 工具函数（同步、RRC滤波器等）
│   ├── visualize.py           ← 可视化
│   └── main_simu_test.py      ← 主测试流程
│
├── docs/                      ← 文档
│   ├── ZKOE公司Agents开发交付要求251003V4p0r1.pdf  ← 交付要求（加密）
│   └── 数学库用户手册.chm                          ← 数学库手册（CHM格式）
│
└── build/                     ← CMake 构建输出（gitignore）
```

---

## 3. 公司编码规范（从 format_ref/ 提取）

### 3.1 文件头格式

每个 .h 和 .cpp 文件必须包含：

```cpp
/******************************************************************************
 * Copyright (c) 2025 Shaoxing Zhike Electro-Optics Computing Co. Ltd. All rights reserved.
 * Confidential and proprietary information.
 * Unauthorized copying, reproduction, or distribution of this software is strictly prohibited.
 *****************************************************************************/

 /**
  * @file: ClassName.h
  * @brief: 简要描述
  * @author: 作者名
  * @version: 1.0
  * @date: YYYY-MM-DD
  * @department: Product Development Department
  * @project: ZKOE Agents Development
  */

  /**
   * Revision History:
   *
   * Version       Date       Author                   Changes
   * --------   ----------   -------    ------------------------------------
   *  1.0     YYYY-MM-DD    Author      Initial version
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

每个 Agent 类遵循统一的结构：

```cpp
class AgentName {
public:
    // --- 枚举（如需）---

    // --- Parameters 结构体 ---
    struct Parameters {
        std::string bandId;        // 波段ID（从GlobalValue系统获取参数）
        long long channelIndex;    // 信道索引
        // ... 功能特定参数 ...
        std::string saveFile;           // 文件保存路径
        mode::WriteMode writeMode;      // 写入模式
        mode::FileType dataFormat;      // 数据保存格式
    };

    // --- Signals 结构体 ---
    struct Signals {
        // 输入/输出信号，使用 ZK 数学库类型
        // cvec = complex vector, vec = real vector, bvec = binary vector
    };

    // --- 静态执行入口 ---
    static void execute(const Parameters& params, Signals& signals);

    // --- 参数验证 ---
    static void checkParam(const Parameters& params);

private:
    // --- 私有辅助函数 ---
};
```

### 3.4 函数文档格式

每个函数需要中文注释，包含函数名称、输入参数、输出、功能描述：

```cpp
/*
 * 静态成员函数名称： static void execute(...);
 * 静态成员函数的输入：
 *     params  // 类型 Xxx::Parameters，包含xxx参数
 *     signals // 类型 Xxx::Signals，包含输入输出信号
 * 静态成员函数的输出： void
 * 静态成员函数实现的功能描述： 详细描述功能...
 */
```

### 3.5 ZK 数学库类型

| 类型 | 说明 | 等价 STL |
|------|------|----------|
| `vec` | 实数向量 | `std::vector<double>` |
| `cvec` | 复数向量 | `std::vector<std::complex<double>>` |
| `bvec` | 二进制向量 | 可位索引的布尔向量 |
| `cmat` | 复数矩阵 | — |
| `bin` | 单个二进制值 | — |
| `ZK::Vec<T>` | 泛型向量 | — |
| `ZK::Mat<T>` | 泛型矩阵 | — |

头文件路径（按需引入）：
```cpp
#include <vec.h>        // vec, cvec, bvec
#include <mat.h>        // 矩阵类型
#include <fft.h>        // FFT 相关
#include <fstreamIO.h>  // saveData() 文件 I/O
#include <GlobalValue.h> // 波段/信道参数系统
#include <commfunc.h>   // 通用工具函数
#include <basetypes.h>  // 基础类型定义
```

### 3.6 文件 I/O

使用统一的 `saveData()` 函数：
```cpp
saveData(signal, params.saveFile, params.dataFormat, params.writeMode);
// params.saveFile = "" 或 "noSave" 时跳过保存
```

### 3.7 其他规范

- 头文件使用 `#pragma once`（不使用传统的 `#ifndef` 守卫）
- 缩进使用 Tab 或 4空格（format_ref 中不完全一致，建议统一 Tab）
- 类成员命名：使用驼峰式（如 `bandId`, `channelIndex`, `prbsOrder`）
- 私有成员函数注释在 .cpp 文件中

---

## 4. Python 参考实现分析（py_ref/）

### 4.1 系统参数 (para.py)

| 参数 | 值 | 说明 |
|------|-----|------|
| `Nsym` | 2^15 | 每信道符号数 |
| `M` | 16 | QAM 阶数 |
| `Rs` | 32e9 | 符号率 |
| `Nch` | 3 | WDM 信道数 |
| `DeltaF` | 50e9 | 信道间隔 |
| `L_span` | 100e3 m | 每跨段长度 |
| `Nspans` | 10 | 跨段数 |
| `alpha_dBpm` | 0.2e-3 dB/m | 损耗系数 |
| `Dispersion` | 16.5e-6 s/m² | 色散 D |
| `Dis_S` | 0.08e3 s/m³ | 色散斜率 |
| `n2` | 2.6e-20 m²/W | 非线性折射率 |
| `Aeff` | 80e-12 m² | 有效面积 |
| `gamma` | 计算得 ~1.3 /W/m | 非线性系数 |
| `pmd_coeff` | 0 | PMD 系数（当前关闭） |
| `method` | 'constant' | 步长控制方法 |
| `dz` | 2e3 m | 默认 SSFM 步长 |

### 4.2 发射端 DSP (tx_DSP.py)

流程：随机比特 → 16-QAM 调制 → RRC 脉冲成形 → 功率归一化 → 频移复用(WDM)

### 4.3 信道传播 (channel.py)

核心算法：**对称 SSFM (Strang Splitting)**
```
for each span:
    for z < L_span:
        choose_step()           // 自适应步长
        NL(h/2) → Disp(h) → NL(h/2)   // Manakov 方程
    apply_edfa()                // 放大 + ASE 噪声
```

**关键细节**：
- Manakov 因子: `gamma_manakov = (8/9) * gamma`
- 非线性相位: `φ = -γ_manakov * P_total * h`
- 色散在频域施加: `H = exp(-α_np*h/2 - j*β(ω)*h)`
- 步长控制方法:
  - `constant`: 固定步长 dz
  - `local_error`: `h = (LE_target / denom)^(1/3)`
  - `global_error`: `h = (GE_target / denom)^(1/2)`

### 4.4 DBP (dbp.py)

**两种模式**：
1. `dbp_fullband_to_subband()`: 提取子带 → 反向 SSFM → 重构全频带
2. `dbp_subband()`: 直接在已下采样的子带上做 DBP

反向传播核心（与正向对称）：
```
for span in reverse(Nspans):
    remove EDFA gain
    for z < L:
        NL(-h/2) → Disp(-h) → NL(-h/2)   // h 为负值
```

### 4.5 接收端 DSP (rx_DSP.py)

流程：下变频 → 子带提取 → DBP → 匹配滤波 → 时钟恢复(decimator) → 帧同步 → 相位补偿 → 功率归一化 → 16-QAM 解调 → BER

---

## 5. 现有 C++ 代码分析与待重构项 (fiber_WDM.*)

### 5.1 现有代码架构

- **FiberWDM 类**：构造函数接受物理参数 + 采样参数
- **propagate()**: 主 SSFM 循环（仅单跨段，无 EDFA）
- **applyDispersion()**: 频域色散（使用 FFTW）
- **applyNonlinearity()**: 时域 Kerr 非线性
- **chooseStep()**: 支持 constant/local_error/global_error
- **fft/ifft/fftshift/ifftshift**: FFTW 封装

### 5.2 需要重构的问题

| 问题 | 严重程度 | 说明 |
|------|----------|------|
| 缺少公司文件头 | **必须** | 无 Copyright、无 @file、无 Revision History |
| 不在 ZK 命名空间 | **必须** | 所有类需要在 `namespace ZK` 中 |
| 不使用 ZK 数学库类型 | **必须** | 使用 `std::vector<std::complex<double>>` 而非 `cvec` |
| 不遵循 Agent 模式 | **必须** | 没有 Parameters/Signals 结构体，没有 static execute() |
| 缺少 Manakov 8/9 因子 | **关键 BUG** | applyNonlinearity 直接使用 gamma，Python 中为 (8/9)*gamma |
| 仅支持单偏振 | **需扩展** | Python 支持 X/Y 双偏振 (Nt, 2) |
| 硬编码 CSV 调试输出 | **需清理** | applyDispersion 中 static bool saved 首次调用写 CSV |
| 非静态方法 | **需改为静态** | 公司规范使用 static execute() |
| 中文注释缺失 | **需补充** | 函数注释应为中文 |
| EDFA 未实现 | **需添加** | 放大 + ASE 噪声在 propagate 之外 |

### 5.3 命名对照（Python → C++）

| Python 名称 | C++ 建议名称 | 说明 |
|-------------|-------------|------|
| `channel_propagation()` | `ChannelPropagation::execute()` | WDM 信道传播 |
| `fiber_prop_DP()` | `FiberPropDP::execute()` | 双偏振光纤 SSFM |
| `dbp_subband()` | `DBPSubband::execute()` | 子带 DBP |
| `dbp_fullband_to_subband()` | `DBPFullband::execute()` | 全频带→子带 DBP |
| `apply_edfa()` | `EDFA::execute()` | EDFA 放大 |
| `multiplex_wdm_channels()` | `WDMMux::execute()` | WDM 复用 |
| `modulate_16qam()` | `MQAMModulator::execute()` | QAM 调制 |
| `sync_align()` | `SyncAlign::execute()` | 帧同步 |

---

## 6. 第三方库依赖

### 6.1 FFTW
- **路径**: `../fiber_WDM/fftw/`
- **头文件**: `fftw3.h`
- **库文件**: `libfftw3-3.lib` / `libfftw3-3.dll`
- **用途**: FFT/IFFT 运算（SSFM 色散步骤）
- **注意**: ZK 数学库的 `<fft.h>` 可能也封装了 FFT — 需确认是否可以直接使用

### 6.2 ZKIC 数学库 (ZKIC_mathworld v2.5)
- **路径**: `C:\Users\PDogan\OneDrive\Files\Scientific research\project\ZKIC\Agent_Dev\code_dev\ZKIC_lib\ZKIC_mathworld\ZKIC_mathworld2p5`
- **头文件**: `include/` 下（vec.h, mat.h, fft.h, fstreamIO.h, basetypes.h 等）
- **库文件**: `x64/debug/lib/ZKIC_math_world2p5d.lib`
- **用途**: 向量/矩阵运算、FFT、文件 I/O

### 6.3 GlobalValue 库
- **路径**: `C:\Users\PDogan\OneDrive\Files\Scientific research\project\ZKIC\Agent_Dev\code_dev\ZKIC_lib\GlobalValueForZD`
- **头文件**: `include/GlobalValue.h`
- **库文件**: `x64/debug/lib/signals_globalvalue_world1p1d.lib`
- **用途**: 波段/信道参数统一管理系统

---

## 7. 构建系统

- **CMake** (CMakeLists.txt)：主要构建方式
- **编译器**: MSVC v143 (Visual Studio 2022)
- **平台**: x64, Windows
- **C++ 标准**: C++17
- **源文件自动发现**: `file(GLOB ...)` 只扫描根目录 .cpp/.h
- **启动方式**: 双击 `start_vsc.bat` → 自动配置 MSVC 环境 → 启动 VSCode

---

## 8. .gitignore 策略

```
# 同步（不忽略）：
✓ *.cpp, *.h           — 源代码
✓ CMakeLists.txt        — 构建配置
✓ .vscode/              — VSCode 配置（团队共享）
✓ .gitignore            — Git 忽略规则
✓ claude.md             — 项目指南
✓ py_ref/               — Python 参考（开发参考）
✓ format_ref/           — 公司示范代码（规范参考）
✓ docs/                 — 文档
✓ start_vsc.bat         — 启动脚本
✓ fiber_WDM/fftw/       — FFTW 库（项目依赖）

# 忽略：
✗ build/                — CMake 构建输出
✗ x64/, Win32/          — VS 构建输出
✗ .vs/                  — VS IDE 缓存
✗ *.sln, *.vcxproj*     — VS 项目文件（CMake 替代）
✗ *.dat, *.tmp          — 临时/数据文件
✗ __pycache__/          — Python 缓存
```

---

## 9. 新电脑配置流程

当将项目迁移到新电脑时，确保新电脑有以下环境后，直接对 Claude Code 说：

> 我正在配置一个从旧电脑迁移过来的光通信仿真 C++ 项目。项目使用 CMake + MSVC（Visual Studio 2022）。请帮我：
> 1. 检测本机 VS 2022 和 Windows SDK 安装路径
> 2. 更新 CMakeLists.txt 中所有第三方库路径（FFTW、ZKIC_mathworld、GlobalValue）
> 3. 更新 .vscode/ 下所有配置文件中的编译器路径、include 路径、SDK 版本
> 4. 在 CMake Tools 的 kits 文件中添加 VS 2022 工具包
> 5. 运行 CMake 配置验证能否构建
> 6. 创建 start_vsc.bat 启动脚本
> 详细背景见 claude.md。

---

## 10. 待确认问题

以下问题需要在开始编码前确认：

1. **PDF 交付要求**：`docs/ZKOE公司Agents开发交付要求251003V4p0r1.pdf` 有密码保护，请提供可读版本或口头总结关键交付要求
2. **数学库 API**：CHM 文件无法直接读取，请确认 `<fft.h>` 是否已封装 FFTW，是否可以直接使用替代现有的 FFTW 封装
3. **DBP 实现方案**：DBP 是作为独立 Agent 类，还是集成到现有的 FiberWDM 类中？
4. **双偏振支持**：是否必须支持双偏振（X/Y Pol），还是单偏振即可？Python 中是 DP
5. **PMD 支持**：Python 中 PMD 系数设为 0（关闭），C++ 版本是否需要支持 PMD？
6. **EDFA 实现**：EDFA 是作为独立 Agent 类还是作为 channel 传播循环的一部分？
7. **命名规则**：类名是使用 `FiberSSFM`、`FiberPropDP` 还是保留 `FiberWDM`？
8. **参数传递**：物理参数（L_span, alpha_dBpm, beta2...）是通过 Parameters struct 传入，还是像 Python 一样统一用 GlobalValue 系统管理？
