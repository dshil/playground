#include "heap_allocator.h"

namespace owd {
namespace core {

HeapAllocator::~HeapAllocator() {
}

void *HeapAllocator::alloc(size_t size) {
    return new char[size];
}

void HeapAllocator::free(void *ptr) {
    delete[] (char *)ptr;
}

} // namespace core
} // namespace owd
