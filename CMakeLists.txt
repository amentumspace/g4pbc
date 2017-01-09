cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

# build the library and create a cmake package that is relocateable

project(g4pbc)

MESSAGE( STATUS "Building library ")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set(g4pbc_VERSION 1.0.0)

include(GenerateExportHeader)

# Build the geant4 libraries

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE})

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(g4pbc SHARED ${sources} ${headers})

target_link_libraries(g4pbc ${Geant4_LIBRARIES})

# export header

generate_export_header(g4pbc)

set_property(TARGET g4pbc PROPERTY VERSION ${g4pbc_VERSION})
set_property(TARGET g4pbc PROPERTY SOVERSION 1)
set_property(TARGET g4pbc PROPERTY INTERFACE_g4pbc_MAJOR_VERSION 1)
set_property(TARGET g4pbc APPEND PROPERTY COMPATIBLE_INTERFACE_STRING
  g4pbc_MAJOR_VERSION
)

# Creating Config.cmake files

#install export headers

install(
  FILES ${headers} "${CMAKE_CURRENT_BINARY_DIR}/g4pbc_export.h"
  DESTINATION include
)

set(INCLUDE_INSTALL_DIR include/)

include(CMakePackageConfigHelpers)

set(ConfigPackageLocation lib/cmake/g4pbc)

configure_package_config_file(
  g4pbcConfig.cmake.in g4pbcConfig.cmake
  INSTALL_DESTINATION  ${ConfigPackageLocation}
  PATH_VARS  INCLUDE_INSTALL_DIR
)

write_basic_package_version_file(
  "g4pbcConfigVersion.cmake"
  VERSION ${g4pbc_VERSION}
  COMPATIBILITY AnyNewerVersion
)


# Put the installed targets into an "export-set" using install (TARGETS ... EXPORT ... )
install(
  TARGETS g4pbc EXPORT g4pbcTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(
  EXPORT g4pbcTargets
  FILE g4pbcTargets.cmake
  NAMESPACE g4pbc::
  DESTINATION ${ConfigPackageLocation}
)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/g4pbcConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/g4pbcConfigVersion.cmake"
  DESTINATION ${ConfigPackageLocation}
)

# to generate a deb package

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Iwan Cornelius") #required

INCLUDE(CPack)

# To use this:
# make package
# sudo dpkg -i <package_name>.deb
