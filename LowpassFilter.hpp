#ifndef LOWPASSFILTER_HPP
#define LOWPASSFILTER_HPP

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class LowpassFilter
{
  public:
    LowpassFilter(float cutOffFrequency, float deltaTime)
        : mOutput(0.)
        , mEPow(1 - exp(-deltaTime * 2 * M_PI * cutOffFrequency))
    {
    }

    float update(float input)
    {
        return mOutput += (input - mOutput) * mEPow;
    }

    float update(float input, float deltaTime, float cutOffFreqeuncy)
    {
        reconfigureFilter(deltaTime, cutOffFreqeuncy);
        return mOutput += (input - mOutput) * mEPow;
    }

    void reconfigureFilter(float deltaTime, float cutOffFrequency)
    {
        mEPow = 1 - exp(-deltaTime * 2 * M_PI * cutOffFrequency);
    }

  private:
    float mOutput;
    float mEPow;
};

#endif // LOWPASSFILTER_HPP
