#pragma once
#include <iostream>
#include <chrono>

struct  Timer
{
	std::chrono::time_point < std::chrono::high_resolution_clock> start, end;

	Timer() {
		start = std::chrono::high_resolution_clock::now();
	}

	~Timer() {
		end = std::chrono::high_resolution_clock::now();
		auto m_start = std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count();
		auto m_end = std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count();

		auto duration = m_end - m_start;
		double  ms = duration * 0.001;

		std::cout << "Timer Use: " << ms << "ms" << std::endl;
	}

};