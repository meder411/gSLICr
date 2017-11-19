# FOR COMPILING GSLICR EXTERNAL DEPENDENCY

#################
# FIND PACKAGES
#################
find_package(CUDA)

if(CUDA_FOUND)
	find_package(OpenCV REQUIRED)
	find_package(Boost COMPONENTS
		program_options
		filesystem
		system
		REQUIRED)

	#######################
	# INCLUDE DIRECTORIES
	#######################
	# include_directories(${CUDA_INCLUDE_DIRS})
	# include_directories(${OpenCV_INCLUDE_DIRS}, ${Boost_INCLUDE_DIRS})

	#########################
	# ADD UTIL SUBDIRECTORY
	#########################
	add_subdirectory(ORUtils)


	###################################
	# FILES FOR GSLICR LIBRARY
	###################################
	set(
		GSLICR_LIB
		gSLICr_Lib/engines/gSLICr_core_engine.h
		gSLICr_Lib/engines/gSLICr_seg_engine.h
		gSLICr_Lib/engines/gSLICr_seg_engine_GPU.h
		gSLICr_Lib/engines/gSLICr_seg_engine_shared.h
		gSLICr_Lib/engines/gSLICr_core_engine.cpp
		gSLICr_Lib/engines/gSLICr_seg_engine.cpp
		gSLICr_Lib/engines/gSLICr_seg_engine_GPU.cu
		gSLICr_Lib/objects/gSLICr_settings.h
		gSLICr_Lib/objects/gSLICr_spixel_info.h
		gSLICr_Lib/gSLICr_defines.h
		gSLICr_Lib/gSLICr.h
	)

	list(APPEND "-std=c++11 -ftree-vectorize")
	SOURCE_GROUP(engines FILES ${GSLICR_LIB})

	cuda_add_library(gSLICr_lib
				${GSLICR_LIB}
				NVTimer.h
				OPTIONS -gencode arch=compute_30,code=compute_30)


	#########################################
	# LINK CUDA LIBRARIES TO GSLICR LIBRARY
	#########################################
	target_link_libraries(gSLICr_lib ${CUDA_LIBRARY})

	set(
		GSLICR_INCLUDES
		${CUDA_INCLUDE_DIRS}
		${OpenCV_INCLUDE_DIRS}
		${Boost_INCLUDE_DIRS}
		CACHE INTERNAL "gSLICr includes"
	)

	target_include_directories(
		gSLICr_lib PUBLIC
		${GSLICR_INCLUDES}
	)

	set(
		GSLICR_LIBRARIES
		gSLICr_lib
		${OpenCV_LIBS}
		${Boost_LIBRARIES}
		CACHE INTERNAL "gSLICr libraries"
	)


ENDIF(CUDA_FOUND)