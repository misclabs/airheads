# General target installation settings
install(TARGETS ${NAME}
  BUNDLE DESTINATION .
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Settings for packaging per platform
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  include(${PROJECT_SOURCE_DIR}/app/cmake/packaging/win32.cmake)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  include(${PROJECT_SOURCE_DIR}/app/cmake/packaging/linux.cmake)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  include(${PROJECT_SOURCE_DIR}/app/cmake/packaging/mac.cmake)
endif ()
