#pragma once



#include <stdint.h>
#include <os_wrapper.h>
#include <appconfig.h>

#include "Servo.h"

namespace service
{
class ICommandService
{
	IServoService *servoService;
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
};

ICommandService *Get_MavlinkCommandService();
}
