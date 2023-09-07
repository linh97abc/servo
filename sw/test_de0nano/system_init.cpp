#include <Service/service.h>

void InitSystem()
{
	int stt;
	// init flash service
	service::IFlashService *_flashService = service::GetFlashService();

	// get config
	ServoConfigArg servoConfigArg;
	_flashService->GetServoConfig(servoConfigArg);


	// init servo service
	service::IServoService *_servoService = service::GetServoService();
	_servoService->ApplyConfig(servoConfigArg);

	// init command service
	service::ICommandService *_commandService = service::GetCommandService();
	_commandService->SetServoService(_servoService);

	service::IOSAnalyzerService *_osAnalyzerService = service::GetOSAnalyzerService();

	service::IMonitorService *_monitorService = service::GetMonitorService();
	_monitorService->SetCommandService(_commandService);
	_monitorService->SetServoService(_servoService);
	_monitorService->SetOSAnalyzerService(_osAnalyzerService);
}
