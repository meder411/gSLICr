#ifndef SUPERPIXELS_SRC_CORE_VIDEO_SEGMENTER_H_
#define SUPERPIXELS_SRC_CORE_VIDEO_SEGMENTER_H_

#include "segmenter.h"

#include <string>

namespace Superpixels
{

	class VideoSegmenter : public Segmenter
	{

		private:
			double _sampling_rate = 1.0;

		public:
			VideoSegmenter(const SLICSettings &settings);
			
			inline void setSamplingRate(const double sampling_rate);

			virtual void setInput(const std::string &input_video);
			virtual void segment();
	};

	void VideoSegmenter::setSamplingRate(const double sampling_rate) { _sampling_rate = sampling_rate; }


} // namespace Superpixels

#endif // SUPERPIXELS_SRC_CORE_VIDEO_SEGMENTER_H_