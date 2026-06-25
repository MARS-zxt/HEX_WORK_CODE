/**
 * test_client_example.c — Example C test program for the Valve Simulator
 *
 * Compile (MinGW on Windows):
 *   gcc test_client_example.c valve_api.c -o test_client.exe -lws2_32
 *
 * Compile (MSVC on Windows):
 *   cl test_client_example.c valve_api.c ws2_32.lib /Fe:test_client.exe
 *
 * Usage:
 *   1. Start ValveSimulator.exe
 *   2. Run test_client.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include "valve_api.h"

#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

int main(int argc, char *argv[])
{
    ValveClient client;
    valve_init(&client);

    printf("Connecting to Valve Simulator...\n");
    if (valve_connect(&client, "127.0.0.1", 9876) != 0) {
        fprintf(stderr, "ERROR: Cannot connect. Is ValveSimulator running?\n");
        return 1;
    }
    printf("Connected!\n\n");

    // --- Test 1: Open valve and read data ---
    printf("=== Test 1: Open Valve ===\n");
    valve_open(&client);

    for (int i = 0; i < 50; i++) {
        ValveData data;
        if (valve_get_data(&client, &data) == 0) {
            printf("  t=%.1fs  I=%.1fmA  pos=%.3f  open_limit=%d  close_limit=%d\n",
                   data.elapsed_time, data.current, data.position,
                   data.open_limit, data.close_limit);

            if (data.open_limit) {
                printf("  >>> Open limit reached!\n");
                break;
            }
        }
        SLEEP_MS(100);
    }

    SLEEP_MS(500);

    // --- Test 2: Stop valve ---
    printf("\n=== Test 2: Stop Valve ===\n");
    valve_stop(&client);
    SLEEP_MS(200);

    ValveData data;
    valve_get_data(&client, &data);
    printf("  After stop: t=%.1fs  I=%.1fmA  pos=%.3f\n",
           data.elapsed_time, data.current, data.position);

    SLEEP_MS(500);

    // --- Test 3: Close valve ---
    printf("\n=== Test 3: Close Valve ===\n");
    valve_close(&client);

    for (int i = 0; i < 50; i++) {
        if (valve_get_data(&client, &data) == 0) {
            printf("  t=%.1fs  I=%.1fmA  pos=%.3f  open_limit=%d  close_limit=%d\n",
                   data.elapsed_time, data.current, data.position,
                   data.open_limit, data.close_limit);

            if (data.close_limit) {
                printf("  >>> Close limit reached!\n");
                break;
            }
        }
        SLEEP_MS(100);
    }

    SLEEP_MS(500);

    // --- Test 4: Change parameters ---
    printf("\n=== Test 4: Change Parameters ===\n");
    ValveParamsC params = { 0.8, 2.0, 150.0, 400.0, 250.0 };
    if (valve_set_params(&client, &params) == 0) {
        printf("  Parameters updated successfully.\n");
    }

    // --- Test 5: Open with new parameters ---
    printf("\n=== Test 5: Open with New Params ===\n");
    valve_open(&client);
    for (int i = 0; i < 40; i++) {
        if (valve_get_data(&client, &data) == 0) {
            printf("  t=%.1fs  I=%.1fmA  pos=%.3f\n",
                   data.elapsed_time, data.current, data.position);
            if (data.open_limit) break;
        }
        SLEEP_MS(100);
    }

    valve_stop(&client);

    printf("\nAll tests complete.\n");
    valve_disconnect(&client);
    return 0;
}
