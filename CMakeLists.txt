cmake_minimum_required(VERSION 3.23)
project(
  VectorVoxel
  VERSION 1.0.0
  DESCRIPTION "3D engine that combines vector and voxel approach"
  LANGUAGES CXX)

# ----------------------------------------
# ----------- BASIC OPTIONS --------------
# ----------------------------------------

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(VECTORVOXEL_BUILD_EXAMPLES "Build examples" OFF)
option(VECTORVOXEL_BUILD_TESTS "Build tests" OFF)

# ---------------------------------------
# -------- DECLARE MAIN LIBRARY ---------
# ---------------------------------------

set(SOURCES src/render.cpp src/screen.cpp)

set(PUBLIC_HEADERS include/vectorvoxel/vectorvoxel.h)

add_library(VectorVoxel ${SOURCES} ${PUBLIC_HEADERS})
add_library(VectorVoxel::VectorVoxel ALIAS VectorVoxel)

# Generate export.h
include(GenerateExportHeader)
generate_export_header(VectorVoxel EXPORT_FILE_NAME
                       include/vectorvoxel/export.h)

# Add export.h to public headers of the main library
set(PUBLIC_HEADERS ${PUBLIC_HEADERS}
                   ${CMAKE_CURRENT_BINARY_DIR}/include/vectorvoxel/export.h)

set_target_properties(VectorVoxel PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

# Export only those symbols that uses VECTORVOXEL_EXPORT macros
if(BUILD_SHARED_LIBS)
  set_target_properties(VectorVoxel PROPERTIES CXX_VISIBILITY_PRESET hidden
                                               VISIBILITY_INLINES_HIDDEN ON)
endif()

target_include_directories(
  VectorVoxel
  PUBLIC # For FetchContent
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
         # For find_package
         $<INSTALL_INTERFACE:include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(VectorVoxel PUBLIC VECTORVOXEL_STATIC_DEFINE)
endif()

set_target_properties(
  VectorVoxel
  PROPERTIES VERSION ${PROJECT_VERSION}
             SOVERSION ${PROJECT_VERSION_MAJOR}
             PUBLIC_HEADER "${PUBLIC_HEADERS}")

# ------------------------------------
# ------------ INSTALL ---------------
# ------------------------------------

if(PROJECT_IS_TOP_LEVEL AND NOT CMAKE_SKIP_INSTALL_RULES)
  include(GNUInstallDirs)

  # Install libraries and define headers for them
  install(
    TARGETS VectorVoxel
    EXPORT VectorVoxelTargets # For find_package
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # .exe/.dll
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # .so/.dynlib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # .a/.lib
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vectorvoxel)

  # Install header files
  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  # -----------------------------------
  # ------ INSTALL CMAKE CONFIG -------
  # -----------------------------------

  set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/VectorVoxel")

  # Install target for find_package
  install(
    EXPORT VectorVoxelTargets
    FILE VectorVoxelTargets.cmake
    NAMESPACE VectorVoxel::
    DESTINATION ${CONFIG_INSTALL_DIR})

  # Generate CMake config
  include(CMakePackageConfigHelpers)
  configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/VectorVoxelConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/VectorVoxelConfig.cmake
    INSTALL_DESTINATION ${CONFIG_INSTALL_DIR})

  # Generating version file
  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/VectorVoxelConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

  # Install configuration
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/VectorVoxelConfig.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/VectorVoxelConfigVersion.cmake
          DESTINATION ${CONFIG_INSTALL_DIR})

  # -----------------------------------
  # INSTALL PDB FILES (for Windows)
  # -----------------------------------
  if(MSVC)
    if(BUILD_SHARED_LIBS)
      install(
        FILES $<TARGET_PDB_FILE:VectorVoxel>
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        CONFIGURATIONS Debug RelWithDebInfo
        OPTIONAL)
    else()
      install(
        FILES
          $<TARGET_FILE_DIR:VectorVoxel>/$<TARGET_FILE_BASE_NAME:VectorVoxel>.pdb
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        CONFIGURATIONS Debug RelWithDebInfo
        OPTIONAL)
    endif()
  endif()
endif()
