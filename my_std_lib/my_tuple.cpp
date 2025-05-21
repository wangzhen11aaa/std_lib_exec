#include <iostream>
#include <type_traits>

namespace self {
template <size_t _Idx, typename... _Elements> struct _my_tuple_impl;

template <typename _Tp> struct __is_empty_non_tuple : std::is_empty<_Tp> {};

template <typename _Tp>
using __empty_not_final =
    typename std::conditional<__is_final(_Tp), std::false_type,
                              __is_empty_non_tuple<_Tp>>::type;
template <size_t _Idx, typename _Head, bool = __empty_not_final<_Head>::value>
struct _Head_base;

template <size_t _Idx, typename _Head> struct _Head_base<_Idx, _Head, true> {
  constexpr _Head_base() : _M_head_impl() {}
  constexpr _Head_base(const _Head &__h) : _M_head_impl(__h) {}

  constexpr _Head_base(const _Head_base &) = default;
  constexpr _Head_base(_Head_base &&) = default;

  template <typename _UHead>
  constexpr _Head_base(_UHead &&__h)
      : _M_head_impl(std::forward<_UHead>(__h)) {}

  static constexpr _Head &_M_head(_Head_base &__b) noexcept {
    return __b._M_head_impl;
  }

  static constexpr const _Head &_M_head(const _Head_base &__b) noexcept {
    return __b._M_head_impl;
  }

  _Head _M_head_impl;
};

template <size_t _Idx, typename _Head> struct _Head_base<_Idx, _Head, false> {
  constexpr _Head_base() : _M_head_impl() {}
  constexpr _Head_base(const _Head &__h) : _M_head_impl(__h) {}

  constexpr _Head_base(const _Head_base &) = default;
  constexpr _Head_base(_Head_base &&) = default;

  template <typename _UHead>
  constexpr _Head_base(_UHead &&__h)
      : _M_head_impl(std::forward<_UHead>(__h)) {}

  static constexpr _Head &_M_head(_Head_base &__b) noexcept {
    return __b._M_head_impl;
  }

  static constexpr const _Head &_M_head(const _Head_base &__b) noexcept {
    return __b._M_head_impl;
  }

  _Head _M_head_impl;
};

template <size_t _Idx, typename _Head, typename... _Tail>
struct _my_tuple_impl<_Idx, _Head, _Tail...>
    : public _my_tuple_impl<_Idx + 1, _Tail...>,
      private _Head_base<_Idx, _Head> {
  typedef _my_tuple_impl<_Idx + 1, _Tail...> _Inherited;
  typedef _Head_base<_Idx, _Head> _Base;

  static constexpr _Head &_M_Head(_my_tuple_impl &_t) noexcept {
    return _Base::_M_head(_t);
  }

  static constexpr const _Head &_M_Head(const _my_tuple_impl &_t) noexcept {
    return _Base::_M_head(_t);
  }

  _my_tuple_impl(const _Head &_head, const _Tail &..._tail)
      : _Inherited(_tail...), _Base(_head) {}
};

template <size_t _Idx, typename _Head> struct _my_tuple_impl<_Idx, _Head> {
  using type = _Head;

  const type &get() { return _item; }
  _my_tuple_impl(const _Head &_head) : _item(_head) {}
  _Head _item;
};

template <size_t _Np, typename... Types> struct _Nth_type {};

template <typename _Tp0, typename... _Rest>
struct _Nth_type<0, _Tp0, _Rest...> {
  using type = _Tp0;
};

template <typename _Tp0, typename _Tp1, typename... _Rest>
struct _Nth_type<1, _Tp0, _Tp1, _Rest...> {
  using type = _Tp1;
};

template <typename _Tp0, typename _Tp1, typename _Tp2, typename... _Rest>
struct _Nth_type<2, _Tp0, _Tp1, _Tp2, _Rest...> {
  using type = _Tp2;
};

template <size_t _Np, typename _Tp0, typename _Tp1, typename _Tp2,
          typename... _Rest>
struct _Nth_type<_Np, _Tp0, _Tp1, _Tp2, _Rest...>
    : _Nth_type<_Np - 3, _Rest...> {};

template <typename... _Elements>
class my_tuple : public _my_tuple_impl<0, _Elements...> {
public:
  typedef _my_tuple_impl<0, _Elements...> Inherited;
  my_tuple(_Elements... _elements) : Inherited(_elements...) {}
};

template <size_t _i, typename _Head, typename... _Tail>
constexpr const _Head &
__get_helper(const _my_tuple_impl<_i, _Head, _Tail...> &_t) noexcept {
  return _my_tuple_impl<_i, _Head, _Tail...>::_M_Head(_t);
}

template <size_t _i, typename... _Elements>
const typename _my_tuple_impl<_i, _Elements...>::type &
get(const my_tuple<_Elements...> &t) noexcept {
  return __get_helper<_i>(t);
}
} // namespace self

int main() {
  self::my_tuple<int, int, int> t{1, 2, 3};
  std::cout << self::get<1>(t) << std::endl;
}