cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

option(test "Build all tests." OFF)

set(PROJECT_NAME CppSqliteDatastorage)
set(CMAKE_CXX_FLAGS "-g -Wall")

project(
  ${PROJECT_NAME}
  VERSION 0.1
  DESCRIPTION "Application description."
)

message(STATUS "${PROJECT_NAME} build mode: ${CMAKE_BUILD_TYPE}")

add_library(calculator_lib src/calculator/calculator.cpp)
add_executable(
  ${PROJECT_NAME}
  src/main.cpp
)
target_link_libraries(${PROJECT_NAME} calculator_lib)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)



set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/ )



if (test)
  enable_testing()

  # This adds another subdirectory, which has 'project(gtest)'.
  add_subdirectory( external/googletest )
  add_subdirectory( tests )

endif()
