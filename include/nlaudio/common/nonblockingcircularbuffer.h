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

#pragma once

#include <iostream>
#include "lockfreecircularbuffer.h"
#include "samplespecs.h"

namespace Nl {

/** \ingroup Audio
 *
 * \brief A blocking circular buffer implementation
 * \tparam Type of buffer elements
 * \param size Buffersize in elements of type T
 *
 * A circular buffer implementation which blocks the callee of get()
 * if nothing to read and the callee of set() when no space
 * to write
 *
*/
template <typename T>
class BlockingCircularBuffer
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
	BlockingCircularBuffer(const std::string& name) :
		m_size(0),
		m_name(name),
		m_bytesRead(0),
		m_bytesWritten(0)
	{}

	~BlockingCircularBuffer()
	{

	}

	void init(int size)
	{

	}

	// Not sure if this is proper. This class should not know something about SampleSpecs.
	void init(const SampleSpecs &sampleSpecs)
	{
		init(sampleSpecs.buffersizeInBytes);
		m_sampleSpecs = sampleSpecs;
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
	void get(T *buffer, unsigned int size)
	{

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
	void set(T *buffer, unsigned int size)
	{

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
	void getStat(unsigned long *readBytes, unsigned long *writtenBytes) const
	{
		*readBytes = m_bytesRead;
		*writtenBytes = m_bytesWritten;
	}

	/** \ingroup Audio
	 *
	 * \brief Returns number of available elements to read.
	 * \return A number of available elements to read.
	 *
	 * Returns how many elements can be read from the buffer,
	 * before the callee is blocked.
	*/
	inline unsigned int availableToRead() const
	{

	}

	/** \ingroup Audio
	 *
	 * \brief Returns number of available elements to write.
	 * \return A number of available elements to write.
	 *
	 * Returns how many elements can be written to the buffer,
	 * before the callee is blocked.
	*/
	inline unsigned int availableToWrite() const
	{

	}

	/** \ingroup Audio
	 *
	 * \brief Returns the size of the buffer in elements.
	 * \return The size of the buffer in elements.
	 *
	 * Returns the total size in elements of the buffer.
	*/
	inline int size() const
	{
		return m_size;
	}

	/** \ingroup Audio
	 *
	 * \brief Returns the name of the buffer.
	 * \return The name of the Buffer.
	 *
	 * Returns the name of the buffer.
	*/
	inline std::string name() const
	{
		return m_name;
	}

	/** \ingroup Audio
	 *
	 * \brief Returns the SampleSpecs_t of the buffer.
	 * \return SampleSpecs_t which has been used to initialize this buffer.
	 *
	 * Returns the SampleSpecs_t which has been used to initialize this buffer.
	*/
	inline SampleSpecs sampleSpecs()
	{
		return m_sampleSpecs;
	}

private:
	std::string m_name;
	int m_size;

	std::atomic<unsigned long> m_bytesRead;
	std::atomic<unsigned long> m_bytesWritten;

	SampleSpecs m_sampleSpecs;
	CircularFifo<u_int8_t, 512> m_buffer;


};

/*! A shared handle to a \ref BlockingCircularBuffer<uint8_t> */
typedef std::shared_ptr<BlockingCircularBuffer<uint8_t>> SharedBufferHandle;

} // namespace Nl
