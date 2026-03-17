cmake_minimum_required(VERSION 3.16)
project(FlappyBird LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.2
    GIT_SHALLOW TRUE
)
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
FetchContent_MakeAvailable(SFML)

add_executable(FlappyBird
    src/main.cpp
    src/Game.cpp
    src/Bird.cpp
    src/PipeManager.cpp
    src/Leaderboard.cpp
)

target_include_directories(FlappyBird PRIVATE src)
target_link_libraries(FlappyBird PRIVATE sfml-graphics sfml-window sfml-system)
