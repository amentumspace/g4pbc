cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(test)

MESSAGE( STATUS "Building test ")

option(WITH_GEANT4_UIVIS "Build test with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

# cyclic boundary physics
find_package(g4pbc REQUIRED NO_MODULE)
include_directories(${g4pbc_INCLUDE_DIRS})

# HDF5
find_package(HDF5 REQUIRED COMPONENTS C HL CXX)
include_directories(${HDF5_INCLUDE_DIRS} hdf5_hl_cpp )

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh )

add_executable(test test.cc ${sources} ${headers})
target_link_libraries(test ${Geant4_LIBRARIES})
target_link_libraries(test g4pbc::g4pbc)
target_link_libraries(test ${HDF5_LIBRARIES} hdf5_hl_cpp)

file(GLOB macros ${PROJECT_SOURCE_DIR}/*.mac)
file(COPY ${macros} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
