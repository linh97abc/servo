#pragma once

#include <ucos_ii.h>
#include <stdint.h>
#include <appconfig.h>

#define OS_STACK_DEF(name) static OS_STK name[CONFIG_DEFAULT_STK_SIZE]

namespace os
{



struct TaskInfo
{
    const char *taskName;
    uint32_t stk_used;
    uint32_t stk_size;
    uint32_t swCnt;
    uint8_t prio;
};

void CreateTask(const void *name, void (*fn)(void*), void *arg, OS_STK *stk, uint8_t prio);

uint8_t GetCpuUsage();

void  TaskInfoForeach (bool (*fn)(TaskInfo* item));

}
