# ========================================================================== #
# language standard requirements
# ========================================================================== #

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED true)
endif ()


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

set(SIMLIB_INCLUDE
        include/options.hh
        include/psim.hh
        include/utils.hh
        include/register.hh
        include/mmbar.hh
        include/assembler.hh)

set(SIMEXEC_SRCS)

include_directories(include)

# ========================================================================== #
# simulation library
# ========================================================================== #

add_library(SIMLIB SHARED ${SIMLIB_SRCS} ${SIMLIB_INCLUDE})
target_include_directories(SIMLIB PRIVATE include)

add_executable(ttintegration test/ttintegration.cc)
target_link_libraries(ttintegration SIMLIB)
target_include_directories(ttintegration PRIVATE include)


# ========================================================================== #
# functional tests
# ========================================================================== #

#add_subdirectory(test)