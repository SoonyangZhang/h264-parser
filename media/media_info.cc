#include <string.h>
#include <stdio.h>
#include "media_info.h"
#include "media_util.h"
#include "h264_bit_reader.h"
#include "h264_parser.h"
namespace media{
MediaInfo::MediaInfo(std::string &s){
    std::string name=s+"_info.txt";
    fd_.open(name.c_str(),std::fstream::out);
}
MediaInfo::~MediaInfo(){
	if(fd_.is_open()){
		fd_.close();
	}
}
void MediaInfo::OnInfo(int nal_type,int slice_type,int start_code_len,int nalulen){
	no_++;
	if(fd_.is_open()){
		char line [512];
		memset(line,0,512);
		if(nal_type==H264NALU::kIDRSlice||nal_type==H264NALU::kNonIDRSlice){
			sprintf (line, "%d %d %d %s %s",no_,start_code_len,nalulen,
					convert_nalu_string(nal_type),convert_slice_string(slice_type));
		}else{
			sprintf (line, "%d %d %d %s %s",no_,start_code_len,nalulen,
					convert_nalu_string(nal_type),convert_nalu_string(nal_type));
		}
		fd_<<line<<std::endl;
	}
}
}
