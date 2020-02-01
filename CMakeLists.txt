cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# option(BUILD_COVERAGE "Build coverage" OFF)

set(
  HUNTER_CACHE_SERVERS
  "https://github.com/bmstu-iu8-cpp-sem-3/hunter-cache"
  CACHE STRING "Default cache server"
)

include("tools/gate/cmake/HunterGate.cmake")

HunterGate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.23.218.tar.gz"
  SHA1 "9a3594a61227967fb92159339ba326701b287b19"
)

project(producer_consumer)

hunter_add_package(GTest)
hunter_add_package(Boost COMPONENTS program_options
thread filesystem)
find_package(Boost CONFIG REQUIRED program_options
thread filesystem)
find_package(GTest CONFIG REQUIRED)


add_library(gumbo INTERFACE
)
add_library(beast INTERFACE
)
add_library(asio INTERFACE
)
add_library(producer_consumer INTERFACE
)
add_library(safe_queue INTERFACE
)
add_library(task INTERFACE
)

add_executable(tests
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp
)

target_include_directories(gumbo
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/gumbo-parser/src
)
target_include_directories(asio
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/boost/include
)
target_include_directories(beast
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/beast/include/boost
)
target_include_directories(task
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(producer_consumer
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(tests
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(producer_consumer INTERFACE safe_queue)
target_link_libraries(task INTERFACE producer_consumer
Boost::program_options Boost::thread Boost::filesystem asio beast gumbo)
target_link_libraries(tests GTest::main task)

enable_testing()
add_test(NAME unit_tests COMMAND tests)

# if(BUILD_COVERAGE)
#   set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
#   list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#   find_package(codecov)
#   add_coverage(producer_consumer)
#   add_coverage(tests)
#   list(APPEND LCOV_REMOVE_PATTERNS "'${PROJECT_SOURCE_DIR}/tests/*'")
#   coverage_evaluate()
# endif()
