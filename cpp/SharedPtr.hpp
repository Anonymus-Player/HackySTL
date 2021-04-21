#pragma once

#include "Utility.hpp"
#include "_XUtility.hpp"
#include "Allocator.hpp"

namespace hsd
{
    namespace non_atomic_types
    {
        namespace shared_detail
        {
            template <typename T, typename U>
            concept ConvertibleDerived = std::is_convertible_v<U, T> || std::is_base_of_v<T, U>;

            template < typename T, template <typename> typename Allocator >
            class storage : private Allocator<remove_array_t<T>>
            {
            private:
                usize _size = 0;
                using alloc_type = Allocator<remove_array_t<T>>;
                using pointer_type = typename alloc_type::pointer_type;
                using value_type = typename alloc_type::value_type;

                template <typename U, template <typename> typename Alloc>
                friend class storage;

            public:
                HSD_CONSTEXPR storage()
                requires (std::is_default_constructible_v<Allocator<uchar>>) = default;

                HSD_CONSTEXPR storage(pointer_type ptr, usize size)
                requires (std::is_default_constructible_v<Allocator<uchar>>)
                    : _size{size}
                {
                    this->_data = ptr;
                }

                HSD_CONSTEXPR storage(const Allocator<value_type>& alloc, usize size)
                requires (std::is_copy_constructible_v<value_type>)
                    : Allocator<value_type>(alloc), _size{size}
                {
                    this->_data = this->allocate(size);
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

                HSD_CONSTEXPR storage(const storage& other)
                requires (std::is_copy_constructible_v<Allocator<value_type>>)
                    : Allocator<value_type>(other)
                {
                    this->_data = other._data;
                    this->_size = other._size;
                }

                HSD_CONSTEXPR storage(const storage& other)
                requires (!std::is_copy_constructible_v<Allocator<value_type>>)
                {
                    this->_data = other._data;
                    this->_size = other._size;
                }

                HSD_CONSTEXPR storage(storage&& other)
                requires (std::is_move_constructible_v<Allocator<value_type>>)
                    : Allocator<value_type>(move(other))
                {
                    this->_data = exchange(other._data, nullptr);
                    swap(this->_size, other._size);
                }

                HSD_CONSTEXPR storage(storage&& other)
                requires (!std::is_move_constructible_v<Allocator<value_type>>)
                {
                    this->_data = exchange(other._data, nullptr);
                    swap(this->_size, other._size);
                }

                template <typename U = T>
                HSD_CONSTEXPR storage(const storage<U, Allocator>& other)
                requires (std::is_copy_constructible_v<Allocator<value_type>>)
                    : Allocator<value_type>(other)
                {
                    this->_data = other._data;
                    this->_size = other._size;
                }

                template <typename U = T>
                HSD_CONSTEXPR storage(const storage<U, Allocator>& other)
                requires (!std::is_copy_constructible_v<Allocator<value_type>>)
                {
                    this->_data = other._data;
                    this->_size = other._size;
                }

                template <typename U = T>
                HSD_CONSTEXPR storage(storage<U, Allocator>&& other)
                requires (std::is_move_constructible_v<Allocator<value_type>>)
                    : Allocator<value_type>(move(other))
                {
                    this->_data = exchange(other._data, nullptr);
                    swap(this->_size, other._size);
                }

                template <typename U = T>
                HSD_CONSTEXPR storage(storage<U, Allocator>&& other)
                requires (!std::is_move_constructible_v<Allocator<value_type>>)
                {
                    this->_data = exchange(other._data, nullptr);
                    swap(this->_size, other._size);
                }

                HSD_CONSTEXPR storage& operator=(const storage& rhs)
                {
                    this->_data = rhs._data;
                    this->_size = rhs._size;
                    return *this;
                }

                HSD_CONSTEXPR storage& operator=(storage&& rhs)
                {
                    this->_data = exchange(rhs._data, nullptr);                    
                    swap(this->_size, rhs._size);
                    return *this;
                }

                template <typename U = T>
                HSD_CONSTEXPR storage& operator=(const storage<U, Allocator>& rhs)
                {
                    this->_data = rhs._data;
                    this->_size = rhs._size;
                    return *this;
                }

                template <typename U = T>
                HSD_CONSTEXPR storage& operator=(storage<U, Allocator>&& rhs)
                {
                    this->_data = exchange(rhs._data, nullptr);                    
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

            template < template <typename> typename Allocator >
            class counter
                : private Allocator<usize>
            {
            private:
                using alloc_type = Allocator<usize>;
                using pointer_type = typename alloc_type::pointer_type;
                using value_type = typename alloc_type::value_type;

                template < template <typename> typename Alloc >
                friend class counter;                

            public:
                HSD_CONSTEXPR counter()
                requires (std::is_default_constructible_v<Allocator<usize>>)
                {
                    this->_data = this->allocate(1).unwrap();
                    *this->_data = 1;
                }

                HSD_CONSTEXPR counter(const Allocator<usize>& alloc)
                requires (std::is_copy_constructible_v<Allocator<usize>>)
                    : Allocator<usize>(alloc)
                {
                    this->_data = this->allocate(1).unwrap();
                    *this->_data = 1;
                }

                HSD_CONSTEXPR counter(usize* ptr)
                requires (std::is_default_constructible_v<Allocator<usize>>)
                {
                    this->_data = ptr;
                    (*this->_data)++;
                }

                HSD_CONSTEXPR counter(const Allocator<usize>& alloc, usize* ptr)
                requires (std::is_copy_constructible_v<Allocator<usize>>)
                    : Allocator<usize>(alloc)
                {
                    this->_data = ptr;
                    (*this->_data)++;
                }

                HSD_CONSTEXPR counter(const counter& other)
                requires (std::is_copy_constructible_v<Allocator<usize>>)
                    : Allocator<usize>(other)
                {
                    this->_data = other._data;
                    (*this->_data)++;
                }

                HSD_CONSTEXPR counter(const counter& other)
                requires (!std::is_copy_constructible_v<Allocator<usize>>)
                {
                    this->_data = other._data;
                    (*this->_data)++;
                }

                HSD_CONSTEXPR counter& operator=(const counter& rhs)
                {
                    this->_data = rhs._data;
                    (*this->_data)++;
                    return *this;
                }

                HSD_CONSTEXPR counter& operator=(counter&& rhs)
                {
                    this->_data = exchange(rhs._data, nullptr);
                    return *this;
                }

                constexpr usize& operator*()
                {
                    return *this->_data;
                }

                constexpr Allocator<usize>& get_allocator()
                {
                    return *this;
                }

                constexpr usize get_size() const
                {
                    return 1u;
                }

                constexpr auto* get_pointer() const
                {
                    return this->_data;
                }

                constexpr void set_pointer(pointer_type ptr)
                {
                    this->_data = ptr;
                }
            };
            
        } // namespace shared_detail

        template < typename T, template <typename> typename Allocator = allocator >
        class shared_ptr
        {
        private:
            shared_detail::storage<T, Allocator> _value;
            shared_detail::counter<Allocator> _count;

            template <typename U, template <typename> typename Alloc>
            friend class shared_ptr;

            HSD_CONSTEXPR void _delete()
            {
                if(_count.get_pointer() != nullptr)
                {
                    (*_count)--;

                    if(*_count == 0)
                    {
                        if (get() != nullptr) {
                            if constexpr (is_array<T>::value)
                            {
                                for (usize i = 0, size = _value.get_size(); i < size; ++i)
                                    _destroy_inplace(get()[size-i]);
                            }
                            else
                            {
                                _destroy_inplace(*get());
                            }
                        }
                        _value.get_allocator().deallocate(
                            _value.get_pointer(), _value.get_size());
                        _count.get_allocator().deallocate(
                            _count.get_pointer(), _count.get_size());
                    }

                    _value.set_pointer(nullptr);
                    _count.set_pointer(nullptr);
                }
            }

        public:
            using alloc_type = Allocator< remove_array_t<T> >;
            using pointer_type = typename alloc_type::pointer_type;
            using value_type = typename alloc_type::value_type;
            using reference_type = typename alloc_type::value_type&;

            HSD_CONSTEXPR shared_ptr() = default;
            HSD_CONSTEXPR shared_ptr(NullType) {}

            HSD_CONSTEXPR shared_ptr(pointer_type ptr) 
            requires (std::is_default_constructible_v<Allocator<uchar>>)
                : _value{ptr, 1u}
            {}

            HSD_CONSTEXPR shared_ptr(pointer_type ptr, usize size) 
            requires (std::is_default_constructible_v<Allocator<uchar>>)
                : _value{ptr, size}
            {}

            HSD_CONSTEXPR shared_ptr(const Allocator<value_type>& alloc) 
                : _value{alloc, 1u}, _count{alloc}
            {}

            HSD_CONSTEXPR shared_ptr(const Allocator<value_type>& alloc, usize size) 
                : _value{alloc, size}, _count{alloc}
            {}

            HSD_CONSTEXPR shared_ptr(pointer_type ptr, 
                const Allocator<value_type>& alloc, usize size) 
                : _value{ptr, alloc, size}, _count{alloc}
            {}

            template <typename U = T> 
            constexpr shared_ptr(const shared_ptr<U, Allocator>& other)
            requires(shared_detail::ConvertibleDerived<T, U>)
                : _value{other._value}, _count{other._count}
            {}

            template <typename U = T> 
            constexpr shared_ptr(shared_ptr<U, Allocator>&& other)
            requires(shared_detail::ConvertibleDerived<T, U>)
                : _value{move(other._value)}, _count{move(other._count)}
            {}

            HSD_CONSTEXPR ~shared_ptr()
            {
                _delete();
            }

            HSD_CONSTEXPR shared_ptr& operator=(NullType)
            {
                _delete();
                return *this;
            }

            template <typename U = T> 
            requires(shared_detail::ConvertibleDerived<T, U>)
            HSD_CONSTEXPR shared_ptr& operator=(shared_ptr<U, Allocator>& rhs)
            {
                _delete();
                _value = rhs._value;
                _count = rhs._count;
                return *this;
            }

            template <typename U = T> 
            requires(shared_detail::ConvertibleDerived<T, U>)
            HSD_CONSTEXPR shared_ptr& operator=(shared_ptr<U, Allocator>&& rhs)
            {
                _delete();
                _value = move(rhs._value);
                _count = move(rhs._count);
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

            constexpr usize get_count()
            {
                return *_count;
            }

            constexpr bool is_unique()
            {
                return get_count() == 1;
            }
        };

        template < typename T, template <typename> typename Allocator >
        struct MakeShr
        {
            using single_object = shared_ptr<T, Allocator>;
        };

        template < typename T, template <typename> typename Allocator >
        struct MakeShr<T[], Allocator>
        {
            using array = shared_ptr<T[], Allocator>;
        };

        template < typename T, usize N, template <typename> typename Allocator >
        struct MakeShr<T[N], Allocator>
        {
            struct invalid_type {};  
        };

        template <typename T, template <typename> typename Allocator = allocator, typename... Args>
        requires (std::is_default_constructible_v<Allocator<uchar>>)
        static HSD_CONSTEXPR typename MakeShr<T, Allocator>::single_object 
        make_shared(Args&&... args)
        {
            Allocator<remove_array_t<T>> _alloc;
            auto* _ptr = _alloc.allocate(1).unwrap();
            _alloc.construct_at(_ptr, forward<Args>(args)...);
            return shared_ptr<T, Allocator>(_ptr);
        }

        template <typename T, template <typename> typename Allocator = allocator, typename U, typename... Args>
        static HSD_CONSTEXPR typename MakeShr<T, Allocator>::single_object 
        make_shared(Allocator<U>& alloc, Args&&... args)
        {
            auto* _ptr = static_cast<Allocator<remove_array_t<T>>>(alloc).allocate(1).unwrap();
            alloc.construct_at(_ptr, forward<Args>(args)...);
            return shared_ptr<T, Allocator>(_ptr, static_cast<Allocator<remove_array_t<T>>>(alloc), 1);
        }

        template <typename T, template <typename> typename Allocator = allocator, typename... Args>
        requires (std::is_default_constructible_v<Allocator<uchar>>)
        static HSD_CONSTEXPR typename MakeShr<T, Allocator>::array
        make_shared(usize size)
        {
            Allocator<remove_array_t<T>> _alloc;
            auto* _ptr = _alloc.allocate(size).unwrap();
            return shared_ptr<T, Allocator>(_ptr, size);
        }

        template <typename T, template <typename> typename Allocator = allocator, typename U, typename... Args>
        static HSD_CONSTEXPR typename MakeShr<T, Allocator>::array
        make_shared(Allocator<U>& alloc, usize size)
        {
            return shared_ptr<T, Allocator>(static_cast<Allocator<remove_array_t<T>>>(alloc), size);
        }

        template <typename T, typename U, template <typename> typename Allocator = allocator, typename... Args>
        static HSD_CONSTEXPR typename MakeShr<T, Allocator>::invalid_type 
        make_shared(const Allocator<U>&, Args&&...) = delete;
    }
}