#pragma once

#include <chrono>
#include <string>

namespace intel_uncore_freq_dumper {

/// This struct contains the parsed config from the command line for intel-uncore-freq-dumper.
struct Config {
  /// The argument vector from the command line.
  const char** Argv;
  /// The argument count from the command line.
  int Argc;

  /// The time to skip from the measurement start
  std::chrono::milliseconds StartDelta = std::chrono::milliseconds(0);
  /// The time to skip from the measurement stop
  std::chrono::milliseconds StopDelta = std::chrono::milliseconds(0);
  /// The duration of the measurement
  std::chrono::milliseconds MeasurementDuration = std::chrono::milliseconds(0);
  /// The duration after which the uncore frequency is read.
  std::chrono::milliseconds MeasurementInterval = std::chrono::milliseconds(0);

  /// The path where the output should be saved.
  std::string OutfilePath;

  Config() = delete;

  /// Parser the config from the command line argumens.
  Config(int Argc, const char** Argv);
};

} // namespace intel_uncore_freq_dumper