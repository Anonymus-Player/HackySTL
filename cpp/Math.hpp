#pragma once

#include "IntegerSequence.hpp"
#include "Limits.hpp"
#include "Concepts.hpp"
#include <cmath>

namespace hsd
{
    namespace math
    {
        namespace constants
        {
            static constexpr f128 pi = 3.14159265358979323846264338327950288l;
            static constexpr f128 euler = 2.71828182845904523536028747135266249l;
            static constexpr f128 ln_table[9] = {
                0.6931471805599453094172321214581765680755l,
                1.0986122886681096913952452369225257046475l,
                1.3862943611198906188344642429163531361510l,
                1.6094379124341003746007593332261876395256l,
                1.7917594692280550008124773583807022727230l,
                1.9459101490553133051053527434431797296371l,
                2.0794415416798359282516963643745297042265l,
                2.1972245773362193827904904738450514092950l,
                2.3025850929940456840179914546843642076011l
            };
        } // namespace constants
        
        template <typename T>
        concept Middleable = is_number<T>::value ||
            is_same<remove_cv_t<T>, T>::value ||
            negation<is_same<T, bool>>::value;

        template <Middleable T>
        constexpr auto midpoint(T a, T b);
        template <IsObject T>
        constexpr auto midpoint(T* a, T* b);
        template <IsNumber T>
        static constexpr auto abs(const T& value);

        namespace constexpr_math
        {
            template <IsFloat T>
            static constexpr auto floor(const T& value);
            template <IsNumber T>
            static constexpr auto exp(T value);
            template <typename T>
            static constexpr auto log(T value);

            namespace detail
            {
                template <IsNumber T>
                static constexpr auto sgn(const T& value)
                {
                    if(value != 0)
                    {
                        return value < 0 ? -1 : 1;
                    }
                    else
                    {
                        return 0;
                    }
                }

                template <IsFloat T>
                static constexpr auto find_fraction(const T& value)
                {
                    if(math::abs(value - constexpr_math::floor(value)) >= static_cast<T>(0.5))
                    {
                        return value - constexpr_math::floor(value) - sgn(value);
                    }
                    else
                    {
                        return value - constexpr_math::floor(value);
                    }
                }

                template <IsFloat T>
                static constexpr auto find_whole(const T& value)
                {
                    if(math::abs(value - constexpr_math::floor(value)) >= static_cast<T>(0.5))
                    {
                        return static_cast<i64>(constexpr_math::floor(value) + sgn(value));
                    }
                    else
                    { 
                        return static_cast<i64>(constexpr_math::floor(value));
                    }
                }

                template < typename T1, IsIntegral T2 >
                static constexpr T1 pow_int_compute_rec(
                    const T1& base, const T1& val, const T2& exp_term)
                {
                    if(exp_term > static_cast<T2>(1))
                    {
                        if((exp_term & 1U) != 0)
                        {
                            return pow_int_compute_rec(base * base, val * base, exp_term / 2);
                        }
                        else
                        {
                            return pow_int_compute_rec(base * base, val, exp_term / 2);
                        }
                    }
                    else
                    {
                        return exp_term == static_cast<T2>(1) ? val * base : val;
                    }         
                }

                template < typename T1, IsUnsigned T2 >
                static constexpr auto pow_int_sgn_check(const T1& base, const T2& exp_term)
                {
                    return pow_int_compute_rec(base, static_cast<T1>(1), exp_term);
                }

                template < typename T1, IsIntegral T2 >
                static constexpr auto pow_int_compute(const T1& base, const T2& exp_term)
                {
                    if(exp_term == 3)
                    {
                        return base * base * base;
                    }
                    else if(exp_term == 2)
                    {
                        return base * base;
                    }
                    else if(exp_term == 1)
                    {
                        return base;
                    }
                    else if(exp_term == 0)
                    {
                        return static_cast<T1>(1);
                    }
                    else if(exp_term == limits<T2>::min)
                    {
                        return static_cast<T1>(0);
                    }
                    else if(exp_term == limits<T2>::max)
                    {
                        return limits<T1>::infinity;
                    }
                    else 
                    {
                        return pow_int_sgn_check(base, exp_term);
                    }
                }

                template < typename T1, IsIntegral T2 >
                static constexpr auto pow_int_type_check(const T1& base, const T2& exp_term)
                {
                    return pow_int_compute(base, exp_term);
                }

                template < typename T1, IsFloat T2 >
                static constexpr auto pow_int_type_check(const T1& base, const T2& exp_term)
                {
                    return pow_int_compute(base, static_cast<i64>(exp_term));
                }

                template <IsNumber T>
                static constexpr auto pow_integral(const T& base, const usize& exp)
                {
                    return pow_int_type_check(base, exp);                
                }

                template < usize Depth, usize MaxDepth, IsNumber T >
                static constexpr auto exp_cf_rec(const T& value)
                {
                    if constexpr(Depth < MaxDepth)
                    {
                        if constexpr(Depth == 1)
                        {
                            return static_cast<T>(1) - value / 
                                exp_cf_rec< Depth + 1, MaxDepth >(value);
                        }
                        else
                        {
                            return static_cast<T>(1) + value / static_cast<T>(Depth - 1) - 
                                value / Depth / exp_cf_rec< Depth + 1, MaxDepth >(value);
                        }
                    }
                    else
                    {
                        return static_cast<T>(1);
                    }
                }

                template <typename T>
                static constexpr auto exp_cf(const T value)
                {
                    return static_cast<T>(1) / exp_cf_rec<1, 25>(value);
                }

                template <typename T>
                static constexpr auto exp_split(const T value)
                {
                    return pow_integral(
                        constants::euler, find_whole(value)) * 
                        exp_cf(find_fraction(value)
                    );
                }

                template <IsNumber T>
                static constexpr T mantissa(const T& value)
                {
                    if(value < static_cast<T>(1))
                    {
                        return mantissa(value * static_cast<T>(10));
                    }
                    else if(value > static_cast<T>(10))
                    {
                        return mantissa(value / static_cast<T>(10));
                    }
                    else
                    {
                        return value;
                    }
                }

                template <typename T>
                static constexpr i64 find_exponent(const T value, const i64& exponent)
                {
                    if(value < static_cast<T>(1))
                    {
                        return find_exponent(value * static_cast<T>(10), exponent - static_cast<i64>(1));
                    }
                    else if(value > static_cast<T>(10))
                    {
                        return find_exponent(value / static_cast<T>(10) , exponent + static_cast<i64>(1));
                    }
                    else
                    {
                        return exponent;
                    }
                }

                template <IsNumber T>
                static constexpr auto tan_series_exp_long(const T& value)
                {   
                    return (
                        -1 / value + (value / 3 + (pow_integral(value, 3) / 45 + 
                        (2 * pow_integral(value, 5) / 945 + pow_integral(value, 7) / 4725)))
                    );
                }

                template <IsNumber T>
                static constexpr auto tan_series_exp(const T& value)
                {
                    using constants::pi;
                    if(limits<T>::epsilon > math::abs(value - static_cast<T>(pi / 2)))
                    {
                        return static_cast<T>(pi / 2);
                    }
                    else 
                    {
                        return tan_series_exp_long(value - static_cast<T>(pi / 2));
                    }
                }

                template < usize Depth, usize MaxDepth, IsNumber T >
                static constexpr auto tan_cf_rec(const T& value)
                {
                    if constexpr(Depth < MaxDepth)
                    {
                        return static_cast<T>(2 * Depth - 1) - value / 
                            tan_cf_rec< Depth + 1, MaxDepth >(value);
                    }
                    else
                    {
                        return static_cast<T>(2 * MaxDepth - 1);
                    }
                }

                template <IsNumber T>
                static constexpr auto tan_cf_main(const T& value)
                {
                    if(value > static_cast<T>(1.55) && value < static_cast<T>(1.60))
                    {
                        return tan_series_exp(value);
                    }
                    else if(value > static_cast<T>(1.4))
                    {
                        return value / tan_cf_rec<1, 45>(value * value);
                    }
                    else if(value > static_cast<T>(1))
                    {
                        return value / tan_cf_rec<1, 35>(value * value);
                    }
                    else
                    {
                        return value / tan_cf_rec<1, 25>(value * value);
                    }
                }

                template <IsNumber T>
                static constexpr auto tan_begin(T value)
                {
                    using constants::pi;

                    if(value > static_cast<T>(pi))
                    {
                        value -= static_cast<T>(pi) * floor(value / static_cast<T>(pi));

                        if(value > static_cast<T>(pi))
                        {
                            return limits<T>::nan;     
                        }
                        else
                        {
                            return tan_cf_main(value);
                        }        
                    }
                    else
                    {
                        return tan_cf_main(value);
                    }
                }

                template <IsFloat T>
                static constexpr bool invlaid_case(const T& value)
                {
                    return (
                        value == limits<T>::infinity || 
                        value == -limits<T>::infinity || 
                        value == limits<T>::nan
                    );
                }

                template <IsNumber T>
                static constexpr auto sin_compute(const T& value)
                {
                    return static_cast<T>(2) * value / (static_cast<T>(1) + value * value);
                }

                template <IsNumber T>
                static constexpr auto cos_compute(const T& value)
                {
                    return (static_cast<T>(1) - value * value) / (static_cast<T>(1) + value * value);
                }

                template < usize Depth, usize MaxDepth, IsNumber T >
                static constexpr auto log_cf_main(const T& value)
                {
                    if constexpr(Depth < MaxDepth)
                    {
                        return static_cast<T>(2 * Depth - 1) - 
                        static_cast<T>(Depth * Depth) * value / 
                        log_cf_main< Depth + 1, MaxDepth >(value);
                    }
                    else
                    {
                        return static_cast<T>(2 * Depth - 1);
                    }
                }

                template <IsNumber T>
                static constexpr auto log_cf_begin(const T& value)
                { 
                    return static_cast<T>(2) * value / log_cf_main<1, 25>(value * value);
                }

                template <IsNumber T>
                static constexpr auto log_main(const T& value)
                { 
                    return log_cf_begin((value - static_cast<T>(1)) / (value + static_cast<T>(1)));
                }

                static constexpr f128 log_mantissa_integer(const i32& value)
                {
                    if(value > 1 && value < 11)
                    {
                        return constants::ln_table[value - 2];
                    }
                    else
                    {
                        return 0.l;
                    }
                }

                template <IsNumber T>
                static constexpr auto log_mantissa(const T& value)
                {   
                    return log_main(value / static_cast<T>(static_cast<i32>(value))) + 
                        static_cast<T>(log_mantissa_integer(static_cast<i32>(value)));
                }

                template <IsNumber T>
                static constexpr auto log_breakup(const T& value)
                {   
                    return log_mantissa(mantissa(value)) + 
                        static_cast<T>(constants::ln_table[8]) * 
                        static_cast<T>(find_exponent(value, 0));
                }

                template <typename T>
                static constexpr auto pow_dbl(const T& base, const T& exp_term)
                {
                    return constexpr_math::exp(exp_term * constexpr_math::log(base));
                }

                template <typename T1, IsFloat T2>
                static constexpr auto pow_type_check(const T1& base, const T2& exp_term)
                {
                    using ReturnType = std::common_type_t<T1, T2>;

                    if(base < static_cast<T1>(0))
                    {
                        return limits<ReturnType>::nan;
                    }
                    else
                    {
                        return pow_dbl(
                            static_cast<ReturnType>(base), 
                            static_cast<ReturnType>(exp_term)
                        );
                    }
                }

                template <typename T1, IsIntegral T2>
                static constexpr auto pow_type_check(const T1& base, const T2& exp_term)
                    -> std::common_type_t<T1, T2>
                {
                    return pow_integral(base, exp_term);
                }
            }

            template <IsFloat T>
            static constexpr auto mod(const T& value, const T& factor)
            {
                if(detail::invlaid_case(value))
                {
                    return value;
                }
                else
                {
                    i64 _rez = static_cast<i64>(value / factor);
                    return value - static_cast<T>(_rez) * factor;
                }
            }

            template <IsFloat T>
            static constexpr auto floor(const T& value)
            {
                if(detail::invlaid_case(value))
                {
                    return value;
                }
                else
                {
                    i64 _rez = static_cast<i64>(value);
                    return static_cast<T>(_rez - (value < 0 && 
                        constexpr_math::mod(value, static_cast<T>(1)) != 0)
                    );
                }
            }

            template <IsFloat T>
            static constexpr auto ceil(const T& value)
            {
                if(detail::invlaid_case(value))
                {
                    return value;
                }
                else
                {
                    i64 _res = static_cast<i64>(value);

                    return static_cast<T>(_res + (value > 0 && 
                        constexpr_math::mod(value, static_cast<T>(1)) != 0)
                    );
                }
            }

            template <IsFloat T>
            static constexpr auto round(const T& value)
            {
                if(detail::invlaid_case(value))
                {
                    return value;
                }
                else
                {
                    return floor(value) + 
                        (value > 0 && constexpr_math::mod(value, static_cast<T>(1)) >= 0.5) + 
                        (value < 0 && constexpr_math::mod(1 + 
                            constexpr_math::mod(value, static_cast<T>(1)), static_cast<T>(1)) >= 0.5
                    );
                }
            }

            template <IsNumber T>
            static constexpr auto tan(T value)
            {
                if(value == limits<T>::nan)
                {
                    return value;
                }
                else if(limits<T>::epsilon > math::abs(value))
                {
                    return static_cast<T>(0);
                }
                else
                {
                    return value < 0 ? -detail::tan_begin(-value) : detail::tan_begin(value);
                }
            }

            template <IsNumber T>
            static constexpr auto sin(T value)
            {
                using constants::pi;
                if(value == limits<T>::nan)
                {
                    return value;
                }
                else if(limits<T>::epsilon > math::abs(value))
                {
                    return static_cast<T>(0);
                }
                else if(limits<T>::epsilon > math::abs(value - static_cast<T>(pi / 2)))
                {
                    return static_cast<T>(1);
                }
                else if(limits<T>::epsilon > math::abs(value + static_cast<T>(pi / 2)))
                {
                    return -static_cast<T>(1);
                }
                else if(limits<T>::epsilon > math::abs(value - static_cast<T>(pi)))
                {
                    return static_cast<T>(0);
                }
                else if(limits<T>::epsilon > math::abs(value + static_cast<T>(pi)))
                {
                    return -static_cast<T>(0);
                }
                else
                {
                    return detail::sin_compute(constexpr_math::tan(value / static_cast<T>(2)));
                }
            }

            template <IsNumber T>
            static constexpr auto cos(T value)
            {
                using constants::pi;
                if(value == limits<T>::nan)
                {
                    return value;
                }
                else if(limits<T>::epsilon > math::abs(value))
                {
                    return static_cast<T>(1);
                }
                else if(limits<T>::epsilon > math::abs(value - static_cast<T>(pi / 2)))
                {
                    return static_cast<T>(0);
                }
                else if(limits<T>::epsilon > math::abs(value + static_cast<T>(pi / 2)))
                {
                    return static_cast<T>(0);
                }
                else if(limits<T>::epsilon > math::abs(value - static_cast<T>(pi)))
                {
                    return -static_cast<T>(1);
                }
                else if(limits<T>::epsilon > math::abs(value + static_cast<T>(pi)))
                {
                    return -static_cast<T>(1);
                }
                else
                {
                    return detail::cos_compute(constexpr_math::tan(value / static_cast<T>(2)));
                }
            }

            template <typename T>
            static constexpr auto log(T value)
            {
                if(value == limits<T>::nan || value < static_cast<T>(0))
                {
                    return limits<T>::nan;
                }
                else if(limits<T>::epsilon > value)
                {
                    return -limits<T>::infinity;
                }
                else if(limits<T>::epsilon > math::abs(value - static_cast<T>(1)))
                {
                    return static_cast<T>(0);
                }
                else if(value == limits<T>::infinity)
                {
                    return limits<T>::infinity;
                }
                else
                {
                    if(value < static_cast<T>(0.5) || value > static_cast<T>(1.5))
                    {
                        return detail::log_breakup(value);
                    }
                    else
                    {
                        return detail::log_main(value);
                    }
                }  
            }

            template <typename T>
            static constexpr auto log2(T value)
            {
                if(value == limits<T>::nan || value < static_cast<T>(0))
                {
                    return limits<T>::nan;
                }
                else if(limits<T>::epsilon > value)
                {
                    return -limits<T>::infinity;
                }
                else if(limits<T>::epsilon > math::abs(value - static_cast<T>(1)))
                {
                    return static_cast<T>(0);
                }
                else if(value == limits<T>::infinity)
                {
                    return limits<T>::infinity;
                }
                else
                {
                    return log(value) / static_cast<T>(constants::ln_table[0]);
                }  
            }

            template <typename T>
            static constexpr auto log10(T value)
            {
                if(value == limits<T>::nan || value < static_cast<T>(0))
                {
                    return limits<T>::nan;
                }
                else if(limits<T>::epsilon > value)
                {
                    return -limits<T>::infinity;
                }
                else if(limits<T>::epsilon > math::abs(value - static_cast<T>(1)))
                {
                    return static_cast<T>(0);
                }
                else if(value == limits<T>::infinity)
                {
                    return limits<T>::infinity;
                }
                else
                {
                    return log(value) / static_cast<T>(constants::ln_table[8]);
                }  
            }

            template <IsNumber T>
            static constexpr auto exp(T value)
            {
                if(value == limits<T>::nan || value == limits<T>::infinity)
                {
                    return value;
                }
                else if(value == -limits<T>::infinity)
                {
                    return static_cast<T>(0);
                }
                else if(limits<T>::epsilon > math::abs(value))
                {
                    return static_cast<T>(1);
                }
                else if(math::abs(value) < static_cast<T>(2))
                {
                    return static_cast<T>(detail::exp_cf(value));
                }
                else
                {
                    return static_cast<T>(detail::exp_split(value));
                }
            }

            template <typename T1, typename T2>
            static constexpr auto pow(T1 base, T2 exp_term)
            {
                return detail::pow_type_check(base,exp_term);
            }

            template <IsFloat T>
            static constexpr auto sqrt(const T& value)
            {
                auto s = static_cast<T>(1.0);

                if(value <= static_cast<T>(-0.0))
                {
                    return limits<T>::nan;
                }
                for(usize i = 0; i < 10; i++)
                {
                    auto e = value / s;
                    s = midpoint(s, e);
                }

                return s;
            }
        } // namespace constexpr_math

        template <Middleable T>
        constexpr auto midpoint(T a, T b)
        {
            if constexpr(hsd::is_integral<T>::value)
            {
                if(a > b)
                {
                    return a - 1 * static_cast<T>(static_cast<usize>(a - b) / 2);
                }
                else
                {
                    return a + 1 * static_cast<T>(static_cast<usize>(b - a) / 2);
                }
            }
            else
            {
                constexpr auto _low_limit = limits<T>::min * 2;
                constexpr auto _high_limit = limits<T>::max / 2;
                const auto _abs_a = math::abs(a);
                const auto _abs_b = math::abs(b);

                if(_abs_a <= _high_limit && _abs_b <= _high_limit)
                {
                    return (a + b) / 2;
                }
                else if(_abs_a < _low_limit)
                {
                    return a + b / 2;
                }
                else if(_abs_b < _low_limit)
                {
                    return a / 2 + b;
                }
                else
                {
                    return a / 2 + b / 2;
                }
            }
        }

        template <IsObject T>
        constexpr T* midpoint(T* a, T* b)
        {
            static_assert(sizeof(T) != 0, "type must be complete");
            return a + (b - a) / 2;
        }

        template <IsFloat T>
        static constexpr auto mod(const T& value, const T& factor)
        {
            if(std::is_constant_evaluated())
            {
                return constexpr_math::mod(value, factor);
            }
            else
            {
                return std::fmod(value, factor);
            }
        }

        template <IsFloat T>
        static constexpr auto floor(const T& value)
        {
            if(std::is_constant_evaluated())
            {
                return constexpr_math::floor(value);
            }
            else
            {
                return std::floor(value);
            }
        }

        template <IsFloat T>
        static constexpr auto ceil(const T& value)
        {
            if(std::is_constant_evaluated())
            {
                return constexpr_math::ceil(value);
            }
            else
            {
                return std::ceil(value);
            }
        }

        template <IsFloat T>
        static constexpr auto round(const T& value)
        {
            if(std::is_constant_evaluated())
            {
                return constexpr_math::round(value);
            }
            else
            {
                return std::round(value);
            }
        }

        template <IsNumber T>
        static constexpr auto abs(const T& value)
        {
            return value < 0 ? -value : value;
        }

        template <IsNumber T1>
        static constexpr T1 min(const T1& first, const T1& second)
        {
            return first < second ? first : second;
        }

        template <IsNumber T1>
        static constexpr T1 max(const T1& first, const T1& second)
        {
            return first > second ? first : second;
        }

        template <IsNumber T>
        static constexpr auto tan(T value)
        {
            if(std::is_constant_evaluated())
            {
                return constexpr_math::tan(value);
            }
            else
            {
                return std::tan(value);
            }
        }

        template <IsNumber T>
        static constexpr auto sin(T value)
        {
            if(std::is_constant_evaluated())
            {
                return constexpr_math::sin(value);
            }
            else
            {
                return std::sin(value);
            }
        }

        template <IsNumber T>
        static constexpr auto cos(T value)
        {
            if(std::is_constant_evaluated())
            {
                return constexpr_math::cos(value);
            }
            else
            {
                return std::cos(value);
            }
        }

        template <typename T>
        static constexpr auto log(T value)
        {
            if(std::is_constant_evaluated())
            {
                return constexpr_math::log(value);
            }
            else
            {
                return std::log(value);
            }
        }

        template <typename T>
        static constexpr auto log2(T value)
        {
            if(std::is_constant_evaluated())
            {
                return constexpr_math::log2(value);
            }
            else
            {
                return std::log2(value);
            } 
        }

        template <typename T>
        static constexpr auto log10(T value)
        {
            if(std::is_constant_evaluated())
            {
                return constexpr_math::log10(value);
            }
            else
            {
                return std::log10(value);
            }
        }

        template <IsNumber T>
        static constexpr auto sqrt(T value)
        {
            if(std::is_constant_evaluated())
            {
                return constexpr_math::sqrt(value);
            }
            else
            {
                return std::sqrt(value);
            }
        }

        template <IsNumber T>
        static constexpr auto exp(T value)
        {
            if(std::is_constant_evaluated())
            {
                return constexpr_math::exp(value);
            }
            else
            {
                return std::exp(value);
            }
        }
        
        template < typename T1, typename T2 >
        static constexpr auto pow(T1 base, T2 exp_term)
        {
            if(std::is_constant_evaluated())
            {
                return constexpr_math::pow(base, exp_term);
            }
            else
            {
                return std::pow(base, exp_term);
            }
        }
    } // namespace math
} // namespace hsd