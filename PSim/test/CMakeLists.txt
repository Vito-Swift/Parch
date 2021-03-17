include_directories(${GTEST_INCLUDE_DIRS})

MESSAGE(${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/PSim/test/testfiles DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


add_executable(ttassembler ttassembler.cc)
target_link_libraries(ttassembler
        ${GTEST_BOTH_LIBRARIES}
        SIMLIB
        pthread)
gtest_discover_tests(ttassembler)

enable_testing()