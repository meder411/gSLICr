#ifndef SUPERPIXELS_SRC_CORE_IMAGE_SEGMENTER_H_
#define SUPERPIXELS_SRC_CORE_IMAGE_SEGMENTER_H_

#include "segmenter.h"

#include <string>

namespace Superpixels
{

	class ImageSegmenter : public Segmenter
	{

		protected:
			bool _recursive = false;
			std::string _ext;

			void segmentImage(const std::string &input_path, const std::string &output_path);

			void writeBoundaryToBinary(const std::string &output_path, const cv::Mat &boundary) const;

		public:
			ImageSegmenter(const SLICSettings &settings);
			
			inline void setRecursive(const bool recursive);
			inline void setExtension(const std::string &ext);

			virtual inline void setInput(const std::string &input);
			virtual void segment();


	};

	void ImageSegmenter::setRecursive(const bool recursive) { _recursive = recursive; }
	void ImageSegmenter::setExtension(const std::string &ext) { _ext = ext; }
	
	void ImageSegmenter::setInput(const std::string &input) { _input_path = input; }

} // namespace Superpixels

#endif // SUPERPIXELS_SRC_CORE_IMAGE_SEGMENTER_H_