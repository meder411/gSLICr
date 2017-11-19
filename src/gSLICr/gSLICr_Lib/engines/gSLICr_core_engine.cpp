// Copyright 2014-2015 Isis Innovation Limited and the authors of gSLICr

#pragma once
#include "gSLICr_core_engine.h"
#include "../objects/gSLICr_spixel_info.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <iostream>
#include <tuple>

using namespace gSLICr;
using namespace gSLICr::objects;
using namespace std;

gSLICr::engines::core_engine::core_engine(const objects::settings& in_settings)
{
	slic_seg_engine = new seg_engine_GPU(in_settings);
}

gSLICr::engines::core_engine::~core_engine()
{
	delete slic_seg_engine;
}

void gSLICr::engines::core_engine::Process_Frame(UChar4Image* in_img)
{
	slic_seg_engine->Perform_Segmentation(in_img);
}

const IntImage * gSLICr::engines::core_engine::Get_Seg_Res()
{
	return slic_seg_engine->Get_Seg_Mask();
}

void gSLICr::engines::core_engine::Draw_Segmentation_Result(UChar4Image* out_img)
{
	slic_seg_engine->Draw_Segmentation_Result(out_img);
}

void gSLICr::engines::core_engine::Draw_Boundary_Only(UChar4Image* out_img)
{
	slic_seg_engine->Draw_Boundary_Only(out_img);
}

void gSLICr::engines::core_engine::Write_Seg_Res_To_PGM(const char* fileName)
{
	const IntImage* idx_img = slic_seg_engine->Get_Seg_Mask();
	int width = idx_img->noDims.x;
	int height = idx_img->noDims.y;
	const int* data_ptr = idx_img->GetData(MEMORYDEVICE_CPU);

	ofstream f(fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
	f << "P5\n" << width << " " << height << "\n65535\n";
	for (int i = 0; i < height * width; ++i)
	{
		ushort label = (ushort)data_ptr[i];
		ushort label_buffer = (label << 8 | label >> 8);
		f.write((const char*)&label_buffer, sizeof(ushort));
	}
	f.close();
}

bool gSLICr::engines::core_engine::Write_Centroids_To_Binary(const char* fileName)
{
	unordered_map<ushort, std::pair<float, float>> centroids;

	// Get Get_Superpixel_Map
	SpixelMap* spixel_map = slic_seg_engine->Get_Superpixel_Map();
	const size_t num_segs = spixel_map->dataSize;
	spixel_info* spixel_list = spixel_map->GetData(MEMORYDEVICE_CPU);

	// Go through all superpixels and make an ID-->(Cx,Cy) map
	for (int i = 0; i < num_segs; i++)
	{
			centroids.emplace(spixel_list[i].id, std::make_pair(spixel_list[i].center.x, spixel_list[i].center.y));
	}

	if (centroids.size() == 0)
	{
		return false;
	}

	const IntImage* idx_img = slic_seg_engine->Get_Seg_Mask();
	int width = idx_img->noDims.x;
	int height = idx_img->noDims.y;
	const int* data_ptr = idx_img->GetData(MEMORYDEVICE_CPU);

	ofstream f(fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	// Write dimensions
	f.write((const char*)&height, sizeof(int));
	f.write((const char*)&width, sizeof(int));
	for (int i = 0; i < height * width; ++i)
	{
		ushort label = (ushort)data_ptr[i];
		ushort label_buffer = (label << 8 | label >> 8);
		auto c = centroids.at(label);
		f.write((const char*)&(c.first), sizeof(float));
		f.write((const char*)&(c.second), sizeof(float));
	}
	f.close();

	return true;
}

bool gSLICr::engines::core_engine::Write_Colors_To_Binary(const char* fileName)
{
	unordered_map<ushort, std::tuple<float, float, float>> centroids;

	// Get Get_Superpixel_Map
	SpixelMap* spixel_map = slic_seg_engine->Get_Superpixel_Map();
	const size_t num_segs = spixel_map->dataSize;
	spixel_info* spixel_list = spixel_map->GetData(MEMORYDEVICE_CPU);

	// Go through all superpixels and make an ID-->(Cx,Cy) map
	for (int i = 0; i < num_segs; i++)
	{
			centroids.emplace(spixel_list[i].id, 
				std::make_tuple(spixel_list[i].color_info.r,
					spixel_list[i].color_info.g,
					spixel_list[i].color_info.b));
	}

	if (centroids.size() == 0)
	{
		return false;
	}

	const IntImage* idx_img = slic_seg_engine->Get_Seg_Mask();
	int width = idx_img->noDims.x;
	int height = idx_img->noDims.y;
	const int* data_ptr = idx_img->GetData(MEMORYDEVICE_CPU);

	ofstream f(fileName, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	// Write dimensions
	f.write((const char*)&height, sizeof(int));
	f.write((const char*)&width, sizeof(int));
	for (int i = 0; i < height * width; ++i)
	{
		ushort label = (ushort)data_ptr[i];
		ushort label_buffer = (label << 8 | label >> 8);
		auto c = centroids.at(label);
		f.write((const char*)&(std::get<0>(c)), sizeof(float));
		f.write((const char*)&(std::get<1>(c)), sizeof(float));
		f.write((const char*)&(std::get<2>(c)), sizeof(float));
	}
	f.close();

	return true;
}


void gSLICr::engines::core_engine::Write_Superpixel_Info_To_TXT(const char* filename, gSLICr::COLOR_SPACE color_space)
{
	SpixelMap* spixel_map = slic_seg_engine->Get_Superpixel_Map();
	const size_t num_segs = spixel_map->dataSize;
	spixel_info* spixel_list = spixel_map->GetData(MEMORYDEVICE_CPU);

	// Open output file for writing
	std::ofstream file;
	file.open(filename, std::ios::trunc);

	file << "# Centroid information" << std::endl;
	switch (color_space)
	{
		case RGB:
			file << "# ID NUM_PIX CX CY R G B A" << std::endl;
			break;
		case XYZ:
			file << "# ID NUM_PIX CX CY X Y Z A" << std::endl;
			break; 
		case CIELAB:
			file << "# ID NUM_PIX CX CY L A B A" << std::endl;
			break;
	}
	file << num_segs << std::endl;
	for (int i = 0; i < num_segs; i++)
	{
		if (spixel_list[i].no_pixels > 0)
		{
			file << spixel_list[i].id << " ";
			file << spixel_list[i].no_pixels << " ";
			file << spixel_list[i].center.x << " ";
			file << spixel_list[i].center.y <<" ";
			file << spixel_list[i].color_info.r << " ";
			file << spixel_list[i].color_info.g << " ";
			file << spixel_list[i].color_info.b << " ";
			file << spixel_list[i].color_info.a << std::endl;
		}
	}
	file << std::endl;

	file.close();
}


