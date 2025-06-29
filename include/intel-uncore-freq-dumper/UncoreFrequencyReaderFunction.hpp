#pragma once

#include "firestarter/Measurement/Summary.hpp"
#include <atomic>
#include <chrono>
#include <cpucounters.h>
#include <firestarter/Measurement/TimeValue.hpp>
#include <vector>

namespace intel_uncore_freq_dumper {

struct UncoreFrequencyReaderFunction {
  /// The thread function that periodically read the uncore counters and save the uncore frequency to the ReadValues
  /// vector.
  /// \arg SleepTime Read the frequency MSR counter every specified seconds.
  /// \arg ReadValues The reference to the map from the metric name to the vector of read time values.
  /// \arg ReadValuesMutex The reference to the mutex that is used to lock acces to ReadValues
  /// \arg StopThread The reference to the mutex that is used to terminate the thread function.
  static void
  threadFunction(std::chrono::milliseconds SleepTime,
                 std::unordered_map<std::string, std::vector<firestarter::measurement::TimeValue>>& ReadValues,
                 std::mutex& ReadValuesMutex, std::atomic<bool>& StopThread);
};

} // namespace intel_uncore_freq_dumper