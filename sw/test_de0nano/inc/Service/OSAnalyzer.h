#pragma once

#include <os_wrapper.h>

namespace service
{
class IOSAnalyzerService
{
protected:
	typedef void (*CpuUsageChangedHandle)(void*, uint8_t);
	typedef void (*TaskInfoChangedHandle)(void*, os::TaskInfo&);

	CpuUsageChangedHandle onCpuUsageChanged;
	TaskInfoChangedHandle onTaskInfoChanged;
	void *arg;

public:
	void OnCpuUsageChanged(uint8_t cpuChanged)
	{
		if (this->onCpuUsageChanged)
		{
			this->onCpuUsageChanged(this->arg, cpuChanged);
		}
	}

	void OnTaskInfoChanged(os::TaskInfo& taskInfo)
	{
		if (this->onTaskInfoChanged)
		{
			this->onTaskInfoChanged(this->arg, taskInfo);
		}
	}


	void RegisterCpuUsageChangedHandle(CpuUsageChangedHandle fn) { this->onCpuUsageChanged = fn; }
	void RegisterTaskInfoChangedHandle(TaskInfoChangedHandle fn) { this->onTaskInfoChanged = fn; }
	void RegisterEventArg(void* arg) { this->arg = arg; }
};

IOSAnalyzerService *Get_UcosAnalyzerService();
}
