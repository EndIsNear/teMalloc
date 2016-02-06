#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "tmBaseAlloc.hpp"

/////////////////////////////////////////////////
// teAllocBase
/////////////////////////////////////////////////
TEST_CASE("MemoryAllocator")
{
	SECTION("Default alignment")
	{
		const size_t defaultAlignment = 16;
		BaseAlloc<> allocator;
		Block mem = allocator.alloc(64);
		CHECK(mem.ptr != nullptr);
		CHECK(mem.len == 64);
		const int res = reinterpret_cast<int>(mem.ptr) % defaultAlignment;
		CHECK(res == 0);
		allocator.free(mem);
		CHECK(mem.len == 0);
		CHECK(mem.ptr == nullptr);
	}

	SECTION("alligment 32")
	{
		const size_t alignment = 32;
		BaseAlloc<alignment> allocator;
		Block mem = allocator.alloc(64);
		CHECK(mem.ptr != nullptr);
		CHECK(mem.len == 64);
		const int res = reinterpret_cast<int>(mem.ptr) % alignment;
		CHECK(res == 0);
		allocator.free(mem);
		CHECK(mem.len == 0);
		CHECK(mem.ptr == nullptr);
	}
}