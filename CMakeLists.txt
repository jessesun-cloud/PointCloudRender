cmake_minimum_required(VERSION 3.9)

#InitQt("Core;Widgets;LinguistTools;OpenGL")

set(PUBLIC_HEADERS 
  inc/VertexBuffer.hpp
)
set(PROJECT_SRCS
  VertexBuffer.cpp
)

find_package(OpenMP)
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
set(PUBLIC_HEADER_PATH "${CMAKE_SOURCE_DIR}/VertexBuffer/inc")


#project VertexBuffer
set(VertexBuffer "VertexBuffer")
add_library(${VertexBuffer} STATIC "${PROJECT_SRCS}" "${PUBLIC_HEADERS}")

AddExternalDependency(${VertexBuffer} PRIVATE Glext)
target_include_directories(${VertexBuffer} PUBLIC ${PUBLIC_HEADER_PATH})
target_include_directories(${VertexBuffer} PUBLIC ${CMAKE_SOURCE_DIR})
set_target_properties(${VertexBuffer} PROPERTIES FOLDER ${VertexBuffer})
source_group("Source Files" FILES ${PROJECT_SRCS})
source_group("Public Headers" FILES ${PUBLIC_HEADERS})
##############################################################################################


