---
name: code-conventions
description: Qt C++ 代码风格约定
metadata:
  type: project
---

## 代码风格约定

- **Qt 字符串**：`QStringLiteral("...")` 优先
- **信号槽**：新式语法 `connect(sender, &Sender::sig, this, &Receiver::slot)`
- **C++ 标准**：C++17，使用 `[[nodiscard]]`, `constexpr`
- **命名**：类 `PascalCase`，成员变量 `snake_case_`（尾部下划线），局部变量 `camelCase`
- **3D 按钮**：QSS `qlineargradient` 四段渐变模拟凹凸
- **UI 文本**：全部中文
- **Include**：扁平化，不写相对路径（通过 build system include dirs 解决）
- **文档**：公开 API 需要 Doxygen/QDoc 风格注释（@brief/@param/@return）
- **提交**：中文简短描述

**Why:** 确保所有代码修改风格一致，避免 code review 返工。
**How to apply:** 写新代码前检查命名和 include 规范。[[project-architecture]]
