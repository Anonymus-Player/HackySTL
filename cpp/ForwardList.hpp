#pragma once

#include "Utility.hpp"
#include "Types.hpp"
#include "Result.hpp"
#include <new>

namespace hsd
{
    template <typename T> class forward_list;

    namespace forward_list_detail
    {
        struct bad_iterator
        {
            const char* operator()() const
            {
                return "Null pointer access denied";
            }
        };

        template <typename T>
        class iterator
        {
        private:
            struct forward_list_impl
            {
                T _value;
                forward_list_impl* _next = nullptr;

                HSD_CONSTEXPR forward_list_impl() {}

                HSD_CONSTEXPR forward_list_impl(const T& val)
                    : _value{val}
                {}

                HSD_CONSTEXPR forward_list_impl(T&& val)
                {
                    _value = hsd::move(val);
                }
            };
            
            forward_list_impl *_iterator = nullptr;
            friend class forward_list<T>;

            HSD_CONSTEXPR iterator() = default;
            HSD_CONSTEXPR iterator(hsd::NullType) {}

            constexpr T* get()
            {
                return &_iterator->_value;
            }

            constexpr T* get() const
            {
                return &_iterator->_value;
            }

            constexpr void push_back(const T& value)
            {
                _iterator->_next = new forward_list_impl(value);
                _iterator = _iterator->_next;
            }

            constexpr void push_back(T&& value)
            {
                _iterator->_next = new forward_list_impl(hsd::move(value));
                _iterator = _iterator->_next;
            }

            template <typename... Args>
            constexpr void emplace_back(Args&&... args)
            {
                _iterator->_next = new forward_list_impl();
                _iterator->_next->_value.~T();
                new (&_iterator->_next->_value) T(hsd::forward<Args>(args)...);
                _iterator = _iterator->_next;
            }

            constexpr void push_front(const T& value)
            {
                forward_list_impl* _element = new forward_list_impl(value);
                _element->_next = _iterator;
                _iterator = _element;
            }

            constexpr void push_front(T&& value)
            {
                forward_list_impl* _element = new forward_list_impl(hsd::move(value));
                _element->_next = _iterator;
                _iterator = _element;
            }

            template <typename... Args>
            constexpr void emplace_front(Args&&... args)
            {
                forward_list_impl* _element = new forward_list_impl();
                _element->_next = _iterator;
                _element->_value.~T();
                new (&_element->_value) T(hsd::forward<Args>(args)...);
                _iterator = _element;
            }

            constexpr void pop_front()
            {
                forward_list_impl* _element = _iterator;
                _iterator = _iterator->_next;
                delete _element;
            }
        public:

            HSD_CONSTEXPR iterator(const iterator& other)
            {
                _iterator = other._iterator;
            }

            constexpr iterator& operator=(const iterator& rhs)
            {
                _iterator = rhs._iterator;
                return *this;
            }

            constexpr friend bool operator==(const iterator& lhs, const iterator& rhs)
            {
                return lhs._iterator == rhs._iterator;
            }
        
            constexpr friend bool operator!=(const iterator& lhs, const iterator& rhs)
            {
                return lhs._iterator != rhs._iterator;
            }

            constexpr auto& operator++()
            {
                _iterator = _iterator->_next;
                return *this;
            }

            constexpr iterator operator++(i32)
            {
                iterator tmp = *this;
                operator++();
                return tmp;
            }

            constexpr auto& operator*() 
            {
                return _iterator->_value;
            }

            constexpr const auto& operator*() const
            {
                return _iterator->_value;
            }

            constexpr auto* operator->() 
            {
                return get();
            }

            constexpr const auto* operator->() const
            {
                return get();
            }
        };
    } // namespace forward_list_detail

    template <typename T>
    class forward_list
    {
    private:
        forward_list_detail::iterator<T> _head;
        forward_list_detail::iterator<T> _tail;
        usize _size = 0;
        
    public:
        using iterator = forward_list_detail::iterator<T>;
        using const_iterator = const iterator;
        HSD_CONSTEXPR forward_list() {}

        HSD_CONSTEXPR forward_list(const forward_list& other)
        {
            for(const auto& _element : other)
                push_back(_element);
        }

        HSD_CONSTEXPR forward_list(forward_list&& other)
        {
            _head = other._head;
            _tail = other._tail;
            other._head = nullptr;
            other._tail = nullptr;
        }

        template <usize N>
        HSD_CONSTEXPR forward_list(const T (&arr)[N])
        {
            for(usize _index = 0; _index < N; _index++)
                push_back(arr[_index]);
        }

        template <usize N>
        HSD_CONSTEXPR forward_list(T (&&arr)[N])
        {
            for(usize _index = 0; _index < N; _index++)
                push_back(move(arr[_index]));
        }

        HSD_CONSTEXPR ~forward_list()
        {
            clear();
        }

        constexpr forward_list& operator=(const forward_list& rhs)
        {
            clear();
            
            for(const auto& _element : rhs)
                push_back(_element);
            
            return *this;
        }

        constexpr forward_list& operator=(forward_list&& rhs)
        {
            _head = rhs._head;
            _tail = rhs._tail;
            rhs._head = nullptr;
            rhs._tail = nullptr;
            
            return *this;
        }

        template <usize N>
        constexpr forward_list& operator=(const T (&arr)[N])
        {
            clear();
            usize _index = 0;

            for(auto _it = begin(); _it != end() && _index < N; _it++, _index++)
                *_it = arr[_index];

            for(; _index < N; _index++)
                push_back(arr[_index]);

            return *this;
        }

        template <usize N>
        constexpr forward_list& operator=(T (&&arr)[N])
        {
            clear();
            usize _index = 0;

            for(auto _it = begin(); _it != end() && _index < N; _it++, _index++)
                *_it = move(arr[_index]);

            for(; _index < N; _index++)
                push_back(move(arr[_index]));

            return *this;
        }

        constexpr auto erase(const_iterator pos)
            -> Result<iterator, runtime_error>
        {
            if(pos._iterator == nullptr)
            {
                // this in the only situation when
                // .erase() will "throw" because
                // there is no fast way to check
                // if it belongs to this list or not
                return runtime_error{"Accessed an null element"};
            }
            else if(_size == 1)
            {
                pop_front();
                return end();
            }
            else if(pos == begin())
            {
                pop_front();
                return begin();
            }
            else
            {
                iterator _next_iter, _back_iter;
                _next_iter._iterator = pos._iterator->_next;
                auto _check_iter = [&]{
                    return _back_iter._iterator->_next != 
                        pos._iterator && _back_iter != end();
                };
                
                for(_back_iter = begin(); _check_iter(); _back_iter++) {}

                if(_back_iter == end())
                {
                    return runtime_error{"Undefined Behaviour"};
                }
                else
                {
                    _back_iter._iterator->_next = 
                        _next_iter._iterator;

                    _size--;
                    delete pos._iterator;
                    return _next_iter;
                }
            }

            return runtime_error{"Undefined Behaviour"};
        }

        constexpr void push_back(const T& value)
        {
            if(empty())
            {
                _head._iterator = new typename iterator::forward_list_impl(value);
                _tail = _head;
            }
            else
                _tail.push_back(value);

            _size++;
        }

        constexpr void push_back(T&& value)
        {
            if(empty())
            {
                _head._iterator = new typename iterator::forward_list_impl(hsd::move(value));
                _tail = _head;
            }
            else
                _tail.push_back(hsd::move(value));

            _size++;
        }

        template <typename... Args>
        constexpr void emplace_back(Args&&... args)
        {
            if(empty())
            {
                _head._iterator = new typename iterator::forward_list_impl();
                _head._iterator->_value.~T();
                new (&_head._iterator->_value) T(hsd::forward<Args>(args)...);
                _tail = _head;
            }
            else
                _tail.emplace_back(hsd::forward<Args>(args)...);

            _size++;
        }

        constexpr void push_front(const T& value)
        {
            if(empty())
            {
                _head._iterator = new typename iterator::forward_list_impl(value);
                _tail = _head;
            }
            else
                _head.push_front(value);

            _size++;
        }

        constexpr void push_front(T&& value)
        {
            if(empty())
            {
                _head._iterator = new typename iterator::forward_list_impl(hsd::move(value));
                _tail = _head;
            }
            else
                _head.push_front(hsd::move(value));

            _size++;
        }

        template <typename... Args>
        constexpr void emplace_front(Args&&... args)
        {
            if(empty())
            {
                _head._iterator = new typename iterator::forward_list_impl();
                _head._iterator->_value.~T();
                new (&_head._iterator->_value) T(hsd::forward<Args>(args)...);
                _tail = _head;
            }
            else
                _head.emplace_front(hsd::forward<Args>(args)...);

            _size++;
        }

        constexpr void pop_front()
        {
            if(!empty())
            {
                _head.pop_front();
            }
            else
                _tail = _head;
                
            _size--;
        }

        constexpr void clear()
        {
            for(; !empty(); pop_front());
            pop_front();
        }

        constexpr bool empty()
        {
            return _size == 0;
        }

        constexpr T& front()
        {
            return *_head.get();
        }

        constexpr T& back()
        {
            return *_tail.get();
        }

        constexpr iterator begin()
        {
            return _head;
        }

        constexpr const_iterator begin() const
        {
            return cbegin();
        }

        constexpr iterator end()
        {
            return iterator(nullptr);
        }

        constexpr const_iterator end() const
        {
            return cend();
        }

        constexpr const_iterator cbegin() const
        {
            return _head;
        }

        constexpr const_iterator cend() const
        {
            return iterator(nullptr);
        }
    };
} // namespace hsd