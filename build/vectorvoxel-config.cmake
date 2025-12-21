macro(import_targets type)
  if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/vectorvoxel-${type}-targets.cmake")
    set(${CMAKE_FIND_PACKAGE_NAME}_NOT_FOUND_MESSAGE
        "vectorvoxel ${type} libraries were requested but not found")
    set(${CMAKE_FIND_PACKAGE_NAME}_FOUND OFF)
    return()
  endif()

  include("${CMAKE_CURRENT_LIST_DIR}/vectorvoxel-${type}-targets.cmake")
endmacro()

if(NOT TARGET VectorVoxel::VectorVoxel)
  set(type "")

  if(DEFINED VECTORVOXEL_SHARED_LIBS)
    if(VECTORVOXEL_SHARED_LIBS)
      set(type "shared")
    else()
      set(type "static")
    endif()
  elseif(BUILD_SHARED_LIBS
         AND EXISTS
             "${CMAKE_CURRENT_LIST_DIR}/vectorvoxel-shared-targets.cmake")
    set(type "shared")
  elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/vectorvoxel-static-targets.cmake")
    set(type "static")
  else()
    set(type "shared")
  endif()

  import_targets(${type})
  check_required_components(VectorVoxel)
  message(
    "-- Found ${type} vectorvoxel (version ${${CMAKE_FIND_PACKAGE_NAME}_VERSION})"
  )
endif()
