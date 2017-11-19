#include "image_segmenter.h"

#include "util.h"

#include "../gSLICr/NVTimer.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace Superpixels
{
	ImageSegmenter::ImageSegmenter(const SLICSettings &settings) :
		Segmenter(settings)
		{}

	void ImageSegmenter::segment()
	{
		// If the input is a single file, just segment it
		if (!Util::Files::isDir(_input_path))
		{
			// Try to make the directory if it doesn't exist
			Util::Files::mkdir(_output_root);

			// Perform the image segmentation
			segmentImage(_input_path, _output_root);
		}
		else
		{
			// Load all file paths from the directory
			// NOTE: This should only be used for reasonable size image 
			// directories as it searches for and loads all paths into memory. 
			// Use RecursiveImageSegmenter for large scale image sets
			std::vector<std::string> files;
			Util::Files::getFilesOfTypeInDirectory(files, _input_path, _ext,
				 _recursive);

			// For loop to process all the files
			for (const auto &file : files)
			{
				// Remove top-level directory from the path (i.e. remove the input_path directory)
				const std::string topless_path = Util::Files::stripRootDir(file);

				// Remove the filename and recursively make the directories needed for the dir tree
				const std::string base_path = Util::Files::getBasePathFromPath(topless_path);
				const std::string output_dir = Util::Files::joinPathAndFile(_output_root, base_path);
				Util::Files::mkdirs(output_dir);

				// Segment the input image
				segmentImage(file, output_dir);
			}
		}
	}

	void ImageSegmenter::writeBoundaryToBinary(const std::string &output_path, const cv::Mat & boundary) const
	{
		uint8_t one = 1;
		uint8_t zero = 0;
		std::ofstream f(output_path.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

		// Write dimensions
		int height = boundary.rows;
		int width = boundary.cols;
		f.write((const char*)&height, sizeof(int));
		f.write((const char*)&width, sizeof(int));

		for (int i = 0; i < height; ++i)
		{
			for (int j = 0; j < width; ++j)
			{
				if (boundary.at<cv::Vec3b>(i,j)[0] > 0)
				{
					f.write((const char*)&one, sizeof(uint8_t));
				}
				else
				{
					f.write((const char*)&zero, sizeof(uint8_t));
				}
			}
		}
		f.close();
	}

	void ImageSegmenter::segmentImage(const std::string &input_path, const std::string &output_path)
	{
		// Read the image from disk
		if (_verbose)
		{
			std::cout << "Segmenting image: '" << input_path << "'" << std::endl;
		}

		cv::Mat old_frame = cv::imread(input_path);
		if (!old_frame.data){ EXCEPTION_THROWER(Util::Exception::IOException, "Error loading image") }

		// Set image size by max side length (preserving aspect ratio)
		if (!_use_scale)
		{
			if (old_frame.cols <= old_frame.rows)
			{
				_settings.img_size.y = (int)_max_sidelen;
				float scale = _max_sidelen / old_frame.rows;
				_settings.img_size.x = (int)(scale * old_frame.cols);
			}
			else
			{
				_settings.img_size.x = (int)_max_sidelen;
				float scale = _max_sidelen / old_frame.cols;
				_settings.img_size.y = (int)(scale * old_frame.rows);
			}
		}
		// Or set image size using a scale factor
		else
		{
			_settings.img_size.x = (int)(_scale * old_frame.cols);
			_settings.img_size.y = (int)(_scale * old_frame.rows);
		}
		cv::Size s(_settings.img_size.x, _settings.img_size.y);

		// Instantiate a core_engine
		std::unique_ptr<gSLICr::engines::core_engine> gSLICr_engine = std::unique_ptr<gSLICr::engines::core_engine>(new gSLICr::engines::core_engine(_settings));

		// gSLICr takes gSLICr::UChar4Image as input and output
		std::unique_ptr<gSLICr::UChar4Image> in_img = std::unique_ptr<gSLICr::UChar4Image>(new gSLICr::UChar4Image(_settings.img_size, true, true));
		std::unique_ptr<gSLICr::UChar4Image> out_seg = std::unique_ptr<gSLICr::UChar4Image>(new gSLICr::UChar4Image(_settings.img_size, true, true));
		std::unique_ptr<gSLICr::UChar4Image> out_bound = std::unique_ptr<gSLICr::UChar4Image>(new gSLICr::UChar4Image(_settings.img_size, true, true));

		// Resize the image
		cv::Mat frame;
		cv::resize(old_frame, frame, s);

		// GPU call: load input image from CPU onto the GPU
		load_image(frame, in_img.get());

		StopWatchInterface *my_timer;
		sdkCreateTimer(&my_timer);
		sdkResetTimer(&my_timer);
		sdkStartTimer(&my_timer);
		
		// Perform the segmentation
		gSLICr_engine->Process_Frame(in_img.get());
		
		// Stop the timer and print the time
		sdkStopTimer(&my_timer);
		if (!_verbose)
		{
			std::cout << "\rSegmentation in:["<< sdkGetTimerValue(&my_timer) << "]ms" << std::flush;
		}
		else
		{
			std::cout << "\tSegmentation in:["<< sdkGetTimerValue(&my_timer) << "]ms" << std::endl;
		}

		// GPU call: draw segmentation on an output image
		// This call draws the segmentation visualized on the input image
		gSLICr_engine->Draw_Segmentation_Result(out_seg.get());

		// Load viz image from GPU to CPU
		cv::Mat img_draw_frame;
		img_draw_frame.create(s, CV_8UC3);
		load_image(out_seg.get(), img_draw_frame);


		// Uncommenting this will draw the segmentation boundaries only
		// gSLICr_engine->Draw_Boundary_Only(out_bound.get());

		// Load bounday-only image from GPU to CPU
		// cv::Mat boundary_draw_frame;
		// boundary_draw_frame.create(s, CV_8UC3);
		// load_image(out_bound.get(), boundary_draw_frame);
		
		// Form output paths
		std::string fname = Util::Files::getFilenameFromPath(input_path);
		std::string full_out_path = Util::Files::joinPathAndFile(output_path, fname);

		// Write viz image
		std::string viz_out_name = full_out_path + ".viz.png";
		cv::imwrite(viz_out_name, img_draw_frame);
		
		// Write segmentation PGM
		std::string pgm_out_name = full_out_path + ".slic.pgm";
		gSLICr_engine->Write_Seg_Res_To_PGM(pgm_out_name.c_str());
		

		///////////////////////////////////////////////////////////////
		// Extra information that can be written to file if need be
		///////////////////////////////////////////////////////////////
		
		// Uncomment to write the superpixel stats of each image to a text file
		// std::string txt_out_name = full_out_path + ".centers.txt";
		// gSLICr_engine->Write_Superpixel_Info_To_TXT(txt_out_name.c_str(), _settings.color_space);
		
		
		// Uncomment to write a binary file where each pixel is given the coordinates of the centroid it's assigned to
		// std::string bin_out_name = full_out_path + ".slic.bin";
		// bool success = gSLICr_engine->Write_Centroids_To_Binary(bin_out_name.c_str());

		// Uncomment to write a binary file where each pixel is given the color of the centroid it's assigned to
		// std::string colors_out_name = full_out_path + ".colors.bin";
		// bool success = gSLICr_engine->Write_Colors_To_Binary(colors_out_name.c_str());
		
		// Uncomment to write just the boundary data to a binary file
		// std::string boundary_out_name = full_out_path + ".boundary.bin";
		// this->writeBoundaryToBinary(boundary_out_name, 
		// 	boundary_draw_frame);

		if (_verbose)
		{
			std::cout << "\tSegmentations written to: '" << full_out_path << "'" << std::endl;
		}
	}

} // namespace Superpixels
