#include "waveform_chart.h"
#include <QPainter>
#include <QPainterPath>
#include <QtMath>
#include <algorithm>

WaveformChart::WaveformChart(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(false);
}

void WaveformChart::setData(const QVector<DataPoint> &data)
{
    data_ = data;
    autoScaleY();
    update();
}

void WaveformChart::clearData()
{
    data_.clear();
    y_max_ = 50.0;
    update();
}

void WaveformChart::setDarkMode(bool dark)
{
    dark_mode_ = dark;
    update();
}

void WaveformChart::autoScaleY()
{
    if (data_.isEmpty()) {
        y_max_ = 50.0;
        return;
    }

    double maxVal = 0.0;
    for (const auto &dp : data_) {
        if (dp.current > maxVal) maxVal = dp.current;
    }

    // Round up to nice number with headroom
    if (maxVal < 10)       y_max_ = 10;
    else if (maxVal < 50)  y_max_ = 50;
    else if (maxVal < 100) y_max_ = 100;
    else if (maxVal < 200) y_max_ = 200;
    else if (maxVal < 500) y_max_ = 500;
    else {
        // Round to nearest 100
        y_max_ = std::ceil(maxVal / 100.0) * 100.0;
    }
    // Ensure at least some visible range
    if (y_max_ < 50) y_max_ = 50;
}

void WaveformChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    QRect plotArea(MARGIN_LEFT, MARGIN_TOP,
                   w - MARGIN_LEFT - MARGIN_RIGHT,
                   h - MARGIN_TOP - MARGIN_BOTTOM);

    QColor bgColor   = dark_mode_ ? QColor(0x2D, 0x2D, 0x30) : QColor(0xFA, 0xFA, 0xFA);
    QColor gridColor = dark_mode_ ? QColor(0xFF, 0xFF, 0xFF, 30) : QColor(0, 0, 0, 20);
    QColor axisColor = dark_mode_ ? QColor(0xCC, 0xCC, 0xCC) : QColor(0x44, 0x44, 0x44);
    QColor textColor = dark_mode_ ? QColor(0xBB, 0xBB, 0xBB) : QColor(0x55, 0x55, 0x55);

    // Background
    p.setPen(Qt::NoPen);
    p.setBrush(bgColor);
    p.drawRoundedRect(plotArea.adjusted(-2, -2, 2, 2), 4, 4);

    // Border
    p.setPen(QPen(dark_mode_ ? QColor(0x55, 0x55, 0x55) : QColor(0xCC, 0xCC, 0xCC), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(plotArea.adjusted(-2, -2, 2, 2), 4, 4);

    // --- Draw grid lines & Y-axis labels ---
    QFont labelFont = p.font();
    labelFont.setPointSize(8);
    p.setFont(labelFont);

    // Y axis: 5 divisions
    int yDivs = 5;
    for (int i = 0; i <= yDivs; ++i) {
        double val = y_min_ + (y_max_ - y_min_) * i / yDivs;
        int y = plotArea.bottom() - static_cast<int>(plotArea.height() * i / yDivs);

        // Grid line
        p.setPen(QPen(gridColor, 1, Qt::DotLine));
        p.drawLine(plotArea.left(), y, plotArea.right(), y);

        // Label
        p.setPen(textColor);
        QRectF labelRect(0, y - 8, MARGIN_LEFT - 5, 16);
        p.drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, QString("%1").arg(val, 0, 'f', 0));
    }

    // X axis: 11 divisions (0..10, step 1.0)
    int xDivs = 10;
    for (int i = 0; i <= xDivs; ++i) {
        double val = x_max_ * i / xDivs;
        int x = plotArea.left() + static_cast<int>(plotArea.width() * i / xDivs);

        p.setPen(QPen(gridColor, 1, Qt::DotLine));
        p.drawLine(x, plotArea.top(), x, plotArea.bottom());

        p.setPen(textColor);
        QRectF labelRect(x - 20, plotArea.bottom() + 4, 40, 16);
        p.drawText(labelRect, Qt::AlignCenter, QString("%1").arg(val, 0, 'f', 1));
    }

    // --- Axis titles ---
    QFont titleFont = p.font();
    titleFont.setPointSize(9);
    titleFont.setBold(true);
    p.setFont(titleFont);
    p.setPen(axisColor);

    // Y-axis title
    p.save();
    p.translate(12, plotArea.center().y());
    p.rotate(-90);
    p.drawText(QRectF(-30, -12, 60, 24), Qt::AlignCenter, "mA");
    p.restore();

    // X-axis title
    p.drawText(QRectF(plotArea.center().x() - 30, h - 18, 60, 16),
               Qt::AlignCenter, QStringLiteral("时间 (s)"));

    // --- Draw waveform line ---
    if (data_.size() < 2) return;

    // Clip to plot area
    p.save();
    p.setClipRect(plotArea);

    QPainterPath path;
    bool firstPoint = true;

    for (const auto &dp : data_) {
        if (dp.time > x_max_) break; // don't draw beyond x max

        double px = plotArea.left() + (dp.time / x_max_) * plotArea.width();
        double py = plotArea.bottom() - ((dp.current - y_min_) / (y_max_ - y_min_)) * plotArea.height();
        py = qBound(static_cast<double>(plotArea.top() - 50), py,
                    static_cast<double>(plotArea.bottom() + 50));

        if (firstPoint) {
            path.moveTo(px, py);
            firstPoint = false;
        } else {
            path.lineTo(px, py);
        }
    }

    // Line
    QPen linePen(QColor(0x30, 0x80, 0xF0), 2);
    p.setPen(linePen);
    p.setBrush(Qt::NoBrush);
    p.drawPath(path);

    // Fill under curve
    if (!data_.isEmpty()) {
        QPainterPath fillPath = path;
        double lastX = plotArea.left() + (data_.last().time / x_max_) * plotArea.width();
        fillPath.lineTo(lastX, plotArea.bottom());
        fillPath.lineTo(plotArea.left(), plotArea.bottom());
        fillPath.closeSubpath();

        QLinearGradient fillGrad(0, plotArea.top(), 0, plotArea.bottom());
        fillGrad.setColorAt(0.0, QColor(0x30, 0x80, 0xF0, 60));
        fillGrad.setColorAt(1.0, QColor(0x30, 0x80, 0xF0, 10));
        p.setPen(Qt::NoPen);
        p.setBrush(fillGrad);
        p.drawPath(fillPath);
    }

    p.restore();
}
