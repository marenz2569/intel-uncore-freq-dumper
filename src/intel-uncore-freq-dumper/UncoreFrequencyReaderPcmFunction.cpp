#include "intel-uncore-freq-dumper/UncoreFrequencyReaderPcmFunction.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cpucounters.h>
#include <firestarter/Measurement/MetricInterface.h>
#include <firestarter/Measurement/Summary.hpp>
#include <firestarter/Measurement/TimeValue.hpp>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace intel_uncore_freq_dumper {

void UncoreFrequencyReaderPcmFunction::readServerUncoreCounterState(
    pcm::PCM& Pcm, std::vector<pcm::ServerUncoreCounterState>& ReadVal) {
  for (auto I = 0; I < ReadVal.size(); I++) {
    ReadVal[I] = Pcm.getServerUncoreCounterState(I);
  }
}

void UncoreFrequencyReaderPcmFunction::threadFunction(
    const std::chrono::milliseconds SleepTime,
    std::vector<std::vector<firestarter::measurement::TimeValue>>& ReadValues, std::mutex& ReadValuesMutex,
    std::atomic<bool>& StopThread) {
  auto& Pcm = *pcm::PCM::getInstance();
  // Programm the counters
  Pcm.checkError(Pcm.program());

  const auto NumSockets = Pcm.getNumSockets();
  const auto UncoreFreqFactor =
      static_cast<double>(Pcm.getNumOnlineSockets()) / static_cast<double>(Pcm.getNumOnlineCores());

  // Set the size of the values vector to the number of sockets.
  ReadValues.resize(NumSockets);

  std::vector<pcm::ServerUncoreCounterState> BeforeState(NumSockets);
  std::vector<pcm::ServerUncoreCounterState> AfterState(NumSockets);

  readServerUncoreCounterState(Pcm, BeforeState);

  while (!StopThread.load()) {
    std::this_thread::sleep_for(SleepTime);

    readServerUncoreCounterState(Pcm, AfterState);

    {
      const std::lock_guard Lk(ReadValuesMutex);

      for (auto Socket = 0; Socket < NumSockets; Socket++) {
        ReadValues[Socket].emplace_back(std::chrono::system_clock::now(),
                                        getAverageUncoreFrequency(BeforeState[Socket], AfterState[Socket]) *
                                            UncoreFreqFactor / 1e9);
      }
    }

    std::swap(BeforeState, AfterState);
  }
}

} // namespace intel_uncore_freq_dumper
