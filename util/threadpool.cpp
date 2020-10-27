#include "threadpool.h"
#include <algorithm>
#include <set>
#include <cstdlib>

namespace
{
	std::set<atmik::util::thread_pool*> pools;

	/*
	void stop_all()
	{
		for (atmik::util::thread_pool* p : pools)
			if (p) p->stop();
	}

	[[maybe_unused]] const int init = (std::atexit(stop_all), 1);
	*/
}

atmik::util::thread_pool& atmik::util::thread_pool::default_pool()
{
	static thread_pool pool{ 2 };
	return pool;
}

atmik::util::thread_pool::thread_pool(std::size_t nthreads)
{
	nthreads = std::max(std::thread::hardware_concurrency(), nthreads);

	for (std::size_t i = 0; i < nthreads; ++i)
	{
		threads.emplace_back([this] { this->thread_fun(); });
	}

	pools.insert(this);
}

atmik::util::thread_pool::~thread_pool()
{
	if (!stopped) stop();
	pools.erase(this);
}

void atmik::util::thread_pool::stop()
{
	if (!stopped)
	{
		stopping = true;
		cv.notify_all();
		for (auto& thr : threads) thr.join();
		// --num_thread_pools;
		stopped = true;
	}
}

void atmik::util::thread_pool::thread_fun()
{
	task the_task;

	while (true)
	{

		{
			std::unique_lock<std::mutex> lock(queue_mutex);

			cv.wait(lock, [this]() { return this->stopping || !pending_tasks.empty(); });

			if (stopping && pending_tasks.empty()) return;

			if (!pending_tasks.empty())
			{
				the_task = std::move(pending_tasks.front());
				pending_tasks.pop();
			}

			else the_task = [] {}; // exceuted only if(spruious wakeup && stopping && !pending_tasks.empty())
		}

		the_task();
	}
}

// std::atomic<int> atmik::util::thread_pool::num_thread_pools{ 0 };

