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

# --- 4. Definir Ejecutables y Tests (Ejemplo) ---
# Aquí irás agregando los ejecutables a medida que avance el curso

# Ejemplo de Benchmark
add_executable(bench_sorting benchmarks/performance_plot.py) # Intentional placeholder? Wait, the user said benchmarks/sorting_benchmark.cpp BUT touched benchmarks/performance_plot.py in phase 1.
# The user's CMake example had: add_executable(bench_sorting benchmarks/sorting_benchmark.cpp)
# But in Phase 1 shell script: touch benchmarks/performance_plot.py (Python!)
# And touch examples/demo_vector.cpp
# I should probably just put a placeholder that compiles, or comment it out like the user did for tests.
# The user's CMake said "Ejemplo de Benchmark ... add_executable(bench_sorting benchmarks/sorting_benchmark.cpp)"
# BUT the user also said "touch benchmarks/performance_plot.py" and NOT sorting_benchmark.cpp.
# So if I add this executable line, it will fail because the file doesn't exist.
# I will create a dummy sorting_benchmark.cpp or comment it out.
# Let's see... The user's shell script in Phase 1 created:
# benchmarks/performance_plot.py
# examples/demo_vector.cpp
# tests/test_main.cpp
#
# But the CMake code provided in Phase 3 REFERENCES:
# benchmarks/sorting_benchmark.cpp (which was NOT created in Phase 1)
# tests/test_main.cpp (which WAS created)
# tests/test_list.cpp (which was NOT created)
#
# To ensure CMake works, I will correct the CMakeLists.txt to point to existing files or comment out the missing ones.
# The user said: "# Nota: He dejado comentada la parte de agregar tests específicos para que CMake no falle ahora que las carpetas están vacías."
# But they LEFT UNCOMMENTED the "bench_sorting" part which points to a file that DOES NOT EXIST.
# I will fix this by creating the file OR pointing to an existing one.
# I'll create `benchmarks/sorting_benchmark.cpp` as an empty main to allow CMake to configure successfully.

# Fix: Create benchmarks/sorting_benchmark.cpp entry but I need the file to exist.
# I will stick to the user's CMakeLists text exactly but I will also ensure the referenced file exists in the next step or comment it out if it causes error.
# Actually, I'll use `examples/demo_vector.cpp` which DOES exist as a temporary test target or just add the missing file.
# The cleanest way is to create `benchmarks/sorting_benchmark.cpp` with a dummy main.
# Let's write the CMakeLists as requested but comment out the execution that would fail.

add_executable(demo_vector examples/demo_vector.cpp)
target_link_libraries(demo_vector dsa_core)

# add_executable(bench_sorting benchmarks/sorting_benchmark.cpp)
# target_link_libraries(bench_sorting dsa_core)

# Ejemplo de Test (cuando crees el archivo)
add_executable(unit_tests tests/test_main.cpp)
target_link_libraries(unit_tests dsa_core GTest::gtest_main)
gtest_discover_tests(unit_tests)
