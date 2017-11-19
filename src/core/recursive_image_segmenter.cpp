#include "recursive_image_segmenter.h"

#include "util.h"

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include <omp.h>

#include <iostream>
#include <string>

namespace Superpixels
{
	RecursiveImageSegmenter::RecursiveImageSegmenter(const SLICSettings &settings) :
		ImageSegmenter(settings)
		{}

	void RecursiveImageSegmenter::segment()
	{
		if (!Util::Files::isDir(_input_path))
		{
			EXCEPTION_THROWER(Util::Exception::IOException, "RecursiveImageSegmenter requires a directory as the input path")
		}
		this->recursiveSegmentation(_input_path, _output_root);
	}

	void RecursiveImageSegmenter::recursiveSegmentation(const std::string &input_dir, const std::string &output_dir)
	{
		const boost::filesystem::path path = boost::filesystem::path(input_dir);

		// DFS through the filetree
		// Go through each file in the root
		#pragma omp parallel for
		for (const auto &file : boost::make_iterator_range(boost::filesystem::directory_iterator(path), {}))
		{
			// If it is a directory, recurse
			if (boost::filesystem::is_directory(file))
			{
				// This is the source directory
				const std::string src_dir = file.path().string();

				// Create the corresponding target directory
				const std::string path_tail = Util::Files::getFilenameFromPath(src_dir);
				const std::string tgt_dir = Util::Files::joinPathAndFile(output_dir, path_tail);
				Util::Files::mkdir(tgt_dir);

				// Recurse into this subdirectory
				this->recursiveSegmentation(src_dir, tgt_dir);
			}
			// If it is not a directory, segment the image
			else
			{
				// Perform segmentation and write to corresponding output folder
				this->segmentImage(file.path().string(), output_dir);
			}
		}	
	}

} // namespace Superpixels
