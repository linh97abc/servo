#pragma once

#include <errno.h>
#include "Flash.h"
#include <appconfig.h>
#include <stdint.h>

namespace service
{
class IServoService
{
public:
	virtual int ApplyConfig(ServoConfigArg &arg) = 0;
	virtual int Start() = 0;
	virtual int Stop() = 0;
	virtual int SetPosition(int16_t pos[CONFIG_NUM_SERVO]) = 0;
	virtual int GetPosition(int16_t pos[CONFIG_NUM_SERVO]) = 0;

};

IServoService *Get_HwServoService();
IServoService *Get_MockServoService();
}
