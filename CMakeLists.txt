cmake_minimum_required(VERSION 3.20)
project(TaoArkanoid VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Fetch SFML ----
include(FetchContent)

set(SFML_BUILD_AUDIO    OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC      OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
    GIT_SHALLOW    ON
    GIT_PROGRESS   ON
)
FetchContent_MakeAvailable(SFML)

# ---- Sub-projects ----
add_subdirectory(shared)
add_subdirectory(game)
add_subdirectory(server)
add_subdirectory(client)

# ---- Tests (optional) ----
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
