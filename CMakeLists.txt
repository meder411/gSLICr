#***************************
# TOP LEVEL CMAKELISTS.TXT
#***************************
cmake_minimum_required(VERSION 2.8)
project(SuperpixelSegmenter)


######################
# CMAKE DEPENDENCIES
######################
# Point CMake to check cmake folder for Find* modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})

# Set default install prefix
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	SET(CMAKE_INSTALL_PREFIX ${PROJECT_ROOT} CACHE PATH "Default install directory prefix" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# C++11 compatibility
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Set OpenMP flags if necessary
if(OPENMP_FOUND)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


###########################
# UNINSTALL TARGET SCRIPT
###########################

# Create a target for uninstall
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CMakeUninstall.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CMakeUninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/CMakeUninstall.cmake)


#############################
# ADD SOURCE SUBDIRECTORIES
#############################

add_subdirectory(src)