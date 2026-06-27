---
name: build-system
description: 三轨构建体系——CMake+Ninja、MSBuild、Release打包
metadata:
  type: project
---

## 构建三轨制

| 场景 | 构建方式 | 输出目录 | 启动 |
|------|---------|----------|------|
| VS F5/Ctrl+F5 调试 | MSBuild `.vcxproj` → `.slnx` | `debug\` | VS 自动 |
| 双击 exe 运行 | CMake + Ninja `build_vs.bat` | `build\` | 双击 |
| 发布安装包 | `build_release.bat` | `build_rel\` + `installer\` | Inno Setup 6 |

### 关键路径
- VS 2022: `D:\vv_ss\` (Community, v18.0)
- Qt 6.11.1 MSVC: `C:\Qt\6.11.1\msvc2022_64`
- CMake: `C:\Qt\Tools\CMake_64\bin\cmake.exe`
- Ninja: `C:\Qt\Tools\Ninja\ninja.exe`
- windeployqt: `C:\Qt\6.11.1\msvc2022_64\bin\windeployqt.exe`
- Inno Setup 6: `D:\into_set\Inno Setup 6\ISCC.exe`

### 重要规则
1. **命令行 CMake 必须在 cmd.exe /c + vcvars64 环境运行**（Git Bash 找不到 rc.exe/mt.exe）
2. **新增文件必须同时更新 CMakeLists.txt 和 .vcxproj**
3. **Debug 构建双击运行需部署 CRT DLL**：从 `D:\vv_ss\VC\Redist\MSVC\14.51.36231\debug_nonredist\x64\Microsoft.VC145.DebugCRT\` 复制
4. **Release 构建不需 CRT DLL**（系统自带）
5. **安装包路径**：setup.iss 中相对于 scripts/ 目录，用 `..\build_rel` 和 `..\installer`

### VS 操作（通过 vs-mcp）
直接调 `build_solution` + `debug_start_without_debugging`（运行）或 `debug_start`（调试）

**Why:** 构建体系是最高频操作，三轨制容易混淆，需要快速参考。
**How to apply:** 构建前确认场景选择正确轨道。[[project-architecture]] [[pitfalls-collection]]
