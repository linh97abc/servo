#include <ucos_ii.h>
#include <os_wrapper.h>
#include <sys/alt_debug.h>
#include <appconfig.h>

namespace os
{


void CreateTask(const void *name, void (*fn)(void*), void *arg, OS_STK *stk, uint8_t prio)
{
	INT8U err;

	err = OSTaskCreateExt(
			fn,
			arg,
			&stk[CONFIG_DEFAULT_STK_SIZE-1],
			prio,
			prio,
			stk,
			CONFIG_DEFAULT_STK_SIZE,
			NULL,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	ALT_DEBUG_ASSERT((err == OS_ERR_NONE));

	OSTaskNameSet(prio, (INT8U *)name, &err);
	ALT_DEBUG_ASSERT((err == OS_ERR_NONE));
}

uint8_t GetCpuUsage()
{
	uint8_t cpuUsage = OSCPUUsage;

	return (cpuUsage > 100)? 0: cpuUsage;
}


void  TaskInfoForeach (bool (*fn)(TaskInfo* item))
{
    OS_TCB      *ptcb;
    INT8U        prio;

    TaskInfo taskInfo;

    for (prio = 0; prio <= OS_TASK_IDLE_PRIO; prio++) {
		ptcb = OSTCBPrioTbl[prio];
		if (ptcb != (OS_TCB *)0) {                               /* Make sure task 'ptcb' is ...   */
			if (ptcb != OS_TCB_RESERVED) {                       /* ... still valid.               */

				taskInfo.stk_used = ptcb->OSTCBStkUsed;
				taskInfo.stk_size = ptcb->OSTCBStkSize * sizeof(OS_STK);
				taskInfo.taskName = (const char*) ptcb->OSTCBTaskName;
				taskInfo.swCnt = ptcb->OSTCBCtxSwCtr;
				taskInfo.prio = prio;

				if (fn(&taskInfo))
				{
					break;
				}
			}
		}

    }
}

}
