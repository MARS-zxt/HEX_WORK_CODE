# CLAUDE.md — 虚拟阀门模拟器 (Valve Simulator)

> 每次新会话启动时自动加载。保持简洁、精确、可操作。

---

## 环境路径

| 工具 | 路径 |
|------|------|
| Visual Studio 2022 | `D:\vv_ss\` (Community, v18.0) |
| vcvars64 | `D:\vv_ss\VC\Auxiliary\Build\vcvars64.bat` |
| Qt 6.11.1 MSVC | `C:\Qt\6.11.1\msvc2022_64` |
| CMake (Qt 自带) | `C:\Qt\Tools\CMake_64\bin\cmake.exe` |
| Ninja (Qt 自带) | `C:\Qt\Tools\Ninja\ninja.exe` |
| windeployqt | `C:\Qt\6.11.1\msvc2022_64\bin\windeployqt.exe` |
| Inno Setup 6 | `D:\into_set\Inno Setup 6\ISCC.exe` |
| Windows SDK | `C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64` |
| MSVC toolset | `D:\vv_ss\VC\Tools\MSVC\14.51.36231` (v145) |
| Debug CRT DLLs | `D:\vv_ss\VC\Redist\MSVC\14.51.36231\debug_nonredist\x64\Microsoft.VC145.DebugCRT` |

---

## 项目目录结构（模块化）

```
ValveSimulator/
├── CMakeLists.txt                     # CMake 构建配置
├── ValVeSimulator.slnx                # VS 方案
├── ValVeSimulator.vcxproj             # MSBuild 项目
│
├── src/
│   ├── core/                          # 核心引擎 + 数据类型
│   │   ├── valve_types.h              #   枚举/结构体 (ValveState, ValveParams, DataPoint)
│   │   ├── valve_simulator.h/cpp      #   模拟引擎 (100ms 定时器, 状态机, 抛物线电流)
│   │   └── version.h                  #   版本号 + 更新日志 (APP_CHANGELOG 宏)
│   │
│   ├── ui/                            # 用户界面
│   │   ├── main.cpp                   #   程序入口 (QApplication, 图标, 样式)
│   │   ├── main_window.h/cpp          #   主窗口 + TCP 服务 + 内置测试序列
│   │   ├── custom_titlebar.h/cpp      #   自定义标题栏 (macOS 风格红黄绿按钮)
│   │   ├── valve_display.h/cpp        #   阀门轨道 + 蓝色小球 + 到位信号灯
│   │   ├── valve_params_dialog.h/cpp  #   参数设置弹窗 (无边框, 3D 按钮)
│   │   ├── waveform_chart.h/cpp       #   电流波形图 (QPainter, 滚动 X 轴)
│   │   ├── test_panel.h/cpp           #   测试面板 (数据展示 + 日志 + 运行按钮)
│   │   └── version_dialog.h/cpp       #   关于对话框 (版本号 + 更新日志)
│   │
│   └── network/                       # 网络通信
│       └── valve_api.h/cpp            #   C 语言 TCP API (外部测试程序用)
│
├── resources/                         # 资源文件
│   ├── resources.qrc                  #   Qt 资源索引
│   ├── style.qss                      #   全局样式表
│   ├── app_icon.ico                   #   自定义图标 (248x243 PNG, ICO 3.00)
│   ├── app_icon.rc                    #   图标资源 (CMake+Ninja 用)
│   └── ValVeSimulator_resource.rc     #   图标资源 (MSBuild 用)
│
├── scripts/                           # 构建/部署脚本
│   ├── build_vs.bat                   #   CMake+Ninja Debug 构建
│   ├── build_release.bat              #   Release 构建 + windeployqt + Inno Setup 打包
│   └── setup.iss                      #   Inno Setup 6 安装包脚本
│
└── external/                          # 外部测试程序
    └── test_client_example.c          #   C 语言 TCP 客户端示例
```

### Include 路径策略

**`#include` 语句保持扁平，不写相对路径。**

通过构建系统添加 include 目录实现：
- CMake: `target_include_directories(... PRIVATE src/core src/ui src/network)`
- MSBuild: `AdditionalIncludeDirectories` 中添加 `src/core;src/ui;src/network`

即代码中始终写 `#include "valve_simulator.h"`，无需改成 `#include "core/valve_simulator.h"`。

---

## 构建体系

### 构建 A：CMake + Ninja → `build\`（双击 exe / CLI 构建）

```bash
# 构建（从项目根目录）
powershell -NoProfile -Command "Start-Process cmd.exe -ArgumentList '/c', '\"e:\work_space\HEX_WORK_CODE\ValveSimulator\scripts\build_vs.bat\"' -Wait -NoNewWindow"

# 构建后 DLL 部署
"C:\Qt\6.11.1\msvc2022_64\bin\windeployqt.exe" build\ValveSimulator.exe --no-translations
copy "D:\vv_ss\VC\Redist\MSVC\14.51.36231\debug_nonredist\x64\Microsoft.VC145.DebugCRT\*.dll" build\
```

### 构建 B：MSBuild → `debug\`（VS 调试 F5/Ctrl+F5）

VS 中 via `.slnx` → `build_solution`。VS 自动管理 Qt PATH。

```bash
# 如需双击 debug\ exe，同样需要 DLL 部署
"C:\Qt\6.11.1\msvc2022_64\bin\windeployqt.exe" debug\ValveSimulator.exe --no-translations
copy "D:\vv_ss\VC\Redist\MSVC\14.51.36231\debug_nonredist\x64\Microsoft.VC145.DebugCRT\*.dll" debug\
```

### 构建 C：Release 打包 → `build_rel\` + `installer\`（一键）

```bash
powershell -NoProfile -Command "Start-Process cmd.exe -ArgumentList '/c', '\"e:\work_space\HEX_WORK_CODE\ValveSimulator\scripts\build_release.bat\"' -Wait -NoNewWindow"
```

`build_release.bat` 自动执行全部 5 步：
1. 清理 `build_rel\`
2. CMake Release 配置 (Ninja)
3. 并行编译
4. windeployqt Release 部署
5. Inno Setup 6 生成安装包 → `installer\ValveSimulator_Setup_vX.X.X.exe`

### 构建对应表

| 场景 | 构建方式 | 输出目录 | 启动 |
|------|---------|----------|------|
| VS F5/Ctrl+F5 | MSBuild `.vcxproj` | `debug\` | VS |
| 双击 exe 运行 | CMake+Ninja `build_vs.bat` | `build\` | 双击 |
| 发布安装包 | `build_release.bat` | `build_rel\` + `installer\` | ISCC |

### 构建系统同步

- 修改/新增 `.cpp` `.h` → 同时更新 `CMakeLists.txt` 和 `.vcxproj`
- RC 文件两份同步修改

### Inno Setup（跳过编译单独打包）
```bash
"D:\into_set\Inno Setup 6\ISCC.exe" "e:\work_space\HEX_WORK_CODE\ValveSimulator\scripts\setup.iss"
```

---

## 技术架构

### 阀门状态机
```
IDLE → STARTING_OPEN/CLOSE → MOVING_OPEN/CLOSE → STALLING_OPEN/CLOSE
  ↑                                                    |
  └────────────── STOP ←────────────────────────────────┘
```
- **TICK_INTERVAL = 100ms**
- **STARTING**：电流抛物线爬坡 `I(t)=I_startup×(2f−f²)`，**位置从第一帧即刻移动**
- **MOVING**：恒定运行电流，位置匀速变化 `position ±= dt / travel_time`
- **STALLING**：堵转电流抛物线爬坡 `0.3s`，定时器不停（仅 STOP 可停）

### 抛物线电流公式
```cpp
double f = phase_elapsed_ / duration;  // 0→1
current_ = I_start + (I_end - I_start) * (2.0 * f - f * f);
// concave-down: 快起慢收，f=1 时为峰值
```

### TCP 协议 (localhost:9876)
```
→ OPEN / CLOSE / STOP
→ GET_DATA
← DATA time=X current=Y position=Z state=NAME open_limit=A close_limit=B
→ SET_PARAMS T_startup T_travel I_run I_stall I_startup
```

### 内置测试序列 (TestPhase 状态机)
```
IDLE → OPENING(2.2s) → PAUSE(0.3s) → CLOSING(2.4s) → DONE
```
- 100ms 非阻塞 QTimer 驱动
- 每 tick 输出电流/位置到测试日志
- 自动记录开阀/关阀电流上下限 + 到位时间
- 测试期间面板禁用防重入

### 版本管理
- `src/core/version.h`：`APP_VERSION_STRING "X.X.X"` + `APP_CHANGELOG` 宏
- `src/ui/version_dialog.cpp`：关于对话框（无边框+阴影+圆角），展示版本号/更新日志/Qt 版本
- 入口：主窗口控制栏「关于」按钮

---

## 已踩坑记录

> 详细版本见 `.claude/memory/pitfalls-collection.md`，此处仅列关键点。

1. **图标不显示** ⚠️ — RC 文件必须 `#define IDI_ICON1 1`，两个 RC 文件同步修改
2. **Qt 6 兼容** — `enterEvent(QEnterEvent*)`, `#include <QMutexLocker>`
3. **Mutex 死锁** — QMutex 非递归，只在最内层函数加锁
4. **CMD 环境** — Git Bash 中 cmake 缺少 rc.exe/mt.exe，须 `cmd.exe /c` + vcvars64
5. **DLL 缺失** — Debug: windeployqt + CRT DLL; Release: windeployqt 即可
6. **双构建系统** — 新增文件同步更新 CMakeLists.txt 和 .vcxproj
7. **安装包路径** — setup.iss 相对 scripts/，用 `..\build_rel`, `..\installer`

---

## 代码风格

- Qt 字符串：`QStringLiteral("...")`
- 信号槽：新式 `connect(sender, &Sender::sig, this, &Receiver::slot)`
- C++17：`[[nodiscard]]`, `constexpr`
- 命名：类 `PascalCase`，成员 `snake_case_`，局部变量 `camelCase`
- 3D 按钮：QSS `qlineargradient` 四段渐变模拟凹凸
- 所有 UI 文本中文

---

## Git

- 用户：`zxt`，分支：`main`
- 提交说明风格：`阀门仿真程序第一版V1.0` / `代码优化` / `补充提交`
