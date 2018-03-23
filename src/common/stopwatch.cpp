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

#include "common/stopwatch.h"
#include <iomanip>

namespace Nl {

/** \ingroup Tools
 *
 * \brief Constructor
 * \param sw Pointer to \ref StopWatch object
 * \param name Name of timestamp
 *
 * Calls StopWatch::start() in a RAII fashion.
 *
*/
StopBlockTime::StopBlockTime(std::shared_ptr<StopWatch> sw, std::string name = "noname") :
    m_currentStopWatch(sw)
{
    if (m_currentStopWatch)
        m_currentStopWatch->start(name);
}

/** \ingroup Tools
 *
 * \brief Destructor
 *
 * Calls StopWatch::stop() in a RAII fashion.
 *
*/
StopBlockTime::~StopBlockTime()
{
    if (m_currentStopWatch)
        m_currentStopWatch->stop();

    m_currentStopWatch = nullptr;
}

/** \ingroup Tools
 *
 * \brief Constructor
 *
 * Creates a StopWatch object
 *
*/
StopWatch::StopWatch(const std::string &name, uint32_t windowSize, Mode m) :
    m_mutex(),
    m_timestamps(),
    m_currentTimeStamp(),
    m_waitingForStop(false),
    m_name(name),
    m_windowSize(windowSize),
    m_mode(m)
{
}

/** \ingroup Tools
 *
 * \brief Set start timestamp to now.
 * \param name Name of timestamp
 *
 * Sets the start point for a duration to now.
 * \ref StopBlockTime provides a RAII style interface for the same purpos.
 *
*/
void StopWatch::start(const std::string &name)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_waitingForStop) {
        std::cout << "ERR: " << "Unbalanced start()/stop(). Ignoring start()!" << std::endl;
        return;
    }

    m_currentTimeStamp.start = std::chrono::high_resolution_clock::now();
    m_currentTimeStamp.name = name;

    m_waitingForStop = true;
}
/** \ingroup Tools
 *
 * \brief Set stop timestamp to now.
 *
 * Sets the stop point for a duration to now.
 * \ref StopBlockTime provides a RAII style interface for the same purpos.
 *
*/
void StopWatch::stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_waitingForStop) {
        std::cout << "ERR: " << "Unbalanced start()/stop(). Ignoring stop()!" << std::endl;
        return;
    }

    m_currentTimeStamp.stop = std::chrono::high_resolution_clock::now();

    // Do moving window, if 10000 values are reached
    if (m_timestamps.size() >= 10000)
        m_timestamps.pop();

    // Protect by mutex
    m_timestamps.push(m_currentTimeStamp);


    m_waitingForStop = false;
}

/** \ingroup Tools
 *
 * \brief Is used for prints using operator<<, depending on \ref Mode
 * \param rhs A stream of type \ref std::ostream where data is written to.
 * \return A stream of type \ref std::ostream containing new data.
 *
 * This function prints a detailed list, oder a summery, dependent on \ref Mode
 *
*/
std::ostream& StopWatch::print(std::ostream& rhs)
{
    if (m_mode == SUMMARY)
        return printSummary(rhs);

    return printDetailed(rhs);
}

/** \ingroup Tools
 *
 * \brief Prints a detailed list with all timestamps
 * \param rhs A stream of type \ref std::ostream where data is written to.
 * \return A stream of type \ref std::ostream containing new data.
 *
 * This function prints a list with all start, stop and duration times.
 *
*/
std::ostream& StopWatch::printDetailed(std::ostream &rhs)
{
    //TODO: Make this print the whole buffer with all details.
    //      Not used right now.

    // Create an new empty copy and swap with the full one.
    m_mutex.lock();
    std::queue<Timestamp> workCopy;
    std::swap(m_timestamps, workCopy);
    m_mutex.unlock();

    while (!workCopy.empty()) {
        Timestamp cur = workCopy.front();
        workCopy.pop();

        int delta_us = std::chrono::duration_cast<std::chrono::microseconds>(cur.stop-cur.start).count();

        std::cout << cur.name
                  << "  "
                     //<< std::chrono::system_clock::to_time_t(cur.start)
                     //<< std::put_time(std::chrono::system_clock::to_time_t(cur.start), "%F %T")
                     //<< "  "
                     //<< std::put_time(std::chrono::system_clock::to_time_t(cur.stop), "%F %T")
                  << "  "
                  << delta_us <<  " us" << std::endl;
    }

    return rhs;
}

/** \ingroup Tools
 *
 * \brief Prints a summary of all timestamps
 * \param rhs A stream of type \ref std::ostream where data is written to.
 * \return A stream of type \ref std::ostream containing new data.
 *
 * This function prints a summary of start, stop and duration times and empties the queue.
 *
*/
std::ostream& StopWatch::printSummary(std::ostream &rhs)
{
    // Create an new empty copy and swap with the full one.
    m_mutex.lock();
    std::queue<Timestamp> workCopy;
    std::swap(m_timestamps, workCopy);
    m_mutex.unlock();

    unsigned long itemCount = workCopy.size();
    unsigned long sum = 0;
    unsigned int min = std::numeric_limits<unsigned int>::max();
    unsigned int max = 0;
    std::string minName = "";
    std::string maxName = "";

    while (!workCopy.empty()) {
        Timestamp cur = workCopy.front();
        workCopy.pop();

        unsigned int delta_us = std::chrono::duration_cast<std::chrono::microseconds>(cur.stop-cur.start).count();
        sum += delta_us;

        if (delta_us < min) {
            min = delta_us;
            minName = cur.name;
        }

        if (delta_us > max) {
            max = delta_us;
            maxName = cur.name;
        }
    }

    double mean = static_cast<double>(sum) / static_cast<double>(itemCount);

    rhs << std::setiosflags(std::ios::fixed) << std::setprecision(2)
        << "Timing: [" << m_name << "] " << std::endl
        << "values=" << itemCount << " "
        << "sum=" << sum << "us "
        << "min[" << minName << "]=" << min << "us "
        << "max[" << maxName << "]=" << max << "us "
        << "mean=" << mean << "us" << std::endl;

    return rhs;
}

/** \ingroup Tools
 *
 * \brief Print StopWatch using operator<<
 * \param lhs Left hand Side
 * \param rhs Right hand Side
 * \return std::ostream with string data
 *
 * Helper overload of operator<< for StopWatch
 * so it can be used as:
 *
 * \code{.cpp}
 *	std::cout << "ExecutionTimes: " << std::endl << stopWatchInstance << std::endl;
 * \endcode
*/
std::ostream& operator<<(std::ostream& lhs, StopWatch& rhs)
{
    return rhs.print(lhs);
}

}
