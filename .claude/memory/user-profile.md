---
name: user-profile
description: 开发者身份、技能栈、工作偏好
metadata:
  type: user
---

开发者 zxt，中文环境，使用 Visual Studio 2022 + Qt 6.11.1 开发 Windows 桌面应用。

**核心技能栈**：C++17、Qt 6（Widgets/QML）、CMake + Ninja、MSBuild、Windows 桌面开发。

**工作习惯**：
- 主要用 VS 2022 调试（F5/Ctrl+F5），通过 vs-mcp 工具自动化 VS 操作
- 构建系统双轨制：CMake+Ninja（CLI/双击运行）和 MSBuild+.vcxproj（VS 调试）
- 提交说明风格：简短中文描述（如"代码优化""补充提交"）
- Git 用户 `zxt`，分支 `main`

**LLM 使用偏好**：
- 需要使用 vs-mcp MCP 工具操作 VS（构建、调试、UI 测试）
- 偏好直接用工具操作而非口头描述步骤
- 写代码时匹配现有项目风格（命名、注释、缩进）

**Why:** 从 CLAUDE.md 和 git 历史中提取，保证每次对话理解开发者背景。
**How to apply:** 处理任务时优先用 vs-mcp 工具，构建前检查双轨制，提交风格保持一致。
