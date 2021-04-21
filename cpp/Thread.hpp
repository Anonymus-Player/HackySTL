#pragma once

#include "Utility.hpp"
#include "Tuple.hpp"

#if defined(HSD_PLATFORM_POSIX)
#include <pthread.h>
#include <cstdlib>
#elif defined(HSD_PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace hsd 
{
	struct thread 
	{
	private:
		#if defined(HSD_PLATFORM_POSIX)
		using native_id_type = u64;
		#elif defined(HSD_PLATFORM_WINDOWS)
		using native_id_type = DWORD;
		#endif
	public:
		struct id 
		{
			friend thread;

			id() = default;

			friend bool operator==(id lhs, id rhs) 
			{
				return lhs._id == rhs._id;
			}

		private:
			thread::native_id_type _id;

			id(thread::native_id_type id) 
				: _id{id}
			{}
		};

		#if defined(HSD_PLATFORM_POSIX)
		using native_handle_type = pthread_t;
		#elif defined(HSD_PLATFORM_WINDOWS)
		using native_handle_type = HANDLE;
		#endif

		thread() = default;

		~thread() 
		{
			if (joinable())
				abort();
		}

		template <typename F, typename... Args>
		thread(F&& func, Args&&... args) 
		{
			#if defined(HSD_PLATFORM_POSIX)
			pthread_attr_t _attr;
			pthread_attr_init(&_attr);
			#endif

			auto decay_copy = []<typename T>(T&& t) -> decay_t<T>
			{
				return hsd::forward<T>(t);
			};

			#if defined(HSD_PLATFORM_POSIX)
			hsd::i32 _ready = 0;
			#else
			hsd::u64 _ready = 0;
			#endif

			struct thread_data 
			{
				#if defined(HSD_PLATFORM_POSIX)
				hsd::i32* ready;
				#else
				hsd::u64* ready;
				#endif

				hsd::decay_t<F> func;
				hsd::tuple<decay_t<Args>...> args;

				#if defined(HSD_PLATFORM_POSIX)
				static void* enter_thread(void* arg)
				#else
				static DWORD enter_thread(void* arg)
				#endif
				{
					auto td = hsd::move(*reinterpret_cast<thread_data*>(arg));

					// Tell our parent we are ready and copied the data
					#if defined(HSD_PLATFORM_POSIX)
					__atomic_store_n(td.ready, 1, __ATOMIC_RELEASE);
					#elif defined(HSD_PLATFORM_WINDOWS)
					InterlockedIncrementRelease(td.ready);
					#endif
					
					hsd::apply(td.func, td.args);

					#if defined(HSD_PLATFORM_POSIX)
					return nullptr;
					#else
					return 0ul;
					#endif
				}
			} _td {
				&_ready,
				decay_copy(hsd::forward<F>(func)),
				hsd::make_tuple(decay_copy(hsd::forward<Args>(args))...)
			};

			#if defined(HSD_PLATFORM_POSIX)
			i32 _res = pthread_create(&_handle, &attr, thread_data::enter_thread, &td);
			
			if (_res) abort();

			// Spinlock time
			while (!__atomic_load_n(&_ready, __ATOMIC_ACQUIRE));
			_id = id{static_cast<native_id_type>(_handle)};
			#elif defined(HSD_PLATFORM_WINDOWS)
			native_id_type _native_id;
			_handle = CreateThread(nullptr, 0, thread_data::enter_thread, &_td, 0, &_native_id);
			
			if (!_handle) abort();

			while (!InterlockedCompareExchangeAcquire(&_ready, 0, 1));
			_id = id{_native_id};
			#endif
		}

		thread(const thread&) = delete;
		thread& operator=(const thread&) = delete;

		thread(thread&& other) 
		{
			swap(other); 
		}

		thread& operator=(thread&& other) 
		{
			swap(other); return *this;
		}

		void swap(thread &other) 
		{ 
			hsd::swap(_handle, other._handle);
			hsd::swap(_id, other._id);
		}

		id get_id() 
		{
			return id{_id};
		}

		bool joinable() {
			return get_id() != id{};
		}

		native_handle_type native_handle() 
		{
			return _handle;
		}

		static u32 hardware_concurrency() 
		{
			return 0;
		}

		void detach() 
		{
			#if defined(HSD_PLATFORM_POSIX)
			pthread_detach(_handle);
			_handle = 0;
			#elif defined(HSD_PLATFORM_WINDOWS)
			CloseHandle(_handle);
			_handle = nullptr;
			#endif
			
			_id = {};
		}

		void join() 
		{
			#if defined(HSD_PLATFORM_POSIX)
			pthread_join(_handle, nullptr);
			_handle = 0;
			#elif defined(HSD_PLATFORM_WINDOWS)
			WaitForSingleObject(_handle, INFINITE);
			CloseHandle(_handle);
			_handle = nullptr;
			#endif

			_id = {};
		}
	
	private:
		native_handle_type _handle{};
		id _id{};
	}; // struct thread
} // namespace hsd