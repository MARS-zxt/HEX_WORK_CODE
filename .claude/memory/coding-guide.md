---
name: coding-guide
description: Qt C++ 代码风格 + 已踩坑记录
metadata:
  type: project
---

## 代码风格约定

- Qt 字符串：`QStringLiteral("...")` 优先
- 信号槽：新式语法 `connect(sender, &Sender::sig, this, &Receiver::slot)`
- C++17：`[[nodiscard]]`, `constexpr`
- 命名：类 `PascalCase`，成员 `snake_case_`（尾下划线），局部 `camelCase`
- 3D 按钮：QSS `qlineargradient` 四段渐变模拟凹凸
- UI 文本：全部中文，Include 扁平化（不写相对路径）
- 提交：中文简短描述，用户 `zxt`，分支 `main`

---

## 已踩坑记录（修改代码前必查）

### ⚠️ 1. 图标不显示（最高频）
RC 文件 `IDI_ICON1` 必须 `#define IDI_ICON1 1`。两份 RC（`resources/app_icon.rc` 和 `resources/ValveSimulator_resource.rc`）同步更新。

### 2. Qt 6 兼容
`enterEvent(QEnterEvent*)` 非 `QEvent*`；`#include <QMutexLocker>` 需显式引入。

### 3. Mutex 死锁
`QMutex` 非递归，只在最内层加锁，外层不加。

### 4. CLI cmake 环境
Git Bash 中缺 rc.exe/mt.exe，必须 `cmd.exe /c` + vcvars64。

### 5. DLL 缺失
Debug：windeployqt + CRT DLL；Release：仅 windeployqt。

### 6. 双构建系统
新增 .cpp/.h 同步更新 CMakeLists.txt 和 .vcxproj。

### 7. 安装包路径
setup.iss 相对 scripts/，用 `..\build_rel` 和 `..\installer`。

**Why:** 风格一致性 + 每次重犯浪费大量时间。
**How to apply:** 写新代码前检查命名规范，图标/构建/部署前查坑。[[project-reference]]
