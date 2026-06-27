---
name: project-architecture
description: 阀门模拟器核心架构——状态机、TCP协议、模块划分
metadata:
  type: project
---

## 项目：虚拟阀门模拟器 (Valve Simulator)

**核心业务**：模拟阀门电气特性（电流、位置），通过 TCP 对外暴露控制接口。

### 阀门状态机
```
IDLE → STARTING_OPEN/CLOSE → MOVING_OPEN/CLOSE → STALLING_OPEN/CLOSE → STOP → IDLE
```
- TICK_INTERVAL = 100ms，每个 tick 更新电流+位置
- STARTING：电流抛物线爬坡 `I(t)=I_startup×(2f−f²)`，位置从第一帧即刻移动
- MOVING：恒定运行电流，位置匀速变化
- STALLING：0.3s 堵转电流爬坡，定时器不停（仅 STOP 可停）

### TCP 协议 (localhost:9876)
文本协议，换行分隔：`OPEN`/`CLOSE`/`STOP`/`GET_DATA`/`SET_PARAMS`
响应格式：`DATA time=X current=Y position=Z state=NAME open_limit=A close_limit=B`

### 模块划分
- `src/core/` — valve_types.h, valve_simulator.h/cpp, version.h
- `src/ui/` — main_window, custom_titlebar, valve_display, valve_params_dialog, waveform_chart, test_panel, version_dialog
- `src/network/` — valve_api.h/cpp（C 语言 TCP API）
- `include 策略`：扁平化，不写相对路径，通过 CMake/MSBuild 的 include dirs 配置

### 内置测试序列
TestPhase 状态机：IDLE→OPENING(2.2s)→PAUSE(0.3s)→CLOSING(2.4s)→DONE
100ms 非阻塞 QTimer 驱动，测试期间面板禁用防重入。

**Why:** 每次对话时需要快速理解项目架构，避免重复读取 CLAUDE.md 全文。
**How to apply:** 修改状态机/测试序列/TCP 协议时参考。[[build-system]] [[pitfalls-collection]]
