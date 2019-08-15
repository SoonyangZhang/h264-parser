#pragma once
#include <string>
#include <memory>
#include <utility>
#include <fstream>
#include <stdint.h>
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
class FrameSinkInterface{
public:
	virtual void OnDataAvalable(const char *data,int size)=0;
	virtual ~FrameSinkInterface(){}
};
class Frame{
public:
	Frame(){}
	void RegisterSink(FrameSinkInterface*sink);
	void OnData(int nalutype,const uint8_t *data,int off,int start_code_len);
private:
	FrameSinkInterface *sink_{nullptr};
	DataBuffer buffer_;
	int frame_no_{1};
};
class FrameSinkToFile:public FrameSinkInterface{
public:
	FrameSinkToFile(std::string &name);
	~FrameSinkToFile();
	void OnDataAvalable(const char *data,int size) override;
private:
	std::fstream f_out_;
};
}
