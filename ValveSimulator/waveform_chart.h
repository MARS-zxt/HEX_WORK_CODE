#ifndef WAVEFORM_CHART_H
#define WAVEFORM_CHART_H

#include <QWidget>
#include <QVector>
#include "valve_types.h"

// ============================================================================
// WaveformChart — real-time current-vs-time chart (custom painted)
// ============================================================================
class WaveformChart : public QWidget
{
    Q_OBJECT
public:
    explicit WaveformChart(QWidget *parent = nullptr);

    void setData(const QVector<DataPoint> &data);
    void clearData();
    void setDarkMode(bool dark);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<DataPoint> data_;
    bool dark_mode_ = false;

    // Axis limits
    double x_min_ = 0.0;   // left edge (scrolls)
    double x_max_ = 10.0;  // right edge (x_min_ + 10.0)
    double y_max_ = 50.0;  // mA (auto-scale)
    double y_min_ = 0.0;

    void updateXAxis();

    void autoScaleY();

    static constexpr int   MARGIN_LEFT   = 55;
    static constexpr int   MARGIN_RIGHT  = 15;
    static constexpr int   MARGIN_TOP    = 15;
    static constexpr int   MARGIN_BOTTOM = 35;
};

#endif // WAVEFORM_CHART_H
