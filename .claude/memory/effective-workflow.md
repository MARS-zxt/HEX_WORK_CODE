---
name: effective-workflow
description: 高效开发工作流——如何与 Claude 配合节省 token
metadata:
  type: feedback
---

## 高效开发工作流（Token 节省策略）

### 已验证的高效操作模式
1. **VS 操作直接用 vs-mcp**：`build_solution` → `get_build_errors` → `debug_start_without_debugging`，比 CLI 命令快且不需要冷启动
2. **修改代码后立即构建验证**：避免累积多个修改后一次构建（出错难定位）
3. **用 skill 而非重复解释**：5 个 skill 覆盖了 90% 的开发场景（qt-cmake/qml-coding/qt-cpp-review/qt-cpp-doc/commit）
4. **Memory 文件缓存上下文**：架构、构建、踩坑等关键信息持久化，避免每次重新理解

### Token 节省规则（每次开发默认遵守）

| 规则 | 说明 |
|------|------|
| 构建优先用 vs-mcp | `build_solution` 而非 CLI 脚本，避免编译日志灌入上下文 |
| 搜索用 Glob/Grep 工具 | 不用 `find`/`grep` bash 命令，工具结果更精简 |
| 编辑后不 Read 验证 | Edit/Write 失败即报错，成功则已生效——不浪费 token 二次确认 |
| 大文件分段读 | 用 `offset` + `limit` 只读相关行，不全文加载 |
| 多文件探索用 Explore Agent | 派发子任务扫目录，只接收结论而非文件转储 |
| 不同文件独立修改用并行 Agent | 互不依赖的修改并行处理，减少轮次 |
| CLAUDE.md 保持精简 | 详细参考放 memory，CLAUDE.md 仅放关键操作信息 |
| Memory 自动注入上下文 | 项目架构/构建/踩坑已持久化，无需每次重新描述 |

### 缓存命中率预估（优化后）

```
System Prompt   ████████ 95%
CLAUDE.md       ████████ 90%  (207行，瘦身16.5%)
Memory (6文件)  ████████ 85%  (首次建立)
Skills (5个)    ██████   70%
Source Code     ████     40%
MCP Calls       ██       15%
```

**Why:** 用户明确要求分析缓存命中率并给出 token 节省建议。已执行 CLAUDE.md 瘦身（248→207行）、建立 Memory 体系（6文件）、优化 .gitignore。
**How to apply:** 每次开发时默认使用这些模式。代码修改后直接构建，不 Read 验证。多文件探索用 Agent。[[user-profile]] [[build-system]]
