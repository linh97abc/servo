#include <Service/Servo.h>
#include <system.h>


namespace service
{
class MockServoService : public IServoService
{


	int ApplyConfig(ServoConfigArg &arg);
	int Start();
	int Stop();
	int SetPosition(int16_t pos[CONFIG_NUM_SERVO]);
	int GetPosition(int16_t pos[CONFIG_NUM_SERVO]);

public:
	MockServoService();
};
}

service::MockServoService::MockServoService()
{

}

int service::MockServoService::ApplyConfig(ServoConfigArg &arg)
{
    return 0;
}

int service::MockServoService::Start()
{
	return 0;
}

int service::MockServoService::Stop()
{
	return 0;
}

int service::MockServoService::SetPosition(int16_t pos[CONFIG_NUM_SERVO])
{


	return 0;
}

int service::MockServoService::GetPosition(int16_t pos[CONFIG_NUM_SERVO])
{
	return 0;
}

service::IServoService *service::Get_MockServoService()
{
	static MockServoService inst;
	return &inst;
}
