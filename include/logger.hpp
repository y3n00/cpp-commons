#pragma once

#include <array>
#include <chrono>
#include <format>
#include <iostream>
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

  public:
	constexpr Logger() = default;

	template <typename... Args>
	constexpr inline void log(LoggerLevel level,
							  const std::source_location& loc,
							  std::format_string<Args...> fmt,
							  Args&&... args)
	{
		std::scoped_lock lock(mutex_);

		const std::string message = std::format(fmt, std::forward<Args>(args)...);
		const std::string level_str = std::format("[{}]", get_level_string(level));
		const std::string timestamp_str = std::format("{:%T}", std::chrono::system_clock::now());

		const auto log_text = std::format("{:<12}{} {}:{},\t{}",
										  level_str,
										  timestamp_str,
										  loc.file_name(),
										  loc.line(),
										  message);

		std::cerr << Output::Text(log_text, get_level_style(level)) << '\n';
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

  private:
	[[nodiscard]] constexpr inline static std::string_view get_level_string(LoggerLevel level)
	{
		constexpr static auto levelToText = std::to_array<std::string_view>({
			"TRACE",
			"INFO",
			"WARNING",
			"EXCEPTION",
			"ERROR",
		});

		return levelToText[std::to_underlying(level)];
	}

	[[nodiscard]] constexpr inline static uint64_t get_level_style(LoggerLevel level)
	{
		using namespace Output::Style;
		constexpr static auto levelToColor = std::to_array<uint64_t>({
			text_brightgray,
			text_cyan,
			text_yellow,
			text_magenta,
			text_red,
		});

		return levelToColor[std::to_underlying(level)];
	}

	std::mutex mutex_;
};

#ifndef NDEBUG
	#define DBG_TRACE(fmt, ...) Logger::get_instance().trace(std::source_location::current(), fmt, ##__VA_ARGS__)
	#define DBG_INFO(fmt, ...) Logger::get_instance().info(std::source_location::current(), fmt, ##__VA_ARGS__)
	#define DBG_WARN(fmt, ...) Logger::get_instance().warn(std::source_location::current(), fmt, ##__VA_ARGS__)
	#define DBG_ERROR(fmt, ...) Logger::get_instance().error(std::source_location::current(), fmt, ##__VA_ARGS__)
	#define DBG_EXCEPTION(fmt, ...) Logger::get_instance().exception(std::source_location::current(), fmt, ##__VA_ARGS__)
#else
	#define DBG_TRACE(fmt, ...)
	#define DBG_INFO(fmt, ...)
	#define DBG_WARN(fmt, ...)
	#define DBG_ERROR(fmt, ...)
	#define DBG_EXCEPTION(fmt, ...)
#endif
