#ifndef QUADENCODER_H
#define QUADENCODER_H
// clang-format off
#include "ch.hpp"
#include "hal.h"
#include "usbcfg.h"
#include "chprintf.h"
#include "stdlib.h"
#include "time.h"
// clang-format on

class QuadEncoder : public chibios_rt::BaseStaticThread<512>
{
  public:
    // using
    using RpmMailBox = chibios_rt::Mailbox<float*, 2>;
    /**
     * @brief THREAD_SLEEP_TIME The time the thread sleeps in milliseconds.
     */
    static constexpr uint8_t THREAD_SLEEP_TIME = 10;
    // Constants
    static constexpr uint32_t LOW_WRAP = 6553;
    static constexpr uint32_t HIGH_WRAP = 58981;
    static constexpr float ONE_BY_TWO_PI = 0.15915494309;
    // Constructor
    QuadEncoder(void)
        : BaseStaticThread<512>()
        , mCurrentEncoderCount(0)
        , mLastEncoderValue(0)
        , mMultiplicationFactor(0)
    {
    }
    // Destructor
    ~QuadEncoder() {}
    /**
     * @brief getRpmMailBox Getter to get the reference to the mailbox. This will be used to fetch
     * the messages.
     * @return Reference to the internal mailbox
     */
    RpmMailBox& getRpmMailBox()
    {
        return mRpmMailBox;
    }

  protected:
    void main(void) override
    {
        // Set name of the thread.
        setName("QuadEncoderThread");
        uint16_t lastQeiCount = qeiGetCount(&QEID3);
        float angularVelocity = 0.;
        while(true)
        {
            uint16_t qeiCount = qeiGetCount(&QEID3);
            if(qeiCount < LOW_WRAP && lastQeiCount > HIGH_WRAP)
            {
                mMultiplicationFactor += 1;
            }
            if(qeiCount > HIGH_WRAP && lastQeiCount < LOW_WRAP)
            {
                mMultiplicationFactor -= 1;
            }
            // Make sure that the Max value used correspond to the encoder settings.
            mCurrentEncoderCount = 1.0 * (qeiCount + mMultiplicationFactor * (65535));
            angularVelocity =
                (ONE_BY_TWO_PI * 7.66E-4 * (mCurrentEncoderCount - mLastEncoderValue)) / 0.01;
            angularVelocity *= 60.0;
            mCurrentRPM = &angularVelocity;
            mRpmMailBox.post(mCurrentRPM, TIME_IMMEDIATE);
            mLastEncoderValue = mCurrentEncoderCount;
            lastQeiCount = qeiCount;
            chThdSleepMilliseconds(10);
        }
    }

  private:
    /**
     * @brief mCurrentRPM The pointer to the current RPM
     */
    float* mCurrentRPM;
    /**
     * @brief mCurrentEncoderCount The current encoder count in 32bit.
     * If the timers TIM2 and TIM5 are used then this is not necessary as they are
     * already 32 bit timers.
     */
    int32_t mCurrentEncoderCount;
    /**
     * @brief mRpmMailBox The mail box for holding the RPM messages.
     */
    RpmMailBox mRpmMailBox;
    /**
     * @brief mLastEncoderValue The last encoder value
     */
    int32_t mLastEncoderValue;
    /**
     * @brief mMultiplicationFactor The multiplication factor for converting the 16bit to 32 bit
     * value.
     */
    int32_t mMultiplicationFactor;
};
#endif // QUADENCODER_H
