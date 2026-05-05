cmake_minimum_required(VERSION 3.20)
project(MiProyecto)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ruta de SFML
set(SFML_DIR "C:/msys64/mingw64/lib/cmake/SFML")

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(${PROJECT_NAME} main.cpp)
add_executable(run_v006 demo_v006.cpp)

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

if(WIN32)
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MINGW_BIN_DIR}/libsfml-graphics-3.dll"
        "${MINGW_BIN_DIR}/libsfml-window-3.dll"
        "${MINGW_BIN_DIR}/libsfml-system-3.dll"
        "${MINGW_BIN_DIR}/libstdc++-6.dll"
        "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
        "${MINGW_BIN_DIR}/libwinpthread-1.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()
