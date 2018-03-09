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

#include <atomic>
#include <cstddef>

template<typename Element>
class CircularFifo{
public:
	CircularFifo() : m_tail(0), m_head(0), m_size(0) {}
	virtual ~CircularFifo() {}

	bool push(const Element& item); // pushByMOve?
	bool pop(Element& item);

	bool wasEmpty() const;
	bool wasFull() const;
	bool isLockFree() const;

private:
	size_t increment(size_t idx) const;

	std::atomic<size_t>  m_tail;  // tail(input) index
	Element    *m_array;
	std::atomic<size_t>   m_head; // head(output) index
	size_t m_size;
};


// Here with memory_order_seq_cst for every operation. This is overkill but easy to reason about
//
// Push on tail. TailHead is only changed by producer and can be safely loaded using memory_order_relexed
//         head is updated by consumer and must be loaded using at least memory_order_acquire
template<typename Element>
bool CircularFifo<Element>::push(const Element& item)
{
	const auto current_tail = m_tail.load();
	const auto next_tail = increment(current_tail);
	if(next_tail != m_head.load())
	{
		m_array[current_tail] = item;
		m_tail.store(next_tail);
		return true;
	}

	return false;  // full queue
}


// Pop by Consumer can only update the head
template<typename Element>
bool CircularFifo<Element>::pop(Element& item)
{
	const auto current_head = m_head.load();
	if(current_head == m_tail.load())
		return false;   // empty queue

	item = m_array[current_head];
	m_head.store(increment(current_head));
	return true;
}

// snapshot with acceptance of that this comparison function is not atomic
// (*) Used by clients or test, since pop() avoid double load overhead by not
// using wasEmpty()
template<typename Element>
bool CircularFifo<Element>::wasEmpty() const
{
	return (m_head.load() == m_tail.load());
}

// snapshot with acceptance that this comparison is not atomic
// (*) Used by clients or test, since push() avoid double load overhead by not
// using wasFull()
template<typename Element>
bool CircularFifo<Element>::wasFull() const
{
	const auto next_tail = increment(m_tail.load());
	return (next_tail == m_head.load());
}

template<typename Element>
bool CircularFifo<Element>::isLockFree() const
{
	return (m_tail.is_lock_free() && m_head.is_lock_free());
}

template<typename Element>
size_t CircularFifo<Element>::increment(size_t idx) const
{
	return (idx + 1) % m_size;
}
