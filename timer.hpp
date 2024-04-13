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
    using SC = std::chrono::steady_clock;
    using TP = std::chrono::time_point<SC>;

   public:
    Timer() = default;

    [[nodiscard]] auto start_timestamp() const { return m_start; }
    [[nodiscard]] auto stop_timestamp() const { return m_stop; }
    [[nodiscard]] auto all_timestamps() const {
        return m_timestamps |
               std::views::transform([](const auto& ns) { return std::chrono::duration_cast<M>(ns); }) |
               std::ranges::to<std::vector<M>>();
    }

    [[nodiscard]] auto get_duration() const {
        const auto duration = (is_running ? SC::now() : m_stop) - m_start;
        return std::chrono::duration<double, typename M::period>(duration);
    }

    void reset() {
        m_start = m_stop = {};
        m_timestamps.clear();
        is_running = false;
    }

    void start() {
        reset();
        m_timestamps.emplace_back(0);
        m_start = SC::now();
        is_running = true;
    }

    void stop() {
        if (!is_running)
            return;
        timestamp();
        m_stop = SC::now();
        is_running = false;
    }

    void timestamp() {
        if (is_running)
            m_timestamps.emplace_back(SC::now() - m_start);
    }

   private:
    bool is_running = false;
    TP m_start = {}, m_stop = {};
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

    void start_all() {
        for (auto& pair : m_timers)
            pair.second.start();
    }

    void start_all_par() {
        std::for_each(std::execution::par, m_timers.begin(), m_timers.end(),
                      [](auto& pair) { pair.second.start(); });
    }

    void stop_all() {
        for (auto& pair : m_timers)
            pair.second.stop();
    }

    void stop_all_par() {
        std::for_each(std::execution::par, m_timers.begin(), m_timers.end(),
                      [](auto& pair) { pair.second.stop(); });
    }

    void make_timestamp(const std::string& title) {
        auto it = m_timers.find(title);
        if (it != m_timers.end()) {
            it->second.timestamp();
        }
    }

    [[nodiscard]] auto get_all() const { return m_timers; }
    void remove(const std::string& title) { m_timers.erase(title); }
    void remove_all() { m_timers.clear(); }

   private:
    TimerMap m_timers;
};

template <Measurement M>
class Timer_Wrapper {
    using Timer_t = Timer<M>;

   public:
    explicit Timer_Wrapper(Timer_t& timer) : m_timer(timer) { m_timer.start(); }
    ~Timer_Wrapper() { m_timer.stop(); }

    Timer_Wrapper(const Timer_Wrapper&) = delete;
    Timer_Wrapper& operator=(const Timer_Wrapper&) = delete;
    Timer_Wrapper(Timer_Wrapper&&) = delete;
    Timer_Wrapper& operator=(Timer_Wrapper&&) = delete;

   private:
    Timer_t& m_timer;
};