#ifndef VALVE_SIMULATOR_H
#define VALVE_SIMULATOR_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QMutex>
#include "valve_types.h"

// ============================================================================
// ValveSimulator — the core simulation engine
// ============================================================================
// Runs on a 100ms tick. Computes position, current, and time for each step.
// Emits signals so the UI can stay in sync.
// ============================================================================
class ValveSimulator : public QObject
{
    Q_OBJECT

public:
    explicit ValveSimulator(QObject *parent = nullptr);
    ~ValveSimulator() override;

    // --- Commands -----------------------------------------------------------
    void openValve();          // 开阀
    void closeValve();         // 关阀
    void stopValve();          // 停止

    // --- Parameters ---------------------------------------------------------
    void setParams(const ValveParams &params);
    ValveParams params() const;

    // --- Snapshot (thread-safe read) ---------------------------------------
    ValveSnapshot snapshot() const;

    // --- Waveform data -----------------------------------------------------
    QVector<DataPoint> waveformData() const;
    void clearWaveform();

    // --- Running state -----------------------------------------------------
    bool isRunning() const;

signals:
    void stateChanged();        // emitted each tick — UI should read snapshot()
    void actionStarted();       // new open/close action just began
    void actionStopped();       // valve stopped (IDLE or STOPPED)
    void limitReached(bool isOpenLimit); // reached an endpoint

private slots:
    void onTick();

private:
    void startAction(bool opening);
    void resetToIdle();

    // --- Mutable state (protected by mutex for API thread safety) ----------
    mutable QMutex mutex_;

    ValveParams    params_;
    ValveState     state_ = ValveState::IDLE;
    double         position_     = 0.0;   // 0..1
    double         elapsed_time_ = 0.0;   // total seconds since action start
    double         current_      = 0.0;   // mA
    double         phase_elapsed_= 0.0;   // seconds inside current phase
    bool           opening_;              // direction hint
    bool           stall_started_ = false;

    QVector<DataPoint> waveform_;

    QTimer        *tick_timer_ = nullptr;
    static constexpr double TICK_INTERVAL = 0.1;  // 100 ms
};

#endif // VALVE_SIMULATOR_H
