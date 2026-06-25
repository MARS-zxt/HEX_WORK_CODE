#include "valve_params_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QGraphicsDropShadowEffect>

ValveParamsDialog::ValveParamsDialog(const ValveParams &current, QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    setupUi(current);
    applyStyle();
}

void ValveParamsDialog::setupUi(const ValveParams &params)
{
    // Outer layout for shadow
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(10, 10, 10, 10);

    // Container widget with rounded corners
    auto *container = new QFrame(this);
    container->setObjectName("dialogContainer");
    container->setStyleSheet(
        "#dialogContainer {"
        "  background: #FFFFFF;"
        "  border-radius: 12px;"
        "  border: 1px solid #D0D0D0;"
        "}");

    auto *shadow = new QGraphicsDropShadowEffect(container);
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 60));
    container->setGraphicsEffect(shadow);

    auto *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(20, 16, 20, 20);
    containerLayout->setSpacing(12);

    // Title bar (dialog internal)
    auto *titleLayout = new QHBoxLayout();
    auto *titleLabel = new QLabel(QStringLiteral("阀门参数设置"), container);
    titleLabel->setStyleSheet("font-size: 15px; font-weight: bold; color: #333; background: transparent;");

    auto *closeBtn = new QPushButton(QStringLiteral("✕"), container);
    closeBtn->setFixedSize(28, 28);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton { background: #F0F0F0; border: 1px solid #C0C0C0; border-radius: 6px; "
        "font-size: 14px; color: #666; }"
        "QPushButton:hover { background: #FF5F57; color: white; border-color: #FF5F57; }");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);

    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(closeBtn);
    containerLayout->addLayout(titleLayout);

    // Separator
    auto *sep = new QFrame(container);
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #E0E0E0;");
    containerLayout->addWidget(sep);

    // Form
    auto *formLayout = new QFormLayout();
    formLayout->setSpacing(10);
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto makeSpinBox = [&](double val, double min, double max, double step, int decimals,
                            const QString &suffix) {
        auto *sb = new QDoubleSpinBox(container);
        sb->setRange(min, max);
        sb->setSingleStep(step);
        sb->setDecimals(decimals);
        sb->setValue(val);
        sb->setSuffix(suffix);
        sb->setFixedWidth(160);
        sb->setStyleSheet(
            "QDoubleSpinBox {"
            "  padding: 5px 8px; border: 1px solid #C0C0C0; border-radius: 4px;"
            "  font-size: 13px; background: #FAFAFA;"
            "}"
            "QDoubleSpinBox:focus { border-color: #3080F0; }");
        return sb;
    };

    startup_time_    = makeSpinBox(params.startup_time,    0.1, 10.0, 0.1, 1, " s");
    travel_time_     = makeSpinBox(params.travel_time,     0.1, 60.0, 0.1, 1, " s");
    running_current_ = makeSpinBox(params.running_current, 1.0, 5000.0, 1.0, 1, " mA");
    stall_current_   = makeSpinBox(params.stall_current,   1.0, 5000.0, 1.0, 1, " mA");
    startup_current_ = makeSpinBox(params.startup_current, 1.0, 5000.0, 1.0, 1, " mA");

    auto makeLabel = [&](const QString &text, const QString &tooltip) {
        auto *lbl = new QLabel(text, container);
        lbl->setToolTip(tooltip);
        lbl->setStyleSheet("font-size: 13px; color: #444; background: transparent;");
        return lbl;
    };

    formLayout->addRow(makeLabel(QStringLiteral("启动时间:"),
                                 QStringLiteral("启动后电流攀升至启动电流值所需时间")),
                       startup_time_);
    formLayout->addRow(makeLabel(QStringLiteral("到位时间:"),
                                 QStringLiteral("阀门从一端到另一端所需时间")),
                       travel_time_);
    formLayout->addRow(makeLabel(QStringLiteral("动作电流:"),
                                 QStringLiteral("阀门在行进过程中的电流")),
                       running_current_);
    formLayout->addRow(makeLabel(QStringLiteral("堵转电流:"),
                                 QStringLiteral("到达端点时的电流")),
                       stall_current_);
    formLayout->addRow(makeLabel(QStringLiteral("启动电流:"),
                                 QStringLiteral("启动时间内电流攀升的峰值")),
                       startup_current_);

    containerLayout->addLayout(formLayout);

    containerLayout->addSpacing(8);

    // Buttons
    auto *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    auto *okBtn = new QPushButton(QStringLiteral("确定"), container);
    okBtn->setFixedWidth(90);
    okBtn->setCursor(Qt::PointingHandCursor);

    auto *cancelBtn = new QPushButton(QStringLiteral("取消"), container);
    cancelBtn->setFixedWidth(90);
    cancelBtn->setCursor(Qt::PointingHandCursor);

    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    containerLayout->addLayout(btnLayout);

    outerLayout->addWidget(container);

    connect(okBtn,     &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    resize(420, 380);
}

void ValveParamsDialog::applyStyle()
{
    // 3D button style for OK / Cancel
    QString btn3D = R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #fafafa, stop:0.4 #eaeaea, stop:0.5 #dddddd, stop:0.6 #d0d0d0, stop:1 #e2e2e2);
            border: 1px solid #b0b0b0;
            border-top-color: #e0e0e0;
            border-left-color: #e0e0e0;
            border-right-color: #a0a0a0;
            border-bottom-color: #909090;
            border-radius: 5px;
            padding: 6px 16px;
            font-size: 13px;
            color: #333;
            font-weight: bold;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #ffffff, stop:0.4 #f2f2f2, stop:0.5 #e6e6e6, stop:0.6 #d8d8d8, stop:1 #eaeaea);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #c8c8c8, stop:0.4 #d0d0d0, stop:0.5 #d4d4d4, stop:0.6 #d8d8d8, stop:1 #d0d0d0);
            border-top-color: #909090;
            border-left-color: #909090;
            border-right-color: #c0c0c0;
            border-bottom-color: #c0c0c0;
            padding-top: 7px;
            padding-bottom: 5px;
        }
    )";

    // Find all QPushButtons in the dialog and apply the 3D style
    for (auto *btn : findChildren<QPushButton*>()) {
        if (btn->text() == QStringLiteral("确定") || btn->text() == QStringLiteral("取消")) {
            btn->setStyleSheet(btn3D);
        }
    }
}

ValveParams ValveParamsDialog::getParams() const
{
    ValveParams p;
    p.startup_time    = startup_time_->value();
    p.travel_time     = travel_time_->value();
    p.running_current = running_current_->value();
    p.stall_current   = stall_current_->value();
    p.startup_current = startup_current_->value();
    return p;
}
