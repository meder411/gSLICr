#ifndef SUPERPIXELS_SRC_CORE_OPTIONS_H_
#define SUPERPIXELS_SRC_CORE_OPTIONS_H_

#include <boost/program_options.hpp>

#include <string>
#include <iostream>

namespace Superpixels
{
	
	struct SuperpixelUserOptions
	{
		// SLIC options
		int num_segs = 128;
		int spixel_size = 256;
		float coh_weight = 0.6f;
		int num_iters = 6;
		std::string color_space = "XYZ";
		std::string seg_method = "GIVEN_SIZE";
		bool no_enforce_connectivity = false;

		// Interface options
		std::string input_path;
		std::string output_path;
		double scale = 1.0;
		double max_sidelen = 480.0;
		bool use_scale = true;
		bool verbose = false;

		// Unique for video
		double sampling_rate = 1.0;

		// Unique for images
		std::string ext;
		bool recursive = false;
		bool large_scale = false;

	};


	// From https://stackoverflow.com/a/21914921/3427580
	inline void conflicting_options(const boost::program_options::variables_map & vm,
		const std::string & opt1, const std::string & opt2)
	{
		if (vm.count(opt1) && !vm[opt1].defaulted() &&
			vm.count(opt2) && !vm[opt2].defaulted())
		{
			throw std::logic_error(std::string("Mutually exlusive options '") +
				opt1 + "' and '" + opt2 + "'.");
		}
	}


	inline int parseVideoSegmenterCommandLine(int argc, char **argv, SuperpixelUserOptions &input_options)
	{
		boost::program_options::options_description req("Required inputs");
		req.add_options()
			("input_path", boost::program_options::value<std::string>(&input_options.input_path)->required(), "Video file to sample")
			("output_path", boost::program_options::value<std::string>(&input_options.output_path)->required(), "Directory to save sampled frames")
			("sampling_rate", boost::program_options::value<double>(&input_options.sampling_rate)->required(), 
				"How frequently to sample video (in terms of frames)");
			
		// Defines optional input option group  
		boost::program_options::options_description opt("Optional inputs");
		opt.add_options()
			("help", "Print help info")
			("scale", boost::program_options::value<double>(&input_options.scale),"Scale to resize the images")
			("max_sidelen", boost::program_options::value<double>(&input_options.max_sidelen),"Maximum side-length to resize the images to (preserving aspect ratio")
			("coh_weight", boost::program_options::value<float>(&input_options.coh_weight)->default_value(0.6),"Color cohesion weight")
			("color_space", boost::program_options::value<std::string>(&input_options.color_space)->default_value("XYZ"),
				"'XYZ', 'RGB', or 'CIELAB'. Color space in which to perform clustering")
			("no_enforce", boost::program_options::bool_switch(&input_options.no_enforce_connectivity), 
				"Flag disables enforcement of superpixel connectivity")
			("num_iters", boost::program_options::value<int>(&input_options.num_iters)->default_value(5),"Number of clustering iterations")
			("num_segs", boost::program_options::value<int>(&input_options.num_segs)->default_value(128),
				"Number of superpixels to segment image into. Used with seg_method = GIVEN_NUM.")
			("seg_method", boost::program_options::value<std::string>(&input_options.seg_method)->default_value("GIVEN_SIZE"),
				"'GIVEN_SIZE' or 'GIVEN_NUM'. SLIC Segmentation constraint (size of superpixel or total number of them)")
			("spixel_size", boost::program_options::value<int>(&input_options.spixel_size)->default_value(256),
				"Size of superpixels in pixels. Used with seg_method = GIVEN_SIZE.")
			("verbose", boost::program_options::bool_switch(&input_options.verbose)->default_value(false), "Verbosity");


		// Creates a combined option group
		boost::program_options::options_description all("Allowed inputs");
		all.add(req).add(opt);

		try
		{
			// Parse the inputs and map to their corresponding variables
			boost::program_options::variables_map vm;
			boost::program_options::store(boost::program_options::parse_command_line(argc, argv, all), vm);

			// Handle mutually exlusive options
			conflicting_options(vm, "scale", "max_sidelin");
			conflicting_options(vm, "large_scale", "recursive");

			if (vm.count("scale"))
			{
				input_options.use_scale = true;
			}
			else if (vm.count("max_sidelen"))
			{
				input_options.use_scale = false;
			}

			// If help is input, print full usage
			if (vm.count("help"))
			{
				std::cout << all << std::endl;
				return -1;
			}

			// Try to assign input values to their mapped variables
			// Throws exceptions if failure (wrong type, missing args, etc)
			boost::program_options::notify(vm);
		}
		catch(std::exception& e)
		{
			std::cout << all << std::endl;
			return -1;
		}
		catch(...)
		{
			std::cerr << "Exception of unknown type!" << std::endl;
			return -1;
		}

		return 0;
	}

	inline int parseImageSegmenterCommandLine(int argc, char **argv, SuperpixelUserOptions &input_options)
	{
		boost::program_options::options_description req("Required inputs");
		req.add_options()
			("input_path", boost::program_options::value<std::string>(&input_options.input_path)->required(), "Video file to sample")
			("output_path", boost::program_options::value<std::string>(&input_options.output_path)->required(), "Directory to save sampled frames")
			("ext", boost::program_options::value<std::string>(&input_options.ext)->required(), 
				"File extension of images to segment (WITH leading period");
			
		// Defines optional input option group  
		boost::program_options::options_description opt("Optional inputs");
		opt.add_options()
			("help", "Print help info")
			("scale", boost::program_options::value<double>(&input_options.scale),"Scale to resize the images")
			("max_sidelen", boost::program_options::value<double>(&input_options.max_sidelen),"Maximum side-length to resize the images to (preserving aspect ratio")
			("recursive", boost::program_options::bool_switch(&input_options.recursive), 
				"Simple recursion into subdirectorys to load images (loads all paths into memory)")
			("large_scale", boost::program_options::bool_switch(&input_options.large_scale), 
				"Faster recursion into subdirectorys to load images (for large-scale datasets)")
			("coh_weight", boost::program_options::value<float>(&input_options.coh_weight)->default_value(0.6),"Color cohesion weight")
			("color_space", boost::program_options::value<std::string>(&input_options.color_space)->default_value("XYZ"),
				"'XYZ', 'RGB', or 'CIELAB'. Color space in which to perform clustering")
			("no_enforce", boost::program_options::bool_switch(&input_options.no_enforce_connectivity), 
				"Flag disables enforcement of superpixel connectivity")
			("num_iters", boost::program_options::value<int>(&input_options.num_iters)->default_value(5),"Number of clustering iterations")
			("num_segs", boost::program_options::value<int>(&input_options.num_segs)->default_value(128),
				"Number of superpixels to segment image into. Used with seg_method = GIVEN_NUM.")
			("seg_method", boost::program_options::value<std::string>(&input_options.seg_method)->default_value("GIVEN_SIZE"),
				"'GIVEN_SIZE' or 'GIVEN_NUM'. SLIC Segmentation constraint (size of superpixel or total number of them)")
			("spixel_size", boost::program_options::value<int>(&input_options.spixel_size)->default_value(256),
				"Size of superpixels in pixels. Used with seg_method = GIVEN_SIZE.")
			("verbose", boost::program_options::bool_switch(&input_options.verbose)->default_value(false), "Verbosity");


		// Creates a combined option group
		boost::program_options::options_description all("Allowed inputs");
		all.add(req).add(opt);

		try
		{
			// Parse the inputs and map to their corresponding variables
			boost::program_options::variables_map vm;
			boost::program_options::store(boost::program_options::parse_command_line(argc, argv, all), vm);

			// Handle mutually exlusive options
			conflicting_options(vm, "scale", "max_sidelin");
			conflicting_options(vm, "large_scale", "recursive");

			if (vm.count("max_sidelen"))
			{
				std::cout << "OM" << std::endl;
				input_options.use_scale = false;
			}

			// If help is input, print full usage
			if (vm.count("help"))
			{
				std::cout << all << std::endl;
				return -1;
			}

			// Try to assign input values to their mapped variables
			// Throws exceptions if failure (wrong type, missing args, etc)
			boost::program_options::notify(vm);
		}
		catch(std::exception& e)
		{
			std::cout << all << std::endl;
			return -1;
		}
		catch(...)
		{
			std::cerr << "Exception of unknown type!" << std::endl;
			return -1;
		}

		return 0;
	}

} // namespace Superpixels

#endif // SUPERPIXELS_SRC_CORE_OPTIONS_H_