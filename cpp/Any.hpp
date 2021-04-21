#pragma once

// define HSD_ANY_ENABLE_TYPEINFO to enable type_info

#ifdef HSD_ANY_ENABLE_TYPEINFO
#   include <typeinfo>
#endif

#include "Result.hpp"
#include "Types.hpp"
#include "Utility.hpp"
#include "UniquePtr.hpp"

namespace hsd
{
    template <typename T>
    concept Copyable = std::is_copy_constructible_v<T>;

    struct bad_any_cast
    {
        const char* operator()() 
        {
            return "Illegal casting:\n"
            "Cannot cast into different type";
        }
    };

    class _any_base 
    {
    public:
        virtual ~_any_base() = default;
        virtual hsd::unique_ptr<_any_base> clone() const = 0;

        #ifdef HSD_ANY_ENABLE_TYPEINFO
        virtual const std::type_info& get_typeinfo() const noexcept = 0;
        #endif
    };

    template <Copyable T>
    class _any_derived : public _any_base 
    {
        T _value;
        friend class any;

    public:
        HSD_CONSTEXPR _any_derived(T value) 
            : _value(move(value)) 
        {}

        hsd::unique_ptr<_any_base> clone() const override 
        {
            return hsd::make_unique<_any_derived>(_value);
        }

        #ifdef HSD_ANY_ENABLE_TYPEINFO
        const std::type_info& get_typeinfo() const noexcept override 
        {
            return typeid(T);
        }
        #endif
    };

    class any
    {
    private:
        hsd::unique_ptr<_any_base> _data = nullptr;

    public:
        HSD_CONSTEXPR any() noexcept = default;

        template <Copyable T>
        HSD_CONSTEXPR any(T other)
        {
            _data = hsd::make_unique<_any_derived<T>>(move(other));
        }

        any(const any& other)
        {
            _data = other._data->clone();
        }

        any(any&& other)
        {
            this->swap(other);
        }

        ~any() = default;

        any& operator=(any rhs)
        {
            this->swap(rhs);
            return *this;
        }

        template <typename T>
        HSD_CONSTEXPR auto cast_to() const
            -> Result<reference<T>, bad_any_cast>
        {
            using type = typename std::remove_pointer<T>::type;

            if(auto* p_base = dynamic_cast<_any_derived<type>*>(_data.get()))
            {
                return static_cast<T>(p_base->_value);
            }
            else
            {
                return bad_any_cast{};
            }
        }

        template <typename T>
        T* cast_if() const
        {
            if (auto* p_base = dynamic_cast<_any_derived<T>*>(_data.get()))
                return &p_base->_value;

            return nullptr;
        }

        template <typename T>
        bool holds_type() const
        {
            return dynamic_cast<_any_derived<T>*>(_data.get());
        }

        #ifdef HSD_ANY_ENABLE_TYPEINFO
        const std::type_info& type() const noexcept
        {
            return _data != nullptr ? _data->get_typeinfo() : typeid(void);
        }
        #endif

        void swap(any& other) noexcept
        {
            hsd::swap(_data, other._data);
        }

        friend void swap(any& lhs, any& rhs) noexcept
        {
            lhs.swap(rhs);
        }

        template < typename T, typename... Args >
        void emplace(Args&&... args)
        {
            _data = hsd::make_unique<_any_derived<T>>(T(forward<Args>(args)...));
        }

        bool has_value() const
        {
            return _data != nullptr;
        }

        HSD_CONSTEXPR void reset()
        {
            _data = nullptr;
        }
    };
}
