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

#include <mutex>
#include <condition_variable>

namespace Nl {

/** \ingroup Audio
 *
 * \brief A blocking linear buffer implementation
 * \tparam Type of buffer elements
 * \param size Buffersize in elements of type T
 *
 * A linear buffer implementation which blocks the callee of get()
 * if nothing to read and the callee of set() if no space
 * to write
 *
*/
template <typename T>
class BlockingLinearBuffer
{
public:
	/** \ingroup Audio
	 *
	 * \brief Constructor
	 * \tparam T element type
	 * \param size Buffersize in elements
	 *
	 * Sets up a new buffer.
	 *
	*/
	BlockingLinearBuffer(unsigned int size) :
		m_buffer(new T[size]),
		m_size(size),
		m_bytesRead(0),
		m_bytesWritten(0)
	{
		memset(m_buffer, 0, m_size);
	}

	~BlockingLinearBuffer()
	{
		delete[] m_buffer;
	}

	/** \ingroup Audio
	 *
	 * \brief Read data from the buffer
	 * \param buffer Buffer to save data to
	 * \param size Buffersize int bytes
	 *
	 * Reads size bytes into buffer. Blocks callee if
	 * less data available than requested.
	 *
	*/
	void BlockingLinearBuffer::get(T *buffer, unsigned int size)
	{
		std::unique_lock<std::mutex> mlock(m_mutex);
		while ((m_bytesWritten - m_bytesRead) < size)
			m_condition.wait(mlock);

		m_bytesRead += size;
		memcpy(buffer, m_buffer, size);

		return size;
	}

	/** \ingroup Audio
	 *
	 * \brief Write data to the buffer
	 * \param buffer Buffer to read data from
	 * \param size Buffersize int bytes
	 *
	 * Writes size bytes into buffer. Blocks callee if
	 * less space available than requested. Wakes waiting
	 * readers.
	 *
	*/
	void set(void *buffer, unsigned int size)
	{
		std::unique_lock<std::mutex> mlock(m_mutex);
		m_bytesWritten += size;
		memcpy(m_buffer, buffer, size);
		mlock.unlock();
		m_condition.notify_one();
	}

	/** \ingroup Audio
	 *
	 * \brief Returns information on read/write cycles on the buffer
	 * \param readBytes Total bytes read from buffer
	 * \param writtenBytes Total writes to buffer
	 *
	 * Returns how many write and how man read cycles have been performed
	 * on the buffer.
	*/
	getStat(unsigned long *readBytes, unsigned long *writtenBytes)
	{
		std::unique_lock<std::mutex> mlock(m_mutex);
		*readBytes = m_bytesRead;
		*writtenBytes = m_bytesWritten;
	}

private:
	T *m_buffer;
	int m_size;

	std::mutex m_mutex;
	std::condition_variable m_condition;
	unsigned long m_bytesRead;
	unsigned long m_bytesWritten;
};

} // namespace Nl
