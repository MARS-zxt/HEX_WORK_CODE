# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ValveSimulator_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ValveSimulator_autogen.dir\\ParseCache.txt"
  "ValveSimulator_autogen"
  )
endif()
