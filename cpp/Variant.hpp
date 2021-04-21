#pragma once

#include "_XUtility.hpp"
#include "Utility.hpp"
#include "Result.hpp"

namespace hsd
{
    // class monostate
    struct monostate {};

    constexpr bool operator==(monostate, monostate) noexcept
    {
        return true;
    }

    constexpr bool operator<(monostate, monostate) noexcept
    {
        return false;
    }

    // class in_place_index
    template <usize _Idx>
    struct in_place_index_t
    {};

    template <usize _Idx>
    constexpr inline in_place_index_t<_Idx> in_place_index{};

    namespace variant_detail
    {
        //// helper classes
        // class template _value_holder
        template <typename _Ty = void>
        struct _value_holder
        {
            _Ty value;
        };

        template <>
        struct _value_holder<void> {};

        // class template variant_index
        template <typename _Ty, usize _Idx, typename _Tfirst, typename... _Trest>
        struct _variant_index_helper : _variant_index_helper<_Ty, _Idx + 1, _Trest...> {};

        template <typename _Ty, usize _Idx, typename... _Ts>
        struct _variant_index_helper<_Ty, _Idx, _Ty, _Ts...> : literal_constant<usize, _Idx> {};

        template <typename _Ty, typename... _Ts>
        struct _variant_index : _variant_index_helper<_Ty, 0, _Ts...> {};

        // class template _select_overload_for
        template <typename... _Ts>
        struct _select_overload_for
        {
            constexpr static void get();
        };

        template <typename _Tfirst, typename... _Trest>
        struct _select_overload_for<_Tfirst, _Trest...> : _select_overload_for<_Trest...>
        {
            using _select_overload_for<_Trest...>::get;
            constexpr static _Tfirst get(_Tfirst);
        };
    } // namespace variant_detail

    // class bad_variant_access
    struct bad_variant_access
    {
        const char* operator()() const 
        {
            return "A variant was accessed "
                "with the wrong stored type";
        }
    };

    // forward declarations
    template <typename _Ty>
    struct variant_size;

    template <usize _Idx, typename _Ty>
    struct variant_alternative;

    template <usize _Idx, typename... _Ts>
    using variant_alternative_t = typename variant_alternative<_Idx, _Ts...>::type;

    // class template variant
    template <typename... _Ts>
    struct variant
    {
        static_assert(sizeof...(_Ts) != 0, "empty variant is not allowed");
    };

    namespace variant_detail
    {
        template <typename _Ty, usize _Idx>
        struct index_tagged
        {
            using type = _Ty;
            using basic_type = remove_reference_t<_Ty>;
            static constexpr usize index = _Idx;
            _Ty val;
        };

        template <usize _Val>
        using index_constant = literal_constant<usize, _Val>;

        template <typename _Stor>
        struct variant_storage_traits;

        template <typename... _Ts>
        struct variant_storage {};

        template <typename _Tfirst, typename... _Trest>
        struct variant_storage<_Tfirst, _Trest...>
        {
            union
            {
                _Tfirst value_first;
                variant_storage<_Trest...> storage_rest;
            };

            using RestTraits = variant_storage_traits<variant_storage<_Trest...>>;

            // Empty constructor
            constexpr variant_storage() {}
            // TODO: Constexpr
            constexpr ~variant_storage() {}

            // Forwarding constructors
            template <usize _Idx, typename... _Ts>
            constexpr variant_storage(index_constant<_Idx>, _Ts&&... val)
                : storage_rest(index_constant<_Idx - 1>(), forward<_Ts>(val)...)
            {}

            template <typename... _Ts>
            constexpr variant_storage(index_constant<0>, _Ts&&... val)
                : value_first(forward<_Ts>(val)...) 
            {}

            variant_storage(variant_storage const&) = default;
            variant_storage(variant_storage&&) = default;
            variant_storage& operator=(variant_storage const&) = default;
            variant_storage& operator=(variant_storage&&) = default;
        };

        template <typename... _Ts>
        struct variant_storage_traits<variant_storage<_Ts...>>
        {
            using Storage = variant_storage<_Ts...>;
            static constexpr bool NotEmpty()
            {
                return false;
            }
        };

        template <typename _Tfirst, typename... _Trest>
        struct variant_storage_traits<variant_storage<_Tfirst, _Trest...>>
        {
            using Storage = variant_storage<_Tfirst, _Trest...>;
            constexpr static bool NotEmpty()
            {
                return true;
            }

            // Out-of-place constructor
            constexpr static void construct_copy(Storage& l, Storage const& r, usize& li, usize ri)
            {
                visit(l, li = ri, [&](auto val) {
                    _construct_inplace(val.val, get_impl<decltype(val)::index>(r));
                });
            }

            constexpr static void construct_move(Storage& l, Storage&& r, usize& li, usize ri)
            {
                visit(l, li = ri, [&](auto val) {
                    _construct_inplace(val.val, move(get_mut_impl<decltype(val)::index>(r)));
                });
            }

            template <usize _Idx, typename _Ty>
            constexpr static void construct_fwd(Storage& l, _Ty&& value)
            {
                _construct_inplace(l, index_constant<_Idx>(), forward<_Ty>(value));
            }

            // Destructor
            static void destroy(Storage& s, usize id)
            {
                visit(s, id, [](auto val) {
                    _destroy_inplace(val.val);
                });
            }

            template <usize _TheIdx>
            constexpr static auto& get_impl(Storage const& storage)
            {
                if constexpr (_TheIdx == 0)
                {
                    return storage.value_first;
                }
                else
                {
                    return Storage::RestTraits::template get_impl<_TheIdx - 1>(storage.storage_rest);
                }
            }

            template <usize _TheIdx>
            constexpr static auto& get_mut_impl(Storage& storage)
            {
                if constexpr (_TheIdx == 0)
                {
                    return storage.value_first;
                }
                else
                {
                    return Storage::RestTraits::template get_mut_impl<_TheIdx - 1>(storage.storage_rest);
                }
            }

            constexpr static bool equals_impl(usize id, Storage const& a, Storage const& b)
            {
                return visit(const_cast<Storage&>(a), id, [&](auto const val) -> bool 
                {
                    return val.val == get_impl<decltype(val)::index>(b);
                });
            }

            constexpr static void assign_copy(Storage& l, Storage const& r, usize& il, usize ir)
            {
                if (il != ir)
                {
                    destroy(l, il);
                    construct_copy(l, r, il, ir);
                }
                else
                {
                    visit(l, ir, [&](auto val) {
                        val.val = get_impl<decltype(val)::index>(r);
                    });
                }
            }

            constexpr static void assign_move(Storage& l, Storage&& r, usize& il, usize ir)
            {
                if (il != ir)
                {
                    destroy(l, il);
                    construct_move(l, move(r), il, ir);
                }
                else
                {
                    visit(l, ir, [&](auto val) {
                        val.val = move(get_mut_impl<decltype(val)::index>(r));
                    });
                }
            }

            template <usize _Idx, typename _Ty>
            constexpr static void assign_fwd_current(Storage& l, _Ty&& value)
            {
                if constexpr (_Idx == 0)
                {
                    l.value_first = forward<_Ty>(value);
                }
                else if constexpr (Storage::RestTraits::NotEmpty())
                {
                    Storage::RestTraits::template assign_fwd_current<_Idx - 1>(l.storage_rest, forward<_Ty>(value));
                }
            }

            template <usize _Idx, typename _Ty>
            constexpr static void assign_fwd(Storage& l, usize& il, _Ty&& value)
            {
                if (_Idx == il)
                {
                    assign_fwd_current<_Idx>(l, forward<_Ty>(value));
                }
                else
                {
                    destroy(l, il);
                    il = _Idx;
                    construct_fwd<_Idx>(l, forward<_Ty>(value));
                }
            }

            template < typename _Func, usize _Idx = 0 >
            constexpr static auto visit(Storage& s, usize id, _Func&& func, index_constant<_Idx> = index_constant<_Idx>())
            {
                if (id == 0)
                {
                    return forward<_Func>(func)(index_tagged<_Tfirst &, _Idx>{s.value_first});
                }
                else if constexpr (Storage::RestTraits::NotEmpty())
                {
                    return Storage::RestTraits::visit(s.storage_rest, id - 1, forward<_Func>(func), index_constant<_Idx + 1>());
                }
                else
                {
                    return forward<_Func>(func)(index_tagged<_Tfirst&, _Idx>{s.value_first});
                }
            }
        };

        namespace bases
        {
            template <typename _Tfirst, typename... _Trest>
            struct variant_base
            {
                usize _StoredIndex;
                variant_storage<_Tfirst, _Trest...> _Value;
                using _StorageTraits = variant_storage_traits<variant_storage<_Tfirst, _Trest...>>;
                constexpr variant_storage<_Tfirst, _Trest...>& _storage()
                {
                    return _Value;
                }

                constexpr variant_storage<_Tfirst, _Trest...> const& _storage() const
                {
                    return _Value;
                }

                variant_base() {}

                constexpr variant_base(variant_base const&) = default;
                constexpr variant_base(variant_base&&) = default;
                constexpr variant_base& operator=(variant_base const&) = default;
                constexpr variant_base& operator=(variant_base&&) = default;

                template <usize _Idx, typename... _Args>
                constexpr variant_base(in_place_index_t<_Idx>, _Args&&... args)
                    : _StoredIndex(_Idx), _Value(index_constant<_Idx>(), forward<_Args>(args)...) 
                {}

                constexpr static void _construct_copy(variant_base& l, variant_base const& r)
                {
                    _StorageTraits::construct_copy(l._storage(), r._storage(), l._StoredIndex, r._StoredIndex);
                }

                constexpr static void _construct_move(variant_base& l, variant_base&& r)
                {
                    _StorageTraits::construct_move(l._storage(), move(r._storage()), l._StoredIndex, r._StoredIndex);
                }

                constexpr static void _assign_copy(variant_base& l, variant_base const& r)
                {
                    _StorageTraits::assign_copy(l._storage(), r._storage(), l._StoredIndex, r._StoredIndex);
                }

                constexpr static void _assign_move(variant_base& l, variant_base&& r)
                {
                    _StorageTraits::assign_move(l._storage(), move(r._storage()), l._StoredIndex, r._StoredIndex);
                }
            };

            template <typename... _Ts>
            struct csmv_base : variant_base<_Ts...>
            {
                using _Base = variant_base<_Ts...>;
                using _Base::_Base;
                constexpr csmv_base() = default;

                constexpr csmv_base(csmv_base&& other)
                {
                    _Base::_construct_move(*this, move(other));
                }

                constexpr csmv_base(csmv_base const&) = default;
                constexpr csmv_base& operator=(csmv_base&&) = default;
                constexpr csmv_base& operator=(csmv_base const&) = default;
            };
            template <typename... _Ts>
            using csmv_for = conditional_t<conjunction< std::is_trivially_move_constructible<_Ts>...>::value,
                                        variant_base<_Ts...>, csmv_base<_Ts...> >;

            template <typename... _Ts>
            struct cscp_base : csmv_for<_Ts...>
            {
                using _Base = csmv_for<_Ts...>;
                using _Base::_Base;
                constexpr cscp_base() = default;
                constexpr cscp_base(cscp_base&&) = default;

                constexpr cscp_base(cscp_base const& other)
                {
                    _Base::_construct_copy(*this, other);
                }

                constexpr cscp_base& operator=(cscp_base&&) = default;
                constexpr cscp_base& operator=(cscp_base const&) = default;
            };
            template <typename... _Ts>
            using cscp_for = conditional_t< conjunction< std::is_trivially_copy_constructible<_Ts>...>::value,
                                        csmv_for<_Ts...>, cscp_base<_Ts...> >;

            template <typename... _Ts>
            struct asmv_base : cscp_for<_Ts...>
            {
                using _Base = cscp_for<_Ts...>;
                using _Base::_Base;
                constexpr asmv_base() = default;
                constexpr asmv_base(asmv_base&&) = default;
                constexpr asmv_base(asmv_base const&) = default;

                constexpr asmv_base& operator=(asmv_base&& rhs)
                {
                    _Base::_assign_move(*this, move(rhs));
                    return *this;
                }

                constexpr asmv_base& operator=(asmv_base const&) = default;
            };
            template <typename... _Ts>
            using asmv_for = conditional< conjunction< std::is_trivially_move_assignable<_Ts>... >::value,
                                        cscp_for<_Ts...>, asmv_base<_Ts...> >::type;

            template <typename... _Ts>
            struct ascp_base : asmv_for<_Ts...>
            {
                using _Base = asmv_for<_Ts...>;
                using _Base::_Base;
                constexpr ascp_base() = default;
                constexpr ascp_base(ascp_base&&) = default;
                constexpr ascp_base(ascp_base const&) = default;
                constexpr ascp_base& operator=(ascp_base&& rhs) = default;

                constexpr ascp_base& operator=(ascp_base const& rhs)
                {
                    _Base::_assign_copy(*this, rhs);
                    return *this;
                }
            };
            template <typename... _Ts>
            using ascp_for = conditional_t< conjunction< std::is_trivially_copy_assignable<_Ts>... >::value,
                                        asmv_for<_Ts...>, ascp_base<_Ts...> >;
        } // namespace bases
    } // namespace variant_detail

    template <typename _Tfirst, typename... _Trest>
    class variant<_Tfirst, _Trest...>
        : private variant_detail::bases::ascp_for<_Tfirst, _Trest...>
    {
        using _Base = variant_detail::bases::ascp_for<_Tfirst, _Trest...>;
        using _StorageTraits = typename _Base::_StorageTraits;

        template <typename _Func>
        constexpr auto _visit(_Func&& func)
        {
            return _StorageTraits::visit(this->_storage(), this->_StoredIndex, forward<_Func>(func));
        }

        void _destroy()
        {
            _StorageTraits::destroy(this->_storage(), this->_StoredIndex);
        }

        // get the type based on call
        template <typename _Ty>
        struct _type_for
        {
            using type = decltype(variant_detail::_select_overload_for<_Tfirst, _Trest...>::get(declval<_Ty>()));
        };

    public:
        constexpr variant() : _Base(in_place_index<0>) {}

        constexpr variant(variant const& other) = default;
        constexpr variant(variant&& other) = default;

        template <typename _Tother, typename = enable_if_t<!is_same< variant, decay_t<_Tother> >::value >,
                  usize _Idx = variant_detail::_variant_index<typename _type_for<_Tother>::type, _Tfirst, _Trest...>::value>
        constexpr variant(_Tother&& val)
            : _Base(in_place_index<_Idx>, forward<_Tother>(val)) 
        {}

        ~variant()
        {
            _destroy();
        }

        constexpr variant& operator=(variant const& rhs) = default;
        constexpr variant& operator=(variant&& rhs) = default;

        template < typename _Tother, typename = enable_if_t< !is_same< variant, decay_t<_Tother> >::value > >
        constexpr variant& operator=(_Tother&& rhs)
        {
            _Base::_StorageTraits::template assign_fwd<
                variant_detail::_variant_index<typename _type_for<_Tother>::type, _Tfirst, _Trest...>::value
            >(this->_storage(), _Base::_StoredIndex, forward<_Tother>(rhs));
            return *this;
        }

        constexpr usize index() const noexcept
        {
            return _Base::_StoredIndex;
        }

        template <usize _Idx>
        constexpr auto get() -> Result<reference<variant_alternative_t<_Idx, variant>>, bad_variant_access>
        {
            if (index() != _Idx)
                return bad_variant_access{};

            return {_StorageTraits::template get_mut_impl<_Idx>(this->_storage())};
        }

        template <usize _Idx>
        constexpr auto get() const -> Result<reference<const variant_alternative_t<_Idx, variant>>, bad_variant_access>
        {
            if (index() != _Idx)
                return bad_variant_access{};

            return {_StorageTraits::template get_impl<_Idx>(this->_storage())};
        }

        template <typename _Ty>
        constexpr auto get()
        {
            return get<variant_detail::_variant_index<_Ty, _Tfirst, _Trest...>::value>();
        }

        template <typename _Ty>
        constexpr auto get() const
        {
            return get<variant_detail::_variant_index<_Ty, _Tfirst, _Trest...>::value>();
        }

        template <usize _Idx>
        constexpr variant_alternative_t<_Idx, variant>* get_if() noexcept
        {
            if (index() != _Idx)
                return nullptr;

            return &_StorageTraits::template get_mut_impl<_Idx>(this->_storage());
        }

        template <usize _Idx>
        constexpr variant_alternative_t<_Idx, variant> const* get_if() const noexcept
        {
            if (index() != _Idx)
                return nullptr;
            return &_StorageTraits::template get_impl<_Idx>(this->_storage());
        }

        template <typename _Ty>
        constexpr _Ty* get_if() noexcept
        {
            return get_if<variant_detail::_variant_index< _Ty, _Tfirst, _Trest... >::value>();
        }

        template <typename _Ty>
        constexpr _Ty const* get_if() const noexcept
        {
            return get_if<variant_detail::_variant_index< _Ty, _Tfirst, _Trest... >::value>();
        }

        // requires _Ty is a variant alternative
        template <typename _Ty>
        constexpr _Ty& emplace_one(_Ty&& value)
        {
            _destroy();
            constexpr usize _Idx = variant_detail::_variant_index<
                remove_cv_t<remove_reference<_Ty>>, _Tfirst, _Trest... >::value;
            this->_StoredIndex = _Idx;
            _StorageTraits::construct_fwd<_Idx>(this->_storage(), forward<_Ty>(value));
        }

        template <typename _Ty>
        constexpr bool holds_alternative() const noexcept
        {
            return index() == variant_detail::_variant_index< _Ty, _Tfirst, _Trest... >::value;
        }

        friend constexpr bool operator==(variant const& a, variant const& b)
        {
            if (a.index() != b.index())
                return false;

            return _StorageTraits::equals_impl(a.index(), a._Value, b._Value);
        }

        friend constexpr bool operator!=(variant const& a, variant const& b)
        {
            return !(a == b);
        }

        //// function template visit
        template <typename _Visitor>
        constexpr auto visit(_Visitor&& vis)
        {
            return _visit([&](auto val) {
                return forward<_Visitor>(vis)(val.val);
            });
        }
    };

    // class template variant_size
    template <typename... _Ts>
    struct variant_size<variant<_Ts...>>
        : literal_constant<usize, sizeof...(_Ts)>
    {};

    // class template variant_alternative
    namespace variant_detail
    {
        template < usize _Idx, typename _Tfirst, typename... _Trest >
        struct variant_alternative_helper
            : variant_alternative_helper< _Idx - 1, _Trest... >
        {
        };

        template < typename _Tfirst, typename... _Trest >
        struct variant_alternative_helper< 0, _Tfirst, _Trest... >
        {
            using type = _Tfirst;
        };
    } // namespace variant_detail

    template < usize _Idx, typename... _Ts >
    struct variant_alternative< _Idx, variant<_Ts...> >
        : variant_detail::variant_alternative_helper<_Idx, _Ts...>
    {};

    //// Utility functions
    // function template get
    template < usize _Idx, typename... _Ts >
    constexpr variant_alternative_t< _Idx, variant<_Ts...> >& get(variant<_Ts...>& v)
    {
        return v.template get<_Idx>();
    }

    template < usize _Idx, typename... _Ts >
    constexpr variant_alternative_t< _Idx, variant<_Ts...> > const& get(variant<_Ts...> const& v)
    {
        return v.template get<_Idx>();
    }

    template < typename _Ty, typename... _Ts >
    constexpr _Ty& get(variant<_Ts...>& v)
    {
        return v.template get<_Ty>();
    }

    template < typename _Ty, typename... _Ts >
    constexpr _Ty const& get(variant<_Ts...> const& v)
    {
        return v.template get<_Ty>();
    }

    // function template get_if
    template < usize _Idx, typename... _Ts >
    constexpr variant_alternative_t< _Idx, variant<_Ts...> >* get_if(variant<_Ts...>* v) noexcept
    {
        if (v == nullptr)
            return nullptr;

        return v->template get_if<_Idx>();
    }

    template < usize _Idx, typename... _Ts >
    constexpr variant_alternative_t< _Idx, variant<_Ts...> > const* get_if(variant<_Ts...> const* v) noexcept
    {
        if (v == nullptr)
            return nullptr;

        return v->template get_if<_Idx>();
    }

    template < typename _Ty, typename... _Ts >
    constexpr _Ty* get_if(variant<_Ts...>* v) noexcept
    {
        if (v == nullptr)
            return nullptr;

        return v->template get_if<_Ty>();
    }

    template < typename _Ty, typename... _Ts >
    constexpr _Ty const* get_if(variant<_Ts...> const* v) noexcept
    {
        if (v == nullptr)
            return nullptr;

        return v->template get_if<_Ty>();
    }

    // function template holds_alternative
    template < typename _Ty, typename... _Ts >
    constexpr bool holds_alternative(variant<_Ts...> const& v) noexcept
    {
        return v.template holds_alternative<_Ty>();
    }

} // end namespace hsd
