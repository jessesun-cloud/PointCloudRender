cmake_minimum_required(VERSION 3.9)

InitQt("Core;Widgets;LinguistTools;OpenGL")

set(PUBLIC_HEADERS 
  inc/VertexBuffer.hpp
  inc/VaoRender.hpp
)
set(PROJECT_SRCS
  VertexBuffer.cpp
  VaoRender.cpp
)

find_package(OpenMP)
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
set(PUBLIC_HEADER_PATH "${CMAKE_SOURCE_DIR}/VertexBuffer/inc")


#project PointCloudRender
set(PointCloudRender "PointCloudRender")
add_library(${PointCloudRender} STATIC "${PROJECT_SRCS}" "${PUBLIC_HEADERS}")

#target_link_libraries(${PointCloudRender}  Qt5::Core  Qt5::Widgets  Qt5::OpenGL )
AddExternalDependency(${PointCloudRender} PRIVATE Glext)
target_include_directories(${PointCloudRender} PUBLIC ${PUBLIC_HEADER_PATH})
target_include_directories(${PointCloudRender} PUBLIC ${CMAKE_SOURCE_DIR})
set_target_properties(${PointCloudRender} PROPERTIES FOLDER ${PointCloudRender})
source_group("Source Files" FILES ${PROJECT_SRCS})
source_group("Public Headers" FILES ${PUBLIC_HEADERS})
##############################################################################################


