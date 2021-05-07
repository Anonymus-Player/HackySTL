#pragma once

#include <new>

#include "Vector.hpp"
#include "Atomic.hpp"


namespace hsd
{
    
    using counter = atomic_usize;

    namespace priv
    {
        template <typename T>
        struct slot
        {
            void destroy()
            {
                storage.~T();
            }

            ~slot()
            {
                destroy();
            }
            
            hsd::atomic_usize ticket = {0};

            slot() requires (std::is_default_constructible_v<T>)
                : storage{}
            {}

            slot(const slot& other)
                : storage{other.storage}
            {
                ticket.store(other.ticket.load());
            }

            slot(slot&& other)
                : storage{move(other.storage)}
            {
                ticket.store(other.ticket.load());
            }

            slot& operator=(const slot& rhs)
            {
                ticket.store(rhs.ticket.load());
                storage = rhs.storage;
                return *this;
            }

            slot& operator=(slot&& rhs)
            {
                ticket.store(rhs.ticket.load());
                storage = move(rhs.storage);
                return *this;
            }

            
            T storage;
        };
    }

    /**
     * Multiprocessor Multiconsumer Queue [MPMCQueue]
     * Written by PyroFlareX, inspiration from github.com/rigtorp/MPMCQueue for concept.
     * This is a "thread-safe" MPMC queue written for HackySTL, type and max capacity must be known on creation.
     * Probably not the best optimized implementation, but it works well enough.
     * Works as a stack-machine, FIFO
    */
    template<typename T>
    class MPMCQueue
    {
    public:
        MPMCQueue(usize capacity)
            : _capacity(capacity)
        {
            if(_capacity < 1)
            {
                //This is UB
                puts("Capacity is less than 1, this is UB, and an error\n");
                abort();
            }
            _allocation.resize(_capacity + 1);
        }

        MPMCQueue(const MPMCQueue&) = delete;
        MPMCQueue& operator=(const MPMCQueue&) = delete;

        bool try_emplace(T&& value)
        {
            auto head = _head.load();

            for(;;)
            {
                auto& slot = _allocation.at_unchecked(mod(head));

                if(turn(head) * 2 == slot.ticket.load())
                {
                    if(_head.compare_exchange_strong(head, head + 1))
                    {
                        slot.storage = forward<T>(value);
                        slot.ticket.store(turn(head) * 2 + 1);
                        return true;
                    }
                }
                else
                {
                    auto const prevHead = head;
                    head = _head.load();

                    if(head == prevHead)
                    {
                        return false;
                    }
                }
            }

            return false;
        }

        bool try_pop(T& value)
        {
            auto tail = _tail.load();

            for(;;)
            {
                auto& slot = _allocation.at_unchecked(mod(tail));

                if(turn(tail) * 2 + 1 ==  slot.ticket.load())
                {
                    if(_tail.compare_exchange_strong(tail, tail + 1))
                    {
                        value = move(slot.storage);
                        slot.destroy();
                        slot.ticket.store(turn(tail) * 2 + 2);
                        return true;
                    }
                }
                else
                {
                    auto const prevTail = tail;
                    tail = _tail.load();

                    if(tail == prevTail)
                    {
                        return false;
                    }
                }
            }
        }

        bool try_push(T& value) noexcept
        {
            return try_emplace(value);
        }

        void emplace(T&& value)
        {
            auto const head = _head.fetch_add(1);
            auto& slot = _allocation.at(mod(head)).unwrap();

            while(turn(head) * 2 != slot.ticket.load());
            
            slot.storage = forward<T>(value);
            slot.ticket.store(turn(head) * 2 + 1);
        }

        void pop(T& value)
        {
            auto const tail = _tail.fetch_add(1);
            auto& slot = _allocation.at(mod(tail)).unwrap();

            while(turn(tail) * 2 + 1 != slot.ticket.load());

            value = move(slot.storage);
            slot.destroy();
            slot.ticket.store(turn(tail) * 2 + 2);
        }

        void push(T& value)
        {
            emplace(value);
        }

    private:
        usize _capacity;

        // @TODO: Improvement: make the constructor constexpr for ``capacity`` and replace vector with an array
        vector<priv::slot<T>> _allocation;

        atomic_usize _head = {0};
        atomic_usize _tail = {0};

        constexpr usize mod(usize number) const
        {
            return number % _capacity; 
        }

        constexpr usize turn(usize number) const
        {
            return number / _capacity;
        }
    };
}