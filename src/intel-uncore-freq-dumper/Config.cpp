#include "intel-uncore-freq-dumper/Config.hpp"

#include <cstdlib>
#include <cxxopts.hpp>
#include <exception>
#include <iostream>

namespace intel_uncore_freq_dumper {

Config::Config(int Argc, const char** Argv)
    : Argc(Argc)
    , Argv(Argv) {
  const auto* ExecutableName = *Argv;

  cxxopts::Options Parser(ExecutableName);

  // clang-format off
  Parser.add_options()
    ("measurement-duration", "Duration of the uncore frequency measurement in milliseconds",
      cxxopts::value<unsigned>()->default_value("10000"))
    ("measurement-interval", "Interval of measurements in milliseconds",
      cxxopts::value<unsigned>()->default_value("10"))
    ("start-delta", "Cut of first N milliseconds of measurement",
      cxxopts::value<unsigned>()->default_value("5000"), "N")
    ("stop-delta", "Cut of last N milliseconds of measurement",
      cxxopts::value<unsigned>()->default_value("2000"), "N")
    ("outfile", "The path where the results should be saved to.", cxxopts::value<std::string>()->default_value("outfile.csv"))
    ("use-sysfs", "Use sysfs or intel pcm for reading the metric.", cxxopts::value<bool>()->default_value("false"))
  ;
  // clang-format on

  try {
    auto Options = Parser.parse(Argc, Argv);

    MeasurementDuration = std::chrono::milliseconds(Options["measurement-duration"].as<unsigned>());
    MeasurementInterval = std::chrono::milliseconds(Options["measurement-interval"].as<unsigned>());
    StartDelta = std::chrono::milliseconds(Options["start-delta"].as<unsigned>());
    StopDelta = std::chrono::milliseconds(Options["stop-delta"].as<unsigned>());
    OutfilePath = Options["outfile"].as<std::string>();
    UseSysfs = Options["use-sysfs"].as<bool>();
  } catch (std::exception& E) {
    std::cerr << Parser.help() << "\n";
    std::cerr << "\n";
    // NOLINTNEXTLINE(concurrency-mt-unsafe)
    exit(EXIT_SUCCESS);
  }
}

} // namespace intel_uncore_freq_dumper