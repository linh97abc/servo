#include <Service/service.h>

void InitSystem()
{

	// init flash service
//	service::IFlashService *_flashService = service::GetFlashService();
	service::IFlashService *_flashService = service::Get_AltFlashSerive();

	// get config
	ServoConfigArg servoConfigArg;
	_flashService->GetServoConfig(servoConfigArg);


	// init servo service
//	service::IServoService *_servoService = service::GetServoService();
//	service::IServoService *_servoService = service::Get_MockServoService();
	service::IServoService *_servoService = service::Get_HwServoService();
	_servoService->ApplyConfig(servoConfigArg);

	// init command service
//	service::ICommandService *_commandService = service::GetCommandService();
	service::ICommandService *_commandService = service::Get_MavlinkCommandService();
	_commandService->SetServoService(_servoService);
	_commandService->SetFlashService(_flashService);

//	service::IOSAnalyzerService *_osAnalyzerService = service::GetOSAnalyzerService();
	service::IOSAnalyzerService *_osAnalyzerService = service::Get_UcosAnalyzerService();

//	service::IMonitorService *_monitorService = service::GetMonitorService();
	service::IMonitorService *_monitorService = service::Get_MonitorService();
	_monitorService->SetCommandService(_commandService);
	_monitorService->SetServoService(_servoService);
	_monitorService->SetOSAnalyzerService(_osAnalyzerService);
}
