#ifndef TIMER_STATIC_CLASS
#define TIMER_STATIC_CLASS

#include <thread>
#include <chrono>


class Timer final
{
public:
	Timer();

	void restart();
	unsigned int getElapsedTime();

	static void sleep(unsigned int microseconds);
	static float getAppTime();

private:
	std::chrono::steady_clock::time_point start_time;
	static std::chrono::steady_clock::time_point app_time;
};

#endif