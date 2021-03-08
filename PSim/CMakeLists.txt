# ========================================================================== #
# language standard requirements
# ========================================================================== #

if (NOT DEFINED CMAKE_C_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED true)
endif ()


# ========================================================================== #
# external packages
# ========================================================================== #

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# ========================================================================== #
# source code
# ========================================================================== #

set(SIMLIB_SRCS
        src/options.cc
        src/psim.cc
        src/utils.cc
        src/register.cc
        src/mmbar.cc
        src/assembler.cc)

set(SIMEXEC_SRCS)


# ========================================================================== #
# simulation library
# ========================================================================== #

add_library(SIMLIB ${SIMLIB_SRCS})
target_include_directories(SIMLIB
        PRIVATE include)


# ========================================================================== #
# functional tests
# ========================================================================== #

add_subdirectory(test)