# ========================================================================== #
# external packages
# ========================================================================== #

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

MESSAGE(${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/PSim/test/testfiles DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


add_executable(ttassembler ttassembler.cc)
target_link_libraries(ttassembler
        ${GTEST_BOTH_LIBRARIES}
        SIMLIB
        pthread)
gtest_discover_tests(ttassembler)

add_executable(ttsimulator ttsimulator.cc)
target_link_libraries(ttsimulator
        ${GTEST_BOTH_LIBRARIES}
        SIMLIB
        pthread)
gtest_discover_tests(ttsimulator)

enable_testing()