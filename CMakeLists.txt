

cmake_minimum_required (VERSION 3.8)
project(park_manager)
###############################################################################
####Version Info
file(STRINGS "version" VersionConfigContents)
foreach(NameAndValue ${VersionConfigContents})
  # Strip leading spaces
  string(REGEX REPLACE "^[ ]+" "" NameAndValue ${NameAndValue})
  # Find variable name
  string(REGEX MATCH "^[^ ]+" Name ${NameAndValue})
  # Find the value
  string(REPLACE "${Name} " "" Value ${NameAndValue})
  # Set the variable
  set(${Name} "${Value}")
endforeach()
message(STATUS "MAJORVERSION: " ${MAJORVERSION})
message(STATUS "MINORVERSION: " ${MINORVERSION})
message(STATUS "PACHNUMBER: " ${PACHNUMBER})
set(PM_VERSION ${MAJORVERSION}.${MINORVERSION}.${PACHNUMBER})
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

###############################################################################

####Build type
set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
########################################################################################

####add definitions
add_definitions(-DSOFTWARE_VERSION=\"${PM_VERSION}\" )
########################################################################################

####global includes
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Werror=format-security -O2 -pipe -g0 ")

#####Echo info
message(STATUS "Project Version: " ${PM_VERSION}) 
#########################################################################################

#### Executing  Sub-Cmakes
add_subdirectory(src)
add_subdirectory(testUnits)
#########################################################################################


