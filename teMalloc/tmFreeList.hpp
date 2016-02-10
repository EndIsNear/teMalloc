#pragma once
#include "tmBaseAlloc.hpp"
#include <mutex>

//Free list of given allocator
//It should wraps other allocator and contains its freed elements
//returns first block of free list with size bigger than wanted
//otherwise calls BaseAllocator for new block
//NOTE: BaseAllocator Min size have to be at least sizeof(FreeList::FreeNode) + sizeof(size_t)

template <class BaseAllocator>
class FreeList : private BaseAllocator
{
	struct FreeNode
	{
		FreeNode() { nxt = nullptr; }
		FreeNode * nxt;
		size_t size;
	} m_freeList;

	std::mutex m_mutex;
public:
	FreeList() {}
	~FreeList()
	{
		FreeNode * iter = m_freeList.nxt;
		while (iter)
		{
			FreeNode * tmp = iter;
			iter = iter->nxt;
			BaseAllocator::free(Block(static_cast<void*>(tmp), tmp->size));
		}
	}

	Block alloc(size_t size)
	{
		Block res;
		{
			//lock the free list while iterate
			std::lock_guard<std::mutex> guard(m_mutex);
			FreeNode * iter = m_freeList.nxt;
			while (iter)
			{
				if (iter->size >= size)
				{
					res.ptr = reinterpret_cast<void*>(iter);
					res.len = iter->size;
					m_freeList.nxt = m_freeList.nxt->nxt;

					return res;
				}
				iter = iter->nxt;
			}
		}

		return BaseAllocator::alloc(size);
	}

	bool owns(const Block& block) const
	{
		return BaseAllocator::owns(block);
	}

	void free(Block& block)
	{
		if (block && block.ptr && block.len)
		{
			//lock the free list
			std::lock_guard<std::mutex> guard(m_mutex);

			FreeNode * newNode = reinterpret_cast<FreeNode*>(block.ptr);
			newNode->nxt = m_freeList.nxt;
			newNode->size = block.len;
			m_freeList.nxt = reinterpret_cast<FreeNode*>(block.ptr);
			block.len = 0;
			block.ptr = nullptr;
		}
	}

private:
	FreeList(const FreeList&) = delete;
	FreeList(FreeList&&) = delete;
	FreeList& operator=(const FreeList&) = delete;
};