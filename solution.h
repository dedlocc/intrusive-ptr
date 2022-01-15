template<typename T>
struct intrusive_ptr {
  using element_type = T;

  intrusive_ptr() noexcept;
  intrusive_ptr(T * p, bool add_ref = true);

  intrusive_ptr(intrusive_ptr const & r);
  template<class Y> intrusive_ptr(intrusive_ptr<Y> const & r);

  intrusive_ptr(intrusive_ptr && r);
  template<class Y> intrusive_ptr(intrusive_ptr<Y> && r);

  ~intrusive_ptr();

  intrusive_ptr & operator=(intrusive_ptr const & r);
  template<class Y> intrusive_ptr & operator=(intrusive_ptr<Y> const & r);
  intrusive_ptr & operator=(T * r);

  intrusive_ptr & operator=(intrusive_ptr && r);
  template<class Y> intrusive_ptr & operator=(intrusive_ptr<Y> && r);

  void reset();
  void reset(T * r);
  void reset(T * r, bool add_ref);

  T & operator*() const noexcept;
  T * operator->() const noexcept;
  T * get() const noexcept;
  T * detach() noexcept;

  explicit operator bool () const noexcept;

  void swap(intrusive_ptr & b) noexcept;
};

template<class T, class U>
bool operator==(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b) noexcept;

template<class T, class U>
bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b) noexcept;

template<class T, class U>
bool operator==(intrusive_ptr<T> const & a, U * b) noexcept;

template<class T, class U>
bool operator!=(intrusive_ptr<T> const & a, U * b) noexcept;

template<class T, class U>
bool operator==(T * a, intrusive_ptr<U> const & b) noexcept;

template<class T, class U>
bool operator!=(T * a, intrusive_ptr<U> const & b) noexcept;

template<class T>
bool operator<(intrusive_ptr<T> const & a, intrusive_ptr<T> const & b) noexcept;

template<class T> void swap(intrusive_ptr<T> & a, intrusive_ptr<T> & b) noexcept;

template<typename T>
struct intrusive_ref_counter {
  intrusive_ref_counter() noexcept;
  intrusive_ref_counter(const intrusive_ref_counter& v) noexcept;

  intrusive_ref_counter& operator=(const intrusive_ref_counter& v) noexcept;

  unsigned int use_count() const noexcept;

protected:
  ~intrusive_ref_counter() = default;
};

template<class Derived>
void intrusive_ptr_add_ref(
    const intrusive_ref_counter<Derived>* p) noexcept;

template<class Derived>
void intrusive_ptr_release(
    const intrusive_ref_counter<Derived>* p) noexcept;

