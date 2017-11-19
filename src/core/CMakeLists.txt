add_library(
	segmentation
	video_segmenter.cpp video_segmenter.h
	image_segmenter.cpp image_segmenter.h
	recursive_image_segmenter.cpp recursive_image_segmenter.h
)

target_link_libraries(
	segmentation
	${OpenCV_LIBS}
	${GSLICR_LIBRARIES}
)

target_include_directories(
	segmentation PUBLIC
	.
	${OpenCV_INCLUDE_DIRS}
	${GSLICR_INCLUDES}
)

set(SEGMENTATION_LIB segmentation CACHE INTERNAL "Includes for SLIC segmentation functionality")