#include <Service/Flash.h>
#include <system.h>
#include <sys/alt_debug.h>
#include <appconfig.h>
#include <sys/alt_flash.h>

namespace service
{
	class AltFlashService : public IFlashService
	{
		alt_flash_fd* flashFd;
		int flashOffset;
		int GetServoConfig(struct ServoConfigArg &out);
		int SaveServoConfig(const struct ServoConfigArg &cfg);
	public:
		AltFlashService();
	};

}

service::AltFlashService::AltFlashService()
	:flashOffset(FLASH_CFG_OFFSET)
{
	this->flashFd = alt_flash_open_dev(EPCS_FLASH_CONTROLLER_0_NAME);
	ALT_DEBUG_ASSERT((this->flashFd != NULL));

}

int service::AltFlashService::GetServoConfig(struct ServoConfigArg &out)
{
	int stt = alt_read_flash(this->flashFd, this->flashOffset, &out, sizeof(ServoConfigArg));

	return (stt == 0)? 0: -1;
}

int service::AltFlashService::SaveServoConfig(const struct ServoConfigArg &cfg)
{
	int stt = alt_write_flash(this->flashFd, this->flashOffset, &cfg, sizeof(ServoConfigArg));
	return (stt == 0)? 0: -1;
}

service::IFlashService *service::Get_AltFlashSerive()
{
	static AltFlashService inst;
	return &inst;
}
