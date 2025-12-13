# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\os_9_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\os_9_autogen.dir\\ParseCache.txt"
  "os_9_autogen"
  "window\\CMakeFiles\\window_autogen.dir\\AutogenUsed.txt"
  "window\\CMakeFiles\\window_autogen.dir\\ParseCache.txt"
  "window\\window_autogen"
  )
endif()
