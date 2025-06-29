#pragma once

#include "intel-uncore-freq-dumper/UncoreFrequencyReaderFunction.hpp"
#include <atomic>
#include <chrono>
#include <cpucounters.h>
#include <filesystem>
#include <firestarter/Measurement/TimeValue.hpp>
#include <vector>

namespace intel_uncore_freq_dumper {

struct UncoreFrequencyReaderSysfsFunction : UncoreFrequencyReaderFunction {
  /// Read the value of a sysfs entry from a given File
  /// \arg File The file from which to read the content
  /// \returns The value read in the specifed sysfs file
  static auto readSysfsValue(const std::filesystem::path& File) -> unsigned;

  /// The thread function that periodically read the uncore counters and save the uncore frequency to the ReadValues
  /// vector.
  /// \arg SleepTime Read the frequency sysfs entry every specified seconds.
  /// \arg ReadValues The reference to the map from the metric name to the vector of read time values.
  /// \arg ReadValuesMutex The reference to the mutex that is used to lock acces to ReadValues
  /// \arg StopThread The reference to the mutex that is used to terminate the thread function.
  static void
  threadFunction(std::chrono::milliseconds SleepTime,
                 std::unordered_map<std::string, std::vector<firestarter::measurement::TimeValue>>& ReadValues,
                 std::mutex& ReadValuesMutex, std::atomic<bool>& StopThread);

  /// The sysfs folder where the uncore frequency can be read.
  constexpr static const char* SysfsPath = "/sys/devices/system/cpu/intel_uncore_frequency";
};

} // namespace intel_uncore_freq_dumper