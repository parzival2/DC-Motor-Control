#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H
#include "LowpassFilter.hpp"
#include "PIDController.hpp"
#include "QuadEncoder.h"
#include "ch.hpp"
// Ros
#include "ros.h"
#include "std_msgs/Float32.h"
#include "std_msgs/UInt32.h"

class MotorController : public chibios_rt::BaseStaticThread<512>
{
  public:
    ///
    /// \brief DEFAULT_PID The default values for PID controller.
    ///
    static constexpr float DEFAULT_PID_P = 0.0;
    static constexpr float DEFAULT_PID_I = 0.00541;
    static constexpr float DEFAULT_PID_D = 0.0;
    ///
    /// \brief DEFAULT_LP The default values for Lowpass filter.
    ///
    static constexpr float DEFAULT_LP_CUTOFF = 0.02;
    static constexpr float DEFAULT_LP_DELTA_TIME = 0.01;
    MotorController(QuadEncoder::RpmMailBox& rpmMailBox)
        : BaseStaticThread<512>()
        , mRpmMailBox(rpmMailBox)
        , mLowPassFilter(DEFAULT_LP_CUTOFF, DEFAULT_LP_DELTA_TIME)
        , mPidController(DEFAULT_PID_P, DEFAULT_PID_I, DEFAULT_PID_D)
        , mRpmMessage()
        , mRpmPublisher("rpm", &mRpmMessage)
        , mReferenceSubscriber("reference", &MotorController::setReferenceCallback, this)
    {
        // Set some reference
        mPidController.setReference(1000);
    }

  protected:
    void main()
    {
        // Set the name of the thread.
        setName("MotorControllerThread");
        mRosNodeHandle.initNode();
        mRosNodeHandle.advertise(mRpmPublisher);
        mRosNodeHandle.subscribe(mReferenceSubscriber);
        // Start the while loop
        // The float pointer that the Mailbox holds.
        float* rpmValue;
        // PID output
        float output = 0;
        while(true)
        {
            // Fetch the rpm value from the mailbox
            mRpmMailBox.fetch(&rpmValue, TIME_IMMEDIATE);
            // Set the value for PID controller
            output = mPidController.calculateOutput(*rpmValue, 0.01);
            // Set the PWM for the motor
            pwmEnableChannel(&PWMD4, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, output * 1000));
            mRpmMessage.data = *rpmValue;
            mRpmPublisher.publish(&mRpmMessage);
            mRosNodeHandle.spinOnce();
            chThdSleepMilliseconds(10);
        }
    }

  private:
    /**
     * @brief mRpmMailBox The reference to the RPM mailbox in the Quadrature encoder.
     */
    QuadEncoder::RpmMailBox& mRpmMailBox;

    /**
     * @brief mLowPassFilter The low pass filter object for filtering the spikes in RPM calculation.
     */
    LowpassFilter mLowPassFilter;

    /**
     * @brief mPidController The PID controller object that will be used to control the Motor.
     */
    PIDController<float> mPidController;
    /**
     * @brief mRpmMessage The type of message that we are going to publish
     */
    std_msgs::Float32 mRpmMessage;
    /**
     * @brief mRosNodeHandle Rosnodehandle
     */
    ros::NodeHandle mRosNodeHandle;
    /**
     * @brief mRpmPublisher Ros publisher.
     */
    ros::Publisher mRpmPublisher;

    ros::Subscriber<std_msgs::UInt32, MotorController> mReferenceSubscriber;

    void setReferenceCallback(const std_msgs::UInt32& reference)
    {
        mPidController.setReference(reference.data);
    }
};
#endif // MOTORCONTROLLER_H
