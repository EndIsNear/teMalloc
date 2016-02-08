#pragma once

struct Block {
	Block() : ptr(0), len(0) {}
	Block(void * ptr, size_t len) : ptr(ptr), len(len) {}

	~Block() {}

	inline void clean()
	{
		ptr = nullptr;
		len = 0;
	}

	inline operator bool() const 
	{
		return len && ptr;
	}

	void * ptr;
	size_t len;
};


template <size_t DefaultAlignment = sizeof(double)>
class BaseAlloc {
	//checks for proper DefaultAlignment
	static_assert(!(DefaultAlignment < sizeof(double)),
		"TeMalloc:DefaultAlignment is less than 16");
	static_assert((DefaultAlignment & (DefaultAlignment - 1)) == 0,
		"TeMalloc:DefaultAlignment isn't power of 2");

	BaseAlloc(const BaseAlloc&) = delete;
	BaseAlloc(const BaseAlloc&&) = delete;
	BaseAlloc& operator=(const BaseAlloc&) = delete;
public:
	const static size_t aligmentSize = DefaultAlignment;
	BaseAlloc() {}

	inline Block alloc(size_t size) 
	{
		//TODO: portable
		return Block(_aligned_malloc(size, DefaultAlignment), size);
	}

	inline void free(Block& block)
	{
		//TODO: portable
		if (block.ptr)
			_aligned_free(block.ptr);
		block.clean();
	}

	inline void deallocate(Block * block)
	{
		free(*block);
	}
};
