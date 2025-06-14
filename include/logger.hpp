#pragma once

#include <chrono>
#include <format>
#include <iostream>
#include <map>
#include <mutex>
#include <source_location>
#include <string>

#include "console.hpp"
#include "singleton.hpp"

enum class LoggerLevel : uint8_t
{
	Trace,
	Info,
	Warning,
	Exception,
	Error,
};
class Logger : public Singleton<Logger>
{
	friend class Singleton<Logger>;

	[[nodiscard]] inline static auto get_level_fmt_data(LoggerLevel level)
	{
		using enum LoggerLevel;
		using enum Output::Style::EStyles;

		static const std::map<LoggerLevel, std::pair<std::string_view, uint64_t>> level_to_fmt = {
			{Trace, {"TRACE", text_brightgray}},
			{Info, {"INFO", text_cyan}},
			{Warning, {"WARNING", text_yellow}},
			{Exception, {"EXCEPTION", text_magenta}},
			{Error, {"ERROR", text_red}},
		};

		return level_to_fmt.at(level);
	}

  public:
	constexpr Logger() = default;

	template <typename... Args>
	constexpr inline void log(LoggerLevel level,
							  const std::source_location& loc,
							  std::format_string<Args...> fmt,
							  Args&&... args)
	{
		std::scoped_lock lock(mutex_);

		const auto [level_str, level_style] = get_level_fmt_data(level);

		const auto log_text = std::format("{:<12}{} {}:{},\t{}",
										  std::format("[{}]", level_str),
										  std::format("{:%T}", std::chrono::current_zone()->to_local(std::chrono::system_clock::now())),
										  loc.file_name(),
										  loc.line(),
										  std::format(fmt, std::forward<Args>(args)...));

		std::cerr << Output::Text(log_text, level_style) << '\n';
	}

	template <typename... Args>
	constexpr inline void trace(const std::source_location& loc, std::format_string<Args...> fmt, Args&&... args)
	{
		log(LoggerLevel::Trace, loc, fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	constexpr inline void info(const std::source_location& loc, std::format_string<Args...> fmt, Args&&... args)
	{
		log(LoggerLevel::Info, loc, fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	constexpr inline void warn(const std::source_location& loc, std::format_string<Args...> fmt, Args&&... args)
	{
		log(LoggerLevel::Warning, loc, fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	constexpr inline void error(const std::source_location& loc, std::format_string<Args...> fmt, Args&&... args)
	{
		log(LoggerLevel::Error, loc, fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	constexpr inline void exception(const std::source_location& loc, std::format_string<Args...> fmt, Args&&... args)
	{
		log(LoggerLevel::Exception, loc, fmt, std::forward<Args>(args)...);
	}

	std::mutex mutex_;
};

#define LOG_TRACE(fmt, ...) Logger::get_instance().trace(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) Logger::get_instance().info(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) Logger::get_instance().warn(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Logger::get_instance().error(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LOG_EXCEPTION(fmt, ...) Logger::get_instance().exception(std::source_location::current(), fmt, ##__VA_ARGS__)

#ifndef NDEBUG
	#define DBG_TRACE(fmt, ...) LOG_TRACE(fmt, ##__VA_ARGS__)
	#define DBG_INFO(fmt, ...) LOG_INFO(fmt, ##__VA_ARGS__)
	#define DBG_WARN(fmt, ...) LOG_WARN(fmt, ##__VA_ARGS__)
	#define DBG_ERROR(fmt, ...) LOG_ERROR(fmt, ##__VA_ARGS__)
	#define DBG_EXCEPTION(fmt, ...) LOG_EXCEPTION(fmt, ##__VA_ARGS__)
#else
	#define DBG_TRACE(fmt, ...)
	#define DBG_INFO(fmt, ...)
	#define DBG_WARN(fmt, ...)
	#define DBG_ERROR(fmt, ...)
	#define DBG_EXCEPTION(fmt, ...)
#endif
