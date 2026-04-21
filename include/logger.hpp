#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <source_location>
#include <string>
#include <string_view>
#include <syncstream>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "console.hpp"
#include "singleton.hpp"

#ifndef LOGGER_USE_SOURCE_LOCATION
	#define LOGGER_USE_SOURCE_LOCATION 1
#endif

enum class LoggerLevel : int8_t
{
	off,
	debug,
	trace,
	info,
	warning,
	error,
};

namespace _detail
{
	using enum Output::Style::EStyles;

	struct log_params
	{
		std::string_view prefix;
		uint64_t style;
	};

	[[nodiscard]] inline static constexpr log_params get_style_params(LoggerLevel lvl)
	{
		constexpr auto styles_array = std::to_array<log_params>({
			{ "OFF", text_default },
			{ "DEBUG", text_brightgray },
			{ "TRACE", text_white },
			{ "INFO", text_cyan },
			{ "WARNING", text_yellow },
			{ "ERROR", text_red | style_underline },
		});

		return styles_array.at(std::to_underlying(lvl));
	}

	[[nodiscard]] inline auto get_current_time()
	{
		static const auto tz = std::chrono::current_zone();

		return tz->to_local(std::chrono::system_clock::now());
	}

	[[nodiscard]] inline std::string fmt_time()
	{
		return std::format("{:%T}", get_current_time());
	}

	[[nodiscard]] inline bool is_same_day(const auto& t1, const auto& t2) noexcept
	{
		return std::chrono::floor<std::chrono::days>(t1) == std::chrono::floor<std::chrono::days>(t2);
	}
} // namespace _detail

class ILogSink
{
protected:
	std::atomic<LoggerLevel> m_min_level { LoggerLevel::off };

	[[nodiscard]] inline bool should_log(LoggerLevel level) const noexcept
	{
		return level >= m_min_level;
	}

public:
	void set_min_level(LoggerLevel level) noexcept
	{
		m_min_level = level;
	}

	virtual void log(LoggerLevel, const std::string_view) = 0;

	virtual ~ILogSink() = default;
};

template<typename T>
concept IsLoggerSink = std::is_base_of_v<ILogSink, T>;

class ConsoleSink : public ILogSink
{
public:
	ConsoleSink() noexcept = default;

	explicit ConsoleSink(LoggerLevel lvl) noexcept
	{
		set_min_level(lvl);
	}

	void log(LoggerLevel level, const std::string_view message) override
	{
		if (should_log(level))
		{
			const auto text = Output::Text(message, _detail::get_style_params(level).style);
			std::osyncstream(std::cerr) << text << '\n';
		}
	}

	~ConsoleSink() override = default;
};

class FileSink : public ILogSink
{
protected:
	std::ofstream m_log_file;
	std::filesystem::path m_log_directory;
	std::chrono::local_days m_current_day;
	mutable std::mutex m_file_mutex;

	void open_new_log_file(std::chrono::local_time<std::chrono::system_clock::duration> time_point)
	{
		const auto file_path = m_log_directory / std::format("{:%F}", time_point) / std::format("log_{:%H_%M_%S}.log", time_point);
		std::filesystem::create_directories(file_path.parent_path());

		m_log_file.open(file_path, std::ios::ate);

		if (!m_log_file.is_open())
		{
			throw std::runtime_error("Failed to open log file");
		}

		m_current_day = std::chrono::floor<std::chrono::days>(time_point);
	}

public:
	explicit FileSink(std::filesystem::path directory, LoggerLevel level = LoggerLevel::trace)
		: m_log_directory(std::move(directory))
	{
		std::error_code ec;
		std::filesystem::create_directories(m_log_directory, ec);

		if (ec)
		{
			m_log_directory = std::filesystem::temp_directory_path();
		}

		set_min_level(level);
		open_new_log_file(_detail::get_current_time());
	}

	void log(LoggerLevel level, const std::string_view message) override
	{
		if (!should_log(level))
		{
			return;
		}

		std::scoped_lock lock(m_file_mutex);

		if (const auto now = _detail::get_current_time(); !_detail::is_same_day(now, m_current_day))
		{
			if (m_log_file.is_open())
			{
				m_log_file.close();
			}

			open_new_log_file(now);
		}

		if (m_log_file.is_open())
		{
			m_log_file << message << std::endl;
		}
	}

	~FileSink() override
	{
		std::scoped_lock lock(m_file_mutex);

		if (m_log_file.is_open())
		{
			m_log_file.close();
		}
	}
};

class Logger : public Singleton<Logger>
{
	friend class Singleton<Logger>;

protected:
	std::mutex m_sinks_mutex;
	std::unordered_map<std::string, std::shared_ptr<ILogSink>> m_sinks;

private:
	[[nodiscard]] std::vector<std::shared_ptr<ILogSink>> get_active_sinks()
	{
		std::scoped_lock lock(m_sinks_mutex);

		if (m_sinks.empty())
		{
			return {};
		}

		std::vector<std::shared_ptr<ILogSink>> active_sinks;
		active_sinks.reserve(m_sinks.size());

		for (const auto& [key, sink] : m_sinks)
		{
			active_sinks.push_back(sink);
		}

		return active_sinks;
	}

public:
	Logger() = default;

	template<IsLoggerSink Sink_t, typename... Args>
	inline bool add_sink(const std::string_view logger_name, Args&&... args)
	{
		std::scoped_lock lock(m_sinks_mutex);
		auto&& [it, success] = m_sinks.emplace(std::string{logger_name}, std::make_shared<Sink_t>(std::forward<Args>(args)...));

		return success;
	}

#if LOGGER_USE_SOURCE_LOCATION
	template<typename... Args>
	inline void log(LoggerLevel level, const std::source_location loc, std::format_string<Args...> fmt, Args&&... args)
	{
		const auto active_sinks = get_active_sinks();
		if (active_sinks.empty())
		{
			return;
		}

		const auto& prefix = _detail::get_style_params(level).prefix;
		const std::filesystem::path file_path { loc.file_name() };

		const auto& message = std::format("{:<12} {} {}:{},\t{}",
			std::format("[{}]", prefix),
			_detail::fmt_time(),
			file_path.filename().string(),
			loc.function_name(),
			std::format(std::forward<std::format_string<Args...>>(fmt), std::forward<Args>(args)...));

		for (const auto& sink : active_sinks)
		{
			sink->log(level, message);
		}
	}
#else
	template<typename... Args>
	inline void log(LoggerLevel level, std::format_string<Args...> fmt, Args&&... args)
	{
		const auto active_sinks = get_active_sinks();
		if (active_sinks.empty())
		{
			return;
		}

		const auto& prefix = _detail::get_style_params(level).prefix;

		const auto& message = std::format("{:<12} {} {}",
			std::format("[{}]", prefix),
			_detail::fmt_time(),
			std::format(std::forward<std::format_string<Args...>>(fmt), std::forward<Args>(args)...));

		for (const auto& sink : active_sinks)
		{
			sink->log(level, message);
		}
	}
#endif

	inline bool erase_logger(const std::string_view logger_name)
	{
		std::scoped_lock lock(m_sinks_mutex);

		if (const auto it_logger = m_sinks.find(std::string{logger_name}); it_logger != m_sinks.end())
		{
			m_sinks.erase(it_logger);

			return true;
		}

		return false;
	}

	~Logger() noexcept = default;
};

#define LOGGER_SINK_NAMED(type, name, ...) (::Logger::get_instance().add_sink<type>(name, ##__VA_ARGS__))
#define LOGGER_SINK(type, ...)             (::Logger::get_instance().add_sink<type>(#type, ##__VA_ARGS__))

#if LOGGER_USE_SOURCE_LOCATION
	#define LOG_TRACE(...) Logger::get_instance().log(LoggerLevel::trace, std::source_location::current(), __VA_ARGS__)
	#define LOG_DEBUG(...) Logger::get_instance().log(LoggerLevel::debug, std::source_location::current(), __VA_ARGS__)
	#define LOG_INFO(...)  Logger::get_instance().log(LoggerLevel::info,  std::source_location::current(), __VA_ARGS__)
	#define LOG_WARN(...)  Logger::get_instance().log(LoggerLevel::warning, std::source_location::current(), __VA_ARGS__)
	#define LOG_ERROR(...) Logger::get_instance().log(LoggerLevel::error,  std::source_location::current(), __VA_ARGS__)
#else
	#define LOG_TRACE(...) Logger::get_instance().log(LoggerLevel::trace, __VA_ARGS__)
	#define LOG_DEBUG(...) Logger::get_instance().log(LoggerLevel::debug, __VA_ARGS__)
	#define LOG_INFO(...)  Logger::get_instance().log(LoggerLevel::info,  __VA_ARGS__)
	#define LOG_WARN(...)  Logger::get_instance().log(LoggerLevel::warning, __VA_ARGS__)
	#define LOG_ERROR(...) Logger::get_instance().log(LoggerLevel::error,  __VA_ARGS__)
#endif

#ifdef NDEBUG
	#define DBG_TRACE(...)
	#define DBG_DEBUG(...)
	#define DBG_INFO(...)
	#define DBG_WARN(...)
	#define DBG_ERROR(...)
#else
	#define DBG_TRACE(...) LOG_TRACE(__VA_ARGS__)
	#define DBG_DEBUG(...) LOG_DEBUG(__VA_ARGS__)
	#define DBG_INFO(...)  LOG_INFO(__VA_ARGS__)
	#define DBG_WARN(...)  LOG_WARN(__VA_ARGS__)
	#define DBG_ERROR(...) LOG_ERROR(__VA_ARGS__)
#endif