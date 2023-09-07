#include <stdio.h>
#include <system.h>
#include <sys/alt_debug.h>
#include <os_wrapper.h>


OS_STACK_DEF(main_task_stk);

static void _MainTask(void*);

void InitSystem();

int main(void)
{

  printf("MicroC/OS-II\n");

  os::CreateTask("MainTask", _MainTask, NULL, main_task_stk, MAIN_TASK_PRIO);

  OSStart();
  return 0;
}

static void _MainTask(void* arg)
{
	(void)arg;

	InitSystem();

	OSTaskDel(OS_PRIO_SELF);
	OSTaskSuspend(OS_PRIO_SELF);
}
