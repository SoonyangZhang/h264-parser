#pragma once
#include <string>
#include <memory>
#include <utility>
#include <fstream>
#include <stdint.h>
#include <vector>
namespace media{
const char* convert_slice_string(int type);
const char* convert_nalu_string(int type);
class DataBuffer{
public:
	DataBuffer();
	DataBuffer(const DataBuffer&)=delete;
	DataBuffer &operator=(const DataBuffer&)=delete;
	DataBuffer(DataBuffer&&r){
		*this=std::move(r);
	}
	DataBuffer & operator=(DataBuffer&&r){
		data_=std::move(r.data_);
		offset_=r.offset_;
		r.offset_=0;
		len_=r.len_;
		r.len_=0;
		return (*this);
	}
	void Append(const uint8_t *ptr,int size);
	void Reset(){
		offset_=0;
	}
	uint8_t *data() const{
		return (uint8_t*)data_.get();
	}
	int Copy(char *dst,int size);
	int Capacity() const{
		return len_;
	}
	int Size() const {
		return offset_;
	}
private:
	std::unique_ptr<char[]> data_;
	int offset_{0};
	int len_{0};
};
struct NaluInfo{
NaluInfo(int type,int off,int slice,const uint8_t*data,uint8_t start):
nalutype(type),off(off),slicetype(slice),data(data),start_code(start){}
int nalutype;
int off;
int slicetype;
const uint8_t *data;
uint8_t start_code;
};
class FrameSinkInterface{
public:
	virtual void OnFrameAvalable(const std::vector<NaluInfo> &frame)=0;
	virtual ~FrameSinkInterface(){}
};

class Frame{
public:
	Frame(){}
	void RegisterSink(FrameSinkInterface*sink);
	void OnData(int nalutype,int slicetype,const uint8_t *data,int off,int start_code_len);
private:
	int GetFrameSize();
	FrameSinkInterface *sink_{nullptr};
	//DataBuffer buffer_;
	std::vector<NaluInfo> h264_frame_;
	int frame_no_{1};
};
class FrameSinkToFile:public FrameSinkInterface{
public:
	FrameSinkToFile(std::string &name);
	~FrameSinkToFile();
	void OnFrameAvalable(const std::vector<NaluInfo> &frame) override;
private:
	std::fstream f_out_;
};
}
