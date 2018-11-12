#ifndef OWD_CORE_IALLOCATOR_H_
#define OWD_CORE_IALLOCATOR_H_

namespace owd {
namespace core {

class IAllocator {
public:
    virtual ~IAllocator() {}
    virtual void* alloc(size_t size) = 0;
    virtual void free(void*) = 0;
};

} // namespace core
} // namespace owd

inline void* operator new(size_t size, owd::core::IAllocator& allocator) throw() {
    return allocator.alloc(size);
}

inline void* operator new[](size_t size, owd::core::IAllocator& allocator) throw() {
    return allocator.alloc(size);
}

template <class T>
inline void operator delete(void* ptr, owd::core::IAllocator& allocator) throw() {
    allocator.free(ptr);
}

template <class T>
inline void operator delete[](void* ptr, owd::core::IAllocator& allocator) throw() {
    allocator.free(ptr);
}

#endif // OWD_CORE_IALLOCATOR_H_
