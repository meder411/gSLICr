#include "video_segmenter.h"

#include "util.h"

#include "../gSLICr/NVTimer.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <string>

namespace Superpixels
{
	VideoSegmenter::VideoSegmenter(const SLICSettings &settings) :
		Segmenter(settings)
		{}

	void VideoSegmenter::setInput(const std::string &input_video)
	{
		if (Util::Files::isDir(input_video))
		{
			EXCEPTION_THROWER(Util::Exception::IOException, "Input must be a video. Use ImageSegmenter for image sets.")
		}
		_input_path = input_video;
	}


	void VideoSegmenter::segment()
	{
		cv::VideoCapture cap(_input_path);

		// Get size of video fram
		double frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		double frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

		// Set image size by max side length (preserving aspect ratio)
		if (!_use_scale)
		{
			if (frame_width <= frame_height)
			{
				_settings.img_size.y = (int)_max_sidelen;
				float scale = _max_sidelen / frame_height;
				_settings.img_size.x = (int)(scale * frame_width);
			}
			else
			{
				_settings.img_size.x = (int)_max_sidelen;
				float scale = _max_sidelen / frame_width;
				_settings.img_size.y = (int)(scale * frame_height);
			}
		}
		// Or set image size using a scale factor
		else
		{
			_settings.img_size.x = (int)(_scale * frame_width);
			_settings.img_size.y = (int)(_scale * frame_height);
		}
		cv::Size s(_settings.img_size.x, _settings.img_size.y);


		// Instantiate a core_engine
		gSLICr::engines::core_engine* gSLICr_engine = new gSLICr::engines::core_engine(_settings);

		// gSLICr takes gSLICr::UChar4Image as input and output
		gSLICr::UChar4Image* in_img = new gSLICr::UChar4Image(_settings.img_size, true, true);
		gSLICr::UChar4Image* out_img = new gSLICr::UChar4Image(_settings.img_size, true, true);


		cv::Mat oldFrame, frame;
		cv::Mat boundry_draw_frame;
		boundry_draw_frame.create(s, CV_8UC3);

		StopWatchInterface *my_timer;
		sdkCreateTimer(&my_timer);

		int save_count = 0;

		double frame_step = _sampling_rate;
		double current_frame = 0;
		cap.set(CV_CAP_PROP_POS_FRAMES, current_frame);

		while (cap.read(oldFrame))
		{

			cv::resize(oldFrame, frame, s);
			
			load_image(frame, in_img);
		    
		    sdkResetTimer(&my_timer);
		    sdkStartTimer(&my_timer);
			gSLICr_engine->Process_Frame(in_img);
			sdkStopTimer(&my_timer);
			std::cout<<"\rsegmentation in:["<<sdkGetTimerValue(&my_timer)<<"]ms" << std::flush;
		    
			gSLICr_engine->Draw_Segmentation_Result(out_img);
			
			load_image(out_img, boundry_draw_frame);
			char out_name[100];

			sprintf(out_name, Util::Files::joinPathAndFile(_output_root, "img_%06i.png.slic.pgm").c_str(), (int)current_frame);
			gSLICr_engine->Write_Seg_Res_To_PGM(out_name);
			sprintf(out_name, Util::Files::joinPathAndFile(_output_root, "img_%06i.png.centers.txt").c_str(), (int)current_frame);
			gSLICr_engine->Write_Superpixel_Info_To_TXT(out_name, _settings.color_space);
			sprintf(out_name, Util::Files::joinPathAndFile(_output_root, "img_%06i.png.viz.png").c_str(), (int)current_frame);
			imwrite(out_name, boundry_draw_frame);
			
			save_count++;
			current_frame += frame_step;
			cap.set(CV_CAP_PROP_POS_FRAMES, current_frame);
		}
	}


} // namespace Superpixels
