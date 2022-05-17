#include <timer.h>

namespace Dental {
  Timer::Timer() : start_time_(now()) {
  }

  Timer& Timer::instance() {
    static Timer timer;
    return timer;
  }
}