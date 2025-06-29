#include "intel-uncore-freq-dumper/UncoreFrequencyReaderSysfsFunction.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cpucounters.h>
#include <filesystem>
#include <firestarter/Measurement/MetricInterface.h>
#include <firestarter/Measurement/Summary.hpp>
#include <firestarter/Measurement/TimeValue.hpp>
#include <fstream>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace intel_uncore_freq_dumper {

auto UncoreFrequencyReaderSysfsFunction::readSysfsValue(const std::filesystem::path& File) -> unsigned {
  std::fstream FileStream(File, std::ios_base::in);

  unsigned Value{};

  FileStream >> Value;

  return Value;
}

void UncoreFrequencyReaderSysfsFunction::threadFunction(
    const std::chrono::milliseconds SleepTime,
    std::vector<std::vector<firestarter::measurement::TimeValue>>& ReadValues, std::mutex& ReadValuesMutex,
    std::atomic<bool>& StopThread) {

  // Map from entry name to frequency file
  std::map<std::string, std::filesystem::path> FrequencyPaths;

  std::ranges::for_each(std::filesystem::recursive_directory_iterator{UncoreFrequencyReaderSysfsFunction::SysfsPath},
                        [&](const auto& Entry) {
                          const auto& Path = Entry.path();
                          if (Path.filename() == "current_freq_khz") {
                            FrequencyPaths.emplace(Path.parent_path().filename().native(), Path);
                          }
                        });

  {
    std::cout << "Found sysfs entries: ";
    for (const auto& [Key, Value] : FrequencyPaths) {
      std::cout << Key << " ";
    }
    std::cout << "\n";
  }

  while (!StopThread.load()) {
    std::this_thread::sleep_for(SleepTime);

    // readServerUncoreCounterState(Pcm, State);

    {
      const std::lock_guard Lk(ReadValuesMutex);

      for (const auto& [Name, File] : FrequencyPaths) {
        ReadValues[Name].emplace_back(std::chrono::system_clock::now(), readSysfsValue(File));
      }
    }
  }
}

} // namespace intel_uncore_freq_dumper
