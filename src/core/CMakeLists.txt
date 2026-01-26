file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(vectorvoxel_core)
add_library(VectorVoxel::core ALIAS vectorvoxel_core)

include(GenerateExportHeader)
generate_export_header(
  vectorvoxel_core EXPORT_FILE_NAME include/vectorvoxel/core/export.h
  EXPORT_MACRO_NAME VECTORVOXEL_CORE_EXPORT)

set(INCLUDE
    "${VECTORVOXEL_INCLUDE_DIRECTORY}/core/renderer.h"
    "${VECTORVOXEL_INCLUDE_DIRECTORY}/core/backend.h"
    "${VECTORVOXEL_INCLUDE_DIRECTORY}/core/camera.h"
    "${VECTORVOXEL_INCLUDE_DIRECTORY}/core/terrain.h"
    "${CMAKE_CURRENT_BINARY_DIR}/include/vectorvoxel/core/export.h")

target_sources(
  vectorvoxel_core
  PRIVATE ${SOURCES} ${HEADERS}
  PUBLIC FILE_SET
         HEADERS
         BASE_DIRS
         ${VECTORVOXEL_INCLUDE_DIRECTORY}/..
         ${CMAKE_CURRENT_BINARY_DIR}/include
         FILES
         ${INCLUDE})
set_target_properties(
  vectorvoxel_core
  PROPERTIES VERSION ${PROJECT_VERSION}
             SOVERSION ${PROJECT_VERSION_MAJOR}
             CXX_STANDARD 17
             CXX_STANDARD_REQUIRED ON)

if(BUILD_SHARED_LIBS)
  set_target_properties(
    vectorvoxel_core PROPERTIES CXX_VISIBILITY hidden VISIBILITY_INLINES_HIDDEN
                                                      ON)
endif()

target_include_directories(
  vectorvoxel_core
  PUBLIC # For FetchContent
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
         # For find_package
         $<INSTALL_INTERFACE:include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(VECTORVOXEL_MODE_WEB)
  target_compile_definitions(vectorvoxel_core PUBLIC VECTORVOXEL_EMSCRIPTEN)
endif()

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(vectorvoxel_core
                             PUBLIC VECTORVOXEL_CORE_STATIC_DEFINE)
endif()

message(STATUS "Core module configured")

if(VECTORVOXEL_INSTALL)
  include(GNUInstallDirs)
  install(
    TARGETS vectorvoxel_core
    EXPORT VectorVoxelTargets
    FILE_SET HEADERS
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/include/vectorvoxel/core/export.h
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vectorvoxel/core)

  if(MSVC AND NOT VECTORVOXEL_MODE_WEB)
    if(BUILD_SHARED_LIBS)
      install(
        FILES $<TARGET_PDB_FILE:vectorvoxel_core>
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        CONFIGURATIONS Debug RelWithDebInfo
        OPTIONAL)
    else()
      install(
        FILES
          $<TARGET_FILE_DIR:vectorvoxel_core>/$<TARGET_FILE_BASE_NAME:vectorvoxel_core>.pdb
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        CONFIGURATIONS Debug RelWithDebInfo
        OPTIONAL)
    endif()
  endif()
endif()
