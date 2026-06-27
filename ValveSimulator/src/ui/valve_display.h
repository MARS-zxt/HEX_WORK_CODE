#ifndef VALVE_DISPLAY_H
#define VALVE_DISPLAY_H

#include <QWidget>
#include <QLabel>
#include "valve_types.h"

// ============================================================================
// ValveTrackWidget — custom-painted valve track with position ball
// ============================================================================
class ValveTrackWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ValveTrackWidget(QWidget *parent = nullptr);

    void setPosition(double pos);       // 0.0 = closed, 1.0 = open
    void setOpenLimit(bool on);
    void setCloseLimit(bool on);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    double position_     = 0.0;
    bool   open_limit_  = false;
    bool   close_limit_ = false;

    // Cached geometry
    QRectF track_rect_;
    QPointF ball_center_;
    int    ball_radius_ = 10;

    void updateGeometry();
};

// ============================================================================
// ValveDisplay — composite widget: track + signal lights + time/current readouts
// ============================================================================
class ValveDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit ValveDisplay(QWidget *parent = nullptr);

    void updateFromSnapshot(const ValveSnapshot &snap);

    // Access to labels for direct styling
    QLabel* timeLabel()    const { return time_value_; }
    QLabel* currentLabel() const { return current_value_; }

private:
    ValveTrackWidget *track_       = nullptr;

    // Signal indicator lights
    QWidget *open_light_  = nullptr;
    QWidget *close_light_ = nullptr;

    // Time / current readouts
    QLabel *time_label_    = nullptr;
    QLabel *time_value_    = nullptr;
    QLabel *current_label_ = nullptr;
    QLabel *current_value_ = nullptr;

    void setupUi();
};

#endif // VALVE_DISPLAY_H
