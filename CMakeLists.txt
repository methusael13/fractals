# CMake for Fractals

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(Fractals VERSION 0.1 LANGUAGES C)
project(lib VERSION 0.1 LANGUAGES C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")

add_library(lib
    # Core library
    src/gfx/gfx.c
    src/lib/palette.c
)

target_include_directories(lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(COMMON_LIBS lib m X11 pthread gomp)
target_link_libraries(${COMMON_LIBS})

# Define build targets
set(FRAC_EXE
    julia mandelb ramp julia_still
)
foreach(frac_exe ${FRAC_EXE})
    add_executable(${frac_exe} src/examples/${frac_exe}.c)
    target_link_libraries(${frac_exe} ${COMMON_LIBS})
endforeach(frac_exe)
 
