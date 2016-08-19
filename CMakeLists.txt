# project
cmake_minimum_required (VERSION 2.6)
project (libelas)

# make release version
set(CMAKE_BUILD_TYPE DEBUG)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}   -Wall  -O3 -march=native ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall   -O3 -march=native")

# directories
set (LIBELAS_SRC_DIR src)

# include directory
include_directories("${LIBELAS_SRC_DIR}")

# use sse3 instruction set
SET(CMAKE_CXX_FLAGS "-msse3")


# sources
FILE(GLOB LIBELAS_SRC_FILES "src/*.cpp")

find_package(OpenCV 2.4.3 REQUIRED)
include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${OpenCV_INCLUDE_DIR}
)


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# build demo program
add_library(elas SHARED ${LIBELAS_SRC_FILES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(main test/main.cpp)

target_link_libraries(main elas)

add_executable(elasOpencvTest test/elasOpencvTest.cpp)

target_link_libraries(elasOpencvTest elas ${OpenCV_LIBS})



