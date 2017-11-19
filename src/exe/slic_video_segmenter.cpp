#include "../core/video_segmenter.h"
#include "../core/util.h"
#include "../core/options.h"

#include <iostream>


int main(int arg, char ** argv)
{
	try
	{
		// Parse command line arguments
		Superpixels::SuperpixelUserOptions user_options;
		if (Superpixels::parseVideoSegmenterCommandLine(arg, argv, user_options)) { return -1; }
		Superpixels::SLICSettings slic_settings(user_options);

		// Create video segmenter and set parameters
		Superpixels::VideoSegmenter video_segmenter(slic_settings);
		video_segmenter.setInput(user_options.input_path);
		video_segmenter.setOutputDirectory(user_options.output_path);
		video_segmenter.setSamplingRate(user_options.sampling_rate);
		if (user_options.use_scale)
		{
			video_segmenter.setScale(user_options.scale);
		}
		else
		{
			video_segmenter.setMaxSidelen(user_options.max_sidelen);
		}
		video_segmenter.setVerbose(user_options.verbose);

		// Segment the video
		video_segmenter.segment();

    	return 0;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}
