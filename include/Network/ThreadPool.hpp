#ifndef THREAD_POOL_CLASS_HPP
#define THREAD_POOL_CLASS_HPP

#include <condition_variable>
#include <utility>
#include <future>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>


class ThreadPool
{
public:
	ThreadPool() : ThreadPool(std::thread::hardware_concurrency()) {}

	ThreadPool(unsigned int threads_count)
	{
		process = true;
		tasks_total_number = 0;

		threads.reserve(threads_count);
		for (unsigned int i = 0; i < threads_count; i++)
			threads.emplace_back(&ThreadPool::run, this);
	}

	~ThreadPool()
	{
		process = false;
		for (unsigned int i = 0; i < threads.size(); i++)
			threads[i].join();
	}

	template <typename Function, typename ...Args>
	unsigned int addTask(const Function& task_func, Args&&... args)
	{
		unsigned int task_id = tasks_total_number++;
		std::lock_guard<std::mutex> lock(mtx);
		tasks.emplace(std::async(std::launch::deferred, task_func, args...), task_id);
		tasks_cv.notify_one();

		return task_id;
	}

protected:
	std::atomic<bool> process;
	unsigned int tasks_total_number;
	std::mutex mtx;
	std::vector<std::thread> threads;
	std::queue<std::pair<std::future<void>, unsigned int>> tasks;
	std::condition_variable tasks_cv;

	void run()
	{
		while (process)
		{
			std::unique_lock<std::mutex> locker(mtx);
			tasks_cv.wait(locker, [this]()->bool {return !tasks.empty() || !process;});

			if (!tasks.empty())
			{
				auto elem = std::move(tasks.front());
				tasks.pop();
				locker.unlock();

				elem.first.get();
			}
		}
	}
};

#endif //THREAD_POOL_CLASS_HPP