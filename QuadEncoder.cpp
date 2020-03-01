#include "QuadEncoder.hpp" 
#include "ch.hpp"

/**
 * @brief Constructor
 * 
 * @param[in] qeiDriver The pointer to Quadrature encoder driver
 * @param[in] qeiConfig The pointer to the Quadrature encoder driver config
 */
QuadEncoder::QuadEncoder(QEIDriver *qeiDriver, QEIConfig *qeiConfig)
    : mQeiDriver(qeiDriver)
    , mQeiConfig(qeiConfig)
    , mCurrentPulseCount(0)
    , mCurrentDirection(Direction::FORWARD)
    , mCurrentAngleRad(0.)
    , mGpioALine(GPIOA)
    , mGpioBLine(GPIOA)
    , mPinANumber(7)
    , mPinBNumber(8)
{
}

/**
 * @brief Destructor
 */
QuadEncoder::~QuadEncoder()
{
    // Do nothing
}

/**
 * @brief Handles interrupt on PINA connected to Encoder
 * 
 * This interrupt call will handle the direction, overflow and the angle count
 */
void QuadEncoder::handlePinAInterrupt()
{
    // Lock 
    chibios_rt::System::lockFromIsr();
    mCurrentPulseCount = qeiGetCountI(mQeiDriver);
    // Set the direction
    // http://www.creative-robotics.com/quadrature-intro
    if(palReadPad(mGpioALine, mPinBNumber) == 0)
    {
        mCurrentDirection = Direction::FORWARD;
        mCurrentAngleRad += 4 * (6.28319 / mQeiConfig->max);
    }
    else if (palReadPad(mGpioALine, mPinBNumber) == 1)
    {
        mCurrentDirection = Direction::REVERSE;
        mCurrentAngleRad -= 4 * (6.28319 / mQeiConfig->max);
    }
    chibios_rt::System::unlockFromIsr();
}