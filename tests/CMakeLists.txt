cmake_minimum_required(VERSION 2.6)
 
# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
# Link runTests with what we want to test and the GTest and pthread library
add_executable(runTests main_TESTS.cpp binarySearch_test.cpp distance_test.cpp HashTable_test.cpp clustering_test.cpp)
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread)
