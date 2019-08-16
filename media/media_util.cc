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
void Frame::OnData(int nalutype,int slicetype,const uint8_t *data,int off,int start_code_len){
	if(start_code_len==4){
		if(nalutype==H264NALU::kIDRSlice||nalutype==H264NALU::kNonIDRSlice){
			std::cout<<frame_no_<<" len "<<GetFrameSize()<<std::endl;
			if(sink_){
				sink_->OnFrameAvalable(h264_frame_);
			}
			std::vector<NaluInfo> null_vec;
			h264_frame_.swap(null_vec);
			frame_no_++;
		}
	}
	h264_frame_.emplace_back(nalutype,off,slicetype,data,(uint8_t)start_code_len);
}
int Frame::GetFrameSize(){
	int len=0;
	for(auto it=h264_frame_.begin();it!=h264_frame_.end();it++){
		len+=(it->start_code+it->off);
	}
	return len;
}
FrameSinkToFile::FrameSinkToFile(std::string &name){
	f_out_.open(name.c_str(),std::ios::out|std::ios::binary);
}
FrameSinkToFile::~FrameSinkToFile(){
	if(f_out_.is_open()){
		f_out_.close();
	}
}
const char start3[]={0x00,0x00,0x01};
const char start4[]={0x00,0x00,0x00,0x01};
void FrameSinkToFile::OnFrameAvalable(const std::vector<NaluInfo> &frame){
	if(f_out_.is_open()){
		for(auto it=frame.begin();it!=frame.end();it++){
			int start_code_len=it->start_code;
			if(3==start_code_len){
				f_out_.write(start3,sizeof(start3));
			}
			if(4==start_code_len){
				f_out_.write(start4,sizeof(start4));
			}
			const uint8_t *data=it->data;
			int len=it->off;
			f_out_.write((char*)data,len);
		}

	}
}
}
