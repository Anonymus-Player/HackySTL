#pragma once

#include "Utility.hpp"
#include "Allocator.hpp"
#include "Types.hpp"
#include "_XUtility.hpp"

namespace hsd
{
    namespace unique_detail
    {
        template <typename T, typename U>
        concept ConvertibleDerived = std::is_convertible_v<U, T> || std::is_base_of_v<T, U>;

        template < typename T, template <typename> typename Allocator >
        class storage : private Allocator< remove_array_t<T> >
        {
        private:
            usize _size = 0;
            using alloc_type = Allocator< remove_array_t<T> >;
            using pointer_type = typename alloc_type::pointer_type;
            using value_type = typename alloc_type::value_type;
            
            template <typename U, template <typename> typename Alloc>
            friend class storage;

        public:
            HSD_CONSTEXPR storage()
            requires (std::is_default_constructible_v<Allocator<uchar>>) = default;

            HSD_CONSTEXPR storage(pointer_type ptr, usize size)
            requires (std::is_default_constructible_v<Allocator<uchar>> &&
                is_same<decltype(this->_data), pointer_type>::value)
                : _size{size}
            {
                this->_data = ptr;
            }

            HSD_CONSTEXPR storage(const Allocator<value_type>& alloc, usize size)
            requires (std::is_copy_constructible_v<value_type>)
                : Allocator<value_type>(alloc), _size{size}
            {
                this->_data = this->allocate(size).unwrap();
                alloc_type::construct_at(this->_data);

                for(usize _index = 0; _index < size; _index++)
                {
                    alloc_type::construct_at(&this->_data[_index]);
                }
            }

            HSD_CONSTEXPR storage(pointer_type ptr, 
                const Allocator<value_type>& alloc, usize size)
            requires (std::is_copy_constructible_v<value_type>)
                : Allocator<value_type>(alloc), _size{size}
            {
                this->_data = ptr;
            }

            storage(const storage&) = delete;

            template <typename U = T>
            HSD_CONSTEXPR storage(storage<U, Allocator>&& other)
            requires (std::is_move_constructible_v<Allocator<uchar>>)
                : Allocator<value_type>(move(other))
            {
                this->_data = exchange(other._data, nullptr);
                swap(this->_size, other._size);
            }

            template <typename U = T>
            HSD_CONSTEXPR storage(storage<U, Allocator>&& other)
            requires (!std::is_move_constructible_v<Allocator<uchar>>)
            {
                if constexpr(is_same<pointer_type, value_type*>::value)
                {
                    this->_data = exchange(other._data, nullptr);
                }
                else
                {
                    swap(this->_data, other._data);
                }

                swap(this->_size, other._size);
            }

            template <typename U = T>
            HSD_CONSTEXPR storage& operator=(storage<U, Allocator>&& rhs)
            {
                this->deallocate(this->_data, _size);
                
                if constexpr(is_same<pointer_type, value_type*>::value)
                {
                    this->_data = exchange(rhs._data, nullptr);
                }
                else
                {
                    swap(this->_data, rhs._data);
                }
                
                swap(this->_size, rhs._size);
                return *this;
            }

            constexpr Allocator<value_type>& get_allocator()
            {
                return *this;
            }

            constexpr usize get_size() const
            {
                return _size;
            }

            constexpr auto* get_pointer() const
            {
                if constexpr(is_same<decltype(this->_data), value_type*>::value)
                {
                    return this->_data;
                }
                else if constexpr(requires {this->_data.data();})
                {
                    return this->_data.data();
                }
            }

            constexpr void set_pointer(pointer_type ptr)
            {
                this->_data = ptr;
            }

            constexpr void set_size(usize size)
            {
                _size = size;
            }
        };
    } // namespace unique_detail

    template < typename T, template <typename> typename Allocator = allocator >
    class unique_ptr
    {
    private:
        unique_detail::storage<T, Allocator> _value;

        template <typename U, template <typename> typename Alloc>
        friend class unique_ptr;

        HSD_CONSTEXPR void _delete()
        {
            if (get()) {
                if constexpr (is_array<T>::value)
                {
                    for (usize i = 0, size = _value.get_size(); i < size; ++i)
                        _destroy_inplace(get()[size - i]);
                }
                else
                {
                    _destroy_inplace(*get());
                }
            }
            _value.get_allocator().deallocate(
                _value.get_pointer(), _value.get_size()).unwrap();

            if constexpr(is_same<pointer_type, value_type*>::value)
                _value.set_pointer(nullptr);
        }

    public:
        using alloc_type = Allocator< remove_array_t<T> >;
        using pointer_type = typename alloc_type::pointer_type;
        using value_type = typename alloc_type::value_type;
        using reference_type = typename alloc_type::value_type&;
        
        HSD_CONSTEXPR unique_ptr() = default;
        HSD_CONSTEXPR unique_ptr(NullType) {}
        unique_ptr(unique_ptr&) = delete;
        unique_ptr(const unique_ptr&) = delete;

        HSD_CONSTEXPR unique_ptr(pointer_type ptr) 
        requires (std::is_default_constructible_v<Allocator<uchar>>)
            : _value{ptr, 1u}
        {}

        HSD_CONSTEXPR unique_ptr(pointer_type ptr, usize size) 
        requires (std::is_default_constructible_v<Allocator<uchar>>)
            : _value{ptr, size}
        {}

        HSD_CONSTEXPR unique_ptr(const Allocator<value_type>& alloc) 
            : _value{alloc, 1u}
        {}

        HSD_CONSTEXPR unique_ptr(const Allocator<value_type>& alloc, usize size) 
            : _value{alloc, size}
        {}

        HSD_CONSTEXPR unique_ptr(pointer_type ptr, 
            const Allocator<value_type>& alloc, usize size) 
            : _value{ptr, alloc, size}
        {}

        template <typename U = T> 
        requires(unique_detail::ConvertibleDerived<T, U>)
        HSD_CONSTEXPR unique_ptr(unique_ptr<U, Allocator>&& other)
            : _value{move(other._value)}
        {}

        HSD_CONSTEXPR ~unique_ptr()
        {
            _delete();
        }

        HSD_CONSTEXPR unique_ptr& operator=(NullType)
        {
            _delete();
            return *this;
        }

        template <typename U = T> 
        requires(unique_detail::ConvertibleDerived<T, U>)
        HSD_CONSTEXPR unique_ptr& operator=(unique_ptr<U, Allocator>&& rhs)
        {
            _value = move(rhs._value);
            return *this;
        }

        constexpr auto* get()
        {
            return _value.get_pointer();
        }

        constexpr auto* get() const
        {
            return _value.get_pointer();
        }

        constexpr bool operator!=(NullType) const
        {
            return _value.get_pointer() != nullptr;
        }

        constexpr auto* operator->()
        {
            return get();
        }

        constexpr auto* operator->() const
        {
            return get();
        }

        constexpr auto& operator*()
        {
            return *get();
        }

        constexpr auto& operator*() const
        {
            return *get();
        }
    };

    template < typename T, template <typename> typename Allocator >
    struct MakeUniq
    {
        using single_object = unique_ptr<T, Allocator>;
    };

    template < typename T, template <typename> typename Allocator >
    struct MakeUniq<T[], Allocator>
    {
        using array = unique_ptr<T[], Allocator>;
    };
    
    template < typename T, usize N, template <typename> typename Allocator >
    struct MakeUniq<T[N], Allocator>
    {
        struct invalid_type {};  
    };
    
    template <typename T, template <typename> typename Allocator = allocator, typename... Args>
    requires (std::is_default_constructible_v<Allocator<uchar>> && 
        is_same<typename Allocator<remove_array_t<T>>::pointer_type, remove_array_t<T>*>::value)
    static HSD_CONSTEXPR typename MakeUniq<T, Allocator>::single_object make_unique(Args&&... args)
    {
        Allocator<remove_array_t<T>> _alloc;
        auto _ptr = _alloc.allocate(1).unwrap();
        _alloc.construct_at(_ptr, forward<Args>(args)...);
        return unique_ptr<T, Allocator>(_ptr);
    }

    template <typename T, template <typename> typename Allocator = allocator, typename... Args>
    requires (std::is_default_constructible_v<Allocator<uchar>> && 
        !is_same<typename Allocator<remove_array_t<T>>::pointer_type, remove_array_t<T>*>::value)
    static HSD_CONSTEXPR typename MakeUniq<T, Allocator>::single_object make_unique(Args&&... args)
    {
        Allocator<remove_array_t<T>> _alloc;
        auto& _ptr = _alloc.allocate(1).unwrap();
        _alloc.construct_at(&_ptr[0], forward<Args>(args)...);
        return unique_ptr<T, Allocator>(_ptr);
    }

    template <typename T, template <typename> typename Allocator = allocator, typename U, typename... Args>
    static HSD_CONSTEXPR typename MakeUniq<T, Allocator>::single_object make_unique(Allocator<U>& alloc, Args&&... args)
    requires (!std::is_default_constructible_v<Allocator<uchar>>)
    {
        auto* _ptr = static_cast<Allocator<remove_array_t<T>>>(alloc).allocate(1).unwrap();
        alloc.construct_at(_ptr, forward<Args>(args)...);
        return unique_ptr<T, Allocator>(static_cast<Allocator<remove_array_t<T>>>(alloc), 1);
    }

    template <typename T, template <typename> typename Allocator = allocator>
    static HSD_CONSTEXPR typename MakeUniq<T, Allocator>::array make_unique(usize size)
    requires (std::is_default_constructible_v<Allocator<uchar>>)
    {
        Allocator<remove_array_t<T>> _alloc;
        auto* _ptr = _alloc.allocate(size).unwrap();
        return unique_ptr<T, Allocator>(_ptr, size);
    }

    template <typename T, template <typename> typename Allocator = allocator, typename U>
    static HSD_CONSTEXPR typename MakeUniq<T, Allocator>::array make_unique(Allocator<U>& alloc, usize size)
    {
        return unique_ptr<T, Allocator>(static_cast<Allocator<remove_array_t<T>>>(alloc), size);
    }

    template <typename T, typename U, template <typename> typename Allocator = allocator, typename... Args>
    static HSD_CONSTEXPR typename MakeUniq<T, Allocator>::invalid_type make_unique(const Allocator<U>&, Args&&...) = delete;
} // namespace hsd
