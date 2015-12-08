#include "stopwatch.h"

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
StopFunctionTime::StopFunctionTime(StopWatch *sw, std::string name = "noname")
{
	m_currentStopWatch = sw;
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
StopFunctionTime::~StopFunctionTime()
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
StopWatch::StopWatch() :
	m_mutex(),
	m_timestamps(),
	m_currentTimeStamp(),
	m_waitingForStop(false)
{
}

/** \ingroup Tools
 *
 * \brief Set start timestamp to now.
 * \param name Name of timestamp
 *
 * Sets the start point for a duration to now.
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

	// Protect by mutex
	m_timestamps.push(m_currentTimeStamp);

	m_waitingForStop = false;
}

void StopWatch::clear()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	//while (true) {
		//m_mutex.lock();

		//if (!m_waitingForStop) {
			std::queue<Timestamp> empty;
			std::swap(m_timestamps, empty);
			//m_mutex.unlock();
			//break;
		//} else {
		//	m_mutex.unlock();
		//	std::this_thread::sleep_for(std::chrono::milliseconds(10));
		//}
	//}
}

void StopWatch::print()
{
	m_mutex.lock();
	std::queue<Timestamp> workCopy = m_timestamps;
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
					 //<< "  "
				  << delta_us <<  " us" << std::endl;
	}
}

void StopWatch::printSummary()
{
	m_mutex.lock();
	std::queue<Timestamp> workCopy = m_timestamps;
	m_mutex.unlock();

	unsigned long itemCount = workCopy.size();
	unsigned long sum = 0;
	unsigned int min = 0;
	unsigned int max = 0;
//TODO: min and max values can be expressed by their names as well!
//	std::string minName = "";
//	std::string maxName = "";

	while (!workCopy.empty()) {
		Timestamp cur = workCopy.front();
		workCopy.pop();

		unsigned int delta_us = std::chrono::duration_cast<std::chrono::microseconds>(cur.stop-cur.start).count();
		sum += delta_us;
		min = (delta_us < min ? delta_us : min);
		max = (delta_us > max ? delta_us : max);
	}

	double mean = static_cast<double>(sum) / static_cast<double>(itemCount);

	std::cout << "Timing: " << std::endl
			  << " Values=" << itemCount
			  << " sum= " << sum << " us"
			  << " min= " << min << " us"
			  << " max= " << max << " us"
			  << " mean= " << mean << " us" << std::endl;

}

}
