#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <source_location>
#include <string_view>
#include <thread>

#include "console.hpp"
#include "singleton.hpp"

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
	using namespace std::chrono;
	using enum Output::Style::EStyles;

	struct _log_params
	{
		std::string_view prefix;
		uint64_t style;
	};

	inline static constexpr _log_params get_style_params(LoggerLevel lvl)
	{
		constexpr auto styles_array = std::to_array<_log_params>({
			{"OFF", text_default},
			{"DEBUG", text_brightgray},
			{"TRACE", text_white},
			{"INFO", text_cyan},
			{"WARNING", text_yellow},
			{"ERROR", text_red | style_underline},
		});

		return styles_array.at(static_cast<size_t>(lvl));
	}

	inline std::string get_current_time()
	{
		return std::format("{:%T}", system_clock::now());
	}

	inline bool is_same_day(const system_clock::time_point& t1, const system_clock::time_point& t2) noexcept
	{
		return floor<days>(t1) == floor<days>(t2);
	}
} // namespace _detail

class ILogSink
{
  protected:
	std::atomic<LoggerLevel> m_min_level{LoggerLevel::off};

	[[nodiscard]] inline bool should_log(LoggerLevel level) const noexcept
	{
		return level >= m_min_level;
	}

  public:
	void set_min_level(LoggerLevel level) noexcept
	{
		m_min_level = level;
	}

	virtual ~ILogSink() = default;
	virtual void log(LoggerLevel, const std::string_view, const std::string_view) = 0;
};

template <typename T>
concept IsLoggerSink = std::is_base_of_v<ILogSink, T>;

class ConsoleSink : public ILogSink
{
	mutable std::mutex m_console_mutex;

  public:
	ConsoleSink() noexcept = default;

	explicit ConsoleSink(LoggerLevel lvl) noexcept
	{
		set_min_level(lvl);
	}

	void log(LoggerLevel level, const std::string_view message, const std::string_view logger_name) override
	{
		if (should_log(level))
		{
			std::scoped_lock lock(m_console_mutex);
			const auto& [_, styles] = _detail::get_style_params(level);
			std::cerr << Output::Text(message, styles) << '\n';
		}
	}

	~ConsoleSink() override = default;
};

class FileSink : public ILogSink
{
	std::ofstream m_log_file;
	std::filesystem::path m_log_directory;
	std::chrono::system_clock::time_point m_current_day;
	mutable std::mutex m_file_mutex;

	void open_new_log_file(const std::chrono::system_clock::time_point& tp)
	{
		auto day_tp = std::chrono::floor<std::chrono::days>(tp);
		const auto file_path = m_log_directory / std::format("log_{:%F}.log", day_tp);

		m_log_file.open(file_path, std::ios::app);

		if (!m_log_file.is_open())
		{
			throw std::runtime_error("Failed to open log file");
		}

		m_current_day = day_tp;
	}

  public:
	explicit FileSink(std::filesystem::path directory, LoggerLevel level = LoggerLevel::trace)
		: m_log_directory(std::move(directory))
	{
		set_min_level(level);
		std::filesystem::create_directories(m_log_directory);
		open_new_log_file(std::chrono::system_clock::now());
	}

	void log(LoggerLevel level, const std::string_view message, const std::string_view name) override
	{
		if (!should_log(level))
		{
			return;
		}

		auto now = std::chrono::system_clock::now();
		std::scoped_lock lock(m_file_mutex);

		if (!_detail::is_same_day(now, m_current_day))
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

  private:
	std::mutex m_sinks_mutex;
	std::unordered_map<std::string_view, std::unique_ptr<ILogSink>> m_sinks;
	using iterator_type = decltype(m_sinks)::iterator;

  public:
	Logger() = default;

	template <IsLoggerSink Sink_t, typename... Args>
	inline std::optional<iterator_type> add_sink(const std::string_view logger_name, Args&&... args)
	{
		auto sink = std::make_unique<Sink_t>(std::forward<Args>(args)...);
		std::scoped_lock lock(m_sinks_mutex);

		if(auto&& [it, success] = m_sinks.emplace(logger_name, std::move(sink)); success)
		{
			return it;
		}

		return std::nullopt;
	}

	template <typename... Args>
	inline void log(LoggerLevel level, const std::source_location& loc, std::format_string<Args...> fmt, Args&&... args)
	{
		const auto timestamp = _detail::get_current_time();
		const auto& [prefix, _] = _detail::get_style_params(level);
		const std::filesystem::path file_path{ loc.file_name() };

		const auto& message = std::format(
			"{:<12} {} {}:{},\t{}",
			std::format("[{}]", prefix),
			timestamp,
			file_path.filename().string(),
			loc.line(),
			std::format(fmt, std::forward<Args>(args)...));

		std::scoped_lock lock(m_sinks_mutex);

		for(auto&& [name, sink] : m_sinks)
		{
			sink->log(level, message, name);
		}
	}

	[[nodiscard]] inline std::optional<iterator_type> find_logger(const std::string_view logger_name)
	{
		if(auto it_logger = m_sinks.find(logger_name); it_logger != m_sinks.cend())
		{
			return it_logger;
		}

		return std::nullopt;
	}

	inline bool erase_logger(const std::string_view logger_name)
	{
		const auto it = find_logger(logger_name);
		const bool it_belongs = it != m_sinks.end();

		if(it_belongs)
		{
			m_sinks.erase(*it);
		}

		return it_belongs;
	}

	inline bool erase_logger(iterator_type it)
	{
		const bool it_belongs = it != m_sinks.end();

		if(it_belongs)
		{
			m_sinks.erase(it);
		}

		return it_belongs;
	}

	~Logger() noexcept = default;
};

#define LOGGER_SINK_NAMED(type, name, ...) Logger::get_instance().add_sink<type>(name, __VA_ARGS__)
#define LOGGER_SINK(type, ...) LOGGER_SINK_NAMED(type, #type, __VA_ARGS__)

#define LOG_TRACE(...) Logger::get_instance().log(LoggerLevel::trace, std::source_location::current(), __VA_ARGS__)
#define LOG_DEBUG(...) Logger::get_instance().log(LoggerLevel::debug, std::source_location::current(), __VA_ARGS__)
#define LOG_INFO(...) Logger::get_instance().log(LoggerLevel::info, std::source_location::current(), __VA_ARGS__)
#define LOG_WARN(...) Logger::get_instance().log(LoggerLevel::warning, std::source_location::current(), __VA_ARGS__)
#define LOG_ERROR(...) Logger::get_instance().log(LoggerLevel::error, std::source_location::current(), __VA_ARGS__)

#ifdef NDEBUG
	#define DBG_TRACE(...)
	#define DBG_DEBUG(...)
	#define DBG_INFO(...)
	#define DBG_WARN(...)
	#define DBG_ERROR(...)
#else
	#define DBG_TRACE(...) LOG_TRACE(__VA_ARGS__)
	#define DBG_DEBUG(...) LOG_DEBUG(__VA_ARGS__)
	#define DBG_INFO(...) LOG_INFO(__VA_ARGS__)
	#define DBG_WARN(...) LOG_WARN(__VA_ARGS__)
	#define DBG_ERROR(...) LOG_ERROR(__VA_ARGS__)
#endif