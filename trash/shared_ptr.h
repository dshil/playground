#include <cstddef>

template <class T>
struct SharedPtr
{
    explicit SharedPtr(T* ptr = 0) {
        reset_(ptr);
    }

    ~SharedPtr() {
        decref_();
    }

    SharedPtr(const SharedPtr& ptr) {
        ptr_ = ptr.get();
        refcnt_ = ptr.refcnt_;
        incref_();
    }

    SharedPtr& operator=(const SharedPtr& ptr) {
        if (this != &ptr) {
            decref_();

            ptr_ = ptr.get();
            refcnt_ = ptr.refcnt_;

            incref_();
        }

        return *this;
    }

    T* get() const {
        return ptr_;
    }

    void reset(T* ptr = 0) {
        decref_();
        reset_(ptr);
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

private:
    void incref_() {
        if (refcnt_)
            ++*refcnt_;
    }

    void decref_() {
        if (!refcnt_)
            return;

        if (--*refcnt_ == 0)
            free_();
    }

    void reset_(T* ptr) {
        ptr_ = ptr;
        refcnt_ = (ptr) ? new size_t(1) : nullptr;
    }

    void free_() {
        delete ptr_;
        delete refcnt_;
    }

    T* ptr_;
    size_t* refcnt_;
};
