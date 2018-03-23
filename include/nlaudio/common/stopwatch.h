/***
  Copyright (c) 2018 Nonlinear Labs GmbH

  Authors: Pascal Huerst <pascal.huerst@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
***/

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
    StopBlockTime(std::shared_ptr<StopWatch> sw, std::string name);
    ~StopBlockTime();
private:
    std::shared_ptr<StopWatch> m_currentStopWatch;
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
    std::ostream& printDetailed(std::ostream& rhs);
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
