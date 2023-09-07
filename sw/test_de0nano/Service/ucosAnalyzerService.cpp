#include <Service/OSAnalyzer.h>
#include <os_wrapper.h>

namespace service
{
class UcosAnalyzerService : public IOSAnalyzerService
{
public:
	UcosAnalyzerService();
};
}

service::UcosAnalyzerService::UcosAnalyzerService()
{

}

static service::UcosAnalyzerService *inst;

static bool UpdateTaskInfo(os::TaskInfo *taskInfo)
{
	inst->OnTaskInfoChanged(*taskInfo);
	return false;
}

void os_hook()
{
	uint8_t cpuUsage = os::GetCpuUsage();
	inst->OnCpuUsageChanged(cpuUsage);

	os::TaskInfoForeach(UpdateTaskInfo);
}

service::IOSAnalyzerService *service::Get_UcosAnalyzerService()
{
	static UcosAnalyzerService _inst;

	if (inst == NULL) inst = &_inst;

	return inst;
}
