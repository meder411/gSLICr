#ifndef SUPERPIXELS_SRC_CORE_SEGMENTER_H_
#define SUPERPIXELS_SRC_CORE_SEGMENTER_H_

#include "options.h"
#include "util.h"

#include "../gSLICr/gSLICr_Lib/gSLICr.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include <string>
#include <memory>

namespace Superpixels
{
	struct SLICSettings
	{
		int num_segs = 2000;
		int spixel_size = 16;
		float coh_weight = 0.6f;
		int num_iters = 5;
		std::string color_space = "XYZ";
		std::string seg_method = "GIVEN_SIZE";
		bool enforce_connectivity = true;

		SLICSettings(const SuperpixelUserOptions &options) :
			num_segs(options.num_segs),
			spixel_size(options.spixel_size),
			coh_weight(options.coh_weight),
			num_iters(options.num_iters),
			color_space(options.color_space),
			seg_method(options.seg_method),
			enforce_connectivity(!options.no_enforce_connectivity)
			{}
	};

	class Segmenter
	{

		protected:

			gSLICr::objects::settings _settings;
			double _scale = 1.0;
			double _max_sidelen = 480.0;
			bool _verbose = false;
			std::string _input_path;
			std::string _output_root;
			bool _use_scale = true; // If false, uses the maximum side length

			inline void load_image(const cv::Mat& inimg, gSLICr::UChar4Image* outimg) const;
			inline void load_image(const gSLICr::UChar4Image* inimg, cv::Mat& outimg) const;
			
		public:
			inline Segmenter(const SLICSettings &settings);

			// NOTE: Changing settings requires re-initializing gSLICr engine which is slow
			inline void changeSettings(const SLICSettings &settings);

			inline void setScale(const double scale);
			inline void setMaxSidelen(const double max_sidelen);
			inline void setVerbose(const bool verbose);

			virtual inline void setOutputDirectory(const std::string &output_root);
			virtual void setInput(const std::string &) = 0;
			virtual void segment() = 0;
	};


	////////////////////
	// IMPLEMENTATION //
	////////////////////

	Segmenter::Segmenter(const SLICSettings &settings)
	{
		_settings.no_segs = settings.num_segs;
		_settings.spixel_size = settings.spixel_size;
		_settings.coh_weight = settings.coh_weight;
		_settings.no_iters = settings.num_iters;
		
		// gSLICr::GIVEN_SIZE for given size or 
		// gSLICr::GIVEN_NUM for given number
		if (settings.seg_method == "GIVEN_SIZE")
		{
			_settings.seg_method = gSLICr::GIVEN_SIZE;
		}
		else if (settings.seg_method == "GIVEN_NUM")
		{
			_settings.seg_method = gSLICr::GIVEN_NUM;
		}
		else
		{
			// Fail gracefully?
		}
		// gSLICr::XYZ for XYZ, gSLICr::CIELAB for Lab, or gSLICr::RGB for RGB
		if (settings.color_space == "XYZ")
		{
			_settings.color_space = gSLICr::XYZ;
		}
		else if (settings.color_space == "RGB")
		{
			_settings.color_space = gSLICr::RGB;
		}
		else if (settings.color_space == "CIELAB")
		{
			_settings.color_space = gSLICr::CIELAB;
		}
		else
		{
			// Fail gracefully?
		}
		// Whether or not run the enforce connectivity step
		_settings.do_enforce_connectivity = settings.enforce_connectivity;
	}

	void Segmenter::setOutputDirectory(const std::string &output_root)
	{
		_output_root = output_root;
		Util::Files::mkdirs(output_root);
	}
	void Segmenter::setScale(const double scale)
	{
		_scale = scale;
		_use_scale = true;
	}
	void Segmenter::setMaxSidelen(const double max_sidelen)
	{
		_max_sidelen = max_sidelen;
		_use_scale = false;
	}
	void Segmenter::setVerbose(const bool verbose) { _verbose = verbose; }


	void Segmenter::load_image(const cv::Mat& inimg, gSLICr::UChar4Image* outimg) const
	{
		gSLICr::Vector4u* outimg_ptr = outimg->GetData(MEMORYDEVICE_CPU);

		for (int y = 0; y < outimg->noDims.y;y++)
		{
			for (int x = 0; x < outimg->noDims.x; x++)
			{
				int idx = x + y * outimg->noDims.x;
				outimg_ptr[idx].b = inimg.at<cv::Vec3b>(y, x)[0];
				outimg_ptr[idx].g = inimg.at<cv::Vec3b>(y, x)[1];
				outimg_ptr[idx].r = inimg.at<cv::Vec3b>(y, x)[2];
			}
		}
	}

	void Segmenter::load_image(const gSLICr::UChar4Image* inimg, cv::Mat& outimg) const
	{
		const gSLICr::Vector4u* inimg_ptr = inimg->GetData(MEMORYDEVICE_CPU);

		for (int y = 0; y < inimg->noDims.y; y++)
		{
			for (int x = 0; x < inimg->noDims.x; x++)
			{
				int idx = x + y * inimg->noDims.x;
				outimg.at<cv::Vec3b>(y, x)[0] = inimg_ptr[idx].b;
				outimg.at<cv::Vec3b>(y, x)[1] = inimg_ptr[idx].g;
				outimg.at<cv::Vec3b>(y, x)[2] = inimg_ptr[idx].r;
			}
		}
	}

	void Segmenter::changeSettings(const SLICSettings &settings)
	{
		_settings.no_segs = settings.num_segs;
		_settings.spixel_size = settings.spixel_size;
		_settings.coh_weight = settings.coh_weight;
		_settings.no_iters = settings.num_iters;
		// gSLICr::GIVEN_SIZE for given size or gSLICr::GIVEN_NUM for given number
		if (settings.seg_method == "GIVEN_SIZE")
		{
			_settings.seg_method = gSLICr::GIVEN_SIZE;
		}
		else if (settings.seg_method == "GIVEN_NUM")
		{
			_settings.seg_method = gSLICr::GIVEN_NUM;
		}
		else
		{
			// Fail gracefully?
		}
		// gSLICr::XYZ for XYZ, gSLICr::CIELAB for Lab, or gSLICr::RGB for RGB
		if (settings.color_space == "XYZ")
		{
			_settings.color_space = gSLICr::XYZ;
		}
		else if (settings.color_space == "RGB")
		{
			_settings.color_space = gSLICr::RGB;
		}
		else if (settings.color_space == "CIELAB")
		{
			_settings.color_space = gSLICr::CIELAB;
		}
		else
		{
			// Fail gracefully?
		}
		// Whether or not run the enforce connectivity step
		_settings.do_enforce_connectivity = settings.enforce_connectivity;
	}

} // namespace Superpixels




#endif // SUPERPIXELS_SRC_CORE_SEGMENTER_H_