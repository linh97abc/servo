#pragma once

#define CONFIG_DEFAULT_STK_SIZE 2048

enum TaskPrio_t
{
	MAIN_TASK_PRIO = 2,
	SERVO_TASK_PRIO,
	MAVLINK_RECV_TASK_PRIO,
	MSG_MAVLINK_ASYNC_RECV_TASK_PRIO,
	MONITOR_TASK_PRIO,
};


#define FLASH_CFG_OFFSET 0x700000
#define CONFIG_NUM_SERVO 4

#define CONFIG_UART_RECV_LEN 256
