#pragma once

#define NUMBER_OF_TS 50

#include <chrono>
#include <array>
#include <mutex>
#include <algorithm>
#include <iostream>

//namespace Nl {

//class CPU_Stopwatch;

struct CPU_Timestamp {
    std::chrono::time_point<std::chrono::high_resolution_clock> start; ///< Start Time of the time stamp
    std::chrono::time_point<std::chrono::high_resolution_clock> stop; ///< Stop Time of the time stamp
};

class CPU_Stopwatch
{   

public:
    CPU_Stopwatch();            // Constructor
    float mCPU_peak;
    void start();
    void stop();
    void calcCPU();

private:
//    std::mutex m_mutex;
    std::array<CPU_Timestamp, NUMBER_OF_TS> mTimestamps;
    CPU_Timestamp mCurrentTimestamp;
    bool mWaitingForStop;
    int mStampCounter;

};

//} // namespace Nl
