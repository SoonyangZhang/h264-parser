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
}
