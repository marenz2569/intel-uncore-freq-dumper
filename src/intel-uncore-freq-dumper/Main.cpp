#include "intel-uncore-freq-dumper/Config.hpp"
#include "intel-uncore-freq-dumper/UncoreFrequencyReader.hpp"

#include <chrono>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <thread>
#include <variant>

auto main(int Argc, const char** Argv) -> int {
  std::cout << "intel-uncore-freq-dumper. Version " << _INTEL_UNCORE_FREQ_DUMPER_VERSION_STRING << "\n"
            << "Copyright (C) " << _INTEL_UNCORE_FREQ_DUMPER_BUILD_YEAR << " Markus Schmidl" << "\n";

  try {
    const intel_uncore_freq_dumper::Config Cfg{Argc, Argv};

    std::variant<std::unique_ptr<intel_uncore_freq_dumper::UncoreFrequencyPcmReader>,
                 std::unique_ptr<intel_uncore_freq_dumper::UncoreFrequencySysfsReader>>
        Reader;

    if (Cfg.UseSysfs) {
      Reader = std::make_unique<intel_uncore_freq_dumper::UncoreFrequencySysfsReader>(Cfg.MeasurementInterval);
    } else {
      Reader = std::make_unique<intel_uncore_freq_dumper::UncoreFrequencyPcmReader>(Cfg.MeasurementInterval);
    }

    // Wait for the measurement to finish
    auto StartTime = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(Cfg.MeasurementDuration);
    auto StopTime = std::chrono::high_resolution_clock::now();

    auto Summaries = std::visit(
        [&](auto&& Arg) -> auto { return Arg->getSummary(StartTime + Cfg.StartDelta, StopTime - Cfg.StopDelta); },
        Reader);

    std::ofstream OutfileStream(Cfg.OutfilePath);

    OutfileStream << "socket,num_timepoints,duration_ms,average,stddev\n";
    // TODO: move socket description to name, use map instead of vector in vector
    for (auto I = 0; I < Summaries.size(); I++) {
      OutfileStream << I << "," << Summaries[I].NumTimepoints << "," << Summaries[I].Duration.count() << ","
                    << Summaries[I].Average << "," << Summaries[I].Stddev << "\n";
    }
  } catch (std::exception const& E) {
    std::cerr << E.what();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
