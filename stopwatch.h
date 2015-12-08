#pragma once

#include <chrono>
#include <ctime>
#include <queue>
#include <string>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <thread>

namespace Nl {

class StopWatch;

struct Timestamp {
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::chrono::time_point<std::chrono::high_resolution_clock> stop;
	std::string name;
};

/** \ingroup Tools
 *
 * \class StopFunctionTime
 * \brief Helper class, that measures time duration for the time the object exists.
 *
 * This class calls StopWatch::start("") in constructor ans StopWatch::stop() in destructor.
 * Hence it can be used like this, to measure the total executiontime of a function:
 *
 * \code{.cpp}
 * StopWatch sw();
 *
 * void anyFunction()
 * {
 *    StopFunctionTime sft(&sw, "Valuename"); // Start measuring time from here.
 *
 *    // Some very expensive code here
 * }
 * \endcode
 *
*/
class StopFunctionTime
{
public:
	StopFunctionTime(StopWatch *sw, std::string name);
	~StopFunctionTime();
private:
	StopWatch *m_currentStopWatch;
};

/** \ingroup Tools
 *
 * \class StopWatch
 * \brief Helper class, that measures time durations.
 *
 * This class can be used to log execution time.
 *
 * \code{.cpp}
 * StopWatch sw();
 * void anyFunction()
 * {
 *	  sw.start("TimePointName");
 *    // Some very expensive code here
 *    sw.stop();
 * }
 * \endcode
 *
*/
class StopWatch
{
public:
	StopWatch();
	void start(const std::string& name);
	void stop();
	void print();
	void printSummary();
	void clear();

private:
	std::mutex m_mutex;
	std::queue<Timestamp> m_timestamps;
	Timestamp m_currentTimeStamp;
	bool m_waitingForStop;
};

} // namespace Nl
