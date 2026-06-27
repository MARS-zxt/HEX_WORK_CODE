#include "version_dialog.h"
#include "version.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QGraphicsDropShadowEffect>

VersionDialog::VersionDialog(QWidget *parent)
    : QDialog(parent)
{
    // Frameless + translucent for rounded corners + shadow
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(440, 420);
    setModal(true);

    // Shadow
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(25);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0, 0, 0, 70));
    setGraphicsEffect(shadow);

    setupUi();
}

void VersionDialog::setupUi()
{
    // Outer layout with shadow margin
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(8, 8, 8, 8);
    outerLayout->setSpacing(0);

    // --- Content container ---
    auto *content = new QWidget(this);
    content->setObjectName("versionContent");
    content->setStyleSheet(
        "#versionContent {"
        "  background: #FFFFFF;"
        "  border-radius: 14px;"
        "}");

    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(24, 22, 24, 18);
    layout->setSpacing(14);

    // --- Title ---
    auto *titleLabel = new QLabel(QStringLiteral("关于 虚拟阀门模拟器"), content);
    titleLabel->setStyleSheet(
        "font-size: 18px; font-weight: bold; color: #333;"
        "background: transparent; padding-bottom: 4px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    // --- Version number ---
    auto *versionLabel = new QLabel(
        QStringLiteral("版本 v") + QStringLiteral(APP_VERSION_STRING), content);
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setStyleSheet(
        "font-size: 22px; font-weight: bold; color: #2070C0;"
        "background: transparent; padding: 6px 0;");
    layout->addWidget(versionLabel);

    // --- Separator ---
    auto *sep = new QLabel(content);
    sep->setFixedHeight(1);
    sep->setStyleSheet("background: #E0E0E0;");
    layout->addWidget(sep);

    // --- Changelog header ---
    auto *logHeader = new QLabel(QStringLiteral("更新日志"), content);
    logHeader->setStyleSheet(
        "font-size: 13px; font-weight: bold; color: #555;"
        "background: transparent;");
    layout->addWidget(logHeader);

    // --- Changelog text (read-only, styled) ---
    auto *logText = new QTextEdit(content);
    logText->setReadOnly(true);
    logText->setPlainText(QStringLiteral(APP_CHANGELOG));
    logText->setStyleSheet(
        "QTextEdit {"
        "  background: #F8F8F8;"
        "  border: 1px solid #E0E0E0;"
        "  border-radius: 6px;"
        "  font-size: 12px;"
        "  color: #444;"
        "  padding: 8px;"
        "  font-family: 'Microsoft YaHei UI', 'Segoe UI', sans-serif;"
        "}");
    logText->setMinimumHeight(160);
    layout->addWidget(logText, 1);

    // --- Build info ---
    QString buildInfo = QStringLiteral("Qt %1 | MSVC 2022 x64 | Windows 10+")
        .arg(QString::fromLatin1(qVersion()));
    auto *buildLabel = new QLabel(buildInfo, content);
    buildLabel->setAlignment(Qt::AlignCenter);
    buildLabel->setStyleSheet(
        "font-size: 11px; color: #999; background: transparent;");
    layout->addWidget(buildLabel);

    // --- Close button ---
    auto *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    auto *closeBtn = new QPushButton(QStringLiteral("关闭"), content);
    closeBtn->setFixedSize(100, 34);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "      stop:0 #fafafa, stop:0.4 #eaeaea, stop:0.5 #dddddd, stop:0.6 #d0d0d0, stop:1 #e2e2e2);"
        "  border: 1px solid #b0b0b0;"
        "  border-top-color: #e0e0e0; border-left-color: #e0e0e0;"
        "  border-right-color: #a0a0a0; border-bottom-color: #909090;"
        "  border-radius: 6px;"
        "  font-size: 14px; font-weight: bold; color: #555;"
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
        "  padding-top: 2px;"
        "}");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnLayout->addWidget(closeBtn);
    layout->addLayout(btnLayout);

    outerLayout->addWidget(content);
}
