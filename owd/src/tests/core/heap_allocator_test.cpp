#include "catch2/catch.hpp"
#include "core/heap_allocator.h"

using namespace owd;

TEST_CASE("Heap allocator must allocate memory on the heap", "[alloc/free]") {
    core::HeapAllocator allocator;

    SECTION("use allocators method directly") {
        char* ptr = (char*)allocator.alloc(100);
        REQUIRE(ptr != NULL);
        allocator.free(ptr);
    }
    SECTION("use overriden new operator") {
        char* ptr = new(allocator) char;
        REQUIRE(ptr != NULL);
        allocator.free(ptr);
    }
}
