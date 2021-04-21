#pragma once

#include "Types.hpp"
#include "Concepts.hpp"
#include "Random.hpp"
#include "Vector.hpp"

namespace hsd
{
    namespace ranges
    {
        namespace views
        {
            namespace views_detail
            {
                template <ReverseIterable T>
                class reverse_iterator
                {
                private:
                    T _iter;
                
                public:
                    constexpr reverse_iterator(T iter)
                        : _iter{iter}
                    {}

                    constexpr auto& operator++()
                    {
                        _iter--;
                        return *this;
                    }

                    constexpr reverse_iterator operator++(i32)
                    {
                        reverse_iterator tmp = *this;
                        operator++();
                        return tmp;
                    }

                    constexpr bool operator==(const reverse_iterator& rhs) const
                    {
                        return _iter == rhs._iter;
                    }

                    constexpr bool operator!=(const reverse_iterator& rhs) const
                    {
                        return _iter != rhs._iter;
                    }

                    constexpr auto& operator*()
                    {
                        return *_iter;
                    }

                    constexpr const auto& operator*() const
                    {
                        return *_iter;
                    }
                };

                template <ForwardIterable T>
                class forward_iterator
                {
                private:
                    T _iter;
                
                public:
                    constexpr forward_iterator(T iter)
                        : _iter{iter}
                    {}

                    constexpr auto& operator++()
                    {
                        _iter++;
                        return *this;
                    }

                    constexpr forward_iterator operator++(i32)
                    {
                        forward_iterator tmp = *this;
                        operator++();
                        return tmp;
                    }

                    constexpr bool operator==(const forward_iterator& rhs) const
                    {
                        return _iter == rhs._iter;
                    }

                    constexpr bool operator!=(const forward_iterator& rhs) const
                    {
                        return _iter != rhs._iter;
                    }

                    constexpr auto& operator*()
                    {
                        return *_iter;
                    }

                    constexpr const auto& operator*() const
                    {
                        return *_iter;
                    }
                };

                template <ForwardIterable T>
                class random_iterator
                {
                private:
                    T _iter;
                    T _origin;
                    usize _index;
                    static inline mt19937_64 engine{};
                
                public:
                    constexpr random_iterator(T iter, usize index)
                        : _iter{iter}, _origin{iter}, _index{index}
                    {}

                    constexpr auto& operator++()
                    {
                        _iter = _origin;
                        
                        usize _to = engine.generate<usize>(0u, _index + 1).unwrap();
                        for(usize _from = 0; _from < _to; _from++, _iter++);

                        _index++;
                        return *this;
                    }

                    constexpr random_iterator operator++(i32)
                    {
                        auto tmp = *this;
                        operator++();
                        return tmp;
                    }

                    constexpr friend bool operator==(
                        const random_iterator& lhs, const random_iterator& rhs)
                    {
                        return lhs._index == rhs._index;
                    }

                    constexpr friend bool operator!=(
                        const random_iterator& lhs, const random_iterator& rhs)
                    {
                        return lhs._index != rhs._index; 
                    }

                    constexpr auto& operator*()
                    {
                        return *_iter;
                    }

                    constexpr const auto& operator*() const
                    {
                        return *_iter;
                    }
                };

                template <ForwardIterable T>
                class forward
                {
                private:
                    using iter_type = typename views_detail::forward_iterator<T>;
                    usize _view_size = 0;
                    iter_type _begin;
                    iter_type _end;
                
                    constexpr forward(const iter_type& begin, const iter_type& end, usize view_size)
                        : _view_size{view_size}, _begin{begin}, _end{end}
                    {}
                    
                public:
                    using iter_value_type = T;

                    template <IsForwardContainer U>
                    constexpr forward(const U& container)
                        : _view_size{container.size()}, 
                        _begin{container.begin()}, 
                        _end{container.end()}
                    {}

                    constexpr auto drop(usize quantity) const
                        -> Result<forward, runtime_error>
                    {
                        if(quantity > _view_size)
                        {
                            return runtime_error{"Dropping out of bounds"};
                        }
                        else
                        {
                            usize _index = 0;
                            iter_type _result_iter = _begin;
                            for(; _index < quantity; _index++, _result_iter++);
                            return forward{_result_iter, _end, _view_size - _index};
                        }
                    }

                    constexpr auto take(usize quantity) const
                        -> Result<forward, runtime_error> 
                    {
                        if(quantity > _view_size)
                        {
                            return runtime_error{"Dropping out of bounds"};
                        }
                        else
                        {
                            usize _index = 0;
                            iter_type _result_iter = _begin;
                            for(; _index < quantity; _index++, _result_iter++);
                            return forward{_begin, _result_iter, _view_size - _index};
                        }
                    }

                    constexpr usize size() const
                    {
                        return _view_size;
                    }

                    constexpr auto begin()
                    {
                        return _begin;
                    }

                    constexpr const auto begin() const
                    {
                        return _begin;
                    }

                    constexpr auto end()
                    {
                        return _end;
                    }

                    constexpr const auto end() const
                    {
                        return _end;
                    }
                };
                
                template <ReverseIterable T>
                class reverse
                {
                private:
                    using iter_type = views_detail::reverse_iterator<T>;
                    usize _view_size = 0;
                    iter_type _begin;
                    iter_type _end;
                
                    constexpr reverse(const iter_type& begin, const iter_type& end, usize view_size)
                        : _view_size{view_size}, _begin{begin}, _end{end}
                    {}

                public:
                    using iter_value_type = T;

                    template <IsReverseContainer U>
                    constexpr reverse(const U& container)
                        : _view_size{container.size()}, 
                        _begin{container.rbegin()}, 
                        _end{container.rend()}
                    {}

                    constexpr auto drop(usize quantity) const
                        -> Result<reverse, runtime_error> 
                    {
                        if(quantity > _view_size)
                        {
                            return runtime_error{"Dropping out of bounds"};
                        }
                        else
                        {
                            usize _index = 0;
                            iter_type _result_iter = _begin;
                            for(; _index < quantity; _index++, _result_iter++);
                            return reverse{_result_iter, _end, _view_size - _index};
                        }
                    }

                    constexpr auto take(usize quantity) const
                        -> Result<reverse, runtime_error> 
                    {
                        if(quantity > _view_size)
                        {
                            return runtime_error{"Dropping out of bounds"};
                        }
                        else
                        {
                            usize _index = 0;
                            iter_type _result_iter = _begin;
                            for(; _index < quantity; _index++, _result_iter++);
                            return reverse{_begin, _result_iter, _view_size - _index};
                        }
                    }

                    constexpr usize size() const
                    {
                        return _view_size;
                    }

                    constexpr auto begin()
                    {
                        return _begin;
                    }

                    constexpr const auto begin() const
                    {
                        return _begin;
                    }

                    constexpr auto end()
                    {
                        return _end;
                    }

                    constexpr const auto end() const
                    {
                        return _end;
                    }
                };

                template <ForwardIterable T>
                class random
                {
                private:
                    using iter_type = views_detail::random_iterator<T>;
                    usize _view_size = 0;
                    iter_type _begin;
                    iter_type _end;
                
                    constexpr random(const iter_type& begin, const iter_type& end, usize view_size)
                        : _view_size{view_size}, _begin{begin}, _end{end}
                    {}

                public:
                    using iter_value_type = T;

                    template <IsForwardContainer U>
                    constexpr random(const U& container)
                        : _view_size{container.size()}, _begin{container.begin(), 0}, 
                        _end{container.end(), container.size()}
                    {}

                    constexpr auto drop(usize quantity) const
                        -> Result<random, runtime_error>
                    {
                        if(quantity > _view_size)
                        {
                            return runtime_error{"Dropping out of bounds"};
                        }
                        else
                        {
                            usize _index = 0;
                            iter_type _result_iter = _begin;
                            for(; _index < quantity; _index++, _result_iter++);
                            return random{_result_iter, _end, _view_size - _index};
                        }
                    }

                    constexpr auto take(usize quantity) const
                        -> Result<random, runtime_error> 
                    {
                        if(quantity > _view_size)
                        {
                            return runtime_error{"Dropping out of bounds"};
                        }
                        else
                        {
                            usize _index = 0;
                            iter_type _result_iter = _begin;
                            for(; _index < quantity; _index++, _result_iter++);
                            return random{_begin, _result_iter, _view_size - _index};
                        }
                    }

                    constexpr usize size() const
                    {
                        return _view_size;
                    }

                    constexpr auto begin()
                    {
                        return _begin;
                    }

                    constexpr const auto begin() const
                    {
                        return _begin;
                    }

                    constexpr auto end()
                    {
                        return _end;
                    }

                    constexpr const auto end() const
                    {
                        return _end;
                    }
                };

                template <IsForwardContainer U>
                forward(const U& container) -> forward<decltype(container.end())>;
                template <IsReverseContainer U>
                reverse(const U& container) -> reverse<decltype(container.rend())>;
                template <IsForwardContainer U>
                random(const U& container) -> random<decltype(container.end())>;

                struct _forward
                {
                    template <IsForwardContainer U>
                    constexpr friend auto operator|(const U& rhs, const _forward&)
                    {
                        return forward{rhs};
                    }
                };

                struct _reverse
                {
                    template <IsReverseContainer U>
                    constexpr friend auto operator|(const U& rhs, const _reverse&)
                    {
                        return reverse{rhs};
                    }
                };

                struct _random
                {
                    template <IsForwardContainer U>
                    constexpr friend auto operator|(const U& rhs, const _random&)
                    {
                        return random{rhs};
                    }
                };
            } // namespace views_detail
            
            template <typename T>
            concept IsView = (
                IsSame<T, views_detail::forward<typename T::iter_value_type>> ||
                IsSame<T, views_detail::reverse<typename T::iter_value_type>> ||
                IsSame<T, views_detail::random<typename T::iter_value_type>>
            );

            static constexpr views_detail::_forward forward = {};
            static constexpr views_detail::_reverse reverse = {};
            static constexpr views_detail::_random random = {};

            class drop
            {
            private:
                usize _count;

            public:
                constexpr drop(usize count)
                    : _count{count}
                {}

                template <IsView U>
                constexpr friend auto operator|(const U& lhs, const drop& rhs)
                {
                    return lhs.drop(rhs._count).unwrap();
                }

                template <typename U>
                constexpr friend auto operator|(const U& lhs, const drop& rhs)
                requires (IsForwardContainer<U> && !IsView<U>)
                {
                    return (lhs | forward).drop(rhs._count).unwrap();
                }
            };

            template <typename FuncType>
            class drop_while
            {
            private:
                FuncType _func;

            public:
                drop_while(FuncType func)
                    : _func(hsd::forward<FuncType>(func))
                {}

                template <views::IsView U>
                constexpr friend auto operator|(const U& lhs, const drop_while& rhs)
                requires (InvocableRet<bool, FuncType, decltype(*lhs.begin())>)
                {
                    vector<remove_cvref_t<decltype(*lhs.begin())>> _vec;

                    if constexpr(requires{rhs._func(*lhs.begin()).unwrap();})
                    {
                        auto _iter = lhs.begin();
                        for(; _iter != lhs.end() && 
                            rhs._func(*_iter).unwrap(); _iter++);

                        for(; _iter != lhs.end(); _iter++)
                        {
                            _vec.emplace_back(*_iter);
                        }
                    }
                    else
                    {
                        auto _iter = lhs.begin();
                        for(; _iter != lhs.end() && 
                            rhs._func(*_iter); _iter++);

                        for(; _iter != lhs.end(); _iter++)
                        {
                            _vec.emplace_back(*_iter);
                        }
                    }

                    return _vec;
                }

                template <typename U> requires (IsForwardContainer<U> && !views::IsView<U>)
                constexpr friend auto operator|(const U& lhs, const drop_while& rhs)
                requires (InvocableRet<bool, FuncType, decltype(*lhs.begin())>)
                {
                    return lhs | views::forward | rhs;
                }
            };

            class take
            {
            private:
                usize _count;

            public:
                constexpr take(usize count)
                    : _count{count}
                {}

                template <IsView U>
                constexpr friend auto operator|(const U& lhs, const take& rhs)
                {
                    return lhs.take(rhs._count).unwrap();
                }

                template <typename U>
                constexpr friend auto operator|(const U& lhs, const take& rhs)
                requires (IsForwardContainer<U> && !IsView<U>)
                {
                    return (lhs | forward).take(rhs._count).unwrap();
                }
            };

            template <typename FuncType>
            class take_while
            {
            private:
                FuncType _func;

            public:
                take_while(FuncType func)
                    : _func(hsd::forward<FuncType>(func))
                {}

                template <views::IsView U>
                constexpr friend auto operator|(const U& lhs, const take_while& rhs)
                requires (InvocableRet<bool, FuncType, decltype(*lhs.begin())>)
                {
                    vector<remove_cvref_t<decltype(*lhs.begin())>> _vec;

                    if constexpr(requires{rhs._func(*lhs.begin()).unwrap();})
                    {
                        for(auto _iter = lhs.begin(); _iter != lhs.end() 
                            && rhs._func(*_iter).unwrap(); _iter++)
                        {
                            _vec.emplace_back(*_iter);
                        }
                    }
                    else
                    {
                        for(auto _iter = lhs.begin(); _iter != lhs.end() 
                            && rhs._func(*_iter); _iter++)
                        {
                            _vec.emplace_back(*_iter);
                        }
                    }

                    return _vec;
                }

                template <typename U> requires (IsForwardContainer<U> && !views::IsView<U>)
                constexpr friend auto operator|(const U& lhs, const take_while& rhs)
                requires (InvocableRet<bool, FuncType, decltype(*lhs.begin())>)
                {
                    return lhs | views::forward | rhs;
                }
            };
        } // namespace views
        
        template <typename FuncType>
        class filter
        {
        private:
            FuncType _func;

        public:
            filter(FuncType func)
                : _func(hsd::forward<FuncType>(func))
            {}

            template <views::IsView U>
            constexpr friend auto operator|(const U& lhs, const filter& rhs)
            requires (InvocableRet<bool, FuncType, decltype(*lhs.begin())>)
            {
                vector<remove_cvref_t<decltype(*lhs.begin())>> _vec;

                for(auto& _value : lhs)
                {
                    if constexpr(requires{rhs._func(_value).unwrap();})
                    {
                        if(rhs._func(_value).unwrap())
                            _vec.emplace_back(_value);
                    }
                    else
                    {
                        if(rhs._func(_value))
                            _vec.emplace_back(_value);
                    }
                }

                return _vec;
            }

            template <typename U> requires (IsForwardContainer<U> && !views::IsView<U>)
            constexpr friend auto operator|(const U& lhs, const filter& rhs)
            requires (InvocableRet<bool, FuncType, decltype(*lhs.begin())>)
            {
                return lhs | views::forward | rhs;
            }
        };

        template <typename FuncType>
        class transform
        {
        private:
            FuncType _func;

        public:
            transform(FuncType func)
                : _func(hsd::forward<FuncType>(func))
            {}

            template <views::IsView U>
            constexpr friend auto operator|(const U& lhs, const transform& rhs)
            requires (InvocableRet<
                remove_cvref_t<decltype(*lhs.begin())>, FuncType, decltype(*lhs.begin())>)
            {
                vector<remove_cvref_t<decltype(*lhs.begin())>> _vec;

                for(auto& _value : lhs)
                {
                    if constexpr(requires{rhs._func(_value).unwrap();})
                    {
                        _vec.emplace_back(rhs._func(_value).unwrap());
                    }
                    else
                    {
                        _vec.emplace_back(rhs._func(_value));
                    }
                }

                return _vec;
            }

            template <typename U> requires (IsForwardContainer<U> && !views::IsView<U>)
            constexpr friend auto operator|(const U& lhs, const transform& rhs)
            requires (InvocableRet<
                remove_cvref_t<decltype(*lhs.begin())>, FuncType, decltype(*lhs.begin())>)
            {
                return lhs | views::forward | rhs;
            }
        };
    } // namespace ranges

    namespace views = ranges::views;    
} // namespace hsd
