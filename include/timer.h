#ifndef __TIMER_H__
#define __TIMER_H__

#include <chrono>

namespace Dental {
  class Timer {
  public:
    using time_point = std::chrono::steady_clock::time_point;

    Timer();
    ~Timer() {}

    Timer& operator = (Timer&&) noexcept = delete;
    Timer& operator = (const Timer&) = delete;
    Timer(const Timer&) = delete;
    Timer(Timer&&) noexcept = delete;

    static Timer& instance();
    static time_point now() { return std::chrono::steady_clock::now(); }

    void start() { start_time_ = std::chrono::steady_clock::now(); }

    void startTime(time_point &t) { start_time_ = t; }
    time_point startTime() const { return start_time_; }

    // Get elapsed time in seconds
    inline double time_s() const { return delta_s(start_time_, now()); }

    // Get elapsed time in milliseconds
    inline double time_m() const { return delta_m(start_time_, now()); }

    // Get elapsed time in microseconds
    inline double time_u() const { return delta_u(start_time_, now()); }

    // Get elapsed time in nanoseconds
    inline double time_n() const { return delta_n(start_time_, now()); }

    // Get the time in seconds between t1 and t2
    inline double delta_s(const time_point& t1, const time_point& t2) const {
      auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
      return (double)delta.count() * std::chrono::nanoseconds::period::num / std::chrono::nanoseconds::period::den;
    }

    /** Get the time in milliseconds between t1 and t2.*/
    inline double delta_m(const time_point &t1, const time_point &t2) const { return delta_s(t1, t2) * 1e3; }

    /** Get the time in microseconds between t1 and t2.*/
    inline double delta_u(const time_point &t1, const time_point &t2) const { return delta_s(t1, t2) * 1e6; }

    /** Get the time in nanoseconds between timer ticks t1 and t2.*/
    inline double delta_n(const time_point &t1, const time_point &t2) const { return delta_s(t1, t2) * 1e9; }

  protected :

    std::chrono::steady_clock::time_point start_time_;
  };
};
#endif