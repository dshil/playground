#ifndef OWD_CORE_HEAP_ALLOCATOR_H_
#define OWD_CORE_HEAP_ALLOCATOR_H_

#include "stddefs.h"
#include "iallocator.h"

namespace owd {
namespace core {

class HeapAllocator : public IAllocator {
public:
    ~HeapAllocator();
    void *alloc(size_t size) override;
    void free(void *ptr) override;
};

} // namespace core
} // namespace owd

#endif // OWD_CORE_HEAP_ALLOCATOR_H_
