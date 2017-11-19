# FOR BUILDING BINARY EXECUTABLES

####################
# MAKE EXECUTABLES
####################
add_executable(slic_video_segmenter slic_video_segmenter.cpp)
target_link_libraries(
	slic_video_segmenter
	${SEGMENTATION_LIB}
)

add_executable(slic_image_segmenter slic_image_segmenter.cpp)
target_link_libraries(
	slic_image_segmenter
	${SEGMENTATION_LIB}
)


################
# INSTALLATION
################
install(TARGETS slic_video_segmenter DESTINATION bin/)
install(TARGETS slic_image_segmenter DESTINATION bin/)