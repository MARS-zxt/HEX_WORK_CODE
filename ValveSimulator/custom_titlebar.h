#ifndef CUSTOM_TITLEBAR_H
#define CUSTOM_TITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

// ============================================================================
// TrafficLightButton — macOS-style red/yellow/green dot button
// ============================================================================
class TrafficLightButton : public QPushButton
{
    Q_OBJECT
public:
    enum LightColor { Red, Yellow, Green };
    explicit TrafficLightButton(LightColor color, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    LightColor color_;
    bool hovered_ = false;
    QColor baseColor() const;
};

// ============================================================================
// CustomTitleBar — frameless window title bar
// ============================================================================
class CustomTitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit CustomTitleBar(QWidget *parent = nullptr);

    void setTitle(const QString &title);
    void setDarkMode(bool dark);

signals:
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QLabel    *title_label_ = nullptr;
    QWidget   *traffic_light_container_ = nullptr;
    QPoint     drag_start_pos_;
    bool       dragging_ = false;
    bool       dark_mode_ = false;
};

#endif // CUSTOM_TITLEBAR_H
