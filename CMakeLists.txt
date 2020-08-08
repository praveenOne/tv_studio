cmake_minimum_required(VERSION 3.0.0)
project(tv_studio VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)

add_executable(tv_studio src/main.cpp)

target_compile_features(tv_studio PRIVATE cxx_std_17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(tv_studio PRIVATE SDL2::SDL2main SDL2::SDL2-static)
target_link_libraries(tv_studio PRIVATE SDL2::SDL2_image)
