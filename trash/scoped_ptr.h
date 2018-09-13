template <typename T>
struct ScopedPtr
{
    explicit ScopedPtr(T* ptr = 0) : ptr_(ptr) {
    }

    ~ScopedPtr() {
        delete ptr_;
    }

    T* get() const {
        return ptr_;
    }

    T* release() {
        T* ret = ptr_;
        ptr_ = nullptr;
        return ret;
    }

    void reset(T *ptr = 0) {
        delete ptr_;
        ptr_ = ptr;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

private:
    ScopedPtr(const ScopedPtr&);
    ScopedPtr& operator = (const ScopedPtr&);

    T *ptr_;
};
