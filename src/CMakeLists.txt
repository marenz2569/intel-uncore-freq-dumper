# intel-uncore-freq-dumper executable
add_executable(intel-uncore-freq-dumper
    intel-uncore-freq-dumper/Config.cpp
    intel-uncore-freq-dumper/Main.cpp
    intel-uncore-freq-dumper/UncoreFrequencyReaderPcmFunction.cpp
    )

target_link_libraries(intel-uncore-freq-dumper
    Threads::Threads
    firestartercore
    asmjit::asmjit
    PCM_STATIC
    )