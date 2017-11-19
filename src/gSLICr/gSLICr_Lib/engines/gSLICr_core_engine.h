// Copyright 2014-2015 Isis Innovation Limited and the authors of gSLICr

#pragma once
#include "gSLICr_seg_engine_GPU.h"
#include "../gSLICr_defines.h"


namespace gSLICr
{
	namespace engines
	{
		class core_engine
		{
		private:

			seg_engine* slic_seg_engine;

		public:

			core_engine(const objects::settings& in_settings);
			~core_engine();

			// Function to segment in_img
			void Process_Frame(UChar4Image* in_img);

			// Function to get the pointer to the segmented mask image
			const IntImage * Get_Seg_Res();

			// Function to draw segmentation result on out_img
			void Draw_Segmentation_Result(UChar4Image* out_img);
			
			// Function to draw segmentation boundaries on out_img
			void Draw_Boundary_Only(UChar4Image* out_img);

			// Write the segmentation result to a PGM image
			void Write_Seg_Res_To_PGM(const char* fileName);

			bool Write_Colors_To_Binary(const char* fileName);

			bool Write_Centroids_To_Binary(const char* fileName);

			// Write the superpixel
			void Write_Superpixel_Info_To_TXT(const char* filename, gSLICr::COLOR_SPACE);
		};
	}
}

