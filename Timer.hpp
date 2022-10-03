#pragma once
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

inline static const std::string addBrackets(const std::string& str) {
    return {'[' + str + ']'};
}

namespace Timer {
namespace ch = std::chrono;
using ns = ch::nanoseconds;
using ms = ch::milliseconds;
using s = ch::seconds;
using highresclock = ch::high_resolution_clock;

template <typename ToDuration>
inline auto getDiff(const ch::system_clock::duration& first, const ch::system_clock::duration& sec) {
    return ch::duration_cast<ToDuration>(sec - first);
}

template <typename toDuration>
class SimpleTimer {
   public:
    SimpleTimer(const std::string& title)
        : m_title{addBrackets(title)},
          m_begin{highresclock::now()} {
        std::cerr << m_title << " is started\n";
    }

    ~SimpleTimer() {
        std::cerr << m_title << " took: "
                  << ch::duration_cast<toDuration>(highresclock::now() - m_begin).count() << '\n';
    }

   private:
    std::string m_title;
    ch::time_point<highresclock> m_begin;
};

template <typename toDuration>
class LogTimer {
   public:
    LogTimer(const std::string& title)
        : m_title{addBrackets(title)} {}

    inline auto begin() {
        m_begin = highresclock::now();
        return m_begin;
    }

    inline auto end() {
        m_end = highresclock::now();
        return m_end;
    }

    void reset() { m_begin = highresclock::now(); }

    inline std::string log() const {
        std::stringstream sstr;
        sstr << m_title << " spent "
             << getDiff<toDuration>(highresclock::now(), m_begin).count();
        return sstr.str();
    }

   private:
    std::string m_title;
    ch::time_point<highresclock> m_begin, m_end;
};

}  // namespace Timer