cmake_minimum_required(VERSION 3.23)
project(
  VectorVoxel
  VERSION 2.0.0
  DESCRIPTION "3D engine that combines vector and voxel approach"
  LANGUAGES CXX)

# ----------------------------------------
# ----------- BASIC OPTIONS --------------
# ----------------------------------------

# Basic options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Modules
option(VECTORVOXEL_BUILD_EXAMPLES "Build examples" OFF)
option(VECTORVOXEL_BUILD_TESTS "Build tests" OFF)

# Backends
option(VECTORVOXEL_BUILD_BASIC_BACKENDS "Include basic backends in build" ON)
option(VECTORVOXEL_BUILD_SDL_BACKEND
       "Include SDL backend in build (requires SDL2)" OFF)
option(VECTORVOXEL_BUILD_QT_BACKEND
       "Include Qt backend in build (requires Qt6)" OFF)

# Emscripten
set(VECTORVOXEL_MODE_WEB
    ${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten"
    CACHE BOOL "Enable Emscripten mode")

if(VECTORVOXEL_MODE_WEB)
  message(STATUS "Emscripten mode enabled - forcing static build")
  if(BUILD_SHARED_LIBS)
    message(
      WARNING
        "Emscripten: shared libraries are not supported. Forcing to build static"
    )
    set(BUILD_SHARED_LIBS
        OFF
        CACHE BOOL "Build shared libraries" FORCE)
  endif()

  if(VECTORVOXEL_BUILD_QT_BACKEND)
    message(
      WARNING "Emscripten: Qt framework is not supported. Disabling Qt backend")
    set(VECTORVOXEL_BUILD_QT_BACKEND
        OFF
        CACHE BOOL "Qt disabled for Emscripten" FORCE)
  endif()
endif()

# ---------------------------------------
# -------- DECLARE LIBRARIES ------------
# ---------------------------------------

set(VECTORVOXEL_INCLUDE_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/include/vectorvoxel"
    CACHE PATH "Directory with header files")
set(VECTORVOXEL_INSTALL
    (${PROJECT_IS_TOP_LEVEL} AND NOT ${CMAKE_SKIP_INSTALL_RULES})
    CACHE BOOL "Sets whether library is going to be installed")

add_subdirectory(src/core)
if(VECTORVOXEL_BUILD_BASIC_BACKENDS)
  add_subdirectory(src/backends/basic)
endif()

# ------------------------------------
# ------------ INSTALL ---------------
# ------------------------------------

if(VECTORVOXEL_INSTALL)
  include(GNUInstallDirs)

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

endif()
