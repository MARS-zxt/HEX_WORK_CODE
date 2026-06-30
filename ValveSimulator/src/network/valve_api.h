#ifndef VALVE_API_H
#define VALVE_API_H

// ============================================================================
// Valve API — C-compatible interface for external test programs
// ============================================================================
// This header can be included by C programs that need to communicate with
// the Valve Simulator application via TCP (localhost:9876).
//
// Protocol (text-based, newline-terminated):
//   Commands:  OPEN, CLOSE, STOP, GET_DATA, SET_PARAMS <p1> <p2> <p3> <p4> <p5>
//   Responses: OK <cmd>, DATA key=val ..., ERROR <msg>
//
// Usage from C:
//   #include "valve_api.h"
//   ValveClient client;
//   valve_connect(&client, "127.0.0.1", 9876);
//   valve_open(&client);
//   ValveData data;
//   valve_get_data(&client, &data);
//   printf("current=%.1f mA, position=%.3f\n", data.current, data.position);
//   valve_disconnect(&client);
// ============================================================================

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define VALVE_SOCKET SOCKET
    #define VALVE_INVALID_SOCKET INVALID_SOCKET
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define VALVE_SOCKET int
    #define VALVE_INVALID_SOCKET (-1)
#endif

// ============================================================================
// Data structures
// ============================================================================
typedef struct {
    double elapsed_time;   // seconds
    double current;        // mA
    double position;       // 0.0 = closed, 1.0 = open
    int    state;          // 0=IDLE, 1=STARTING_OPEN, ...
    int    open_limit;     // 0 or 1
    int    close_limit;    // 0 or 1
} ValveData;

typedef struct {
    double startup_time;
    double travel_time;
    double running_current;
    double stall_current;
    double startup_current;
} ValveParamsC;

typedef struct {
    VALVE_SOCKET sock;
    int          connected;
} ValveClient;

// ============================================================================
// Connection management
// ============================================================================

/**
 * Initialize a ValveClient structure. Must be called before valve_connect().
 */
void valve_init(ValveClient *client);

/**
 * Connect to the Valve Simulator TCP server.
 * Returns 0 on success, -1 on failure.
 */
int  valve_connect(ValveClient *client, const char *host, int port);

/**
 * Disconnect from the server.
 */
void valve_disconnect(ValveClient *client);

// ============================================================================
// Valve commands
// ============================================================================

/** Send "OPEN" — start opening the valve. Returns 0 on success. */
int valve_open(ValveClient *client);

/** Send "CLOSE" — start closing the valve. Returns 0 on success. */
int valve_close(ValveClient *client);

/** Send "STOP" — stop the valve immediately. Returns 0 on success. */
int valve_stop(ValveClient *client);

// ============================================================================
// Data queries
// ============================================================================

/**
 * Get current valve data. Returns 0 on success.
 * Parses the "DATA ..." response into the ValveData struct.
 */
int valve_get_data(ValveClient *client, ValveData *data);

// ============================================================================
// Parameter control
// ============================================================================

/**
 * Set valve parameters remotely.
 * Returns 0 on success.
 */
int valve_set_params(ValveClient *client, const ValveParamsC *params);

// ============================================================================
// Test data output
// ============================================================================

typedef struct {
    double open_time;       // 开阀到位时间 (s)  — 传 0 表示不更新此字段
    double open_upper;      // 开阀电流上限 (mA)
    double open_lower;      // 开阀电流下限 (mA)
    double open_stall;      // 开到位电流 (mA)
    double close_time;      // 关阀到位时间 (s)
    double close_upper;     // 关阀电流上限 (mA)
    double close_lower;     // 关阀电流下限 (mA)
    double close_stall;     // 关到位电流 (mA)
} TestResultData;

/**
 * Send test result data to the simulator's UI display.
 * Fields with value 0.0 are skipped (not updated).
 * Returns 0 on success.
 */
int valve_set_data(ValveClient *client, const TestResultData *data);

// ============================================================================
// Low-level (for advanced use)
// ============================================================================

/**
 * Send a raw command string and read one line of response.
 * response buffer should be at least 512 bytes.
 * Returns length of response or -1 on error.
 */
int valve_send_command(ValveClient *client, const char *cmd, char *response, int resp_size);

#ifdef __cplusplus
}
#endif

#endif // VALVE_API_H
