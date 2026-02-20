#pragma once
#include <chrono>
#include <iostream>
#include <string>

namespace c = std::chrono;
using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

class Stopwatch
{
public:
	//Stopwatch();

	void start() { if (!running) { running = true; startTime = Clock::now(); } }

	void pause() { if (running) { running = false; pauseTime = Clock::now(); } }

	TimePoint currentTime() const { return Clock::now(); }

	void reset() {
		running = false;
		startTime = {};
		pauseTime = {};
	}

	double elapsed() const {
		if (startTime == TimePoint{}) { return 0.0; }
		if (running) { return c::duration<double>(currentTime() - startTime).count(); }
		return c::duration<double>(pauseTime - startTime).count();
	}

	//Neatly display a formatted time elapsed in the form of 00:00.
	std::string timeIs(const double seconds) {
		int secondsTotal = static_cast<int>(seconds);
		int timeSeconds = secondsTotal % 60;
		int timeMinutes = secondsTotal / 60;

		std::ostringstream os;
		os << timeMinutes << ":" << std::setfill('0') << std::setw(2) << timeSeconds;
		return os.str();
	}

private:
	bool running = false;
	c::time_point<Clock> startTime = {};
	c::time_point<Clock> pauseTime = {};
};

