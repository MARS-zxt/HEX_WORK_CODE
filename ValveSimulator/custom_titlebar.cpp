#include "custom_titlebar.h"
#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

// ============================================================================
// TrafficLightButton
// ============================================================================
TrafficLightButton::TrafficLightButton(LightColor color, QWidget *parent)
    : QPushButton(parent), color_(color)
{
    setFixedSize(14, 14);
    setCursor(Qt::PointingHandCursor);
    setToolTip(color == Red ? QStringLiteral("关闭")
               : color == Yellow ? QStringLiteral("最小化")
                                 : QStringLiteral("最大化"));
}

QColor TrafficLightButton::baseColor() const
{
    switch (color_) {
    case Red:    return QColor(0xFF, 0x5F, 0x57);
    case Yellow: return QColor(0xFF, 0xBD, 0x2E);
    case Green:  return QColor(0x28, 0xCA, 0x41);
    }
    return Qt::gray;
}

void TrafficLightButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QColor fill = baseColor();
    if (hovered_) {
        // Draw the symbol inside
        QRect r = rect().adjusted(2, 2, -2, -2);
        p.setPen(Qt::NoPen);
        p.setBrush(fill);
        p.drawEllipse(r);

        QColor symbol = QColor(0, 0, 0, 60);
        p.setBrush(symbol);
        p.setPen(Qt::NoPen);

        if (color_ == Red) {
            // X shape
            p.setPen(QPen(symbol, 1.2));
            p.drawLine(5, 5, 9, 9);
            p.drawLine(9, 5, 5, 9);
        } else if (color_ == Yellow) {
            // minus
            p.fillRect(4, 7, 6, 1.5, symbol);
        } else {
            // plus / diamond
            p.setPen(QPen(symbol, 1.2));
            p.drawLine(7, 4, 7, 10);
            p.drawLine(4, 7, 10, 7);
        }
    } else {
        p.setPen(Qt::NoPen);
        p.setBrush(fill);
        p.drawEllipse(rect().adjusted(2, 2, -2, -2));
    }
}

void TrafficLightButton::enterEvent(QEvent *)
{
    hovered_ = true;
    update();
}

void TrafficLightButton::leaveEvent(QEvent *)
{
    hovered_ = false;
    update();
}

// ============================================================================
// CustomTitleBar
// ============================================================================
CustomTitleBar::CustomTitleBar(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(48);
    setCursor(Qt::ArrowCursor);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 0, 8, 0);
    layout->setSpacing(6);

    // Traffic light buttons
    traffic_light_container_ = new QWidget(this);
    auto *btnLayout = new QHBoxLayout(traffic_light_container_);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(8);

    auto *closeBtn = new TrafficLightButton(TrafficLightButton::Red, this);
    auto *minBtn   = new TrafficLightButton(TrafficLightButton::Yellow, this);
    auto *maxBtn   = new TrafficLightButton(TrafficLightButton::Green, this);

    connect(closeBtn, &QPushButton::clicked, this, &CustomTitleBar::closeClicked);
    connect(minBtn,   &QPushButton::clicked, this, &CustomTitleBar::minimizeClicked);
    connect(maxBtn,   &QPushButton::clicked, this, &CustomTitleBar::maximizeClicked);

    btnLayout->addWidget(closeBtn);
    btnLayout->addWidget(minBtn);
    btnLayout->addWidget(maxBtn);

    layout->addWidget(traffic_light_container_);

    // Title — centered
    title_label_ = new QLabel(QStringLiteral("虚拟阀门模拟器"), this);
    title_label_->setAlignment(Qt::AlignCenter);
    title_label_->setStyleSheet("color: #333; font-size: 13px; font-weight: 600; background: transparent;");
    layout->addWidget(title_label_, 1);

    // Spacer on the right to balance the traffic lights
    auto *spacer = new QWidget(this);
    spacer->setFixedWidth(14 * 3 + 8 * 2 + 12); // same width as traffic light area
    layout->addWidget(spacer);
}

void CustomTitleBar::setTitle(const QString &title)
{
    title_label_->setText(title);
}

void CustomTitleBar::setDarkMode(bool dark)
{
    dark_mode_ = dark;
    title_label_->setStyleSheet(
        dark ? "color: #eee; font-size: 13px; font-weight: 600; background: transparent;"
             : "color: #333; font-size: 13px; font-weight: 600; background: transparent;");
    update();
}

void CustomTitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        drag_start_pos_ = event->globalPos() - window()->geometry().topLeft();
        dragging_ = true;
    }
}

void CustomTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging_ && (event->buttons() & Qt::LeftButton)) {
        window()->move(event->globalPos() - drag_start_pos_);
    }
}

void CustomTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit maximizeClicked();
    }
}

void CustomTitleBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Semi-transparent background
    QColor bg = dark_mode_ ? QColor(40, 40, 45, 240) : QColor(255, 255, 255, 230);
    p.setBrush(bg);
    p.setPen(Qt::NoPen);
    // Only round top corners
    p.drawRoundedRect(rect().adjusted(0, 0, 0, 0), 20, 20);

    // Bottom separator
    p.setPen(QPen(QColor(0, 0, 0, 20), 1));
    p.drawLine(rect().bottomLeft(), rect().bottomRight());
}
