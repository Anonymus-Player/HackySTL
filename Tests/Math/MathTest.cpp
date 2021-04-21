#include <Math.hpp>
#include <stdio.h>
#include <Io.hpp>

int main()
{
    {
        constexpr hsd::f64 val1 = hsd::math::sqrt(5.0);
        hsd::f64 val2 = std::sqrt(5.0);
        hsd::io::print<"sqrt: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::abs(-5.0);
        hsd::f64 val2 = std::abs(-5.0);
        hsd::io::print<"abs: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::ceil(5.22);
        hsd::f64 val2 = std::ceil(5.22);
        hsd::io::print<"ceil: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::cos(5.0);
        hsd::f64 val2 = std::cos(5.0);
        hsd::io::print<"cos: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::exp(5.0);
        hsd::f64 val2 = std::exp(5.0);
        hsd::io::print<"exp: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::floor(-5.22);
        hsd::f64 val2 = std::floor(-5.22);
        hsd::io::print<"floor: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::log10(5.0);
        hsd::f64 val2 = std::log10(5.0);
        hsd::io::print<"log10: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::log2(5.0);
        hsd::f64 val2 = std::log2(5.0);
        hsd::io::print<"log2: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::log(5.0);
        hsd::f64 val2 = std::log(5.0);
        hsd::io::print<"log: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::max(5.0, 21.0);
        hsd::f64 val2 = std::max(5.0, 21.0);
        hsd::io::print<"max: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::min(5.0, 21.0);
        hsd::f64 val2 = std::min(5.0, 21.0);
        hsd::io::print<"min: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::mod(5.0, 1.3);
        hsd::f64 val2 = std::fmod(5.0, 1.3);
        hsd::io::print<"mod: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::pow(5.0, 2.33);
        hsd::f64 val2 = std::pow(5.0, 2.33);
        hsd::io::print<"pow: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::round(-5.232);
        hsd::f64 val2 = std::round(-5.232);
        hsd::io::print<"round: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::sin(5.0);
        hsd::f64 val2 = std::sin(5.0);
        hsd::io::print<"sin: {} {}\n">(val1, val2);
    }
    
    {
        constexpr hsd::f64 val1 = hsd::math::tan(5.0);
        hsd::f64 val2 = std::tan(5.0);
        hsd::io::print<"tan: {} {}\n">(val1, val2);
    }
}