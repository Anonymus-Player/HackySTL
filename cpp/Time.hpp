#pragma once

#include "TypeTraits.hpp"
#include "_Define.hpp"

#include <time.h>
#include <wchar.h>

namespace hsd
{  
    class time
    {
    private:
        using time_ptr = tm*;
        time_t _epoch_time;
        time_ptr _time;

    public:
        time()
            : _epoch_time{::time(nullptr)},
            _time{localtime(&_epoch_time)}
        {}

        time& update()
        {
            _epoch_time = ::time(nullptr);
            _time = localtime(&_epoch_time);

            return *this;
        }

        u16 second()
        {
            return static_cast<u16>(_time->tm_sec);
        }

        u16 minute()
        {
            return static_cast<u16>(_time->tm_min);
        }

        u16 hour()
        {
            return static_cast<u16>(_time->tm_hour);
        }

        u16 month_day()
        {
            return static_cast<u16>(_time->tm_mday);
        }
        
        u16 week_day()
        {
            return static_cast<u16>(_time->tm_wday);
        }

        u16 month()
        {
            return static_cast<u16>(_time->tm_mon);
        }

        #ifdef HSD_PLATFORM_POSIX
        const char* timezone()
        {
            return _time->tm_zone;
        }
        #endif

        u16 year_day()
        {
            return static_cast<u16>(_time->tm_yday);
        }

        usize year()
        {
            return static_cast<usize>(_time->tm_year) + 1900;
        }

        const char* to_text()
        {
            return asctime(_time);
        }
    };

    class clock
    {
    private:
        i64 _clk;

    public:
        clock(clock_t clk = ::clock())
        {
            _clk = clk;
        }

        clock(const clock& rhs)
            : _clk(rhs._clk)
        {}

        clock& operator=(const clock& rhs)
        {
            _clk = rhs._clk;
            return *this;
        }

        friend clock operator-(const clock& lhs, const clock& rhs)
        {
            return clock{static_cast<clock_t>(lhs._clk - rhs._clk)};
        }

        friend clock operator+(const clock& lhs, const clock& rhs)
        {
            return clock{static_cast<clock_t>(lhs._clk + rhs._clk)};
        }

        bool operator==(const clock& rhs)
        {
            return _clk == rhs._clk;
        }

        bool operator!=(const clock& rhs)
        {
            return _clk != rhs._clk;
        }

        bool operator<(const clock& rhs)
        {
            return _clk < rhs._clk;
        }

        bool operator>(const clock& rhs)
        {
            return _clk > rhs._clk;
        }

        bool operator<=(const clock& rhs)
        {
            return _clk <= rhs._clk;
        }

        bool operator>=(const clock& rhs)
        {
            return _clk >= rhs._clk;
        }

        i64 to_microseconds()
        {
            return static_cast<i64>(to_seconds() * 1000000);
        }

        i32 to_miliseconds()
        {
            return static_cast<i32>(to_seconds() * 1000);
        }

        f32 to_seconds()
        {
            return static_cast<f32>(_clk) / CLOCKS_PER_SEC;
        }

        clock restart()
        {
            clock _old_clk{static_cast<clock_t>(_clk)};
            _clk = ::clock();
            return *this - _old_clk;
        }

        clock elapsed_time()
        {
            clock _new_clk;
            return _new_clk - *this;
        }

        template < typename Func, typename... Args >
        static void sleep_for(f32 seconds, Func&& handle, Args&&... args)
        {
            clock _clk;

            while(_clk.elapsed_time().to_seconds() < seconds)
                handle(forward<Args>(args)...);
        }
    };

    class precise_clock
    {
    private:
        timespec _clk;

    public:
        precise_clock()
        {
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &_clk);
        }

        precise_clock(const timespec& clk)
            : _clk{clk}
        {}

        precise_clock(const precise_clock& rhs)
            : _clk{rhs._clk}
        {}

        precise_clock& operator=(const precise_clock& rhs)
        {
            _clk = rhs._clk;
            return *this;
        }

        friend precise_clock operator-(const precise_clock& lhs, const precise_clock& rhs)
        {
            if(lhs._clk.tv_nsec - rhs._clk.tv_nsec < 0)
            {
                return timespec {
                    lhs._clk.tv_sec - rhs._clk.tv_sec - 1,
                    lhs._clk.tv_nsec - rhs._clk.tv_nsec + 1'000'000'000  
                };
            }
            else
            {
                return timespec {
                    lhs._clk.tv_sec - rhs._clk.tv_sec,
                    lhs._clk.tv_nsec - rhs._clk.tv_nsec  
                };
            }
        }

        friend precise_clock operator+(const precise_clock& lhs, const precise_clock& rhs)
        {
            if(lhs._clk.tv_nsec + rhs._clk.tv_nsec >= 1'000'000'000)
            {
                return timespec {
                    lhs._clk.tv_sec + rhs._clk.tv_sec + 1,
                    lhs._clk.tv_nsec + rhs._clk.tv_nsec - 1'000'000'000  
                };
            }
            else
            {
                return timespec {
                    lhs._clk.tv_sec + rhs._clk.tv_sec + 1,
                    lhs._clk.tv_nsec + rhs._clk.tv_nsec
                };
            }
        }

        bool operator==(const precise_clock& rhs)
        {
            return (
                _clk.tv_sec == rhs._clk.tv_sec &&
                _clk.tv_nsec == rhs._clk.tv_nsec
            );
        }

        bool operator!=(const precise_clock& rhs)
        {
            return (
                _clk.tv_sec != rhs._clk.tv_sec ||
                _clk.tv_nsec != rhs._clk.tv_nsec
            );
        }

        bool operator<(const precise_clock& rhs)
        {
            if(_clk.tv_sec < rhs._clk.tv_sec)
            {
                return true;
            }
            else if(_clk.tv_sec > rhs._clk.tv_sec)
            {
                return false;
            }
            else
            {
                return _clk.tv_nsec < rhs._clk.tv_nsec;
            }
        }

        bool operator>(const precise_clock& rhs)
        {
            if(_clk.tv_sec > rhs._clk.tv_sec)
            {
                return true;
            }
            else if(_clk.tv_sec < rhs._clk.tv_sec)
            {
                return false;
            }
            else
            {
                return _clk.tv_nsec > rhs._clk.tv_nsec;
            }
        }

        bool operator<=(const precise_clock& rhs)
        {
            return *this < rhs || *this == rhs;
        }

        bool operator>=(const precise_clock& rhs)
        {
            return *this > rhs || *this == rhs;
        }

        u64 to_nanoseconds()
        {
            if(_clk.tv_sec != 0)
            {
                return static_cast<u64>(to_seconds() * 1'000'000'000);
            }
            else
            {
                return static_cast<u64>(_clk.tv_nsec);
            }
        }

        i64 to_microseconds()
        {
            return static_cast<i64>(to_seconds() * 1'000'000);
        }

        i32 to_miliseconds()
        {
            return static_cast<i32>(to_seconds() * 1000);
        }

        f64 to_seconds()
        {
            return _clk.tv_nsec / 1'000'000'000.0 + 
                static_cast<f64>(_clk.tv_sec);
        }

        precise_clock restart()
        {
            precise_clock _old_clk{_clk};
            clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &_clk);
            return *this - _old_clk;
        }

        precise_clock elapsed_time()
        {
            precise_clock _new_clk;
            return _new_clk - *this;
        }

        template < typename Func, typename... Args >
        static void sleep_for(f32 seconds, Func&& handle, Args&&... args)
        {
            precise_clock _clk;

            while(_clk.elapsed_time().to_seconds() < seconds)
                handle(forward<Args>(args)...);
        }
    };
} // namespace hsd