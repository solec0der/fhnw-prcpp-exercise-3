#pragma once

#include <chrono>

/// <summary>
/// Stopwatch measuring wall-clock time.
/// CPU time could be measured with std::clock_t startcputime = std::clock();
/// Author: C. Stamm
/// </summary>
class Stopwatch {
	using Clock = std::chrono::system_clock;	// clock type

	Clock::time_point m_start;	// start time
	Clock::duration m_elapsed;	// elapsed duration

public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	Stopwatch() : m_elapsed{ 0 } {}

	/// <summary>
	/// Set start time to now.
	/// </summary>
	void start() {
		m_start = Clock::now();
	}
	/// <summary>
	/// Add time difference between now and start time to elapsed duration.
	/// </summary>
	void stop() {
		m_elapsed += Clock::now() - m_start;
	}
	/// <summary>
	/// Reset elapsed duration.
	/// </summary>
	void reset() {
		m_elapsed = Clock::duration::zero();
	}
	/// <summary>
	/// Return elapsed duration.
	/// </summary>
	Clock::duration GetElapsedTime() const {
		return m_elapsed;
	}
	/// <summary>
	/// Return elapsed duration in seconds.
	/// </summary>
	double getElapsedTimeSeconds() const {
		using sec = std::chrono::duration<double>;
		return std::chrono::duration_cast<sec>(GetElapsedTime()).count();
	}
	/// <summary>
	/// Return elapsed duration in milliseconds.
	/// </summary>
	double getElapsedTimeMilliseconds() const {
		using ms = std::chrono::duration<double, std::milli>;
		return std::chrono::duration_cast<ms>(GetElapsedTime()).count();
	}
	/// <summary>
	/// Return elapsed duration in integral nanoseconds.
	/// </summary>
	long long getElapsedTimeNanoseconds() const {
		return std::chrono::nanoseconds(GetElapsedTime()).count();
	}
};
