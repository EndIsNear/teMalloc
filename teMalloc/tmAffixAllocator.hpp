#pragma once

#include "tmBaseAlloc.hpp"

template <class Allocator, class Affix, bool IsPrefix = true>
class AffixAllocator : private Allocator
{
	const int affixSize = roundTo(sizeof(Affix), Allocator::aligmentSize);

	inline void * moveWith(void * ptr, int size)
	{
		return static_cast<char*>(ptr) + size;
	}


	Block adaptBlock(const Block& b)
	{
		if (IsPrefix)
			return Block(moveWith(b.ptr, -affixSize), b.len + affixSize);
		else
			return Block(b.ptr, b.len + affixSize);
	}

	void placePrefix(Block& block)
	{
		new (block.ptr) Affix();
		block.ptr = moveWith(block.ptr, affixSize);
		block.len -= affixSize;
	}

	void placeSufix(Block& block)
	{
		block.len -= affixSize;
		new (static_cast<char*>(block.ptr) + block.len) Affix();
	}

public:
	AffixAllocator() {}
	Block alloc(size_t size)
	{
		Block res;
		res = Allocator::alloc(size + affixSize);
		if (res)
		{
			if (IsPrefix)
				placePrefix(res);
			else
				placeSufix(res);

		}
		return res;
	}

	bool owns(const Block& block) const
	{
		return Allocator::owns(adaptBlock(block));
	}

	void free(Block& block)
	{
		if (block)
		{
			Block b = adaptBlock(block);
			if (IsPrefix)
				reinterpret_cast<Affix*>(b.ptr)->~Affix();
			else
				reinterpret_cast<Affix*>(static_cast<char*>(b.ptr) + b.len)->~Affix();
			Allocator::free(b);
		}
	}

private:
	AffixAllocator(const AffixAllocator&) = delete;
	AffixAllocator(AffixAllocator&&) = delete;
	AffixAllocator& operator=(const AffixAllocator&) = delete;

};