#pragma once

#include "Result.hpp"
#include "Pair.hpp"
#include "Vector.hpp"
#include "Hash.hpp"

namespace hsd
{
    namespace umap_detail
    {
        template < template <typename> typename Allocator >
        concept DefaultAlloc = std::is_default_constructible_v<Allocator<uchar>>;
        template < template <typename> typename Allocator >
        concept CopyAlloc = std::is_copy_constructible_v<Allocator<uchar>>;
        template < template <typename> typename Alloc1, template <typename> typename Alloc2 >
        concept IsSameAlloc = is_same<Alloc1<uchar>, Alloc2<uchar>>::value;

        struct bad_key
        {
            const char* operator()() const
            {
                return "Tried to use an invalid key";
            }
        };

        struct bad_access
        {
            const char* operator()() const
            {
                return "Tried to access an element out of bounds";
            }
        };
    } // namespace umap_detail

    template< typename Key, typename T, typename Hasher = hash<usize, Key>, 
        template <typename> typename BucketAllocator = allocator,
        template <typename> typename Allocator = BucketAllocator >
    class unordered_map
    {
    private:
        using ref_value = pair< typename Hasher::ResultType, usize >;
        using ref_vector = vector< ref_value, BucketAllocator >;
        using bucket_iter = typename ref_vector::iterator;

        static constexpr f64 _limit_ratio = 0.75f;
        vector< ref_vector, BucketAllocator > _buckets;
        vector< pair<Key, T>, Allocator > _data;

        constexpr void _replace(usize new_size)
        {
            _buckets.clear();
            _buckets.resize(new_size);

            for(usize _index = 0; _index < _data.size(); _index++)
            {
                auto _hash_rez = Hasher::get_hash(_data[_index].first);
                usize _bucket_index = _hash_rez % new_size;
                _buckets[_bucket_index].emplace_back(_hash_rez, _index);
            }
        }

        constexpr pair<usize, usize> _get(const Key& key) const
        {
            auto _key_hash = Hasher::get_hash(key);
            usize _index = _key_hash % _buckets.size();

            for(auto& _val : _buckets[_index])
            {
                if(_key_hash == _val.first)
                {
                    return {_val.second, _index};
                }
            }

            return {static_cast<usize>(-1), _index};
        }

        constexpr auto _get_iter(const Key& key)
            -> Result< pair<bucket_iter, usize>, umap_detail::bad_access >
        {
            auto _key_hash = Hasher::get_hash(key);
            usize _index = _key_hash % _buckets.size();

            for(auto _val = _buckets[_index].begin(); _val != _buckets[_index].end(); _val++)
            {
                if(_key_hash == _val->first)
                {
                    return pair{_val, _index};
                }
            }

            return umap_detail::bad_access{};
        }

    public:
        using reference_type = T&;
        using iterator = typename vector<pair<Key, T>>::iterator;
        using const_iterator = typename vector<pair<Key, T>>::const_iterator;

        HSD_CONSTEXPR ~unordered_map() {}

        HSD_CONSTEXPR unordered_map()
        requires (umap_detail::DefaultAlloc<Allocator> &&
            umap_detail::DefaultAlloc<BucketAllocator>)
            : _buckets(10)
        {}

        HSD_CONSTEXPR unordered_map(const Allocator<uchar>& alloc)
        requires (
            (!umap_detail::DefaultAlloc<Allocator> || 
            umap_detail::CopyAlloc<Allocator>) && 
            umap_detail::IsSameAlloc<Allocator, BucketAllocator>
        )
            : _buckets(10, alloc), _data(alloc)
        {}

        HSD_CONSTEXPR unordered_map(const BucketAllocator<uchar>& alloc)
        requires (
            (!umap_detail::DefaultAlloc<BucketAllocator> || 
            umap_detail::CopyAlloc<BucketAllocator>) && 
            umap_detail::DefaultAlloc<Allocator> &&
            !umap_detail::IsSameAlloc<Allocator, BucketAllocator>
        )
            : _buckets(10, alloc)
        {}

        HSD_CONSTEXPR unordered_map(const Allocator<uchar>& alloc)
        requires (
            (!umap_detail::DefaultAlloc<Allocator> || 
            umap_detail::CopyAlloc<Allocator>) && 
            umap_detail::DefaultAlloc<BucketAllocator> &&
            !umap_detail::IsSameAlloc<Allocator, BucketAllocator>
        )
            : _buckets(10), _data(alloc)
        {}

        HSD_CONSTEXPR unordered_map(
            const BucketAllocator<uchar>& bucket_alloc, 
            const Allocator<uchar>& data_alloc)
        requires (
            (!umap_detail::DefaultAlloc<Allocator> || 
            umap_detail::CopyAlloc<Allocator>) && 
            umap_detail::DefaultAlloc<Allocator> &&
            umap_detail::DefaultAlloc<BucketAllocator> &&
            !umap_detail::IsSameAlloc<Allocator, BucketAllocator>
        )
            : _buckets(10, bucket_alloc), _data(data_alloc)
        {}

        HSD_CONSTEXPR unordered_map(const unordered_map& other)
            : _buckets{other._buckets.size()}, _data{other._data}
        {
            for(auto& _val : _data)
            {
                auto _hash_rez = Hasher::get_hash(_val.get().first);
                usize _index = _hash_rez % _buckets.size();
                _buckets[_index].emplace_back(_hash_rez, _index);
            }
        }

        HSD_CONSTEXPR unordered_map(unordered_map&& other)
            : _buckets{move(other._buckets)}, _data{move(other._data)}
        {}

        template <usize N>
        HSD_CONSTEXPR unordered_map(pair<Key, T> (&&other)[N])
        requires (umap_detail::DefaultAlloc<Allocator> &&
            umap_detail::DefaultAlloc<BucketAllocator>)
            : _buckets(10)
        {
            for(usize _index = 0; _index < N; _index++)
            {
                emplace(
                    move(other[_index].first), 
                    move(other[_index].second)
                );
            }
        }

        HSD_CONSTEXPR unordered_map& operator=(unordered_map&& rhs)
        {
            _buckets = move(rhs._buckets);
            _data = move(rhs._data);
            return *this;
        }

        HSD_CONSTEXPR unordered_map& operator=(const unordered_map& rhs)
        {
            clear();

            for(pair<Key, T>& val : rhs._data)
                emplace(val.first, val.second);

            return *this;
        }

        template <usize N>
        HSD_CONSTEXPR unordered_map& operator=(const pair<Key, T>(&rhs)[N])
        {
            clear();

            for(usize _index = 0; _index < N; _index++)
            {
                emplace(rhs[_index].first, rhs[_index].second);
            }

            return *this;
        }

        template <usize N>
        HSD_CONSTEXPR unordered_map& operator=(pair<Key, T>(&&rhs)[N])
        {
            clear();

            for(usize _index = 0; _index < N; _index++)
            {
                emplace(
                    move(rhs[_index].first), 
                    move(rhs[_index].second)
                );
            }

            return *this;
        }

        constexpr auto& operator[](const Key& key) noexcept
        {
            return emplace(key).first->second;
        }

        constexpr const auto& operator[](const Key& key) const
        {
            return at(key).unwrap();
        }

        constexpr auto at(const Key& key)
            -> Result< reference<T>, umap_detail::bad_key >
        {
            usize _data_index = _get(key).first;

            if(_data_index == static_cast<usize>(-1))
                return umap_detail::bad_key{};

            return {_data[_data_index].second};
        }

        constexpr auto at(const Key& key) const
            -> Result< reference<const T>, umap_detail::bad_key >
        {
            usize _data_index = _get(key).first;

            if(_data_index == static_cast<usize>(-1))
                return umap_detail::bad_key{};

            return {_data[_data_index].second};
        }

        template< typename NewKey, typename... Args >
        constexpr pair<iterator, bool> emplace(const NewKey& key, const Args&... args)
        {
            auto [_data_index, _bucket_index] = _get(key);

            if(_data_index != static_cast<usize>(-1))
            {
                return {_data.begin() + _data_index, false};
            }
            else
            {
                _data.emplace_back(key, T{forward<Args>(args)...});

                if(static_cast<f64>(_data.size()) / static_cast<f64>(_buckets.size()) >= _limit_ratio)
                {
                    if constexpr(requires {BucketAllocator<uchar>::limit();})
                    {
                        if(BucketAllocator<uchar>::limit() > _buckets.size() + _buckets.size() / 2)
                        {
                            _replace(_buckets.size() + _buckets.size() / 2);
                        }
                        else
                        {
                            _replace(BucketAllocator<uchar>::limit());
                        }
                    }
                    else
                    {
                        _replace(_buckets.size() + _buckets.size() / 2);
                    }
                }
                else
                {
                    _buckets[_bucket_index].emplace_back(Hasher::get_hash(_data.back().first), _data.size() - 1);
                }

                return {_data.end() - 1, true};
            }
        }

        template< typename NewKey, typename... Args >
        constexpr pair<iterator, bool> emplace(NewKey&& key, Args&&... args)
        {
            auto [_data_index, _bucket_index] = _get(key);

            if(_data_index != static_cast<usize>(-1))
            {
                return {_data.begin() + _data_index, false};
            }
            else
            {
                _data.emplace_back(move(key), move(T{forward<Args>(args)...}));

                if(static_cast<f64>(_data.size()) / static_cast<f64>(_buckets.size()) >= _limit_ratio)
                {
                    if constexpr(requires {BucketAllocator<uchar>::limit();})
                    {
                        if(BucketAllocator<uchar>::limit() > _buckets.size() + _buckets.size() / 2)
                        {
                            _replace(_buckets.size() + _buckets.size() / 2);
                        }
                        else
                        {
                            _replace(BucketAllocator<uchar>::limit());
                        }
                    }
                    else
                    {
                        _replace(_buckets.size() + _buckets.size() / 2);
                    }
                }
                else
                {
                    _buckets[_bucket_index].emplace_back(Hasher::get_hash(_data.back().first), _data.size() - 1);
                }

                return {_data.end() - 1, true};
            }
        }

        constexpr auto erase(const_iterator pos)
            -> Result<iterator, umap_detail::bad_access>
        {
            auto _result = _get_iter(pos->first);
            
            if(_result.is_ok() == false)
                return umap_detail::bad_access{};
            
            // now .unwrap() should not fail
            auto [_iter, _index] = _result.unwrap();
            _buckets[_index].erase(_iter).unwrap(HSD_FUNCTION_NAME);
            return _data.erase(pos).unwrap(HSD_FUNCTION_NAME);
        }

        constexpr void clear()
        {
            _data.clear();
            _buckets.clear();
        }

        constexpr usize size() const
        {
            return _data.size();
        }

        constexpr iterator begin()
        {
            return _data.begin();
        }

        constexpr const_iterator begin() const
        {
            return cbegin();
        }

        constexpr iterator end()
        {
            return _data.end();
        }

        constexpr const_iterator end() const
        {
            return cend();
        }

        constexpr const_iterator cbegin() const
        {
            return _data.cbegin();
        }

        constexpr const_iterator cend() const
        {
            return _data.cend();
        }

        constexpr iterator rbegin()
        {
            return _data.rbegin();
        }

        constexpr const_iterator rbegin() const
        {
            return crbegin();
        }

        constexpr iterator rend()
        {
            return _data.rend();
        }

        constexpr const_iterator rend() const
        {
            return crend();
        }

        constexpr const_iterator crbegin() const
        {
            return _data.crbegin();
        }

        constexpr const_iterator crend() const
        {
            return _data.crend();
        }
    };

    template< typename Key, typename T, usize N >
    unordered_map(pair<Key, T> (&&other)[N]) 
        -> unordered_map< Key, T, hash<usize, Key>, allocator >;

    template< typename Key, typename T >
    using buffered_umap = unordered_map< Key, T, hash<usize, Key>, buffered_allocator >;
    template< typename Key, typename T, usize N >
    using static_umap = unordered_map< Key, T, hash<usize, Key>, 
        ct_alloc_helper<N + N / 2>::template alloc_type,
        ct_alloc_helper<N>::template alloc_type >;
} // namespace hsd