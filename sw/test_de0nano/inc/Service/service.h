#pragma once

#include "Command.h"
#include "Flash.h"
#include "Servo.h"
#include "Monitor.h"
#include "OSAnalyzer.h"

namespace service
{
	IFlashService *GetFlashService();
	ICommandService *GetCommandService();
	IServoService *GetServoService();
	IMonitorService *GetMonitorService();
	IOSAnalyzerService *GetOSAnalyzerService();
}
