#include "hal.h"

class QuadEncoder  
{
	public:
		/**
		 * Enum for the direction
		 */
		enum class Direction : uint8_t
		{
			FORWARD = 0,
			REVERSE = 1,
		};
		// Constructors
		QuadEncoder(QEIDriver *qeiDriver, QEIConfig *qeiConfig);
		// Destructor
		~QuadEncoder();
		/**
		 * Initialize the driver.
		 * This must be called after the system initialization.
		 */
		void init()
		{
			// Start the QEIDriver
    		qeiStart(mQeiDriver, mQeiConfig);
    		qeiEnable(mQeiDriver);
		}
		/**
		 * GPIO A related parameters that will be used to handle the interrupt
		 */
		void setGpioAParams(GPIO_TypeDef *gpioLine, uint8_t pinNumber)
		{
			mGpioALine = gpioLine;
			mPinANumber = pinNumber;
		}
		/**
		 * GPIO B related parameters that will be used to handle the interrupt
		 */
		void setGpioBParams(GPIO_TypeDef *gpioLine, uint8_t pinNumber)
		{
			mGpioBLine = gpioLine;
			mPinBNumber = pinNumber;
		}
		// Interrupt callback
		void handlePinAInterrupt();
		// Getters
		const Direction& getCurrentDirection() const
		{
			return mCurrentDirection;
		}
		const uint16_t& getPulseCount() const
		{
			return mCurrentPulseCount;
		}
		const double& getCurrentAngleRad() const
		{
			return mCurrentAngleRad;
		}
	private:
		/**
		 * The Quadrature encoder driver that will be used
		 */
		QEIDriver *mQeiDriver;
		QEIConfig *mQeiConfig;
		/**
		 * Internal count that will be saved
		 * Note that it will not have any computations done. Just
		 * a variable that saves the count.
		 */
		uint16_t mCurrentPulseCount;
		/**
		 * The current state of the Encoder. Forward or Reverse
		 * FORWARD - Clockwise
		 * REVERSE - Anti-clockwise
		 */
		Direction mCurrentDirection;
		/**
		 * Current angle in radians
		 */
		double mCurrentAngleRad;
		/**
		 * GPIO that will be used for A pin
		 */
		GPIO_TypeDef *mGpioALine;
		/**
		 * GPIO that will be used for B pin
		 */
		GPIO_TypeDef *mGpioBLine;
		/**
		 * Pin number of A 
		 */
		uint8_t mPinANumber;
		/**
		 * Pin number of B
		 */
		uint8_t mPinBNumber;
};