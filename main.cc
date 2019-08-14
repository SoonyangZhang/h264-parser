#include <iostream>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>

#include <string>
#include "range.h"
#include "optional.h"
#include "h264_bit_reader.h"
#include "h264_parser.h"
#include "logging.h"
#include "media_util.h"
#include "media_info.h"
using namespace std;
using namespace media;
/*
  nonstd::optional<std::string> o1;
  nonstd::optional<std::string> o2("Hello, world!");

  std::cout << std::boolalpha
            << o1.has_value() << std::endl      // Prints "false"
            << o2.has_value() << std::endl      // Prints "true"
            << o1.value_or("Foo") << std::endl  // Prints "Foo"
            << o2.value_or("Foo") << std::endl; // Prints "Hello, world!"
*/
//https://cs.chromium.org/chromium/src/media/video/h264_parser_unittest.cc
////https://zhuanlan.zhihu.com/p/27896239   sps pps
void print_sps_info(const H264SPS *sps,int len){
    int height=sps->pic_height_in_map_units_minus1;
    int width=sps->pic_width_in_mbs_minus1;
    std::cout<<"h "<<(height+1)*16<<" w "<<(width+1)*16<<std::endl;
    std::cout<<"ref "<<sps->max_num_ref_frames<<" "<<len<<std::endl; 
}

void printf_slice_info(media::H264SliceHeader &slice,int len){
    std::cout<<slice.frame_num<<" slice "<<convert_slice_string(slice.slice_type)<<" "<<len<<std::endl;
}
int main()
{
	char *h264_in="1280x720.h264";
	char *info_out="1280x720";
    int fd=0;
    if((fd=open(h264_in,O_RDONLY,S_IRGRP))<0){
        printf("open failed \n");
        return 0;
    }
    struct stat file_stat;
    if((fstat(fd,&file_stat))<0){
        printf("fstat failed \n");
        return 0;        
    }
//https://www.cnblogs.com/matthew-2013/p/4675366.html  S_IRWXU
// http://joe.is-programmer.com/posts/17463.html O_RDWR|O_CREAT
    void *start_fp;
    if((start_fp=mmap(NULL,file_stat.st_size,PROT_READ,MAP_SHARED,fd,0))==MAP_FAILED){
        printf("mmap failed \n");
        return 0;         
    }
    std::string name=std::string(info_out);
    MediaInfo record(name);
    int length=file_stat.st_size;
    std::cout<<"size "<<length<<std::endl;
    H264Parser parser;
    parser.SetStream((uint8_t*)start_fp,length);
    int id;
    int f=0;
    int total_to_parse=10;
    int parse_len=0;
    while(f<total_to_parse){
        media::H264SliceHeader shdr;
        media::H264SEIMessage sei_msg;
        H264NALU nalu;
        int len=0;
        int start_code_len=0;
        H264Parser::Result res = parser.AdvanceToNextNALU(&nalu);
        if(res==H264Parser::kOk){
        len=length-parser.get_offset()-parse_len;
        parse_len+=len;
        start_code_len=len-nalu.size;
        if(nalu.nal_unit_type==H264NALU::kIDRSlice||nalu.nal_unit_type==H264NALU::kNonIDRSlice){

        }else{
        	record.OnInfo(nalu.nal_unit_type,0,start_code_len,len);
        }
        switch (nalu.nal_unit_type){     
        case H264NALU::kIDRSlice:
        case H264NALU::kNonIDRSlice:{
            CHECK_EQ(parser.ParseSliceHeader(nalu, &shdr), H264Parser::kOk);
            printf_slice_info(shdr,len);
            record.OnInfo(nalu.nal_unit_type,shdr.slice_type,start_code_len,len);
            break;            
        }   
        case H264NALU::kSPS:{
            CHECK_EQ(parser.ParseSPS(&id), H264Parser::kOk);
            const H264SPS *sps=parser.GetSPS(id);
            print_sps_info(sps,len);
            break;          
        }
        case H264NALU::kPPS:{
            CHECK_EQ(parser.ParsePPS(&id), H264Parser::kOk);
            //int len=length-parser.get_offset()-parse_len;
            //parse_len+=len;
            break;            
        }  
        case H264NALU::kSEIMessage:{
            //len=length-parser.get_offset()-parse_len;
            //parse_len+=len;
            CHECK_EQ(parser.ParseSEI(&sei_msg), H264Parser::kOk);
            break;            
        }
        default:{
            // Skip unsupported NALU.
            DLOG(INFO)<< "Skipping unsupported NALU";
            //int len=length-parser.get_offset()-parse_len;
            //parse_len+=len;
            break;            
        }
        }   
        f++;
        }else{
            std::cout<<"h264 parser error"<<std::endl;
            break;
        }        
    }
    close(fd);
  return 0;
}
