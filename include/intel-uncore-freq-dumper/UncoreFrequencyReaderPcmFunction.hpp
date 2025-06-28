#pragma once

#include "intel-uncore-freq-dumper/UncoreFrequencyReaderFunction.hpp"
#include <atomic>
#include <chrono>
#include <cpucounters.h>
#include <firestarter/Measurement/TimeValue.hpp>
#include <vector>

namespace intel_uncore_freq_dumper {

struct UncoreFrequencyReaderPcmFunction : UncoreFrequencyReaderFunction {
  /// Read the uncore counter state from PCM and save it to ReadVal
  /// \arg Pcm The PCM instance
  /// \arg ReadVal The vector in which to save the read uncore counter values.
  static void readServerUncoreCounterState(pcm::PCM& Pcm, std::vector<pcm::ServerUncoreCounterState>& ReadVal);

  /// The thread function that periodically read the uncore counters and save the uncore frequency to the ReadValues
  /// vector.
  /// \arg SleepTime Read the frequency MSR counter every specified seconds.
  /// \arg ReadValues The reference to the vector to which to save the values
  /// \arg ReadValuesMutex The reference to the mutex that is used to lock acces to ReadValues
  /// \arg StopThread The reference to the mutex that is used to terminate the thread function.
  static void threadFunction(std::chrono::milliseconds SleepTime,
                             std::vector<std::vector<firestarter::measurement::TimeValue>>& ReadValues,
                             std::mutex& ReadValuesMutex, std::atomic<bool>& StopThread);
};

} // namespace intel_uncore_freq_dumper