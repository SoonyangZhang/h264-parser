#pragma once
#include <fstream>
namespace media{
class MediaInfo{
public:
    MediaInfo(std::string &s);
    ~MediaInfo();
    void OnInfo(int nal_type,int slice_type,int start_code_len,int nalulen);
private:
    int no_{0};
    std::fstream fd_;
};    
}
