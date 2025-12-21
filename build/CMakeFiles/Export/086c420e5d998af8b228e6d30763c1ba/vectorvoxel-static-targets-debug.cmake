#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "VectorVoxel::VectorVoxel" for configuration "Debug"
set_property(TARGET VectorVoxel::VectorVoxel APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(VectorVoxel::VectorVoxel PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib64/libVectorVoxel.a"
  )

list(APPEND _cmake_import_check_targets VectorVoxel::VectorVoxel )
list(APPEND _cmake_import_check_files_for_VectorVoxel::VectorVoxel "${_IMPORT_PREFIX}/lib64/libVectorVoxel.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
