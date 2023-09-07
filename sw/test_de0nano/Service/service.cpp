#include <Service/service.h>

service::IFlashService *service::GetFlashService()
{
	return service::Get_AltFlashSerive();
}
service::ICommandService *service::GetCommandService()
{
	return service::Get_MavlinkCommandService();
}
service::IServoService *service::GetServoService()
{
	return service::Get_HwServoService();
}

service::IMonitorService *service::GetMonitorService()
{
	return NULL;
}

service::IOSAnalyzerService *service::GetOSAnalyzerService()
{
	return NULL;
}
