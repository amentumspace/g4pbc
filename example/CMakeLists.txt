cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(example)

MESSAGE( STATUS "Building example ")

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

find_package(g4pbc REQUIRED NO_MODULE)

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${g4pbc_INCLUDE_DIRS})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh )

add_executable(example example.cc ${sources} ${headers})
target_link_libraries(example ${Geant4_LIBRARIES})
target_link_libraries(example g4pbc::g4pbc)

file(GLOB macros ${PROJECT_SOURCE_DIR}/*.mac)
file(COPY ${macros} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
