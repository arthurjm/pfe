# Resources:
#
# http://www.vtk.org/Wiki/VTK/Examples/Cxx/Qt/RenderWindowNoUiFile

cmake_minimum_required(VERSION 2.8)

project(pfe_lidar)

include_directories(include/)
include_directories(/usr/local/include)
include_directories(/usr/include/)

if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package( OpenCV REQUIRED )
find_package(Qt5 COMPONENTS Widgets REQUIRED)

message(STATUS "VTK_QT_VERSION: " ${VTK_QT_VERSION})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_BINARY_NAME fast_hierarchy)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})

include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Boost 1.54 COMPONENTS program_options filesystem REQUIRED)
set(Boost_COMPONENTS_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
set(Boost_COMPONENTS_LIBRARY_DIRS ${Boost_LIBRARY_DIRS})
set(Boost_COMPONENTS_LIBRARIES ${Boost_LIBRARIES})
include_directories(${Boost_COMPONENTS_INCLUDE_DIRS})
link_directories(${Boost_COMPONENTS_LIBRARY_DIRS})
add_definitions(${Boost_COMPONENTS_DEFINITIONS})

find_package(PCL 1.8 REQUIRED)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

find_package(VTK REQUIRED)
if(VTK_FOUND)
   include (${VTK_USE_FILE})
endif(VTK_FOUND)


set(CPP_FILES
    src/main.cpp
    src/mainwindow.cpp
    src/rangeimage.cpp
    src/shdist.cpp
    src/slic.cpp
    src/superpixelhierarchy.cpp
    src/pointcloud.cpp
    src/clickablelabel.cpp

)
set(HEADERS 
    include/mainwindow.h 
    include/clickablelabel.h
    include/rangeimage.h
    include/shdist.h
    include/slic.h
    include/pointcloud.h
    include/superpixelhierarchy.h
    include/superpixelhierarchymex.hpp

)

set (WRAP_CPP_FILES include/mainwindow.h)
set(WRAP_UI_FILES src/mainwindow.ui)

qt5_wrap_ui(PROJECT_FORMS_HEADERS ${WRAP_UI_FILES} )
# CMAKE_AUTOMOC in ON so the MOC headers will be automatically wrapped.
add_executable(${PROJECT_BINARY_NAME}
  ${CPP_FILES}
  ${HEADERS}
  ${PROJECT_FORMS_HEADERS}
  ${WRAP_CPP_FILES})
qt5_use_modules(${PROJECT_BINARY_NAME} Core Gui Widgets)
target_link_libraries(${PROJECT_BINARY_NAME}
    ${PCL_LIBRARIES}
    ${VTK_LIBRARIES}
    ${OPENGL_gl_LIBRARY}
    ${GLUT_Libraries}
    ${OpenCV_LIBS}
    ${Boost_COMPONENTS_LIBRARIES})

