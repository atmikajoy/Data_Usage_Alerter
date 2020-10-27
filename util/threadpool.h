#ifndef THREAD_POOL_H_INCLUDED
#define THREAD_POOL_H_INCLUDED

#include <thread>
#include <vector>
#include <functional>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace atmik
{
	namespace util
	{
		struct thread_pool
		{
			explicit thread_pool(std::size_t nthreads = 2);
			~thread_pool();

			thread_pool(const thread_pool&) = delete;
			thread_pool& operator= (const thread_pool&) = delete;
			thread_pool(thread_pool&&) = delete;
			thread_pool& operator= (thread_pool&&) = delete;

			static thread_pool& default_pool();

			// static void stop_default_pool();

			using task = std::function< void() >;

			template < typename FN, typename... ARGS >
			bool queue_task(FN&& fn, ARGS&&... args)
			{
				if (stopped) return false;

				{
					std::lock_guard<std::mutex> lock(queue_mutex);
					pending_tasks.emplace(std::bind(std::forward<FN>(fn), std::forward<ARGS>(args)...));
				}
				cv.notify_one();
				return true;
			}

			void stop();

		private:
			std::vector<std::thread> threads;
			std::queue<task> pending_tasks;
			std::mutex queue_mutex;
			std::condition_variable cv;
			std::atomic<bool> stopping{ false };
			void thread_fun();

			// static std::atomic<int> num_thread_pools;

			bool stopped = false;
		};
	}
}

#endif // THREAD_POOL_H_INCLUDED