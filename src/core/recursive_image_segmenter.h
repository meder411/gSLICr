#ifndef SUPERPIXELS_SRC_CORE_RECURSIVE_IMAGE_SEGMENTER_H_
#define SUPERPIXELS_SRC_CORE_RECURSIVE_IMAGE_SEGMENTER_H_

#include "image_segmenter.h"

#include <string>

namespace Superpixels
{

	class RecursiveImageSegmenter : public ImageSegmenter
	{

		private:
			void recursiveSegmentation(const std::string &input_dir, const std::string &output_dir);

		public:
			RecursiveImageSegmenter(const SLICSettings &settings);
			
			inline void setSourceRoot(const std::string &src_root);
			inline void setTargetRoot(const std::string &target_root);

			virtual void segment();


	};

} // namespace Superpixels

#endif // SUPERPIXELS_SRC_CORE_RECURSIVE_IMAGE_SEGMENTER_H_