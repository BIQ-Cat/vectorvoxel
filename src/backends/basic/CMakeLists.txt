file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(vectorvoxel_backends_basic)
add_library(VectorVoxel::BackendsBasic ALIAS vectorvoxel_backends_basic)

include(GenerateExportHeader)
generate_export_header(
  vectorvoxel_core EXPORT_FILE_NAME include/vectorvoxel/backends/export_basic.h
  EXPORT_MACRO_NAME VECTORVOXEL_BACKENDS_BASIC_EXPORT)

set(INCLUDE
    "${VECTORVOXEL_INCLUDE_DIRECTORY}/backends/basic.h"
    "${CMAKE_CURRENT_BINARY_DIR}/include/vectorvoxel/backends/export_basic.h")

target_sources(
  vectorvoxel_backends_basic
  PRIVATE ${SOURCES} ${HEADERS}
  PUBLIC FILE_SET
         HEADERS
         BASE_DIRS
         ${VECTORVOXEL_INCLUDE_DIRECTORY}/..
         ${CMAKE_CURRENT_BINARY_DIR}/include
         FILES
         ${INCLUDE})

target_link_libraries(vectorvoxel_backends_basic PUBLIC vectorvoxel_core)

set_target_properties(
  vectorvoxel_backends_basic
  PROPERTIES VERSION ${PROJECT_VERSION}
             SOVERSION ${PROJECT_VERSION_MAJOR}
             CXX_STANDARD 17
             CXX_STANDARD_REQUIRED ON)

if(BUILD_SHARED_LIBS)
  set_target_properties(
    vectorvoxel_backends_basic PROPERTIES CXX_VISIBILITY hidden
                                          VISIBILITY_INLINES_HIDDEN ON)
endif()

target_include_directories(
  vectorvoxel_backends_basic
  PUBLIC # For FetchContent
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
         # For find_package
         $<INSTALL_INTERFACE:include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(VECTORVOXEL_MODE_WEB)
  target_compile_definitions(vectorvoxel_backends_basic
                             PUBLIC VECTORVOXEL_EMSCRIPTEN)
endif()

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(vectorvoxel_backends_basic
                             PUBLIC VECTORVOXEL_BACKENDS_BASIC_STATIC_DEFINE)
endif()

message(STATUS "Basic backends configured")

if(VECTORVOXEL_INSTALL)
  include(GNUInstallDirs)
  install(
    TARGETS vectorvoxel_backends_basic
    EXPORT VectorVoxelTargets
    FILE_SET HEADERS
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(
    FILES
      ${CMAKE_CURRENT_BINARY_DIR}/include/vectorvoxel/backends/export_basic.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vectorvoxel/backends)

  if(MSVC AND NOT VECTORVOXEL_MODE_WEB)
    if(BUILD_SHARED_LIBS)
      install(
        FILES $<TARGET_PDB_FILE:vectorvoxel_backends_basic>
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        CONFIGURATIONS Debug RelWithDebInfo
        OPTIONAL)
    else()
      install(
        FILES
          $<TARGET_FILE_DIR:vectorvoxel_backends_basic>/$<TARGET_FILE_BASE_NAME:vectorvoxel_backends_basic>.pdb
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        CONFIGURATIONS Debug RelWithDebInfo
        OPTIONAL)
    endif()
  endif()
endif()
