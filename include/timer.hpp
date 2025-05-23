#pragma once

#include <algorithm>
#include <chrono>
#include <concepts>
#include <execution>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

namespace Measurements
{
	using ns = std::chrono::nanoseconds;
	using us = std::chrono::microseconds;
	using ms = std::chrono::milliseconds;
	using s	 = std::chrono::seconds;
	using m	 = std::chrono::minutes;
	using h	 = std::chrono::hours;
} // namespace Measurements

template <typename M>
concept TimeMeasure_t = std::chrono::_Is_duration_v<M>;

template <TimeMeasure_t M>
class Timer
{
	using ClockType		= std::chrono::steady_clock;
	using TimePointType = ClockType::time_point;

  public:
	Timer()									 = default;
	Timer(const Timer& other)				 = default;
	Timer(Timer&& other) noexcept			 = default;
	Timer& operator=(const Timer& other)	 = default;
	Timer& operator=(Timer&& other) noexcept = default;

	[[nodiscard]] auto start_timestamp() const noexcept
	{
		return m_start;
	}

	[[nodiscard]] auto stop_timestamp() const noexcept
	{
		return m_stop;
	}

	[[nodiscard]] auto all_timestamps() const
	{
		return m_timestamps;
	}

	[[nodiscard]] auto get_duration() const noexcept
	{
		const auto duration = (is_running ? ClockType::now() : m_stop) - m_start;
		return std::chrono::duration<double, typename M::period>(duration);
	}

	inline void reset() noexcept
	{
		is_running = false;
		m_start = m_stop = {};
		m_timestamps.clear();
	}

	inline void start() noexcept
	{
		reset();
		m_start	   = ClockType::now();
		is_running = true;
	}

	inline void stop() noexcept
	{
		if (!is_running) [[unlikely]]
			return;
		_make_timestamp();
		m_stop	   = ClockType::now();
		is_running = false;
	}

	inline void timestamp() noexcept
	{
		if (is_running)
			_make_timestamp();
	}

  private:
	bool		   is_running;
	TimePointType  m_start, m_stop;
	std::vector<M> m_timestamps;

	inline void _make_timestamp()
	{
		m_timestamps.emplace_back(std::chrono::duration_cast<M>(ClockType::now() - m_start));
	}
};

template <TimeMeasure_t M>
class BenchTimer
{
	using Timer_t  = Timer<M>;
	using TimerMap = std::unordered_map<std::string, Timer_t>;

  public:
	[[nodiscard]] auto& add(const std::string& title)
	{
		return m_timers[title] = Timer_t{};
	}

	void start_all() noexcept
	{
		process_map([](auto& elem) { elem.start(); });
	}

	void stop_all() noexcept
	{
		process_map([](auto& elem) { elem.stop(); });
	}

	void make_timestamp(const std::string& title) noexcept
	{
		if (auto it = m_timers.find(title); it != m_timers.end() && it->second.is_running)
			it->second.timestamp();
	}

	[[nodiscard]] const auto& get_all() const noexcept
	{
		return m_timers;
	}

	void remove(const std::string& title) noexcept
	{
		m_timers.erase(title);
	}

	void remove_all() noexcept
	{
		m_timers.clear();
	}

  private:
	TimerMap m_timers;

	inline void process_map(auto&& func) noexcept
	{
		std::ranges::for_each(m_timers | std::views::values, func);
	}
};

template <TimeMeasure_t M>
class ScopedTimer
{
	using Timer_t = Timer<M>;

  public:
	explicit ScopedTimer(Timer_t& timer) noexcept
		: m_timer(timer)
	{
		m_timer.start();
	}

	~ScopedTimer() noexcept
	{
		m_timer.stop();
	}

	ScopedTimer(const ScopedTimer&)			   = delete;
	ScopedTimer& operator=(const ScopedTimer&) = delete;
	ScopedTimer(ScopedTimer&&)				   = delete;
	ScopedTimer& operator=(ScopedTimer&&)	   = delete;

  private:
	Timer_t& m_timer;
};