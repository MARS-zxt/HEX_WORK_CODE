---
name: project-reference
description: 项目架构 + 构建体系完整参考
metadata:
  type: project
---

## 项目：虚拟阀门模拟器 (Valve Simulator)

**核心业务**：模拟阀门电气特性（电流、位置），通过 TCP 对外暴露控制接口。

### 阀门状态机
```
IDLE → STARTING_OPEN/CLOSE → MOVING_OPEN/CLOSE → STALLING_OPEN/CLOSE → STOP → IDLE
```
- TICK_INTERVAL = 100ms
- STARTING：电流抛物线爬坡 `I(t)=I_startup×(2f−f²)`，位置从第一帧即刻移动
- MOVING：恒定运行电流，位置匀速变化
- STALLING：0.3s 堵转电流爬坡，定时器不停（仅 STOP 可停）

### TCP 协议 (localhost:9876)
文本协议，换行分隔：`OPEN`/`CLOSE`/`STOP`/`GET_DATA`/`SET_PARAMS`/`SET_DATA`
响应格式：`DATA time=X current=Y position=Z state=NAME open_limit=A close_limit=B`

### 模块划分
- `src/core/` — valve_types.h, valve_simulator.h/cpp, version.h
- `src/ui/` — main_window, custom_titlebar, valve_display, valve_params_dialog, waveform_chart, test_panel, version_dialog
- `src/network/` — valve_api.h/cpp（C 语言 TCP API）
- Include 策略：扁平化，不写相对路径，通过 CMake/MSBuild 的 include dirs 配置

### 内置测试序列
TestPhase 状态机：IDLE→OPENING(2.2s)→PAUSE(0.3s)→CLOSING(2.4s)→DONE

### 版本管理
`src/core/version.h` 中 `APP_VERSION_STRING` + `APP_CHANGELOG`，通过「关于」按钮展示。

---

## 构建三轨制

| 场景 | 构建方式 | 输出目录 | 启动 |
|------|---------|----------|------|
| VS F5/Ctrl+F5 调试 | MSBuild `.vcxproj` → `.slnx` | `debug\` | VS 自动 |
| 双击 exe 运行 | CMake + Ninja `build_vs.bat` | `build\` | 双击 |
| 发布安装包 | `build_release.bat` | `build_rel\` + `installer\` | Inno Setup 6 |

### 关键路径
- VS 2022: `D:\vv_ss\`, Qt 6.11.1: `C:\Qt\6.11.1\msvc2022_64`
- CMake: `C:\Qt\Tools\CMake_64\bin\cmake.exe`
- Ninja: `C:\Qt\Tools\Ninja\ninja.exe`
- windeployqt: `C:\Qt\6.11.1\msvc2022_64\bin\windeployqt.exe`
- Inno Setup 6: `D:\into_set\Inno Setup 6\ISCC.exe`
- Debug CRT: `D:\vv_ss\VC\Redist\MSVC\14.51.36231\debug_nonredist\x64\Microsoft.VC145.DebugCRT\`

### 构建规则
1. CLI cmake 必须在 `cmd.exe /c` + vcvars64 环境（Git Bash 缺 rc.exe/mt.exe）
2. 新增文件需同时更新 CMakeLists.txt 和 .vcxproj
3. Debug 构建双击运行需 windeployqt + 复制 CRT DLL
4. Release 构建仅需 windeployqt
5. setup.iss 路径相对 scripts/，用 `..\build_rel` 和 `..\installer`

**Why:** 架构和构建是最高频参考内容，合并减少碎片。
**How to apply:** 构建前确认场景，修改状态机/协议时参考。[[coding-guide]]
