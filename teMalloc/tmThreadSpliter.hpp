#pragma once
#include <thread>
#include "tmBaseAlloc.hpp"

//Contains MaxThreadsCount allocators
//on free or alloc it checks the thread by id
//and use its allocator if exist

template <class Allocator, size_t MaxThreadsCount = 8>
class ThreadSpliter
{
	//allocators[n] is responsible for threadIDs[n]
	Allocator allocators[MaxThreadsCount];
	std::thread::id threadIDs[MaxThreadsCount];
	size_t threadsCnt;

	inline int contains(const std::thread::id& id) const
	{
		for (int i = 0; i < threadsCnt; ++i)
		{
			if (id == threadIDs[i])
				return i;
		}
		return -1;
	}
public:
	ThreadSpliter() : threadsCnt(0) {}

	Block alloc(size_t size)
	{
		const std::thread::id crnID = std::this_thread::get_id();
		int idx = contains(crnID);
		//add first time used thread
		if (idx == -1)
		{
			assert(!(idx == MaxThreadsCount), "Try to add more threads than MaxThreadsCount");
			idx = threadsCnt++;
			threadIDs[idx] = crnID;
		}
		return allocators[idx].alloc(size);
	}

	bool owns(const Block& block) const
	{
		const std::thread::id crnID = std::this_thread::get_id();
		const int idx = contains(crnID);
		if (idx == -1)
			return false;
		return allocators[idx].owns(block);
	}

	void free(Block& block)
	{
		const std::thread::id crnID = std::this_thread::get_id();
		const int idx = contains(crnID);
		if (idx == -1)
			return;
		allocators[idx].free(block);
	}

private:
	ThreadSpliter(const ThreadSpliter&) = delete;
	ThreadSpliter(ThreadSpliter&&) = delete;
	ThreadSpliter& operator=(const ThreadSpliter&) = delete;

};