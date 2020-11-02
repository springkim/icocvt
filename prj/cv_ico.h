//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                               License Agreement
//                                    cv_ico
//
//               Copyright (C) 2018, Kim Bomm, all rights reserved.
//
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//

#ifndef CVTICO_CV_ICO_H
#define CVTICO_CV_ICO_H
#include<opencv2/opencv.hpp>
namespace cv{
	namespace ico{
#pragma pack(push,1)
		struct ICOCUR_Entires {
			unsigned char width;
			unsigned char height;
			unsigned char color_count;
			unsigned char reserved;
			union {
				unsigned short x_hot_spot;
				unsigned short planes;
			};
			union {
				unsigned short y_hot_spot;
				unsigned short bit_counts;
			};
			unsigned int size_in_bytes;
			unsigned int file_offset;
		};
		struct ICOCUR_InfoHeader {
			unsigned int size;
			unsigned int width;
			unsigned int height;
			unsigned short planes;
			unsigned short bit_count;
			unsigned int compression;
			unsigned int image_size;
			unsigned int x_pixels_per_M;
			unsigned int y_pixels_per_M;
			unsigned int colors_used;
			unsigned int colors_important;
		};
		struct ICOCUR_File {
			short reserved = 0;
			short type = 2;
			short count = 1;
			std::vector<ICOCUR_Entires> entires;
			std::vector<ICOCUR_InfoHeader> info_header;
			std::vector<std::pair<uchar*,int>> colors;
		};
#pragma pack(pop)
		inline bool _imwrite_ico(const cv::String& filename,const cv::Mat& img,const int type,const std::vector<int>& params){
			auto letterbox=[](const cv::Mat& img)->cv::Mat{
				int N=std::max(img.cols,img.rows);
				cv::Mat dst=cv::Mat::zeros(N,N,CV_8UC(img.channels()));	//transparent
				int dx=(N-img.cols)/2;
				int dy=(N-img.rows)/2;
				img.copyTo(dst(cv::Rect(dx,dy,img.cols,img.rows)));
				return dst;
			};
			cv::Mat src=img.cols==img.rows?img.clone():letterbox(img);
			if(src.channels()==1){
				cv::cvtColor(src,src,CV_GRAY2BGRA);
			}
			if(src.channels()==3){
				cv::cvtColor(src,src,CV_BGR2BGRA);
			}
			for(auto it=src.begin<cv::Vec4b>();it!=src.end<cv::Vec4b>();it++){
				if((*it)[3]==0){
					(*it)[0]=(*it)[1]=(*it)[2]=255;
				}
			}
			ICOCUR_File file;
			file.reserved = 0;
			file.type = static_cast<short>(type);	//ico(1), cur(2)
			std::vector<cv::Mat> srcs;
			std::vector<int> support_sizes={16,32,48,64,128};
			for(int i=0;i<support_sizes.size();i++){
				cv::Mat dst;
				cv::resize(src,dst,cv::Size(support_sizes[i],support_sizes[i]),0,0,cv::INTER_AREA);
				srcs.push_back(dst);
			}
			file.count = static_cast<short>(srcs.size());	//256,128,64,32,16
			int fos=6+sizeof(ICOCUR_Entires)*srcs.size();
			for(int i=0;i<srcs.size();i++){
				int W=srcs[i].cols;	//Must be the same.
				int H=srcs[i].rows;
				///Make entires
				ICOCUR_Entires entires={0};
				entires.width = static_cast<uchar>(W);
				entires.height = static_cast<uchar>(H);
				entires.color_count = 0;
				entires.reserved = 0;
				if (type == 2) {
					entires.x_hot_spot = static_cast<unsigned short>(W/2-1);
					entires.y_hot_spot = static_cast<unsigned short>(H/2-1);
				} else if (type == 1) {
					entires.planes = 1;
					entires.bit_counts = 32;
				}
				entires.size_in_bytes = 40U +  (W*H)*4 + (W*H)/8;
				entires.file_offset = fos;
				///Make ICOCUR_InfoHeader
				ICOCUR_InfoHeader info_header={0};
				info_header.size = 40;
				info_header.width = static_cast<unsigned int>(W);
				info_header.height = static_cast<unsigned int>(H+H);
				info_header.planes = 1;
				info_header.bit_count = 32;
				info_header.compression = 0;
				info_header.image_size = (W*H)*4 + (W*H)/8;
				info_header.x_pixels_per_M = 0;
				info_header.y_pixels_per_M = 0;
				info_header.colors_used = 0;
				info_header.colors_important = 0;
				///Make colors
				int size=(W*H) * 4 + (W*H) / 8;
				uchar* p_colors=new uchar[size];
				uchar* colors=p_colors;
				unsigned int* p_monochrome = (unsigned int*)(p_colors + ((W*H) * 4));
				for (int y = H-1; y >= 0; y--) {
					for (int x = W-1; x >= 0; x--) {
						cv::Vec4b pixel = srcs[i].at<cv::Vec4b>(y, W - 1 - x);
						for (int c = 0; c < 4; c++) *p_colors++ = pixel[c];

					}
				}
				file.entires.push_back(entires);
				file.info_header.push_back(info_header);
				file.colors.push_back(std::make_pair(colors,size));
				fos+=(sizeof(ICOCUR_InfoHeader)+size);
			}
			FILE* fp = fopen(filename.c_str(), "wb");
			fwrite(&file.reserved,sizeof(file.reserved),1,fp);
			fwrite(&file.type,sizeof(file.type),1,fp);
			fwrite(&file.count,sizeof(file.count),1,fp);
			for(int i=0;i<file.entires.size();i++) {
				fwrite(&file.entires[i], sizeof(ICOCUR_Entires), 1, fp);
			}
			for(int i=0;i<file.entires.size();i++) {
				fwrite(&file.info_header[i], sizeof(ICOCUR_InfoHeader), 1, fp);
				fwrite(file.colors[i].first, 1, file.colors[i].second, fp);
				delete[] file.colors[i].first;
			}
			fclose(fp);
			return true;
		}
		inline bool imwrite(const cv::String &filename, const cv::Mat &img,const std::vector<int>& params=std::vector<int>()) {
			std::string::size_type dot = filename.find_last_of('.') + 1;
			std::string ext = filename.substr(dot, filename.length() - dot);
			int type=ext=="ico"?1:ext=="cur"?2:0;
			CV_Assert(type!=0);
			return _imwrite_ico(filename,img,type,params);
		}
	}
}

#endif //CVTICO_CV_ICO_H
