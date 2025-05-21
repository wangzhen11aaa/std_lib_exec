#include <atomic>
#include <cassert>
#include <iostream>

template <typename _T> class my_shared_ptr;

template <typename _T> class my_shared_ptr_access {
public:
  using element_type = _T;
  element_type &operator*() const noexcept { return *_M_get(); }
  element_type *operator->() const noexcept { return _M_get(); }

private:
  element_type *_M_get() const noexcept {
    return static_cast<const my_shared_ptr<_T> *>(this)->get();
  }
};

template <typename _T> class my_shared_ptr : public my_shared_ptr_access<_T> {
public:
  using element_type = _T;
  my_shared_ptr() noexcept : _ptr{nullptr}, _ctr_ptr{nullptr} {}
  my_shared_ptr(_T *ptr);

  my_shared_ptr(const my_shared_ptr<_T> &other);
  my_shared_ptr(my_shared_ptr<_T> &&other);

  my_shared_ptr &operator=(const my_shared_ptr<_T> &);
  my_shared_ptr &operator=(my_shared_ptr &&rvlue);

  void swap(my_shared_ptr<_T> &other) {
    std::swap(_ptr, other._ptr);
    std::swap(_ctr_ptr, other._ctr_ptr);
  }

  void reset() {
    if (_ctr_ptr || _ptr) {
      my_shared_ptr().swap(*this);
    }
  }

  ~my_shared_ptr() noexcept {
    std::cout << "~my_shared_ptr() is invoked \n" << std::endl;
    if (_ctr_ptr && --_ctr_ptr->_count == 0) {
      delete _ptr;
      delete _ctr_ptr;
    }
  };

  long use_count() {
    { return _ctr_ptr ? _ctr_ptr->_count.load(std::memory_order_relaxed) : 0; }
  }

  element_type *get() const { return _ptr; }

private:
  class _control_block {
  public:
    _control_block() : _count{0} {}
    _control_block(const _control_block &) = default;
    _control_block &operator=(const _control_block &) = default;

    std::atomic<int> _count;
  };
  _T *_ptr{nullptr};
  _control_block *_ctr_ptr{nullptr};
};

template <typename _T> my_shared_ptr<_T>::my_shared_ptr(_T *ptr) {
  _ctr_ptr = new _control_block{};
  _ptr = ptr;
  _ctr_ptr->_count = 1;
}

template <typename _T>
my_shared_ptr<_T>::my_shared_ptr(const my_shared_ptr<_T> &other) {
  other._ctr_ptr->_count++;
  _ptr = other._ptr;
  _ctr_ptr = other._ctr_ptr;
}

template <typename _T>
my_shared_ptr<_T>::my_shared_ptr(my_shared_ptr<_T> &&other)
    : _ptr(other._ptr), _ctr_ptr{nullptr} {
  std::swap(_ctr_ptr, other._ctr_ptr);
}

template <typename _T>
my_shared_ptr<_T> &
my_shared_ptr<_T>::operator=(const my_shared_ptr<_T> &other) {
  if (_ptr && --_ctr_ptr->_count == 0) {
    delete _ptr;
    delete _ctr_ptr;
  }
  ++other._ctr_ptr->_count;
  _ptr = other._ptr;
  _ctr_ptr = other._ctr_ptr;
  return *this;
}

template <typename _T>
my_shared_ptr<_T> &my_shared_ptr<_T>::operator=(my_shared_ptr<_T> &&other) {
  std::swap(_ctr_ptr, other._ctr_ptr);
  std::swap(_ptr, other._ptr);
  return *this;
}

int main() {
  my_shared_ptr<int> mptr;
  my_shared_ptr<int> mptr0{new int{1}};
  std::cout << mptr0.use_count() << std::endl;
  mptr = mptr0;
  std::cout << mptr.use_count() << std::endl;
  {
    my_shared_ptr<int> mptr2 = mptr;
    std::cout << mptr.use_count() << std::endl;
    my_shared_ptr<int> mptr3 = std::move(mptr2);
    std::cout << mptr.use_count() << std::endl;
  }

  std::cout << "access member: " << *mptr << std::endl;
  std::cout << "mptr.use_count(): " << mptr.use_count() << std::endl;
  std::cout << "mptr0.use_count(): " << mptr0.use_count() << std::endl;
  mptr.reset();
  mptr0.reset();
  std::cout << "mptr.use_count(): " << mptr.use_count() << std::endl;
  std::cout << "mptr0.use_count(): " << mptr0.use_count() << std::endl;
  std::cout << "End of program" << std::endl;
}