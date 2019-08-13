#include <iostream>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>

#include "range.h"
#include "optional.h"
#include "h264_bit_reader.h"
#include "h264_parser.h"
#include "logging.h"
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
int main()
{

    int fd=0;
    if((fd=open("1280x720.h264",O_RDONLY,S_IRGRP))<0){
        printf("open failed \n");
        return 0;
    }
    struct stat file_stat;
    if((fstat(fd,&file_stat))<0){
        printf("fstat failed \n");
        return 0;        
    }
    void *start_fp;
    if((start_fp=mmap(NULL,file_stat.st_size,PROT_READ,MAP_SHARED,fd,0))==MAP_FAILED){
        printf("mmap failed \n");
        return 0;         
    }
    int length=file_stat.st_size;
    std::cout<<"size "<<length<<std::endl;
    H264Parser parser;
    parser.SetStream((uint8_t*)start_fp,length);
    media::H264SliceHeader shdr;
    media::H264SEIMessage sei_msg;
    H264NALU nalu;
    H264Parser::Result res = parser.AdvanceToNextNALU(&nalu);
    if(res==H264Parser::kOk){
         int id;
    switch (nalu.nal_unit_type) {
      
      case H264NALU::kIDRSlice:
      case H264NALU::kNonIDRSlice:
        CHECK_EQ(parser.ParseSliceHeader(nalu, &shdr), H264Parser::kOk);
        break;

      case H264NALU::kSPS:{
        CHECK_EQ(parser.ParseSPS(&id), H264Parser::kOk);
        const H264SPS *sps=parser.GetSPS(id);
        //https://zhuanlan.zhihu.com/p/27896239
        int height=sps->pic_height_in_map_units_minus1;
        int width=sps->pic_width_in_mbs_minus1;
        DLOG(INFO)<<"h "<<(height+1)*16<<" w "<<(width+1)*16;
        DLOG(INFO)<<"ref "<<sps->max_num_ref_frames;
        DLOG(INFO)<<"sps";
        break;          
      }
      case H264NALU::kPPS:
        CHECK_EQ(parser.ParsePPS(&id), H264Parser::kOk);
        break;

      case H264NALU::kSEIMessage:
        CHECK_EQ(parser.ParseSEI(&sei_msg), H264Parser::kOk);
        break;

      default:
        // Skip unsupported NALU.
        //DVLOG(4) << "Skipping unsupported NALU";
        break;
    }         
    }else{
        std::cout<<"h264 parser error"<<std::endl;
    }
    close(fd);
  return 0;
}
