#pragma once

#include <errno.h>
#include <appconfig.h>

struct ServoConfigArg {
	float kp[CONFIG_NUM_SERVO];
	float ki[CONFIG_NUM_SERVO];
	float kd[CONFIG_NUM_SERVO];
};

namespace service
{

class IFlashService
{
public:
	virtual int GetServoConfig(struct ServoConfigArg &out) = 0;
	virtual int SaveServoConfig(const struct ServoConfigArg &cfg) = 0;

};


IFlashService *Get_AltFlashSerive();

}
