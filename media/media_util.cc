#include <memory.h>
#include <iostream>
#include "media_util.h"
#include "h264_bit_reader.h"
#include "h264_parser.h"
namespace media{
const char* convert_slice_string(int type){
    switch(type%5){
        case H264SliceHeader::kPSlice:{
            return "PSlice";
        }
        case H264SliceHeader::kBSlice:{
            return "BSlice";
        }
        case H264SliceHeader::kISlice:{
            return "ISlice";
        }
        case H264SliceHeader::kSPSlice:{
            return "SPSlice";
        }
        case H264SliceHeader::kSISlice:{
            return "SISlice";
        }
        default:{
            return "NoneSlice";
        }
    }
}
const char* convert_nalu_string(int type){
	switch(type){
	case H264NALU::kIDRSlice:{
		return "IDR";
	}
	case H264NALU::kNonIDRSlice:{
		return "NonIDR";
	}
	case H264NALU::kSPS:{
		return "SPS";
	}
	case H264NALU::kPPS:{
		return "PPS";
	}
	case H264NALU::kSEIMessage:{
		return "SEI";
	}
	default:{
		return "NoneNalu";
	}
	}
}
const int kAllocSize=32;
static int NeedBufferSize(int size){
	int remain=size%kAllocSize;
	int devide=size/kAllocSize;
	int alloc=(remain+devide)*kAllocSize;
	return alloc;
}
DataBuffer::DataBuffer(){
	data_.reset(new char[kAllocSize]);
	offset_=0;
	len_=kAllocSize;
}
void DataBuffer::Append(const uint8_t *ptr,int size){
	if(len_==0){
		int alloc=NeedBufferSize(size);
		data_.reset(new char [alloc]);
		len_=alloc;
	}
	int writale=len_-offset_;
	if(writale<size){
		int extend=len_>size?len_:size;
		int alloc=NeedBufferSize(extend);
		extend=2*alloc;
		char *buf=new char[extend];
		if(offset_>0){
			memcpy(buf,data_.get(),offset_);
		}
		data_.reset(buf);
		len_=extend;
	}
	memcpy(data_.get()+offset_,ptr,size);
	offset_+=size;
}
int DataBuffer::Copy(char *dst,int size){
	int read=0;
	read=offset_>size?size:offset_;
	if(read){
		memcpy(dst,data_.get(),read);
	}
	return read;
}
void Frame::RegisterSink(FrameSinkInterface*sink){
	sink_=sink;
}
const uint8_t start3[]={0x00,0x00,0x01};
const uint8_t start4[]={0x00,0x00,0x00,0x01};
void Frame::OnData(int nalutype,const uint8_t *data,int off,int start_code_len){
	if(start_code_len==4){
		if(nalutype==H264NALU::kIDRSlice||nalutype==H264NALU::kNonIDRSlice){
			std::cout<<frame_no_<<" "<<buffer_.Size()<<std::endl;
			if(sink_){
				sink_->OnDataAvalable((char*)buffer_.data(),buffer_.Size());
			}
			buffer_.Reset();
			frame_no_++;
		}
	}
	if(start_code_len==3){
		buffer_.Append(start3,sizeof(start3));
	}
	if(start_code_len==4){
		buffer_.Append(start4,sizeof(start4));
	}
	buffer_.Append(data,off);
}
FrameSinkToFile::FrameSinkToFile(std::string &name){
	f_out_.open(name.c_str(),std::ios::out|std::ios::binary);
}
FrameSinkToFile::~FrameSinkToFile(){
	if(f_out_.is_open()){
		f_out_.close();
	}
}
void FrameSinkToFile::OnDataAvalable(const char *data,int size){
	if(f_out_.is_open()){
		f_out_.write(data,size);
	}
}
}
