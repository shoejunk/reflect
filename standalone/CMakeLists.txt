cmake_minimum_required(VERSION 3.14...3.22)

project(ReflectStandalone LANGUAGES CXX)

# --- Import tools ----

include(../cmake/tools.cmake)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

CPMAddPackage(
  GITHUB_REPOSITORY jarro2783/cxxopts
  VERSION 3.0.0
  OPTIONS "CXXOPTS_BUILD_EXAMPLES NO" "CXXOPTS_BUILD_TESTS NO" "CXXOPTS_ENABLE_INSTALL YES"
)

CPMAddPackage(NAME Reflect SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# Add package: shoejunk/log
CPMAddPackage(
	NAME log
	GITHUB_REPOSITORY shoejunk/log
	VERSION 0.3.0
)

# ---- Create standalone executable ----

file(GLOB sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_executable(${PROJECT_NAME} ${sources})

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 OUTPUT_NAME "Reflect")

target_link_libraries(${PROJECT_NAME} Reflect::Reflect Log::Log cxxopts)
