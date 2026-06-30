/**
 * my_test.c — 阀门控制测试程序（按 bug.md step1-4）
 *
 * 编译:
 *   cl my_test.c ..\src\network\valve_api.c ws2_32.lib /Fe:my_test.exe
 *
 * 运行前: 先启动 ValveSimulator.exe
 */

#include "valve_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

/* ── 简易环形缓冲区（用于稳定检测） ─────────────────────────────── */
#define BUF_SIZE 6
typedef struct {
    double buf[BUF_SIZE];
    int    count;
    int    pos;
} RingBuf;

static void rb_push(RingBuf *rb, double val) {
    rb->buf[rb->pos] = val;
    rb->pos = (rb->pos + 1) % BUF_SIZE;
    if (rb->count < BUF_SIZE) rb->count++;
}

static double rb_avg(const RingBuf *rb) {
    if (rb->count == 0) return 0.0;
    double sum = 0.0;
    for (int i = 0; i < rb->count; i++) sum += rb->buf[i];
    return sum / rb->count;
}

static double rb_min(const RingBuf *rb) {
    if (rb->count == 0) return 0.0;
    double m = rb->buf[0];
    for (int i = 1; i < rb->count; i++)
        if (rb->buf[i] < m) m = rb->buf[i];
    return m;
}

static double rb_max(const RingBuf *rb) {
    if (rb->count == 0) return 0.0;
    double m = rb->buf[0];
    for (int i = 1; i < rb->count; i++)
        if (rb->buf[i] > m) m = rb->buf[i];
    return m;
}

/* ── 到位检测 ──────────────────────────────────────────────────── */
#define STABLE_THRESHOLD 0.05   /* 5% 波动范围 */
#define STABLE_COUNT     5      /* 连续稳定次数 */

static int is_stable(RingBuf *rb) {
    if (rb->count < STABLE_COUNT) return 0;
    double avg = rb_avg(rb);
    if (avg < 0.01) return 0;
    double range = rb_max(rb) - rb_min(rb);
    return (range / avg) < STABLE_THRESHOLD;
}

/* ── 到位检测 — 只检查不再攀升（堵转特征） ────────────────────── */
static int is_stalling(RingBuf *rb) {
    if (rb->count < STABLE_COUNT) return 0;
    /* 检查趋势：最近一段斜率趋近于0 */
    double sum_delta = 0.0;
    for (int i = 1; i < rb->count; i++) {
        sum_delta += fabs(rb->buf[i] - rb->buf[i-1]);
    }
    double avg_delta = sum_delta / (rb->count - 1);
    double avg = rb_avg(rb);
    if (avg < 1.0) return 0;
    /* 平均变化幅度 < 平均值的 3% 且没有持续上升趋势 */
    return (avg_delta / avg) < 0.03;
}

/* ── 打印分隔线 ────────────────────────────────────────────────── */
static void print_sep(const char *title) {
    printf("\n========================================\n");
    printf("  %s\n", title);
    printf("========================================\n");
}

/* ==================================================================
 *  主测试流程
 * ================================================================== */
int main(void)
{
    ValveClient client;
    ValveData   data;
    TestResultData result = {0};
    valve_init(&client);

    /* ── 连接 ─────────────────────────────────────────────── */
    printf(">>> 连接到阀门模拟器 (127.0.0.1:9876) ...\n");
    if (valve_connect(&client, "127.0.0.1", 9876) != 0) {
        fprintf(stderr, "!!! 错误: 无法连接！ValveSimulator.exe 是否在运行？\n");
        return 1;
    }
    printf(">>> 已连接\n");

    /* ════════════════════════════════════════════════════════════
     *  Step1 — 关阀 2.0s 后停止
     *  目的：保证下一步开阀有足够的行程
     * ════════════════════════════════════════════════════════════ */
    print_sep("Step1: 关阀 2.0s → 停止");
    printf("[step1] 正在关阀...\n");
    valve_close(&client);
    SLEEP_MS(2000);
    valve_stop(&client);
    printf("[step1] 完成 — 阀门已停止\n");

    /* ════════════════════════════════════════════════════════════
     *  Step2 — 开阀，每 0.1s 读电流，检测到位后停止
     *  目的：使阀门到达【开到位】状态
     * ════════════════════════════════════════════════════════════ */
    print_sep("Step2: 开阀 → 检测开到位");
    printf("[step2] 正在开阀，等待到达开到位...\n");
    valve_open(&client);

    {
        RingBuf rb = {0};
        double  elapsed = 0.0;
        int     stable_count = 0;

        for (int tick = 0; tick < 600; tick++) {  /* 最多 60s 超时 */
            SLEEP_MS(100);
            elapsed += 0.1;

            if (valve_get_data(&client, &data) != 0) {
                printf("[step2] !!! 读取数据失败\n");
                break;
            }

            rb_push(&rb, data.current);
            printf("[step2] t=%.1fs | I=%.1fmA | pos=%.3f | open_lim=%d close_lim=%d\n",
                   elapsed, data.current, data.position,
                   data.open_limit, data.close_limit);

            /* 到位检测：电流不再攀升 */
            if (elapsed > 1.5 && is_stalling(&rb)) {
                stable_count++;
                if (stable_count >= 3) {
                    printf("[step2] >>> 到达开到位 (电流稳定, t=%.1fs, I=%.1fmA)\n",
                           elapsed, data.current);
                    break;
                }
            } else {
                stable_count = 0;
            }
        }
        valve_stop(&client);
        printf("[step2] 完成 — 已到达开到位状态\n");
    }

    /* ════════════════════════════════════════════════════════════
     *  Step3 — 关阀，每 0.1s 读电流 + 计时，到位后计算 4 参数
     *  目的：分析关阀 → 到位时间/电流上限/电流下限/堵转电流
     * ════════════════════════════════════════════════════════════ */
    print_sep("Step3: 关阀 → 测量关阀参数");
    printf("[step3] 正在关阀，采集中...\n");
    valve_close(&client);

    {
        RingBuf rb = {0};
        double  elapsed   = 0.0;
        int     stable_count = 0;
        double  sum_running = 0.0;
        int     cnt_running = 0;
        double  stall_current = 0.0;
        double  travel_time   = 0.0;
        double  startup_time  = 1.0;   /* 默认启动时间 */

        for (int tick = 0; tick < 600; tick++) {
            SLEEP_MS(100);
            elapsed += 0.1;

            if (valve_get_data(&client, &data) != 0) {
                printf("[step3] !!! 读取数据失败\n");
                break;
            }

            rb_push(&rb, data.current);
            printf("[step3] t=%.1fs | I=%.1fmA | pos=%.3f\n",
                   elapsed, data.current, data.position);

            /* 排除启动时间，统计运行电流 */
            if (elapsed > startup_time) {
                sum_running += data.current;
                cnt_running++;
            }

            /* 到位检测 */
            if (elapsed > (startup_time + 0.5) && is_stalling(&rb)) {
                stable_count++;
                if (stable_count >= 3) {
                    stall_current = data.current;
                    travel_time   = elapsed;
                    printf("[step3] >>> 到达关到位 (电流稳定, t=%.1fs, I=%.1fmA)\n",
                           elapsed, data.current);
                    break;
                }
            } else {
                stable_count = 0;
            }
        }
        valve_stop(&client);

        /* ── 计算 4 参数 ────────────────────────────────── */
        if (cnt_running > 0) {
            double avg_running = sum_running / cnt_running;
            double upper = avg_running * 1.3;
            double lower = avg_running * 0.7;

            printf("[step3] ─── 关阀测量结果 ───\n");
            printf("[step3]   到位时间 : %.2f s\n", travel_time);
            printf("[step3]   电流上限 : %.1f mA  (avg=%.1f × 1.3)\n", upper, avg_running);
            printf("[step3]   电流下限 : %.1f mA  (avg=%.1f × 0.7)\n", lower, avg_running);
            printf("[step3]   堵转电流 : %.1f mA\n", stall_current);

            result.close_time  = travel_time;
            result.close_upper = upper;
            result.close_lower = lower;
            result.close_stall = stall_current;
            valve_set_data(&client, &result);
            printf("[step3] >>> 关阀参数已发送到 UI\n");
        } else {
            printf("[step3] !!! 无有效采样数据\n");
        }
    }

    /* ════════════════════════════════════════════════════════════
     *  Step4 — 开阀，同 step3，计算开阀 4 参数
     *  目的：分析开阀 → 到位时间/电流上限/电流下限/堵转电流
     * ════════════════════════════════════════════════════════════ */
    print_sep("Step4: 开阀 → 测量开阀参数");
    printf("[step4] 正在开阀，采集中...\n");
    valve_open(&client);

    {
        RingBuf rb = {0};
        double  elapsed   = 0.0;
        int     stable_count = 0;
        double  sum_running = 0.0;
        int     cnt_running = 0;
        double  stall_current = 0.0;
        double  travel_time   = 0.0;
        double  startup_time  = 1.0;

        for (int tick = 0; tick < 600; tick++) {
            SLEEP_MS(100);
            elapsed += 0.1;

            if (valve_get_data(&client, &data) != 0) {
                printf("[step4] !!! 读取数据失败\n");
                break;
            }

            rb_push(&rb, data.current);
            printf("[step4] t=%.1fs | I=%.1fmA | pos=%.3f\n",
                   elapsed, data.current, data.position);

            if (elapsed > startup_time) {
                sum_running += data.current;
                cnt_running++;
            }

            if (elapsed > (startup_time + 0.5) && is_stalling(&rb)) {
                stable_count++;
                if (stable_count >= 3) {
                    stall_current = data.current;
                    travel_time   = elapsed;
                    printf("[step4] >>> 到达开到位 (电流稳定, t=%.1fs, I=%.1fmA)\n",
                           elapsed, data.current);
                    break;
                }
            } else {
                stable_count = 0;
            }
        }
        valve_stop(&client);

        if (cnt_running > 0) {
            double avg_running = sum_running / cnt_running;
            double upper = avg_running * 1.3;
            double lower = avg_running * 0.7;

            printf("[step4] ─── 开阀测量结果 ───\n");
            printf("[step4]   到位时间 : %.2f s\n", travel_time);
            printf("[step4]   电流上限 : %.1f mA  (avg=%.1f × 1.3)\n", upper, avg_running);
            printf("[step4]   电流下限 : %.1f mA  (avg=%.1f × 0.7)\n", lower, avg_running);
            printf("[step4]   堵转电流 : %.1f mA\n", stall_current);

            result.open_time  = 0.0;         /* 清零避免覆盖 step3 */
            result.open_upper = 0.0;
            result.open_lower = 0.0;
            result.open_stall = 0.0;
            result.close_time  = 0.0;
            result.close_upper = 0.0;
            result.close_lower = 0.0;
            result.close_stall = 0.0;

            result.open_time  = travel_time;
            result.open_upper = upper;
            result.open_lower = lower;
            result.open_stall = stall_current;
            valve_set_data(&client, &result);
            printf("[step4] >>> 开阀参数已发送到 UI\n");
        } else {
            printf("[step4] !!! 无有效采样数据\n");
        }
    }

    /* ════════════════════════════════════════════════════════════
     *  总结
     * ════════════════════════════════════════════════════════════ */
    print_sep("测试完成");
    printf("  关阀: 到位时间=%.2fs  上限=%.1fmA  下限=%.1fmA  堵转=%.1fmA\n",
           result.close_time, result.close_upper,
           result.close_lower, result.close_stall);
    printf("  开阀: 到位时间=%.2fs  上限=%.1fmA  下限=%.1fmA  堵转=%.1fmA\n",
           result.open_time, result.open_upper,
           result.open_lower, result.open_stall);
    printf("  所有参数已同步到模拟器 UI 面板。\n");

    valve_disconnect(&client);
    return 0;
}
