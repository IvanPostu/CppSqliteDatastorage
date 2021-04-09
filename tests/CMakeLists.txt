
if (APPLE)
  add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
  add_definitions(-D__GLIBCXX__)
endif (APPLE)

include_directories( ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/src )

##############
#
# Configuration for test automation
#
##############

# Simple test case begin:
add_executable(
  runUnitTests
  ${CMAKE_SOURCE_DIR}/tests/src/calculator_test.cpp
)
# target_include_directories(runUnitTests PUBLIC ${CMAKE_SOURCE_DIR}/src/calculator)

# Standard linking to gtest stuff.
target_link_libraries(runUnitTests gtest gtest_main)

# Extra linking for the project.
target_link_libraries(runUnitTests calculator_lib)

# This is so you can do 'make test' to see all your tests run, instead of
# manually running the executable runUnitTests to see those specific tests.
add_test(NAME run-tests COMMAND runUnitTests)
# Simple test case end;


