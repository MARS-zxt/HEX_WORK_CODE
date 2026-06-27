---
name: pitfalls-collection
description: 已踩坑记录——图标、Qt6兼容、Mutex死锁、DLL缺失等7条
metadata:
  type: project
---

## 已踩坑记录（7条硬经验，避免重复犯错）

### ⚠️ 1. 图标不显示（最高频）
RC 文件中 `IDI_ICON1` 必须有 `#define IDI_ICON1 1`，否则 GROUP_ICON 是字符串名，Explorer 不识别。
两份 RC 文件（`resources/app_icon.rc` 和 `resources/ValveSimulator_resource.rc`）内容相同，修改时同步更新。

### 2. Qt 6 兼容
- `enterEvent(QEnterEvent*)` 非 `QEvent*`
- `#include <QMutexLocker>` Qt 6 分离了此头文件

### 3. Mutex 死锁
`QMutex` 默认非递归。外层和内层函数都加锁 → 死锁。
**原则**：只在最内层函数加锁，外层不加。

### 4. 命令行 CMake 环境
Git Bash 中直接 cmake 找不到 `rc.exe`/`mt.exe`。必须通过 `cmd.exe /c` + vcvars64。

### 5. DLL 缺失
- Debug 构建：windeployqt + 复制 Debug CRT DLL
- Release 构建：仅 windeployqt

### 6. 构建系统分离
新增文件需同时更新 CMakeLists.txt 和 .vcxproj 的源文件列表。

### 7. 安装包路径规则
setup.iss 中路径相对 scripts/，用 `..\build_rel` 和 `..\installer`。

**Why:** 这些是花费时间踩过的坑，每次重犯浪费大量时间。
**How to apply:** 图标/构建/部署操作前先检查此列表。[[build-system]] [[project-architecture]]
