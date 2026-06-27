#include "valve_display.h"
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>

// ============================================================================
// ValveTrackWidget
// ============================================================================
ValveTrackWidget::ValveTrackWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(80);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void ValveTrackWidget::setPosition(double pos)
{
    position_ = qBound(0.0, pos, 1.0);
    updateGeometry();
    update();
}

void ValveTrackWidget::setOpenLimit(bool on)
{
    open_limit_ = on;
    update();
}

void ValveTrackWidget::setCloseLimit(bool on)
{
    close_limit_ = on;
    update();
}

void ValveTrackWidget::resizeEvent(QResizeEvent *)
{
    updateGeometry();
}

void ValveTrackWidget::updateGeometry()
{
    const int margin = 40;
    const int track_height = 16;
    const int y_center = height() / 2;

    track_rect_ = QRectF(margin, y_center - track_height / 2,
                          width() - 2 * margin, track_height);

    double x = track_rect_.left() + position_ * track_rect_.width();
    ball_center_ = QPointF(x, y_center);
}

void ValveTrackWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // --- Track background ---
    QLinearGradient trackGrad(track_rect_.topLeft(), track_rect_.bottomLeft());
    trackGrad.setColorAt(0.0, QColor(0xE0, 0xE0, 0xE0));
    trackGrad.setColorAt(0.3, QColor(0xF5, 0xF5, 0xF5));
    trackGrad.setColorAt(0.5, QColor(0xFA, 0xFA, 0xFA));
    trackGrad.setColorAt(0.7, QColor(0xE8, 0xE8, 0xE8));
    trackGrad.setColorAt(1.0, QColor(0xC0, 0xC0, 0xC0));

    p.setPen(QPen(QColor(0xA0, 0xA0, 0xA0), 1));
    p.setBrush(trackGrad);
    p.drawRoundedRect(track_rect_, 4, 4);

    // Inner groove
    QRectF groove = track_rect_.adjusted(3, 3, -3, -3);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0xD0, 0xD0, 0xD0));
    p.drawRoundedRect(groove, 2, 2);

    // --- End labels ---
    p.setPen(QColor(0x66, 0x66, 0x66));
    QFont font = p.font();
    font.setPointSize(9);
    p.setFont(font);

    QRectF leftLabel(track_rect_.left() - 30, track_rect_.bottom() + 10, 60, 20);
    QRectF rightLabel(track_rect_.right() - 30, track_rect_.bottom() + 10, 60, 20);
    p.drawText(leftLabel, Qt::AlignCenter, QStringLiteral("关到位"));
    p.drawText(rightLabel, Qt::AlignCenter, QStringLiteral("开到位"));

    // --- Position ball (blue) ---
    // Outer glow
    QRadialGradient glow(ball_center_, ball_radius_ + 4);
    glow.setColorAt(0.0, QColor(0x40, 0x90, 0xFF, 80));
    glow.setColorAt(1.0, QColor(0x40, 0x90, 0xFF, 0));
    p.setPen(Qt::NoPen);
    p.setBrush(glow);
    p.drawEllipse(ball_center_, ball_radius_ + 4, ball_radius_ + 4);

    // Ball 3D
    QRadialGradient ballGrad(ball_center_.x() - 3, ball_center_.y() - 3, ball_radius_);
    ballGrad.setColorAt(0.0, QColor(0xA0, 0xD0, 0xFF));
    ballGrad.setColorAt(0.5, QColor(0x30, 0x80, 0xF0));
    ballGrad.setColorAt(1.0, QColor(0x10, 0x50, 0xC0));
    p.setBrush(ballGrad);
    p.setPen(QPen(QColor(0x10, 0x40, 0xA0, 100), 1));
    p.drawEllipse(ball_center_, ball_radius_, ball_radius_);

    // Highlight on ball
    QRadialGradient highlight(QPointF(ball_center_.x() - 2, ball_center_.y() - 3), ball_radius_ * 0.6);
    highlight.setColorAt(0.0, QColor(255, 255, 255, 120));
    highlight.setColorAt(1.0, QColor(255, 255, 255, 0));
    p.setBrush(highlight);
    p.setPen(Qt::NoPen);
    p.drawEllipse(ball_center_, ball_radius_, ball_radius_);
}

// ============================================================================
// SignalLight — small helper to draw indicator lamps
// ============================================================================
static QWidget* makeSignalLight(const QColor &onColor, QWidget *parent)
{
    auto *w = new QWidget(parent);
    w->setFixedSize(20, 20);
    w->setStyleSheet(
        QString("background-color: %1; border-radius: 10px; "
                "border: 1px solid #999;")
            .arg(QColor(0xCC, 0xCC, 0xCC).name())); // off = gray
    w->setProperty("onColor", onColor.name());
    w->setProperty("isOn", false);
    return w;
}

static void setSignalLight(QWidget *light, bool on)
{
    bool prev = light->property("isOn").toBool();
    if (prev == on) return;
    light->setProperty("isOn", on);
    QColor c = on ? QColor(light->property("onColor").toString()) : QColor(0xCC, 0xCC, 0xCC);
    light->setStyleSheet(
        QString("background-color: %1; border-radius: 10px; "
                "border: 1px solid #999;").arg(c.name()));
}

// ============================================================================
// ValveDisplay
// ============================================================================
ValveDisplay::ValveDisplay(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void ValveDisplay::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(10);

    // --- Signal lights row ---
    auto *lightsLayout = new QHBoxLayout();
    lightsLayout->setSpacing(8);

    close_light_ = makeSignalLight(QColor(0xFF, 0x40, 0x40), this); // red
    open_light_  = makeSignalLight(QColor(0x40, 0xFF, 0x40), this); // green

    auto *closeLabel = new QLabel(QStringLiteral("关到位信号"), this);
    closeLabel->setStyleSheet("font-size: 11px; color: #555; background: transparent;");
    auto *openLabel = new QLabel(QStringLiteral("开到位信号"), this);
    openLabel->setStyleSheet("font-size: 11px; color: #555; background: transparent;");

    lightsLayout->addWidget(close_light_);
    lightsLayout->addWidget(closeLabel);
    lightsLayout->addSpacing(20);
    lightsLayout->addWidget(open_light_);
    lightsLayout->addWidget(openLabel);
    lightsLayout->addStretch();

    mainLayout->addLayout(lightsLayout);

    // --- Track ---
    track_ = new ValveTrackWidget(this);
    mainLayout->addWidget(track_);

    // --- Time / Current display ---
    auto *readoutLayout = new QHBoxLayout();
    readoutLayout->setSpacing(30);

    // Time group
    auto *timeGroup = new QHBoxLayout();
    time_label_ = new QLabel(QStringLiteral("时间："), this);
    time_label_->setStyleSheet("font-size: 13px; font-weight: bold; color: #333; background: transparent;");
    time_value_ = new QLabel("0.0 s", this);
    time_value_->setStyleSheet(
        "font-size: 18px; font-weight: bold; color: #2070C0; "
        "background: #F0F5FF; border: 1px solid #B0C8E8; border-radius: 4px; padding: 4px 12px;");
    timeGroup->addWidget(time_label_);
    timeGroup->addWidget(time_value_);
    readoutLayout->addLayout(timeGroup);

    // Current group
    auto *currGroup = new QHBoxLayout();
    current_label_ = new QLabel(QStringLiteral("电流："), this);
    current_label_->setStyleSheet("font-size: 13px; font-weight: bold; color: #333; background: transparent;");
    current_value_ = new QLabel("0 mA", this);
    current_value_->setStyleSheet(
        "font-size: 18px; font-weight: bold; color: #C04020; "
        "background: #FFF5F0; border: 1px solid #E8C8B0; border-radius: 4px; padding: 4px 12px;");
    currGroup->addWidget(current_label_);
    currGroup->addWidget(current_value_);
    readoutLayout->addLayout(currGroup);

    readoutLayout->addStretch();
    mainLayout->addLayout(readoutLayout);

    mainLayout->addStretch();
}

void ValveDisplay::updateFromSnapshot(const ValveSnapshot &snap)
{
    track_->setPosition(snap.position);
    track_->setOpenLimit(snap.open_limit);
    track_->setCloseLimit(snap.close_limit);

    setSignalLight(open_light_, snap.open_limit);
    setSignalLight(close_light_, snap.close_limit);

    time_value_->setText(QString("%1 s").arg(snap.elapsed_time, 0, 'f', 2));
    current_value_->setText(QString("%1 mA").arg(snap.current, 0, 'f', 1));
}
