#pragma once



#include <stdint.h>
#include <os_wrapper.h>
#include <appconfig.h>

#include "Servo.h"
#include "Flash.h"

namespace service
{
class ICommandService
{
protected:
	IServoService *servoService;
	IFlashService *flashService;

public:
	virtual void SendCpuUsage(uint8_t seq, uint8_t cpuUsage) = 0;
	virtual void SendTaskInfo(const os::TaskInfo &taskInfo) = 0;
	virtual void SendServoPos(uint8_t seq, uint8_t isRunning, const int16_t pos[CONFIG_NUM_SERVO]) = 0;

	ICommandService() :servoService(NULL) {}
	virtual ~ICommandService() {}

	void SetServoService(IServoService *servoService)
	{
		this->servoService = servoService;
	}

	void SetFlashService(IFlashService *flashService)
	{
		this->flashService = flashService;
	}
};

ICommandService *Get_MavlinkCommandService();
}
