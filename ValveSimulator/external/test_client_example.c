/**
 * my_test.c — C 阀门控制程序模板
 *
 * 编译 (MSVC):
 *   cl my_test.c ..\src\network\valve_api.c ws2_32.lib /Fe:my_test.exe
 *
 * 编译 (MinGW):
 *   gcc my_test.c ..\src\network\valve_api.c -o my_test.exe -lws2_32
 *
 * 使用方式:
 *   1. 启动 ValveSimulator.exe
 *   2. 点击「运行测试程序」按钮（自动启动 debug/my_test.exe）
 *   3. 或直接双击 my_test.exe 手动运行
 *
 * 在此文件中编写你自己的阀门控制逻辑。
 */

#include "valve_api.h"           // 必须在 windows.h 之前，避免 winsock 冲突

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN    // 阻止 windows.h 引入 winsock.h 和多余组件
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((ms) * 1000)
#endif


// 阀门动作电流检测函数
unsigned char valve_check()
{

    
    return 0;
}


int main(void)
{
    ValveClient client;
    ValveData data;
    TestResultData result;
    valve_init(&client);

    printf("连接到阀门模拟器...\n");
    if (valve_connect(&client, "127.0.0.1", 9876) != 0) {
        fprintf(stderr, "错误: 无法连接！模拟器是否在运行？\n");
        return 1;
    }
    printf("已连接\n");

    // ============================================================
    //  在此编写你的测试逻辑
    // ============================================================
    //
    //  阀门控制:
    //    valve_open(&client);            开阀
    //    valve_close(&client);           关阀
    //    valve_stop(&client);            停止
    //
    //  数据查询:
    //    ValveData data;
    //    valve_get_data(&client, &data);
    //
    //    data.elapsed_time   运行时间 (秒)
    //    data.current        当前电流 (mA)
    //    data.position       阀芯位置 0.0(全关) ~ 1.0(全开)
    //    data.state          状态 0=IDLE 1=STARTING_OPEN ...
    //    data.open_limit     开到位信号 (0/1)
    //    data.close_limit    关到位信号 (0/1)
    //
    //  参数修改:
    //    ValveParamsC params = {1.0, 3.0, 150.0, 400.0, 250.0};
    //    valve_set_params(&client, &params);
    //
    //  测试数据输出到 UI 面板:
    //    TestResultData result;
    //    result.open_time  = 2.20;
    //    result.open_upper = 150.0;
    //    result.open_lower = 0.0;
    //    result.open_stall = 90.0;
    //    result.close_time  = 2.40;
    //    result.close_upper = 140.0;
    //    result.close_lower = 0.0;
    //    result.close_stall = 90.0;
    //    valve_set_data(&client, &result);  // 更新 UI 上的 8 个数据字段
    //
    //  定时:
    //    SLEEP_MS(100);    100ms 延迟
    //
    // ============================================================

    //step 1 阀门关阀2.0s后停止
    valve_close(&client);
    SLEEP_MS(2000);
    valve_stop(&client); 
    //step 2 阀门开阀,每0.1秒读取当前电流，当电流明显升高至一个稳定

    // 示例: 开阀 → 200ms 后查状态 → 输出结果到 UI → 停止
    valve_open(&client);
    SLEEP_MS(200);

    valve_get_data(&client, &data);
    printf("time=%.1fs current=%.1fmA position=%.3f state=%d\n",
           data.elapsed_time, data.current, data.position, data.state);

    // 将测试结果写入模拟器 UI 的数据展示面板
    result.open_time  = 0.20;
    result.open_upper = 36.0;
    result.open_lower = 0.0;
    result.open_stall = 36.0;
    valve_set_data(&client, &result);

    valve_stop(&client);

    // ============================================================

    printf("测试完成\n");
    valve_disconnect(&client);
    return 0;
}
