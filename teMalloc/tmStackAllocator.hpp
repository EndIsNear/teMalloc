#pragma once
#include "tmBaseAlloc.hpp"
#include <mutex>

//Extra fast allocator but using stack memory
//Its really good with FreeList on top of it

template <size_t MaxSize, size_t Aligment, bool ThreadSafe = false>
class StackAllocator
{
	static const size_t aligment = Aligment;

	char m_buffer[MaxSize];
	char * m_crnTop;

	bool isLastBlock(const Block &b) const 
	{
		return (static_cast<char*>(b.ptr) + b.len == m_crnTop);
	}

public:
	StackAllocator() : m_crnTop(m_buffer) {}

	Block alloc(size_t size)
	{
		Block res;

		if (!size)
			return res;

		size = roundTo(size, Aligment);
		//out of memory
		{
			if (ThreadSafe) std::lock_guard<std::mutex> guard(m_mutex);
			if (m_crnTop + size > m_crnTop + MaxSize)
				return res;

			res.ptr = m_crnTop;
			m_crnTop += size;
		}
		res.len = size;
		return res;
	}

	bool owns(const Block& block) const
	{
		return m_buffer > block.ptr && m_buffer + MaxSize < block.ptr;
	}

	void free(Block& block)
	{
		if (ThreadSafe) std::lock_guard<std::mutex> guard(m_mutex);
		if (block && isLastBlock(block))
		{
			m_crnTop = static_cast<char*>(block.ptr);
			block.clean();
		}
	}

private:
	StackAllocator(const StackAllocator&) = delete;
	StackAllocator(StackAllocator&&) = delete;
	StackAllocator& operator=(const StackAllocator&) = delete;

};