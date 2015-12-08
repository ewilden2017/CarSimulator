cmake_minimum_required(VERSION 2.4)

PROJECT( neat )

FILE(GLOB neat_sources *.cpp)
LIST(REMOVE_ITEM neat_sources "${CMAKE_CURRENT_SOURCE_DIR}/neatmain.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/experiments.cpp")

ADD_LIBRARY( neat SHARED ${neat_sources} )
