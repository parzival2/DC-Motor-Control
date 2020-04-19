#ifndef PIDCONTROLLER_HPP
#define PIDCONTROLLER_HPP

/**
 * @brief The PIDController class A Simple PID controller class
 * Implemented based on https://en.wikipedia.org/wiki/PID_controller and the
 * pseudocode can be found in the same article.
 */
template<class FType>
class PIDController
{
  public:
    // Constructor
    PIDController(FType kp, FType ki, FType kd)
        : mKp(kp)
        , mKi(ki)
        , mKd(kd)
        , mReference(0.)
        , mIntegral(0.)
        , mPreviousError(0.)
    {
    }
    // Destructor
    ~PIDController()
    {
        reset();
    }
    // Resets all the internal variables
    void reset()
    {
        mKp = 0.;
        mKi = 0.;
        mKd = 0.;
        mIntegral = 0.;
        mReference = 0.;
    }
    /**
     * @brief calculateOutput Calculates the output by taking in the input and the time difference
     * between the inputs. Note that no checking is done is the input is in bounds or the time
     * difference is actually plausible.
     * @param input The input value that will be used to calculate the error between the reference
     * and input
     * @param deltaT The time difference between the two calls.
     * @return output that has been calculated.
     */
    FType calculateOutput(FType input, FType deltaT)
    {
        // Calculate the error
        FType error = mReference - input;
        // Proportional term
        FType propTerm = mKp * error;
        // Integral term
        mIntegral += (error * deltaT);
        // Derivative term
        FType dError = (error - mPreviousError) / deltaT;
        FType derivative = mKd * dError;
        mPreviousError = error;
        return (propTerm + mIntegral * mKi + derivative);
    }

    // Setters
    /**
     * @brief setProportionalValue Sets the proportional value to the value given
     * @param kp The mKp value
     */
    void setProportionalValue(FType kp)
    {
        mKp = kp;
    }
    /**
     * @brief setIntegralValue Sets the integral value to the value given
     * @param ki The mKi value
     */
    void setIntegralValue(FType ki)
    {
        mKi = ki;
    }
    /**
     * @brief setDerivativeValue Sets the derivative value to the given value
     * @param kd The mKd value
     */
    void setDerivativeValue(FType kd)
    {
        mKd = kd;
    }
    /**
     * @brief setReference Set the reference that will be tracked.
     * @param reference Reference value
     */
    void setReference(FType reference)
    {
        mReference = reference;
    }
    // Getters
    /**
     * @brief getProportionalValue Returns the internal proportional value
     * @return Returns the value of mKp
     */
    FType getProportionalValue() const
    {
        return mKp;
    }
    /**
     * @brief getIntegralValue Returns the internal integral value
     * @return Returns the value of mKi
     */
    FType getIntegralValue() const
    {
        return mKi;
    }
    /**
     * @brief getDerivativeValue Returns the derivative value
     * @return Returns the value of mKd
     */
    FType getDerivativeValue() const
    {
        return mKd;
    }
    /**
     * @brief getReferenceValue Returns the value of reference that this controller is tracking.
     * @return Returns the mReference value.
     */
    FType getReferenceValue() const
    {
        return mReference;
    }

  private:
    /**
     * @brief Kp Proportional value in PID controller
     */
    FType mKp;

    /**
     * @brief Ki Integral value in PID controller
     */
    FType mKi;

    /**
     * @brief Kd Derivative value in PID controller
     */
    FType mKd;

    /**
     * @brief mReference The reference to be tracked.
     */
    FType mReference;

    /**
     * @brief mIntegral The integral
     */
    FType mIntegral;

    /**
     * @brief mPreviousError The previous error that will be used for derivative term.
     */
    FType mPreviousError;
};

#endif // PIDCONTROLLER_HPP
