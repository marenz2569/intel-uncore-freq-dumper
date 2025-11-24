cmake_minimum_required(VERSION 3.22)
project(intel-uncore-freq-dumper)
enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include(cmake/GitSubmoduleUpdate.cmake)

# set intel-uncore-freq-dumper version
find_package(Git)
if(Git_FOUND)
	_is_git(${CMAKE_CURRENT_SOURCE_DIR} IN_GIT)
endif()

SET (_INTEL_UNCORE_FREQ_DUMPER_VERSION_TEMPERED OFF)

SET(_INTEL_UNCORE_FREQ_DUMPER_VERSION_STRING "default" CACHE STRING "_INTEL_UNCORE_FREQ_DUMPER_VERSION_STRING should not be used if you do not know what to do.")
if("${_INTEL_UNCORE_FREQ_DUMPER_VERSION_STRING}" STREQUAL "default")
	if (IN_GIT)
		execute_process(COMMAND ${GIT_EXECUTABLE} describe --always --tags --dirty
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			OUTPUT_VARIABLE _INTEL_UNCORE_FREQ_DUMPER_VERSION_STRING
			)
		string(STRIP ${_INTEL_UNCORE_FREQ_DUMPER_VERSION_STRING} _INTEL_UNCORE_FREQ_DUMPER_VERSION_STRING)
	else()
		message(AUTHOR_WARNING "Cannot detect intel-uncore-freq-dumper version.")
		SET(_INTEL_UNCORE_FREQ_DUMPER_VERSION_STRING "unknown")
	endif()
endif()
add_compile_definitions(_INTEL_UNCORE_FREQ_DUMPER_VERSION_STRING="${_INTEL_UNCORE_FREQ_DUMPER_VERSION_STRING}")

SET(_INTEL_UNCORE_FREQ_DUMPER_BUILD_YEAR "default" CACHE STRING "_INTEL_UNCORE_FREQ_DUMPER_BUILD_YEAR should not be used if you do not know what to do.")
if("${_INTEL_UNCORE_FREQ_DUMPER_BUILD_YEAR}" STREQUAL "default")
	if(IN_GIT)
		execute_process(COMMAND ${GIT_EXECUTABLE} show -s --format=%ci HEAD
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			OUTPUT_VARIABLE _INTEL_UNCORE_FREQ_DUMPER_BUILD_YEAR
			)
		string(SUBSTRING ${_INTEL_UNCORE_FREQ_DUMPER_BUILD_YEAR} 0 4 _INTEL_UNCORE_FREQ_DUMPER_BUILD_YEAR)
	else()
		message(AUTHOR_WARNING "Cannot detect intel-uncore-freq-dumper build year.")
		SET(_INTEL_UNCORE_FREQ_DUMPER_BUILD_YEAR "unknown")
	endif()
endif()
add_compile_definitions(_INTEL_UNCORE_FREQ_DUMPER_BUILD_YEAR="${_INTEL_UNCORE_FREQ_DUMPER_BUILD_YEAR}")

# init git submodules
include(cmake/GitSubmoduleUpdate.cmake)
git_submodule_update()

if(NOT DEFINED ASMJIT_STATIC)
	set(ASMJIT_STATIC TRUE)
endif()

# FIRESTARTER binaries get build all the time, but static libraries are not present all the time.
# Therefore we disable the static build.
# These lines can be removed once the issue is resolved: https://github.com/tud-zih-energy/FIRESTARTER/issues/116
if(NOT DEFINED FIRESTARTER_LINK_STATIC)
	set(FIRESTARTER_LINK_STATIC OFF)
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(lib/FIRESTARTER)
add_subdirectory(lib/pcm)

include_directories(include)
include_directories(lib/cxxopts/include)
include_directories(lib/pcm/src)

add_subdirectory(src)

add_subdirectory(test)