#ifndef ROS_CHIBIOS_HARDWARE_H
#define ROS_CHIBIOS_HARDWARE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch.h"
#include "hal.h"
#include "usbcfg.h"

class ChibiOSHardware
{
  public:
	ChibiOSHardware(BaseChannel* io)
	{
		iostream = io;
	}
    ChibiOSHardware()
        : mLastSysTime(chVTGetSystemTimeX())
	{
		iostream = (BaseChannel*)&SDU1;
	}

	void init() {}

	int read()
	{
		return chnGetTimeout(iostream, TIME_IMMEDIATE);
    }

	void write(uint8_t* data, int length)
	{
		chnWrite(iostream, data, length);
	}

	unsigned long time()
	{
        systime_t currentSysTime = chVTGetSystemTimeX();
        mTimeDifference = chTimeDiffX(mLastSysTime, currentSysTime);
        mTimeElapsed += TIME_I2MS(mTimeDifference);
        mLastSysTime = currentSysTime;
        return mTimeElapsed;
	}

  protected:
    BaseChannel* iostream;

  private:
    uint32_t mTimeElapsed;
    systime_t mLastSysTime;
    systime_t mTimeDifference;
};

#ifdef __cplusplus
}
#endif

#endif /* ROS_CHIBIOS_HARDWARE_H */
