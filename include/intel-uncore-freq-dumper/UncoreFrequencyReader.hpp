#pragma once

#include "firestarter/Measurement/Summary.hpp"
#include "intel-uncore-freq-dumper/UncoreFrequencyReaderPcmFunction.hpp"
#include <atomic>
#include <chrono>
#include <cpucounters.h>
#include <firestarter/Measurement/TimeValue.hpp>
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
      -> std::vector<firestarter::measurement::Summary> {
    std::vector<firestarter::measurement::Summary> Summaries;

    auto FindAll = [&StartTime, &StopTime](auto const& Tv) { return StartTime <= Tv.Time && Tv.Time <= StopTime; };

    decltype(ReadValues) CroppedValues(ReadValues.size());

    {
      const std::lock_guard Lk(ReadValuesMutex);

      for (auto I = 0; I < ReadValues.size(); I++) {
        std::copy_if(ReadValues[I].cbegin(), ReadValues[I].cend(), std::back_inserter(CroppedValues[I]), FindAll);
      }
    }

    MetricType Metric{};
    Metric.Absolute = 1;

    for (auto& CroppedValue : CroppedValues) {
      Summaries.emplace_back(firestarter::measurement::Summary::calculate(CroppedValue.begin(), CroppedValue.end(),
                                                                          /*MetricType=*/Metric, /*NumThreads=*/0));
    }

    return Summaries;
  };

private:
  /// The thread that executes the reading
  std::thread ReaderThread;
  /// The vector that holds the vector measurement values for each socket. The vector of measurement values contains the
  /// timepoint of the end of the measurement duration and the average uncore frequency in GHz during the measurement
  /// duration.
  std::vector<std::vector<firestarter::measurement::TimeValue>> ReadValues;
  /// Mutex to access ReadValues
  std::mutex ReadValuesMutex;
  /// Atomic to stop the threads execution
  std::atomic<bool> StopThread = false;
};

using UncoreFrequencyPcmReader = UncoreFrequencyReader<UncoreFrequencyReaderPcmFunction>;

} // namespace intel_uncore_freq_dumper