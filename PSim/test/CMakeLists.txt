include_directories(${GTEST_INCLUDE_DIRS})

add_executable(ttoptions ttoptions.cc)
target_link_libraries(ttoptions ${GTEST_BOTH_LIBRARIES})
target_link_libraries(ttoptions ${CMAKE_THREAD_LIBS_INIT})

add_test(IntegrateTest ttoptions)
enable_testing()