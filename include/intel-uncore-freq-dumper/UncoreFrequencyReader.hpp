#pragma once

#include "firestarter/Measurement/Summary.hpp"
#include "intel-uncore-freq-dumper/UncoreFrequencyReaderPcmFunction.hpp"
#include "intel-uncore-freq-dumper/UncoreFrequencyReaderSysfsFunction.hpp"
#include <atomic>
#include <chrono>
#include <cpucounters.h>
#include <firestarter/Measurement/TimeValue.hpp>
#include <iterator>
#include <unordered_map>
#include <vector>

namespace intel_uncore_freq_dumper {

/// This class wrapps Intel PCM, reads the uncore frequency after a specifed number of milliseconds and saves them to a
/// nice datastructure.
template <class ReaderFunction> class UncoreFrequencyReader {
public:
  UncoreFrequencyReader() = delete;

  /// Start the uncore frequency reader.
  /// \arg SleepTime Read the frequency MSR counter every specified seconds.
  explicit UncoreFrequencyReader(std::chrono::milliseconds SleepTime) {
    // Start the reader thread
    ReaderThread = std::thread(ReaderFunction::threadFunction, SleepTime, std::ref(ReadValues),
                               std::ref(ReadValuesMutex), std::ref(StopThread));
  }

  ~UncoreFrequencyReader() {
    StopThread = true;
    ReaderThread.join();
  }

  /// Read the summary of measured uncore frequencies between two timepoints.
  /// \arg StartTime The time after which to start reading the values
  /// \arg StopTime The time after which to stop reading the values
  /// \returns The summary of the measured uncore frequency per socket.
  auto getSummary(std::chrono::high_resolution_clock::time_point StartTime,
                  std::chrono::high_resolution_clock::time_point StopTime)
      -> std::unordered_map<std::string, firestarter::measurement::Summary> {
    std::unordered_map<std::string, firestarter::measurement::Summary> Summaries;

    auto FindAll = [&StartTime, &StopTime](auto const& Tv) { return StartTime <= Tv.Time && Tv.Time <= StopTime; };

    decltype(ReadValues) CroppedValues;

    {
      const std::lock_guard Lk(ReadValuesMutex);

      for (const auto& [Name, Values] : ReadValues) {
        auto& Result = CroppedValues[Name];
        std::copy_if(Values.begin(), Values.end(), std::back_inserter(Result), FindAll);
      }
    }

    MetricType Metric{};
    Metric.Absolute = 1;

    for (auto& [Name, Values] : CroppedValues) {
      Summaries.emplace(Name, firestarter::measurement::Summary::calculate(Values.begin(), Values.end(),
                                                                           /*MetricType=*/Metric, /*NumThreads=*/0));
    }

    return Summaries;
  };

private:
  /// The thread that executes the reading
  std::thread ReaderThread;
  /// The map from the metric name to the vector of read time values.
  std::unordered_map<std::string, std::vector<firestarter::measurement::TimeValue>> ReadValues;
  /// Mutex to access ReadValues
  std::mutex ReadValuesMutex;
  /// Atomic to stop the threads execution
  std::atomic<bool> StopThread = false;
};

using UncoreFrequencyPcmReader = UncoreFrequencyReader<UncoreFrequencyReaderPcmFunction>;
using UncoreFrequencySysfsReader = UncoreFrequencyReader<UncoreFrequencyReaderSysfsFunction>;

} // namespace intel_uncore_freq_dumper