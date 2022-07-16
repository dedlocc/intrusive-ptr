#include <atomic>
#include <concepts>

template <typename T>
struct intrusive_ref_counter
{
    intrusive_ref_counter() noexcept = default;
    intrusive_ref_counter(intrusive_ref_counter const &v) noexcept
    {}

    intrusive_ref_counter &operator=(intrusive_ref_counter const &) noexcept
    {
        return *this;
    }

    std::size_t use_count() const noexcept
    {
        return _counter;
    }

    friend void intrusive_ptr_add_ref(intrusive_ref_counter const *p) noexcept
    {
        p->_counter.fetch_add(1, std::memory_order_relaxed);
    }

    friend void intrusive_ptr_release(intrusive_ref_counter const *p) noexcept
    {
        if (p->_counter.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete static_cast<const T*>(p);
        }
    }

protected:
    ~intrusive_ref_counter() = default;

private:
    mutable std::atomic_size_t _counter = 0;
};

template <typename T>
struct intrusive_ptr
{
    using element_type = T;

    intrusive_ptr() noexcept = default;

    intrusive_ptr(element_type *ptr, bool add_ref = true) : ptr(ptr)
    {
        if (ptr && add_ref) {
            intrusive_ptr_add_ref(ptr);
        }
    }

    intrusive_ptr(intrusive_ptr const &r) : intrusive_ptr(r.get())
    {}

    template <class Y>
    requires std::convertible_to<Y *, T *>
    intrusive_ptr(intrusive_ptr<Y> const &r) : intrusive_ptr(r.get())
    {}

    intrusive_ptr(intrusive_ptr &&r) : ptr(std::exchange(r.ptr, nullptr))
    {}

    template <class Y>
    intrusive_ptr(intrusive_ptr<Y> &&r) : ptr(std::exchange(r.ptr, nullptr))
    {}

    ~intrusive_ptr()
    {
        if (ptr) {
            intrusive_ptr_release(ptr);
        }
    }

    intrusive_ptr &operator=(intrusive_ptr const &r)
    {
        return operator=<T>(r);
    }

    template <class Y>
    requires std::convertible_to<Y *, T *>
    intrusive_ptr &operator=(intrusive_ptr<Y> const &r)
    {
        return operator=(r.get());
    }

    intrusive_ptr &operator=(element_type *r)
    {
        intrusive_ptr(r).swap(*this);
        return *this;
    }

    intrusive_ptr &operator=(intrusive_ptr &&r)
    {
        return operator=<T>(std::move(r));
    }

    template <class Y>
    requires std::convertible_to<Y *, T *>
    intrusive_ptr &operator=(intrusive_ptr<Y> &&r)
    {
        intrusive_ptr(std::move(r)).swap(*this);
        return *this;
    }

    void reset()
    {
        intrusive_ptr().swap(*this);
    }

    void reset(element_type *r, bool add_ref = true)
    {
        intrusive_ptr(r, add_ref).swap(*this);
    }

    element_type &operator*() const noexcept
    {
        return *get();
    }

    element_type *operator->() const noexcept
    {
        return get();
    }

    element_type *get() const noexcept
    {
        return ptr;
    }

    element_type *detach() noexcept
    {
        return std::exchange(ptr, nullptr);
    }

    explicit operator bool() const noexcept
    {
        return get() != 0;
    }

    void swap(intrusive_ptr &b) noexcept
    {
        std::swap(ptr, b.ptr);
    }

private:
    element_type *ptr = nullptr;

    template <typename Y>
    friend struct intrusive_ptr;
};

template <class T, class U>
bool operator==(intrusive_ptr<T> const &a, intrusive_ptr<U> const &b) noexcept
{
    return a.get() == b.get();
}

template <class T, class U>
bool operator!=(intrusive_ptr<T> const &a, intrusive_ptr<U> const &b) noexcept
{
    return a.get() != b.get();
}

template <class T, class U>
bool operator==(intrusive_ptr<T> const &a, U *b) noexcept
{
    return a.get() == b;
}

template <class T, class U>
bool operator!=(intrusive_ptr<T> const &a, U *b) noexcept
{
    return a.get() != b;
}

template <class T, class U>
bool operator==(T *a, intrusive_ptr<U> const &b) noexcept
{
    return a == b.get();
}

template <class T, class U>
bool operator!=(T *a, intrusive_ptr<U> const &b) noexcept
{
    return a != b.get();
}

template <class T>
bool operator<(intrusive_ptr<T> const &a, intrusive_ptr<T> const &b) noexcept
{
    return std::less(a.get(), b.get());
}

template <class T>
void swap(intrusive_ptr<T> &a, intrusive_ptr<T> &b) noexcept
{
    a.swap(b);
}
