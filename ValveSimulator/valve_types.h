#ifndef VALVE_TYPES_H
#define VALVE_TYPES_H

#include <QString>
#include <QVector>
#include <QPair>
#include <QMutex>

// ============================================================================
// Valve Parameters
// ============================================================================
struct ValveParams {
    double startup_time   = 0.5;   // 启动时间 (seconds)
    double travel_time    = 3.0;   // 到位时间 (seconds)
    double running_current= 100.0; // 动作电流 (mA)
    double stall_current  = 300.0; // 堵转电流 (mA)
    double startup_current= 200.0; // 启动电流 (mA)
};

// ============================================================================
// Valve State Machine
// ============================================================================
enum class ValveState {
    IDLE,            // 空闲
    STARTING_OPEN,   // 开阀启动中
    STARTING_CLOSE,  // 关阀启动中
    MOVING_OPEN,     // 开阀行进中
    MOVING_CLOSE,    // 关阀行进中
    STALLING_OPEN,   // 开到位堵转
    STALLING_CLOSE,  // 关到位堵转
    STOPPED          // 已停止 (user pressed stop)
};

inline const char* valveStateName(ValveState s) {
    switch (s) {
    case ValveState::IDLE:           return "IDLE";
    case ValveState::STARTING_OPEN:  return "STARTING_OPEN";
    case ValveState::STARTING_CLOSE: return "STARTING_CLOSE";
    case ValveState::MOVING_OPEN:    return "MOVING_OPEN";
    case ValveState::MOVING_CLOSE:   return "MOVING_CLOSE";
    case ValveState::STALLING_OPEN:  return "STALLING_OPEN";
    case ValveState::STALLING_CLOSE: return "STALLING_CLOSE";
    case ValveState::STOPPED:        return "STOPPED";
    }
    return "UNKNOWN";
}

// ============================================================================
// Data Point for Waveform
// ============================================================================
struct DataPoint {
    double time;     // seconds
    double current;  // mA
};

// ============================================================================
// Valve Snapshot (for API read)
// ============================================================================
struct ValveSnapshot {
    double elapsed_time = 0.0;   // 当前时间 (s)
    double current      = 0.0;   // 当前电流 (mA)
    double position     = 0.0;   // 当前位置 0.0=关到位, 1.0=开到位
    ValveState state    = ValveState::IDLE;
    bool open_limit     = false; // 开到位信号
    bool close_limit    = false; // 关到位信号
};

// ============================================================================
// Test Data (for test panel display)
// ============================================================================
struct TestData {
    double close_small_travel_time = 0.0;
    double close_current_upper     = 0.0;
    double close_current_lower     = 0.0;
    double open_small_travel_time  = 0.0;
    double open_current_upper      = 0.0;
    double open_current_lower      = 0.0;
};

#endif // VALVE_TYPES_H
