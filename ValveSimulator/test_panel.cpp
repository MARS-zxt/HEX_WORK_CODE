#include "test_panel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFrame>
#include <QScrollBar>

TestPanel::TestPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void TestPanel::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    // =====================================================================
    // Test button
    // =====================================================================
    test_btn_ = new QPushButton(QStringLiteral("▶ 运行测试程序"), this);
    test_btn_->setFixedHeight(38);
    test_btn_->setCursor(Qt::PointingHandCursor);
    test_btn_->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "      stop:0 #fafafa, stop:0.4 #eaeaea, stop:0.5 #dddddd, stop:0.6 #d0d0d0, stop:1 #e2e2e2);"
        "  border: 1px solid #b0b0b0;"
        "  border-top-color: #e0e0e0;"
        "  border-left-color: #e0e0e0;"
        "  border-right-color: #a0a0a0;"
        "  border-bottom-color: #909090;"
        "  border-radius: 6px;"
        "  padding: 6px 24px;"
        "  font-size: 14px;"
        "  color: #2070C0;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "      stop:0 #ffffff, stop:0.4 #f2f2f2, stop:0.5 #e6e6e6, stop:0.6 #d8d8d8, stop:1 #eaeaea);"
        "}"
        "QPushButton:pressed {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "      stop:0 #c8c8c8, stop:0.4 #d0d0d0, stop:0.5 #d4d4d4, stop:0.6 #d8d8d8, stop:1 #d0d0d0);"
        "  border-top-color: #909090;"
        "  border-left-color: #909090;"
        "  border-right-color: #c0c0c0;"
        "  border-bottom-color: #c0c0c0;"
        "  padding-top: 7px;"
        "  padding-bottom: 5px;"
        "}");
    connect(test_btn_, &QPushButton::clicked, this, &TestPanel::onTestButtonClicked);
    mainLayout->addWidget(test_btn_);

    // =====================================================================
    // Data output group
    // =====================================================================
    auto *dataGroup = new QGroupBox(QStringLiteral("测试数据输出"), this);
    dataGroup->setStyleSheet(
        "QGroupBox {"
        "  font-size: 13px; font-weight: bold; color: #444;"
        "  border: 1px solid #D0D0D0; border-radius: 6px;"
        "  margin-top: 8px; padding-top: 16px;"
        "  background: #FAFAFA;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin; left: 12px; padding: 0 6px;"
        "}");

    auto *dataLayout = new QGridLayout(dataGroup);
    dataLayout->setSpacing(8);
    dataLayout->setContentsMargins(12, 18, 12, 12);

    auto makeLabelWidget = [&](const QString &text, const QString &style) {
        auto *lbl = new QLabel(text, dataGroup);
        lbl->setStyleSheet(style);
        lbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        return lbl;
    };
    auto makeValueWidget = [&](QLabel *&valueLabel) {
        valueLabel = new QLabel("--", dataGroup);
        valueLabel->setAlignment(Qt::AlignCenter);
        valueLabel->setStyleSheet(
            "font-size: 14px; font-weight: bold; color: #2070C0;"
            "background: white; border: 1px solid #D0D0D0; border-radius: 4px;"
            "padding: 5px 8px; min-width: 80px;");
        return valueLabel;
    };

    const QString labelStyle = "font-size: 12px; color: #666; background: transparent; font-weight: normal;";
    const QString headerCloseStyle = "font-weight: bold; font-size: 13px; color: #C04020; background: transparent;";
    const QString headerOpenStyle  = "font-weight: bold; font-size: 13px; color: #20A040; background: transparent;";

    // Column 0 : labels
    dataLayout->addWidget(makeLabelWidget(QStringLiteral("到位时间:"), labelStyle), 1, 0);
    dataLayout->addWidget(makeLabelWidget(QStringLiteral("电流上限:"), labelStyle), 2, 0);
    dataLayout->addWidget(makeLabelWidget(QStringLiteral("电流下限:"), labelStyle), 3, 0);

    // Column 1 : 关阀 headers + values
    dataLayout->addWidget(makeLabelWidget(QStringLiteral("关阀"), headerCloseStyle), 0, 1, Qt::AlignCenter);
    dataLayout->addWidget(makeValueWidget(close_time_val_),  1, 1, Qt::AlignCenter);
    dataLayout->addWidget(makeValueWidget(close_upper_val_), 2, 1, Qt::AlignCenter);
    dataLayout->addWidget(makeValueWidget(close_lower_val_), 3, 1, Qt::AlignCenter);

    // Column 2 : 开阀 headers + values
    dataLayout->addWidget(makeLabelWidget(QStringLiteral("开阀"), headerOpenStyle), 0, 2, Qt::AlignCenter);
    dataLayout->addWidget(makeValueWidget(open_time_val_),  1, 2, Qt::AlignCenter);
    dataLayout->addWidget(makeValueWidget(open_upper_val_), 2, 2, Qt::AlignCenter);
    dataLayout->addWidget(makeValueWidget(open_lower_val_), 3, 2, Qt::AlignCenter);

    mainLayout->addWidget(dataGroup);

    // =====================================================================
    // Log output group
    // =====================================================================
    auto *logGroup = new QGroupBox(QStringLiteral("测试日志"), this);
    logGroup->setStyleSheet(
        "QGroupBox {"
        "  font-size: 13px; font-weight: bold; color: #444;"
        "  border: 1px solid #D0D0D0; border-radius: 6px;"
        "  margin-top: 8px; padding-top: 16px;"
        "  background: #FAFAFA;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin; left: 12px; padding: 0 6px;"
        "}");

    auto *logLayout = new QVBoxLayout(logGroup);
    logLayout->setContentsMargins(12, 18, 12, 12);

    log_output_ = new QTextEdit(logGroup);
    log_output_->setReadOnly(true);
    log_output_->setStyleSheet(
        "QTextEdit {"
        "  background: #1E1E1E; color: #D4D4D4;"
        "  border: 1px solid #555; border-radius: 4px;"
        "  font-family: 'Consolas', 'Courier New', monospace;"
        "  font-size: 12px; padding: 6px;"
        "}");
    log_output_->setMinimumHeight(120);
    logLayout->addWidget(log_output_, 1);

    // Clear button
    auto *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    clear_log_btn_ = new QPushButton(QStringLiteral("清空日志"), logGroup);
    clear_log_btn_->setFixedWidth(100);
    clear_log_btn_->setCursor(Qt::PointingHandCursor);
    clear_log_btn_->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "      stop:0 #fafafa, stop:0.4 #eaeaea, stop:0.5 #dddddd, stop:0.6 #d0d0d0, stop:1 #e2e2e2);"
        "  border: 1px solid #b0b0b0;"
        "  border-top-color: #e0e0e0; border-left-color: #e0e0e0;"
        "  border-right-color: #a0a0a0; border-bottom-color: #909090;"
        "  border-radius: 4px; padding: 5px 12px; font-size: 12px; color: #555;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "      stop:0 #ffffff, stop:0.4 #f2f2f2, stop:0.5 #e6e6e6, stop:0.6 #d8d8d8, stop:1 #eaeaea);"
        "}"
        "QPushButton:pressed {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "      stop:0 #c8c8c8, stop:0.4 #d0d0d0, stop:0.5 #d4d4d4, stop:0.6 #d8d8d8, stop:1 #d0d0d0);"
        "  border-top-color: #909090; border-left-color: #909090;"
        "  border-right-color: #c0c0c0; border-bottom-color: #c0c0c0;"
        "}");
    connect(clear_log_btn_, &QPushButton::clicked, this, &TestPanel::clearLog);
    btnRow->addWidget(clear_log_btn_);
    logLayout->addLayout(btnRow);

    mainLayout->addWidget(logGroup, 1);
}

// ============================================================================
// Data display setters
// ============================================================================
void TestPanel::setCloseSmallTravelTime(double val) {
    close_time_val_->setText(QString("%1 s").arg(val, 0, 'f', 2));
}
void TestPanel::setCloseCurrentUpper(double val) {
    close_upper_val_->setText(QString("%1 mA").arg(val, 0, 'f', 1));
}
void TestPanel::setCloseCurrentLower(double val) {
    close_lower_val_->setText(QString("%1 mA").arg(val, 0, 'f', 1));
}
void TestPanel::setOpenSmallTravelTime(double val) {
    open_time_val_->setText(QString("%1 s").arg(val, 0, 'f', 2));
}
void TestPanel::setOpenCurrentUpper(double val) {
    open_upper_val_->setText(QString("%1 mA").arg(val, 0, 'f', 1));
}
void TestPanel::setOpenCurrentLower(double val) {
    open_lower_val_->setText(QString("%1 mA").arg(val, 0, 'f', 1));
}

// ============================================================================
// Log
// ============================================================================
void TestPanel::appendLog(const QString &text)
{
    log_output_->append(text);
    // Auto-scroll to bottom
    auto *sb = log_output_->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void TestPanel::clearLog()
{
    log_output_->clear();
}

// ============================================================================
// Test button callback
// ============================================================================
void TestPanel::setTestButtonCallback(void (*callback)())
{
    test_callback_ = callback;
}

void TestPanel::onTestButtonClicked()
{
    appendLog(QStringLiteral("[系统] 测试程序启动..."));

    if (test_callback_) {
        test_callback_();
    }

    emit testButtonClicked();
}
