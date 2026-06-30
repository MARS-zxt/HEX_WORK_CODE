# CLAUDE.md — 虚拟阀门模拟器 (Valve Simulator)

> 详细架构/构建参考 → `.claude/memory/project-reference.md`
> 代码风格/踩坑记录 → `.claude/memory/coding-guide.md`

---

## 环境路径

| 工具 | 路径 |
|------|------|
| VS 2022 (v18.0) | `D:\vv_ss\` |
| Qt 6.11.1 MSVC | `C:\Qt\6.11.1\msvc2022_64` |
| CMake / Ninja | `C:\Qt\Tools\CMake_64\bin\cmake.exe` / `C:\Qt\Tools\Ninja\ninja.exe` |
| windeployqt | `C:\Qt\6.11.1\msvc2022_64\bin\windeployqt.exe` |
| Inno Setup 6 | `D:\into_set\Inno Setup 6\ISCC.exe` |
| Debug CRT | `D:\vv_ss\VC\Redist\MSVC\14.51.36231\debug_nonredist\x64\Microsoft.VC145.DebugCRT\` |

---

## ⚠️ VS 编译铁律

| 规则 | 说明 |
|------|------|
| **打开 .slnx，不是文件夹** | `ValveSimulator\ValveSimulator.slnx` |
| **编译用 `build_solution`** | vs-mcp 直接调，无需 CLI |
| **构建输出** | `debug\` (Debug) / `release\` (Release) |

标准流程：`get_status` → `build_solution` → (出错时) `get_build_errors` → `debug_start_without_debugging`

### 常见构建问题

| 症状 | 原因 | 解决 |
|------|------|------|
| `build_solution` 超时 | VS 在文件夹模式 | `solution_close` → `solution_open` `.slnx` |
| PDB 锁 (C1083 vc145.pdb) | CMake 和 MSBuild 冲突 | 删 `debug\` 目录重建 |
| RC1110 .rc 找不到 | 删了 `ValveSimulator_resource.rc` | 两份 RC 文件都必须保留 |

---

## 构建体系

| 场景 | 构建方式 | 输出 | 启动 |
|------|---------|------|------|
| VS F5/Ctrl+F5 | MSBuild `.vcxproj` | `debug\` | VS |
| 双击 exe | CMake+Ninja `scripts\build_vs.bat` | `build\` | 双击 |
| 发版打包 | `scripts\build_release.bat` | `build_rel\` + `installer\` | ISCC |

**Include 策略**：`#include` 保持扁平（如 `#include "valve_simulator.h"`），通过 CMake `target_include_directories` 和 MSBuild `AdditionalIncludeDirectories` 添加 `src/core;src/ui;src/network`。

**新增文件**：必须同时更新 `CMakeLists.txt` 和 `.vcxproj`。

**CLI 构建**：必须从 vcvars64 环境运行。最可靠方式：
```bash
powershell -NoProfile -Command "Start-Process cmd.exe -ArgumentList '/c', '\"e:\work_space\HEX_WORK_CODE\ValveSimulator\scripts\build_vs.bat\"' -Wait -NoNewWindow"
```

---

## 核心架构

### 阀门状态机
`IDLE → STARTING_OPEN/CLOSE → MOVING_OPEN/CLOSE → STALLING_OPEN/CLOSE`
- TICK_INTERVAL = 100ms，**位置从第一帧即刻移动**
- 电流：抛物线 `I_start + (I_end - I_start) * (2f - f²)`
- STALLING 定时器不停，仅 STOP 可停

### TCP (localhost:9876)
`OPEN` / `CLOSE` / `STOP` / `GET_DATA` / `SET_PARAMS` / `SET_DATA`

### 已踩坑速查
1. 图标：`#define IDI_ICON1 1`（两份 RC 同步）
2. Qt 6：`enterEvent(QEnterEvent*)`, `#include <QMutexLocker>`
3. Mutex：非递归，只最内层加锁
4. DLL：Debug 需 windeployqt + CRT；Release 仅 windeployqt
5. CLI：Git Bash 缺 rc.exe/mt.exe → 必须 `cmd.exe /c` + vcvars64

---

## 代码风格

- `QStringLiteral("...")` | 新式 `connect` | C++17 | `PascalCase` 类 / `snake_case_` 成员 / `camelCase` 局部
- UI 全中文 | 3D 按钮 `qlineargradient` | Include 扁平化

---

## Git

用户 `zxt`，分支 `main`。提交中文简短描述。
