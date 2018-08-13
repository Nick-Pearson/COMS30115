cmake_minimum_required(VERSION 3.1)
project (SoftwareRenderer)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option (USE_SDL "Link the SDL library to enable realtime and interactive display of render results" ON)

if (USE_SDL)
  find_package(SDL2 REQUIRED)
  include_directories(${SDL2_INCLUDE_DIRS})
  set (EXTRA_LIBS ${EXTRA_LIBS} ${SDL2_LIBRARIES})
   add_definitions(-DUSE_SDL=1)
endif (USE_SDL)

include_directories(src)
include_directories(libs/glm)
include_directories(libs/stb)

file(GLOB_RECURSE SOURCES "src/*.cpp")

string(STRIP ${EXTRA_LIBS} EXTRA_LIBS)

add_executable(raytracer ${SOURCES})
target_link_libraries(raytracer ${EXTRA_LIBS})
target_compile_definitions(raytracer PRIVATE RAYTRACER=1)

add_executable(rasterizer ${SOURCES})
target_link_libraries(rasterizer ${EXTRA_LIBS})
target_compile_definitions(rasterizer PRIVATE RASTERIZER=1)
