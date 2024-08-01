#pragma once
#include <algorithm>
#include <chrono>
#include <execution>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

namespace Measurements {
    using ns = std::chrono::nanoseconds;
    using us = std::chrono::microseconds;
    using ms = std::chrono::milliseconds;
    using s = std::chrono::seconds;
    using m = std::chrono::minutes;
    using h = std::chrono::hours;
}  // namespace Measurements

template <typename M>
concept Measurement = std::chrono::_Is_duration_v<M>;

template <Measurement M>
class Timer {
    using Clock_Type = std::chrono::steady_clock;
    using Time_Point = std::chrono::time_point<Clock_Type>;

   public:
    Timer() = default;
    Timer(const Timer& other) = default;
    Timer(Timer&& other) noexcept = default;
    Timer& operator=(const Timer& other) = default;
    Timer& operator=(Timer&& other) noexcept = default;

    [[nodiscard]] decltype(auto) start_timestamp() const noexcept { return m_start; }

    [[nodiscard]] decltype(auto) stop_timestamp() const noexcept { return m_stop; }

    [[nodiscard]] decltype(auto) all_timestamps() const {
        return m_timestamps |
               std::views::transform([](const auto& ns) { return std::chrono::duration_cast<M>(ns); }) |
               std::ranges::to<std::vector>();
    }

    [[nodiscard]] decltype(auto) get_duration() const noexcept {
        const auto duration = (is_running ? Clock_Type::now() : m_stop) - m_start;
        return std::chrono::duration<double, typename M::period>(duration);
    }

    void reset() noexcept {
        is_running = false;
        m_start = {};
        m_stop = {};
        m_timestamps.clear();
    }

    void start() noexcept {
        reset();
        m_timestamps.emplace_back(0);
        m_start = Clock_Type::now();
        is_running = true;
    }

    void stop() noexcept {
        if (!is_running) [[unlikely]]
            return;
        timestamp();
        m_stop = Clock_Type::now();
        is_running = false;
    }

    void timestamp() noexcept {
        if (!is_running) [[unlikely]]
            return;
        m_timestamps.emplace_back(Clock_Type::now() - m_start);
    }

   private:
    bool is_running = false;
    Time_Point m_start = {}, m_stop = {};
    std::vector<Measurements::ns> m_timestamps;
};

template <Measurement M>
class BenchTimer {
    using Timer_t = Timer<M>;
    using TimerMap = std::unordered_map<std::string, Timer_t>;

   public:
    [[nodiscard]] auto& add(const std::string& title) {
        return m_timers[title] = Timer_t{};
    }

    void start_all() noexcept {
        process_map([](auto& elem) { elem.start(); });
    }

    void stop_all() noexcept {
        process_map([](auto& elem) { elem.stop(); });
    }

    void make_timestamp(const std::string& title) noexcept {
        auto it = m_timers.find(title);
        if (it != m_timers.end() && it->second.is_running)
            it->second.timestamp();
    }

    [[nodiscard]] decltype(auto) get_all() const noexcept { return m_timers; }

    void remove(const std::string& title) noexcept { m_timers.erase(title); }

    void remove_all() noexcept { m_timers.clear(); }

   private:
    TimerMap m_timers;
    void process_map(auto&& func) noexcept {
        auto&& timers = m_timers | std::views::values;
        std::ranges::for_each(timers, func);
    }
};

template <Measurement M>
class Timer_Wrapper {
    using Timer_t = Timer<M>;

   public:
    explicit Timer_Wrapper(Timer_t& timer) noexcept : m_timer(timer) { m_timer.start(); }
    ~Timer_Wrapper() noexcept { m_timer.stop(); }

    Timer_Wrapper(const Timer_Wrapper&) = delete;
    Timer_Wrapper& operator=(const Timer_Wrapper&) = delete;
    Timer_Wrapper(Timer_Wrapper&&) = delete;
    Timer_Wrapper& operator=(Timer_Wrapper&&) = delete;

   private:
    Timer_t& m_timer;
};