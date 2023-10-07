#include <iostream>
#include <fw_update.h>
#include <system.h>
#include <ucos_ii.h>
#include <string.h>

#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];


#define TASK1_PRIORITY      4

void task1(void* pdata)
{
	fw_update_dev *fwUpdateDev;
	fwUpdateDev = fw_update_open_dev(FW_UPDATE_0_NAME);

	const char* version = fw_update_get_version_str(fwUpdateDev);
	std::cout << "FW Version: " << version << std::endl;

	int ret = fw_update_check_current_fw(fwUpdateDev);
	std::cout << "FW Check: " << ret << std::endl;

	ret = fw_update_get_sw_size(fwUpdateDev);
	std::cout << "Software size: " << (float)ret/(1024*1024) << "MB" << std::endl;

	uint8_t configData[FW_UPDATE_CONFIG_BANK_SIZE];
	uint8_t readData[FW_UPDATE_CONFIG_BANK_SIZE];

	strcpy((char*) configData, "Hello world");
	fw_update_save_config(fwUpdateDev, 0, configData);

	strcpy((char*) configData, "Bank B");
	fw_update_save_config(fwUpdateDev, 1, configData);

	fw_update_read_config(fwUpdateDev, 0, readData);
	std::cout << (const char*)readData << std::endl;

	fw_update_read_config(fwUpdateDev, 1, readData);
	std::cout << (const char*)readData << std::endl;

	for(;;);

}

int main()
{
	std::cout << "Test core FW Update" << std::endl;

	  OSTaskCreateExt(task1,
	                  NULL,
	                  &task1_stk[TASK_STACKSIZE-1],
	                  TASK1_PRIORITY,
	                  TASK1_PRIORITY,
	                  task1_stk,
	                  TASK_STACKSIZE,
	                  NULL,
	                  0);

	  OSStart();

	return 0;
}
