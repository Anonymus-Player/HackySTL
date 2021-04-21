#pragma once

#include "Limits.hpp"
#include "Result.hpp"

namespace hsd
{
    template < 
        class UType, UType w, UType n, 
        UType m, UType r, UType a, UType u, 
        UType d, UType s, UType b, UType t,
        UType c, UType l, UType f >
    requires (is_unsigned<UType>::value)
    class mersenne_twister
    {
    public:
        static constexpr UType word_size = w;
        static constexpr UType state_size = n;
        static constexpr UType shift_size = m;
        static constexpr UType mask_bits = r;
        static constexpr UType xor_mask = a;
        static constexpr UType tempering_u = u;
        static constexpr UType tempering_d = d;
        static constexpr UType tempering_s = s;
        static constexpr UType tempering_b = b;
        static constexpr UType tempering_t = t;
        static constexpr UType tempering_c = c;
        static constexpr UType tempering_l = l;
        static constexpr UType init_multiplier = f;
        static constexpr UType default_seed = 5489u;

    private:
        UType _mt[state_size]{};
        UType _index = state_size + 1;
        static constexpr UType _lower_mask = 1 << mask_bits - 1;
        static constexpr UType _upper_mask = !(1 << mask_bits - 1);

        void _twist()
        {
            for(UType _i = 0; _i < state_size; _i++)
            {
                UType _x = (_mt[_i] & _upper_mask) +
                    (_mt[(_i + 1) % state_size] & _lower_mask);

                UType _twist_xor_mask = _x % 2 == 0 ? 
                    _x >> 1 : (_x >> 1) ^ xor_mask;
                
                _mt[_i] = _mt[(_i + shift_size) % state_size] ^ _twist_xor_mask;
            }

            _index = 0;
        }

    public:
        constexpr mersenne_twister(UType seed = default_seed)
            : _index{state_size}
        {
            _mt[0] = seed;

            for(UType _i = 1; _i < state_size; _i++)
            {
                _mt[_i] = init_multiplier * (_mt[_i - 1] ^ 
                    (_mt[_i - 1] >> (word_size - 2))) + _i;
            }
        }

        constexpr ~mersenne_twister() = default;

        // This one can "throw" an error
        template <typename RetType>
        requires (is_number<RetType>::value)
        constexpr auto generate(RetType from, RetType to)
            -> Result<RetType, runtime_error>
        {
            if(from >= to)
                return runtime_error{"Invalid interval"};
            
            if(_index == state_size)
                _twist();

            UType _result = _mt[_index];
            _result = _result ^ ((_result >> tempering_u) & tempering_d);
            _result = _result ^ ((_result >> tempering_s) & tempering_b);
            _result = _result ^ ((_result >> tempering_t) & tempering_c);
            _result = _result ^ (_result >> tempering_l);
            _index++;

            if constexpr(word_size == 64)
            {
                auto _multiplier = static_cast<f64>(_result) / static_cast<f64>(limits<UType>::max);
                return (to - from) * static_cast<RetType>(_multiplier) + from;
            }
            else
            {
                auto _multiplier = static_cast<f32>(_result) / static_cast<f32>(limits<UType>::max);
                return (to - from) * static_cast<RetType>(_multiplier) + from;
            }
        }
        
        // This one can't "throw" an error
        constexpr UType generate()
        {            
            if(_index == state_size)
                _twist();

            UType _result = _mt[_index];
            _result = _result ^ ((_result >> tempering_u) & tempering_d);
            _result = _result ^ ((_result >> tempering_s) & tempering_b);
            _result = _result ^ ((_result >> tempering_t) & tempering_c);
            _result = _result ^ (_result >> tempering_l);
            _index++;

            return _result;
        }
    };

    using mt19937_32 = mersenne_twister< 
        u32, 32, 624, 397, 31, 0x9908b0df, 
        11, 0xffffffff, 7, 0x9d2c5680, 15,
        0xefc60000, 18, 1812433253 >; 	

    using mt19937_64 = mersenne_twister< 
        u64, 64, 312, 156, 31, 0xb5026f5aa96619e9, 29,
        0x5555555555555555, 17, 0x71d67fffeda60000, 37,
        0xfff7eee000000000, 43, 6364136223846793005 >;
} // namespace hsd
