#include "valve_simulator.h"
#include <QtMath>

// ============================================================================
// Construction
// ============================================================================
ValveSimulator::ValveSimulator(QObject *parent)
    : QObject(parent)
{
    tick_timer_ = new QTimer(this);
    tick_timer_->setTimerType(Qt::PreciseTimer);
    connect(tick_timer_, &QTimer::timeout, this, &ValveSimulator::onTick);
}

ValveSimulator::~ValveSimulator()
{
    tick_timer_->stop();
}

// ============================================================================
// Commands
// ============================================================================
void ValveSimulator::openValve()
{
    QMutexLocker lock(&mutex_);
    if (position_ >= 1.0 - 1e-9) {
        // Already fully open — just signal
        return;
    }
    startAction(true);
}

void ValveSimulator::closeValve()
{
    QMutexLocker lock(&mutex_);
    if (position_ <= 1e-9) {
        // Already fully closed
        return;
    }
    startAction(false);
}

void ValveSimulator::stopValve()
{
    {
        QMutexLocker lock(&mutex_);
        tick_timer_->stop();
        current_ = 0.0;
        state_ = ValveState::STOPPED;
    }
    emit stateChanged();
    emit actionStopped();
}

// ============================================================================
// Parameters
// ============================================================================
void ValveSimulator::setParams(const ValveParams &params)
{
    QMutexLocker lock(&mutex_);
    params_ = params;
}

ValveParams ValveSimulator::params() const
{
    QMutexLocker lock(&mutex_);
    return params_;
}

// ============================================================================
// Snapshot
// ============================================================================
ValveSnapshot ValveSimulator::snapshot() const
{
    QMutexLocker lock(&mutex_);
    ValveSnapshot snap;
    snap.elapsed_time = elapsed_time_;
    snap.current      = current_;
    snap.position     = position_;
    snap.state        = state_;
    snap.open_limit   = (position_ >= 1.0 - 1e-9);
    snap.close_limit  = (position_ <= 1e-9);
    return snap;
}

// ============================================================================
// Waveform
// ============================================================================
QVector<DataPoint> ValveSimulator::waveformData() const
{
    QMutexLocker lock(&mutex_);
    return waveform_;
}

void ValveSimulator::clearWaveform()
{
    QMutexLocker lock(&mutex_);
    waveform_.clear();
}

bool ValveSimulator::isRunning() const
{
    QMutexLocker lock(&mutex_);
    return tick_timer_->isActive();
}

// ============================================================================
// Internals
// ============================================================================
void ValveSimulator::startAction(bool opening)
{
    opening_ = opening;
    elapsed_time_ = 0.0;
    phase_elapsed_ = 0.0;
    current_ = 0.0;
    stall_started_ = false;
    waveform_.clear();

    state_ = opening ? ValveState::STARTING_OPEN : ValveState::STARTING_CLOSE;
    tick_timer_->start(static_cast<int>(TICK_INTERVAL * 1000)); // 100 ms
    lock.unlock();

    emit actionStarted();
    emit stateChanged();
}

void ValveSimulator::resetToIdle()
{
    tick_timer_->stop();
    state_ = ValveState::IDLE;
    current_ = 0.0;
}

// ============================================================================
// Tick — core simulation logic, called every 100 ms
// ============================================================================
void ValveSimulator::onTick()
{
    QMutexLocker lock(&mutex_);

    const double dt = TICK_INTERVAL;

    elapsed_time_   += dt;
    phase_elapsed_  += dt;

    switch (state_) {

    // --- STARTING phase ---------------------------------------------------
    case ValveState::STARTING_OPEN:
    case ValveState::STARTING_CLOSE:
    {
        if (phase_elapsed_ <= params_.startup_time) {
            // Ramp current from 0 → startup_current
            double frac = phase_elapsed_ / params_.startup_time;
            current_ = params_.startup_current * frac;
        } else {
            // Startup finished — drop to running current and begin moving
            current_ = params_.running_current;
            phase_elapsed_ = 0.0;
            state_ = (state_ == ValveState::STARTING_OPEN)
                         ? ValveState::MOVING_OPEN
                         : ValveState::MOVING_CLOSE;
        }
        break;
    }

    // --- MOVING phase -----------------------------------------------------
    case ValveState::MOVING_OPEN:
    {
        current_ = params_.running_current;
        position_ += dt / params_.travel_time;
        if (position_ >= 1.0) {
            position_ = 1.0;
            phase_elapsed_ = 0.0;
            state_ = ValveState::STALLING_OPEN;
        }
        break;
    }
    case ValveState::MOVING_CLOSE:
    {
        current_ = params_.running_current;
        position_ -= dt / params_.travel_time;
        if (position_ <= 0.0) {
            position_ = 0.0;
            phase_elapsed_ = 0.0;
            state_ = ValveState::STALLING_CLOSE;
        }
        break;
    }

    // --- STALLING phase (0.3 s ramp to stall current) ---------------------
    case ValveState::STALLING_OPEN:
    case ValveState::STALLING_CLOSE:
    {
        const double STALL_DURATION = 0.3;
        if (phase_elapsed_ <= STALL_DURATION) {
            double frac = phase_elapsed_ / STALL_DURATION;
            current_ = params_.running_current
                       + (params_.stall_current - params_.running_current) * frac;
        } else {
            current_ = params_.stall_current;
            // Stay at limit — don't transition to IDLE automatically
            // (valve stays energized at the endpoint until user stops)
            // But stop ticking new data
            tick_timer_->stop();
        }
        break;
    }

    default:
        break;
    }

    // Record waveform data point
    waveform_.append({elapsed_time_, current_});

    // Emit signals
    lock.unlock();

    if (state_ == ValveState::STALLING_OPEN || state_ == ValveState::STALLING_CLOSE) {
        emit limitReached(state_ == ValveState::STALLING_OPEN);
    }

    emit stateChanged();
}
