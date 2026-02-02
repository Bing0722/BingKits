#pragma once

#include <cstdint>
#include <windows.h>

namespace BKits
{
namespace Os
{

#define BKITS_NANOSEC  1000000000UL // 纳秒
#define BKITS_MICROSEC 1000000UL    // 微秒
#define BKITS_MILLISEC 1000UL       // 毫秒

static LARGE_INTEGER hrtime_frequency_ = {0};
static LARGE_INTEGER hrtime_offset_    = {0};

typedef struct BKits_once_s
{
    unsigned char unused;    // 未使用的字节，可能用于对齐或预留
    INIT_ONCE     init_once; // Windows API 提供的同步原语，确保某段代码只执行一次
} BKits_once_t;

// 定义一个函数指针类型，指向无参数无返回值的回调函数
typedef void (*Bkits__once_cb)(void);

typedef struct
{
    Bkits__once_cb callback;
} BKits__once_data_t;

static BOOL WINAPI Bkits__once_inner(INIT_ONCE* once, void* param, void** context)
{
    // 将 void* 参数转换回实际类型
    BKits__once_data_t* data = static_cast<BKits__once_data_t*>(param);

    // 调用用户提供的回调函数
    data->callback();

    return TRUE;
}

inline void BKits_once(BKits_once_t* guard, Bkits__once_cb callback)
{
    BKits__once_data_t data;
    data.callback = callback;
    // 调用 Windows API，确保 Bkits__once_inner 只执行一次
    // 参数说明：
    // 1. &guard->init_once - 同步对象
    // 2. Bkits__once_inner - 内部适配器函数
    // 3. (void*)&data - 传递给适配器的数据
    // 4. NULL - 不需要输出上下文
    InitOnceExecuteOnce(&guard->init_once, Bkits__once_inner, (void*)&data, NULL);
}

/* 初始化高精度定时器 */
inline void BKits__time_init(void)
{
    LARGE_INTEGER pref_frequency;

    /* 获取性能计数器频率（每秒的滴答数） */
    if (QueryPerformanceFrequency(&pref_frequency))
    {
        hrtime_frequency_ = pref_frequency;
    }
    else
    {
        /* 降级方案：使用毫秒级 GetTickCount64 */
        hrtime_frequency_.QuadPart = 1000;
    }

    /* 获取初始时间偏移 */
    QueryPerformanceCounter(&hrtime_offset_);
}

/* 获取高精度时间 */
inline uint64_t BKits__hrtime(unsigned int scale)
{
    LARGE_INTEGER counter;
    double        scaled_freq;
    double        result;

    /* 使用 QueryPerformanceCounter 获取当前计数 */
    QueryPerformanceCounter(&counter);
    // 关键：减去偏移量，得到程序启动后的 ticks
    // counter.QuadPart -= hrtime_offset_.QuadPart;

    scaled_freq = (double)hrtime_frequency_.QuadPart / scale;
    result      = (double)counter.QuadPart / scaled_freq;

    return (uint64_t)result;
}

inline uint64_t BKits_hrtime(void)
{
    BKits__time_init();
    return BKits__hrtime(BKITS_MILLISEC);
}

} // namespace Os

} // namespace BKits
