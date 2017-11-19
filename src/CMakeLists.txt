#################
# FIND PACKAGES
#################
find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS
	program_options
	filesystem
	system
	REQUIRED)
find_package(CUDA REQUIRED)


#############################
# ADD SOURCE SUBDIRECTORIES
#############################
add_subdirectory(gSLICr)
add_subdirectory(core)
add_subdirectory(exe)