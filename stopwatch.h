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

/** \ingroup Tools
 * \struct Timestamp - Stores time stamps for execution time measurements.
 *
*/
struct Timestamp {
	std::chrono::time_point<std::chrono::high_resolution_clock> start; ///< Start Time of the time stamp
	std::chrono::time_point<std::chrono::high_resolution_clock> stop; ///< Stop Time of the time stamp
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
class StopBlockTime
{
public:
	StopBlockTime(StopWatch *sw, std::string name);
	~StopBlockTime();
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
	StopWatch(const std::string& name);
	void start(const std::string& name);
	void stop();
	std::ostream& printDetailed(std::ostream &rhs);
	std::ostream& printSummary(std::ostream& rhs);

private:
	std::mutex m_mutex;
	std::queue<Timestamp> m_timestamps;
	Timestamp m_currentTimeStamp;
	bool m_waitingForStop;
	std::string m_name;
};

std::ostream& operator<<(std::ostream& lhs, StopWatch& rhs);

} // namespace Nl
