#pragma once

#include "Command.h"
#include "Servo.h"
#include "OSAnalyzer.h"

namespace service
{
class IMonitorService
{
	ICommandService *commandService;
	IServoService *servoService;
	IOSAnalyzerService *osAnalyzerService;

public:
	IMonitorService() :commandService(NULL), servoService(NULL), osAnalyzerService(NULL) {}
	virtual ~IMonitorService() {}

	void SetCommandService(ICommandService *commandService) { this->commandService = commandService; }
	void SetServoService(IServoService *servoService) { this->servoService = servoService; }
	void SetOSAnalyzerService(IOSAnalyzerService *osAnalyzerService) { this->osAnalyzerService = osAnalyzerService; }
};
}
