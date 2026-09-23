cmake_minimum_required(VERSION 3.15)
project(DSACourse VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. Definir la Librería del Curso (Header Only) ---
add_library(dsa_core INTERFACE)
target_include_directories(dsa_core INTERFACE include)

# --- 2. Descargar GoogleTest Automáticamente ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Esto evita que se instale en el sistema, lo mantiene local
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# --- 3. Habilitar Testing ---
enable_testing()
include(GoogleTest)

# --- 4. Definir Ejecutables y Tests ---

add_executable(demo_vector examples/demo_vector.cpp)
target_link_libraries(demo_vector dsa_core)

# Ejemplo de Benchmark
add_executable(bench_sorting benchmarks/sorting_benchmark.cpp)
target_link_libraries(bench_sorting dsa_core)

# Todos los archivos *_test.cpp bajo tests/ se recogen automáticamente,
# así cada módulo nuevo solo necesita añadir su archivo de test.
file(GLOB_RECURSE DSA_TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
add_executable(unit_tests ${DSA_TEST_SOURCES})
target_link_libraries(unit_tests dsa_core GTest::gtest_main)
gtest_discover_tests(unit_tests)
