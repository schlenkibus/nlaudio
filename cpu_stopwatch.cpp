#include "cpu_stopwatch.h"

//namespace Nl {


/// Default Contstuctor

CPU_Stopwatch::CPU_Stopwatch():
    mTimestamps(),
//    m_mutex(),
    mCurrentTimestamp(),
    mWaitingForStop(false),
    mStampCounter(0)
{
}


/// Save the time when the process starts

void CPU_Stopwatch::start()
{
//    std::lock_guard<std::mutex> lock(m_mutex);

    if (mWaitingForStop) {
        std::cout << "ERR: " << "Unbalanced start()/stop(). Ignoring start()!" << std::endl;
        return;
    }

    mCurrentTimestamp.start = std::chrono::high_resolution_clock::now();

    mWaitingForStop = true;
}


/// Save the time when the process starts

void CPU_Stopwatch::stop()
{
//    std::lock_guard<std::mutex> lock(m_mutex);

    if (!mWaitingForStop) {
        std::cout << "ERR: " << "Unbalanced start()/stop(). Ignoring stop()!" << std::endl;
        return;
    }

    mCurrentTimestamp.stop = std::chrono::high_resolution_clock::now();

    // Protect by mutex ... hm!?
    mTimestamps.at(mStampCounter) = mCurrentTimestamp;

    mStampCounter++;

    if(mStampCounter == NUMBER_OF_TS)
    {
        mStampCounter = 0;
        mCPU_peak = 0.f;
    }

    mWaitingForStop = false;
}

/// Calculates the CPUs and sets the peak Value

void CPU_Stopwatch::calcCPU()
{
    for (int i = 0; i < NUMBER_OF_TS - 1; i++)
    {
        CPU_Timestamp ts1 = mTimestamps.at(i);
        CPU_Timestamp ts2 = mTimestamps.at(i+1);

        int numerator = std::chrono::duration_cast<std::chrono::microseconds>(ts1.stop-ts1.start).count();
        int denominator = std::chrono::duration_cast<std::chrono::microseconds>(ts2.start-ts1.start).count();

        float curCPU = static_cast<float>(numerator) / static_cast<float>(denominator);

        mCPU_peak = std::max(curCPU, mCPU_peak);
    }
}

//} // namespace Nl
