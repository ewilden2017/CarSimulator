cmake_minimum_required (VERSION 2.6)
project (CarSimulator)

set (CarSimulator_VERSION_MAJOR 0)
set (CarSimulator_VERSION_MINOR 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/car.cpp
    ${SRC_DIR}/car.h
    ${SRC_DIR}/collision.cpp
    ${SRC_DIR}/collision.h
    ${SRC_DIR}/common.h
    ${SRC_DIR}/detectLine.cpp
    ${SRC_DIR}/detectLine.h
    ${SRC_DIR}/shader.cpp
    ${SRC_DIR}/shader.h
    ${SRC_DIR}/wall.cpp
    ${SRC_DIR}/wall.h
    ${SRC_DIR}/wallLoader.cpp
    ${SRC_DIR}/wallLoader.h
)

configure_file(
  "${PROJECT_SOURCE_DIR}/CarSimConfig.h.in"
  "${PROJECT_BINARY_DIR}/CarSimConfig.h"
)

file(COPY "${PROJECT_SOURCE_DIR}/SimpleTransform.vertexshader" DESTINATION "${PROJECT_BINARY_DIR}")
file(COPY "${PROJECT_SOURCE_DIR}/SingleColor.fragmentshader" DESTINATION "${PROJECT_BINARY_DIR}")
if(EXISTS "${PROJECT_SOURCE_DIR}/Walls.txt")
    file(COPY "${PROJECT_SOURCE_DIR}/Walls.txt" DESTINATION "${PROJECT_BINARY_DIR}")
else()
    file(WRITE "${PROJECT_BINARY_DIR}/Walls.txt" "-20.0,-20.0,0.0\n20.0,-20.0,0.0\n20.0,20.0,0.0\n-20.0,20.0,0.0\n-20.0,-20.0,0.0\n") #generate a wall file
endif()
include_directories("${PROJECT_BINARY_DIR}")

#Find packages
find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GLFW_LIBRARIES})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GLEW_LIBRARIES})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GLM_LIBRARIES})
###############


add_executable(CarSim ${SOURCES})
target_link_libraries(CarSim ${LIBS})
