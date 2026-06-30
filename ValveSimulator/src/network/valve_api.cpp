#include "valve_api.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    static int winsock_initialized = 0;
    static void init_winsock() {
        if (!winsock_initialized) {
            WSADATA wsa;
            WSAStartup(MAKEWORD(2, 2), &wsa);
            winsock_initialized = 1;
        }
    }
#else
    #define closesocket close
    static void init_winsock() {}
#endif

// ============================================================================
void valve_init(ValveClient *client)
{
    client->sock = VALVE_INVALID_SOCKET;
    client->connected = 0;
}

// ============================================================================
int valve_connect(ValveClient *client, const char *host, int port)
{
    init_winsock();

    client->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sock == VALVE_INVALID_SOCKET) {
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)port);

    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        closesocket(client->sock);
        client->sock = VALVE_INVALID_SOCKET;
        return -1;
    }

    if (connect(client->sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        closesocket(client->sock);
        client->sock = VALVE_INVALID_SOCKET;
        return -1;
    }

    client->connected = 1;
    return 0;
}

// ============================================================================
void valve_disconnect(ValveClient *client)
{
    if (client->connected) {
        closesocket(client->sock);
        client->sock = VALVE_INVALID_SOCKET;
        client->connected = 0;
    }
}

// ============================================================================
static int send_and_recv(ValveClient *client, const char *cmd, char *response, int resp_size)
{
    if (!client->connected) return -1;

    // Send
    int len = (int)strlen(cmd);
    if (send(client->sock, cmd, len, 0) != len) return -1;

    // Send newline if not present
    if (cmd[len-1] != '\n') {
        if (send(client->sock, "\n", 1, 0) != 1) return -1;
    }

    // Receive one line
    memset(response, 0, resp_size);
    int total = 0;
    while (total < resp_size - 1) {
        char c;
        int n = recv(client->sock, &c, 1, 0);
        if (n <= 0) return -1;
        response[total++] = c;
        if (c == '\n') break;
    }
    // Trim trailing newline
    if (total > 0 && response[total-1] == '\n') response[total-1] = '\0';
    if (total > 0 && response[total-1] == '\r') response[total-1] = '\0';

    return total;
}

// ============================================================================
int valve_open(ValveClient *client)
{
    char resp[256];
    if (send_and_recv(client, "OPEN", resp, sizeof(resp)) < 0) return -1;
    return (strncmp(resp, "OK", 2) == 0) ? 0 : -1;
}

int valve_close(ValveClient *client)
{
    char resp[256];
    if (send_and_recv(client, "CLOSE", resp, sizeof(resp)) < 0) return -1;
    return (strncmp(resp, "OK", 2) == 0) ? 0 : -1;
}

int valve_stop(ValveClient *client)
{
    char resp[256];
    if (send_and_recv(client, "STOP", resp, sizeof(resp)) < 0) return -1;
    return (strncmp(resp, "OK", 2) == 0) ? 0 : -1;
}

// ============================================================================
int valve_get_data(ValveClient *client, ValveData *data)
{
    char resp[512];
    if (send_and_recv(client, "GET_DATA", resp, sizeof(resp)) < 0) return -1;

    // Parse: DATA time=X current=Y position=Z state=NAME open_limit=A close_limit=B
    memset(data, 0, sizeof(*data));

    char *ptr = resp;
    if (strncmp(ptr, "DATA ", 5) != 0) return -1;
    ptr += 5;

    // Simple key=value parser
    while (*ptr) {
        // Skip whitespace
        while (*ptr == ' ') ptr++;
        if (!*ptr) break;

        // Find '='
        char *eq = strchr(ptr, '=');
        if (!eq) break;
        *eq = '\0';
        char *key = ptr;
        char *val = eq + 1;

        // Find end of value (space or end)
        char *end = val;
        while (*end && *end != ' ') end++;
        char saved = *end;
        *end = '\0';

        // Assign
        if (strcmp(key, "time") == 0)        data->elapsed_time = atof(val);
        else if (strcmp(key, "current") == 0) data->current = atof(val);
        else if (strcmp(key, "position") == 0) data->position = atof(val);
        else if (strcmp(key, "open_limit") == 0) data->open_limit = atoi(val);
        else if (strcmp(key, "close_limit") == 0) data->close_limit = atoi(val);

        *end = saved;
        ptr = (*end) ? end + 1 : end;
    }

    return 0;
}

// ============================================================================
int valve_set_params(ValveClient *client, const ValveParamsC *params)
{
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "SET_PARAMS %.1f %.1f %.1f %.1f %.1f",
             params->startup_time,
             params->travel_time,
             params->running_current,
             params->stall_current,
             params->startup_current);

    char resp[256];
    if (send_and_recv(client, cmd, resp, sizeof(resp)) < 0) return -1;
    return (strncmp(resp, "OK", 2) == 0) ? 0 : -1;
}

// ============================================================================
int valve_set_data(ValveClient *client, const TestResultData *data)
{
    char cmd[512];
    cmd[0] = '\0';
    char *p = cmd;
    int remaining = sizeof(cmd);

    p += snprintf(p, remaining, "SET_DATA");
    remaining = sizeof(cmd) - (p - cmd);

    if (data->open_time  > 0.0) p += snprintf(p, remaining, " open_time=%.2f",  data->open_time);
    remaining = sizeof(cmd) - (p - cmd);
    if (data->open_upper > 0.0) p += snprintf(p, remaining, " open_upper=%.1f", data->open_upper);
    remaining = sizeof(cmd) - (p - cmd);
    if (data->open_lower > 0.0) p += snprintf(p, remaining, " open_lower=%.1f", data->open_lower);
    remaining = sizeof(cmd) - (p - cmd);
    if (data->open_stall > 0.0) p += snprintf(p, remaining, " open_stall=%.1f", data->open_stall);
    remaining = sizeof(cmd) - (p - cmd);
    if (data->close_time  > 0.0) p += snprintf(p, remaining, " close_time=%.2f",  data->close_time);
    remaining = sizeof(cmd) - (p - cmd);
    if (data->close_upper > 0.0) p += snprintf(p, remaining, " close_upper=%.1f", data->close_upper);
    remaining = sizeof(cmd) - (p - cmd);
    if (data->close_lower > 0.0) p += snprintf(p, remaining, " close_lower=%.1f", data->close_lower);
    remaining = sizeof(cmd) - (p - cmd);
    if (data->close_stall > 0.0) p += snprintf(p, remaining, " close_stall=%.1f", data->close_stall);

    char resp[256];
    if (send_and_recv(client, cmd, resp, sizeof(resp)) < 0) return -1;
    return (strncmp(resp, "OK", 2) == 0) ? 0 : -1;
}

// ============================================================================
int valve_send_command(ValveClient *client, const char *cmd, char *response, int resp_size)
{
    return send_and_recv(client, cmd, response, resp_size);
}
