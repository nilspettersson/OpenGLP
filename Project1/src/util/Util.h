#pragma once
#include <iostream>
#include <chrono>

namespace util {
	struct Timer {
		std::chrono::time_point<std::chrono::steady_clock> start, end;
		std::chrono::duration<float> duration;
		Timer() {
			start = std::chrono::high_resolution_clock::now();
		}
		~Timer() {
			auto end = std::chrono::high_resolution_clock::now();
			duration = end - start;

			float ms = duration.count() * 1000.0f;
			std::cout << "Time: " << ms << " ms" << std::endl;
		}
	};
}