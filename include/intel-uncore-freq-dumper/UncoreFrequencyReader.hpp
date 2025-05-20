#pragma once

#include "firestarter/Measurement/Summary.hpp"
#include <chrono>
#include <cpucounters.h>
#include <firestarter/Measurement/TimeValue.hpp>
#include <vector>

namespace intel_uncore_freq_dumper {

/// This class wrapps Intel PCM, reads the uncore frequency after a specifed number of milliseconds and saves them to a
/// nice datastructure.
class UncoreFrequencyReader {
public:
  UncoreFrequencyReader() = delete;

  /// Start the uncore frequency reader.
  /// \arg SleepTime Read the frequency MSR counter every specified seconds.
  explicit UncoreFrequencyReader(std::chrono::milliseconds SleepTime);

  ~UncoreFrequencyReader();

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

  /// Read the summary of measured uncore frequencies between two timepoints.
  /// \arg StartTime The time after which to start reading the values
  /// \arg StopTime The time after which to stop reading the values
  /// \returns The summary of the measured uncore frequency per socket.
  auto
  getSummary(std::chrono::high_resolution_clock::time_point StartTime,
             std::chrono::high_resolution_clock::time_point StopTime) -> std::vector<firestarter::measurement::Summary>;

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

} // namespace intel_uncore_freq_dumper