#ifndef VERSION_H
#define VERSION_H

// ============================================================================
// Application version management
// ============================================================================
// Update VERSION_STRING + VERSION_LOG on each release.
// Version format:  major.minor.patch  (semver)
// ============================================================================

#define APP_VERSION_MAJOR  1
#define APP_VERSION_MINOR  0
#define APP_VERSION_PATCH  0
#define APP_VERSION_STRING "1.0.0"

// ============================================================================
// Changelog — newest first
// ============================================================================
#define APP_CHANGELOG \
    "v1.0.0 (2026-06-27)\n" \
    "────────────────────────\n" \
    "• 阀门仿真引擎：抛物线电流曲线，100ms 精确定时\n" \
    "• 阀门可视化：蓝色小球轨道动画，到位信号灯\n" \
    "• 电流波形图：实时滚动 X 轴，渐变填充\n" \
    "• 参数配置：启动时间/行进时间/电流阈值可调\n" \
    "• TCP 服务：localhost:9876，支持 C 语言外部控制\n" \
    "• 内置测试程序：自动开阀 2.2s → 关阀 2.4s\n" \
    "• 自定义标题栏：macOS 风格红黄绿按钮，圆角阴影\n" \
    "• 自定义应用图标"

#endif // VERSION_H
