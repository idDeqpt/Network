#ifdef THREAD_POLL_CLASS_HPP
#define THREAD_POLL_CLASS_HPP

#include <iostream>
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

		return task_id;
	}

protected:
	std::atomic<bool> process;
	unsigned int tasks_total_number;
	std::mutex mtx;
	std::vector<std::thread> threads;
	std::queue<std::pair<std::future<void>, unsigned int>> tasks;

	void run()
	{
		while (process)
		{
			std::unique_lock<std::mutex> locker(mtx);

			if (!tasks.empty())
			{
				auto elem = std::move(tasks.front());
				tasks.pop();
				locker.unlock();

				elem.first.get();
				std::cout << "Getted\n";
			}
		}
	}
};

#endif //THREAD_POLL_CLASS_HPP