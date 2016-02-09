#pragma once
#include "tmBaseAlloc.hpp"

//If Primaey allocators fail to alloc the needed memory
//Fallback allocator is called to allocate it

template <class Primary, class Fallback>
class FallbackAllocator : private Primary, private Fallback
{
public:
	FallbackAllocator() {}

	Block alloc(size_t size)
	{
		Block res = Primary::alloc(size);
		if (!res)
		{
			res = Fallback::alloc(size);
		}
		return res;
	}

	bool owns(const Block& block) const
	{
		return Primary::owns(block) || Fallback::owns(block);
	}

	void free(Block& block)
	{
		if (Primary::owns(block))
			Primary::free(block);
		else if (Fallback::owns(block))
			Fallback::free(block);
	}

private:
	FallbackAllocator(const FallbackAllocator&) = delete;
	FallbackAllocator(FallbackAllocator&&) = delete;
	FallbackAllocator& operator=(const FallbackAllocator&) = delete;
};