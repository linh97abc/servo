#include <Service/Monitor.h>
#include <os_wrapper.h>
#include <appconfig.h>
#include <ucos_ii.h>

OS_STACK_DEF(monitorStk);

namespace service
{
class MonitorService: public IMonitorService
{
	uint8_t servoPosSeq;
	uint8_t cpuUsageSeq;

	static void TaskMonitor(void *arg);
	static void OnCpuUsageChange(void *arg, uint8_t cpuUsage);
	static void OnTaskInfoChanged(void *arg, os::TaskInfo &taskInfo);

	void DoTaskMonitor();

	virtual void SetOSAnalyzerService(IOSAnalyzerService *osAnalyzerService);
public:
	MonitorService();
};
}

service::MonitorService::MonitorService()
:servoPosSeq(0), cpuUsageSeq(0)
{
	os::CreateTask("TaskMonitor", TaskMonitor, this, monitorStk, MONITOR_TASK_PRIO);


}

void service::MonitorService::SetOSAnalyzerService(IOSAnalyzerService *osAnalyzerService)
{
	this->osAnalyzerService = osAnalyzerService;
	this->osAnalyzerService->RegisterCpuUsageChangedHandle(OnCpuUsageChange);
	this->osAnalyzerService->RegisterTaskInfoChangedHandle(OnTaskInfoChanged);
	this->osAnalyzerService->RegisterEventArg(this);
}

void  service::MonitorService::OnCpuUsageChange(void *arg, uint8_t cpuUsage)
{
	MonitorService *self = (MonitorService*)arg;

	self->commandService->SendCpuUsage(self->cpuUsageSeq++, cpuUsage);
}

void  service::MonitorService::OnTaskInfoChanged(void *arg, os::TaskInfo &taskInfo)
{
	MonitorService *self = (MonitorService*)arg;

	self->commandService->SendTaskInfo(taskInfo);
}

void service::MonitorService::TaskMonitor(void *arg)
{
	MonitorService *self = (MonitorService*)arg;
	self->DoTaskMonitor();
}

void service::MonitorService::DoTaskMonitor()
{
	while(true)
	{
		int16_t pos[CONFIG_NUM_SERVO];
		this->servoService->GetPosition(pos);
		this->commandService->SendServoPos(this->servoPosSeq++, 1, pos);

		OSTimeDly(OS_TICKS_PER_SEC/100);
	}
}

service::IMonitorService* service::Get_MonitorService()
{
	static MonitorService inst;

	return &inst;
}
