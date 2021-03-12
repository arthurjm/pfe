# Resources:
#
# http://www.vtk.org/Wiki/VTK/Examples/Cxx/Qt/RenderWindowNoUiFile

cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 14)

add_compile_options(-Wall -g)

project(benchmark)

include_directories(../include/)
include_directories(/usr/local/include)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)

set(PROJECT_BINARY_NAME benchmark)

set(CPP_FILES
    ../src/rangeimage.cpp
    ../src/slic.cpp
    benchmark_asa_slic.cpp
)
set(HEADERS
    ../include/rangeimage.h
    ../include/slic.h
)

add_executable(${PROJECT_BINARY_NAME}
    ${CPP_FILES}
    ${HEADERS}
)

qt5_use_modules(${PROJECT_BINARY_NAME} Widgets)

target_link_libraries(${PROJECT_BINARY_NAME} ${OpenCV_LIBS})