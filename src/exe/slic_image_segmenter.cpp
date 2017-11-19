#include "../core/image_segmenter.h"
#include "../core/recursive_image_segmenter.h"
#include "../core/util.h"
#include "../core/options.h"

#include <iostream>

using namespace Superpixels;

int main(int arg, char ** argv)
{
	try
	{
		// Parse command line arguments
		SuperpixelUserOptions user_options;
		if (parseImageSegmenterCommandLine(arg, argv, user_options)) { return -1; }
		SLICSettings slic_settings(user_options);

		// TODO: Make this more elegant (factory function?)
		if (user_options.large_scale)
		{
			RecursiveImageSegmenter recursive_image_segmenter(slic_settings);
			recursive_image_segmenter.setInput(user_options.input_path);
			recursive_image_segmenter.setOutputDirectory(user_options.output_path);
			recursive_image_segmenter.setExtension(user_options.ext);
			if (user_options.use_scale)
			{
				recursive_image_segmenter.setScale(user_options.scale);
			}
			else
			{
				recursive_image_segmenter.setMaxSidelen(user_options.max_sidelen);
			}
			recursive_image_segmenter.setVerbose(user_options.verbose);

			// Segment the image(s)
			recursive_image_segmenter.segment();
		}
		else
		{
			// Create image segmenter and set parameters
			ImageSegmenter image_segmenter(slic_settings);
			image_segmenter.setInput(user_options.input_path);
			image_segmenter.setOutputDirectory(user_options.output_path);
			image_segmenter.setExtension(user_options.ext);
			image_segmenter.setRecursive(user_options.recursive);
			if (user_options.use_scale)
			{
				image_segmenter.setScale(user_options.scale);
			}
			else
			{
				image_segmenter.setMaxSidelen(user_options.max_sidelen);
			}
			image_segmenter.setVerbose(user_options.verbose);

			// Segment the image(s)
			image_segmenter.segment();
		}

		return 0;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}
