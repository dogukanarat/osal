#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "osal::osal" for configuration "Debug"
set_property(TARGET osal::osal APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(osal::osal PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libosald.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS osal::osal )
list(APPEND _IMPORT_CHECK_FILES_FOR_osal::osal "${_IMPORT_PREFIX}/lib/libosald.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
