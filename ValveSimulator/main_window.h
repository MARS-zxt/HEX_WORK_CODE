#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QWidget>
#include <QSplitter>
#include <QTcpServer>
#include <QTcpSocket>

#include "valve_types.h"
#include "valve_simulator.h"
#include "custom_titlebar.h"
#include "valve_display.h"
#include "waveform_chart.h"
#include "test_panel.h"

// ============================================================================
// MainWindow — top-level frameless window
// ============================================================================
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // --- Public API for C test-program interop -----------------------------
    ValveSimulator* simulator() { return simulator_; }
    ValveDisplay*   valveDisplay() { return valve_display_; }
    WaveformChart*  waveformChart() { return waveform_chart_; }
    TestPanel*      testPanel() { return test_panel_; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onOpenValve();
    void onCloseValve();
    void onStopValve();
    void onParamsClicked();
    void onSimulatorTick();
    void onSimulatorStarted();
    void onSimulatorStopped();
    void onMinimize();
    void onMaximize();
    void onClose();

    // TCP server for external C test programs
    void onNewTcpConnection();
    void onTcpData();

private:
    void setupUi();
    void setupTcpServer();
    void processTcpCommand(const QString &cmd, QTcpSocket *client);

    // Title bar
    CustomTitleBar *title_bar_ = nullptr;

    // Valve panel
    ValveDisplay  *valve_display_  = nullptr;
    WaveformChart *waveform_chart_ = nullptr;

    // Test tab
    TestPanel *test_panel_ = nullptr;

    // Engine
    ValveSimulator *simulator_ = nullptr;

    // TCP
    QTcpServer *tcp_server_ = nullptr;
    QList<QTcpSocket*> tcp_clients_;

    // Shadow margin
    static constexpr int SHADOW_MARGIN = 10;
};

#endif // MAIN_WINDOW_H
