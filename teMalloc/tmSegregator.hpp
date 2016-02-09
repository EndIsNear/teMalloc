#pragma once
#include "tmBaseAlloc.hpp"

template <size_t Treshold, class SmallAllocator, class LargeAllocator>
class Segregator : private SmallAllocator, private LargeAllocator
{
public:
	Segregator() {}

	Block alloc(size_t size)
	{
		if (size)
		{
			if (size <= Treshold)
				return SmallAllocator::alloc(size);
			return LargeAllocator::alloc(size);
		}
		return Block();
	}

	bool owns(const Block& block) const
	{
		if (block.len <= Treshold)
			return SmallAllocator::owns(block);
		return LargeAllocator::owns(block);
	}

	void free(Block& block)
	{
		if (block)
		{
			if (block.len <= Treshold)
				return SmallAllocator::free(block);
			return LargeAllocator::free(block);
		}
	}

private:
	Segregator(const Segregator&) = delete;
	Segregator(Segregator&&) = delete;
	Segregator& operator=(const Segregator&) = delete;
};