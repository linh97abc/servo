#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdint.h>
#include "ap_int.h"
#include "hls_stream.h"
#include <stdint.h>
#include "crc.h"

//typedef ap_uint<8> data_in_t;
//hls::stream<data_in_t> data_in_stream;  // A stream declaration

//using data_t = ap_uint<8>;

struct data_t {
	uint8_t data;
	ap_uint<1> last;
};

//using data_t = uint8_t;

using data_stream_t = hls::stream<data_t>;
using dout_stream_t = hls::stream<uint8_t>;

void mavlink_parser(data_stream_t &din, dout_stream_t &dout);


#endif
