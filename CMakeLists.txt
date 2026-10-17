cmake_minimum_required(VERSION 3.16)
project(simulador_torneo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(nucleo_simulador simulador.cpp)
target_include_directories(nucleo_simulador PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(simulador main.cpp)
target_link_libraries(simulador PRIVATE nucleo_simulador)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(simulador_tests tests/caracterizacion.cpp)
target_link_libraries(simulador_tests PRIVATE nucleo_simulador GTest::gtest_main)
add_dependencies(simulador_tests simulador)
target_include_directories(simulador_tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/rutas_prueba.h" CONTENT
"#pragma once
#define SIMULADOR_EXE R\"($<TARGET_FILE:simulador>)\"
#define EQUIPOS_SRC R\"(${CMAKE_SOURCE_DIR}/equipos2.txt)\"
#define RUNTIME_DIR R\"(${CMAKE_SOURCE_DIR}/runtime-data)\"
#define GOLDEN_PATH R\"(${CMAKE_SOURCE_DIR}/evidencia/salida_original.txt)\"
")

add_test(NAME caracterizacion COMMAND simulador_tests)
