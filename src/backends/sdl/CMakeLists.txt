file(GLOB_RECURSE SOURCES *.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(vectorvoxel_backend_sdl)
add_library(VectorVoxel::Backends::SDL ALIAS vectorvoxel_backend_sdl)

include(GenerateExportHeader)
generate_export_header(
  vectorvoxel_backend_sdl EXPORT_FILE_NAME
  include/vectorvoxel/backends/export_sdl.h EXPORT_MACRO_NAME
  VECTORVOXEL_BACKEND_SDL_EXPORT)

set(INCLUDE
    "${VECTORVOXEL_INCLUDE_DIRECTORY}/backends/sdl.h"
    "${CMAKE_CURRENT_BINARY_DIR}/include/vectorvoxel/backends/export_sdl.h")
target_sources(
  vectorvoxel_backend_sdl
  PRIVATE ${SOURCES} ${HEADERS}
  PUBLIC FILE_SET
         HEADERS
         BASE_DIRS
         ${VECTORVOXEL_INCLUDE_DIRECTORY}/..
         ${CMAKE_CURRENT_BINARY_DIR}/include
         FILES
         ${INCLUDE})

if(VECTORVOXEL_MODE_WEB)
  target_link_options(vectorvoxel_backend_sdl PRIVATE -s USE_SDL=2 -s
                      ALLOW_MEMORY_GROWTH=1)
  target_link_libraries(vectorvoxel_backend_sdl PUBLIC vectorvoxel_core)
else()
  find_package(SDL2 REQUIRED)
  target_link_libraries(vectorvoxel_backend_sdl PUBLIC vectorvoxel_core
                                                       SDL2::SDL2)
endif()

target_include_directories(
  vectorvoxel_backend_sdl
  PUBLIC # For FetchContent
         $<BUILD_INTERFACE:${VECTORVOXEL_INCLUDE_DIRECTORY}/..>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
         # For find_package
         $<INSTALL_INTERFACE:include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties(
  vectorvoxel_backend_sdl
  PROPERTIES VERSION ${PROJECT_VERSION}
             SOVERSION ${PROJECT_VERSION_MAJOR}
             CXX_STANDARD 17
             CXX_STANDARD_REQUIRED ON)

if(BUILD_SHARED_LIBS)
  set_target_properties(
    vectorvoxel_backend_sdl PROPERTIES CXX_VISIBILITY hidden
                                       VISIBILITY_INLINES_HIDDEN ON)
endif()

if(VECTORVOXEL_MODE_WEB)
  target_compile_definitions(vectorvoxel_backend_sdl
                             PUBLIC VECTORVOXEL_EMSCRIPTEN)
endif()

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(vectorvoxel_backend_sdl
                             PUBLIC VECTORVOXEL_BACKEND_SDL_STATIC_DEFINE)
endif()

message(STATUS "SDL backend configured")

if(VECTORVOXEL_INSTALL)
  include(GNUInstallDirs)
  install(
    TARGETS vectorvoxel_backend_sdl
    EXPORT VectorVoxelTargets
    FILE_SET HEADERS
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/include/vectorvoxel/backends/export_sdl.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/vectorvoxel/backends)

  if(MSVC AND NOT VECTORVOXEL_MODE_WEB)
    if(BUILD_SHARED_LIBS)
      install(
        FILES $<TARGET_PDB_FILE:vectorvoxel_backend_sdl>
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        CONFIGURATIONS Debug RelWithDebInfo
        OPTIONAL)
    else()
      install(
        FILES
          $<TARGET_FILE_DIR:vectorvoxel_backend_sdl>/$<TARGET_FILE_BASE_NAME:vectorvoxel_backend_sdl>.pdb
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        CONFIGURATIONS Debug RelWithDebInfo
        OPTIONAL)
    endif()
  endif()
endif()
