cmake_minimum_required(VERSION 3.10)

project(imgproc-benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

set(DEFAULT_SOURCES
        # Include LodePNG by Lode Vandevenne for PNG support
        src/lodepng/lodepng.h src/lodepng/lodepng.cpp

        # Other files
        src/utils/Image.hpp src/utils/Image.cpp
        src/utils/Kernel.hpp src/utils/Kernel.cpp
        src/utils/Histogram.hpp src/utils/Histogram.cpp
        src/baseline/imgproc.hpp src/baseline/imgproc.cpp
        src/baseline/water.hpp src/baseline/water.cpp

        src/imgproc-benchmark.cpp)

include(CheckLanguage)
check_language(CUDA)

if (CMAKE_CUDA_COMPILER)
    message("[ACS LAB] CUDA is supported. Enabling CUDA sources.")
    enable_language(CUDA)
    add_definitions(-DUSE_CUDA)
    set(CMAKE_CUDA_STANDARD 11)
    set(CUDA_SOURCES
            src/students/imgproc_cuda.hpp src/students/imgproc_cuda.cu
            src/students/water_cuda.hpp src/students/water_cuda.cu
            # ADD YOUR OWN CUDA SOURCES HERE
            )

else ()
    message("[ACS LAB] Could not find CUDA support. Disabling CUDA sources.")
endif ()

add_executable(${PROJECT_NAME} ${DEFAULT_SOURCES} ${CUDA_SOURCES})
