#include "Network/Timer.hpp"

#include <thread>
#include <chrono>


std::chrono::steady_clock::time_point Timer::app_time = std::chrono::steady_clock::now();


Timer::Timer()
{
	restart();
}


void Timer::restart()
{
	start_time = std::chrono::steady_clock::now();
}

unsigned int Timer::getElapsedTime()
{
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
}


void Timer::sleep(unsigned int microseconds)
{
	std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
}

/*
float Timer::waitFPS(float want_fps)
{
	unsigned int elapsed_time = getElapsedTime();
	int time_to_sleep = int(1000000 / want_fps) - elapsed_time;
	if (time_to_sleep > 0)
		sleep(time_to_sleep);
	return getElapsedTime()*0.000001;
}
*/

float Timer::getAppTime()
{
	std::chrono::steady_clock::time_point now_time = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(now_time - app_time).count()*0.000001;
}