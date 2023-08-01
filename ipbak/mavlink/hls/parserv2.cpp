#include "ap_int.h"
#include "hls_stream.h"
#include <stdint.h>
#include "crc.h"

#include "mavlink_parser.h"


#if 1


enum class ParserResult
{
	IDLE,
	PARSING,
	HAS_FRAME,
	CRC_ERR

};

enum class ParserState {
	INIT,
	LEN,
	SEQ,
	SYSID,
	COMID,
	MSGID,
	PAYLOAD,
	CHECKSUM_L,
	CHECKSUM_H
};

struct MavlinkParserData {
	ParserState parserState;
	uint16_t crc_expected;
	unsigned read_cnt;
	uint16_t crc_l;
	uint16_t crc_h;

	uint8_t payload_len;

	MavlinkParserData() {
		parserState = ParserState::INIT;
		crc_expected = X25_INIT_CRC;
		read_cnt = 0;
	}
};

ParserResult Parser_step(uint8_t d, MavlinkParserData &parserData)
{
	ParserResult ret = ParserResult::PARSING;

	if ((parserData.parserState != ParserState::INIT)
			&& (parserData.parserState != ParserState::CHECKSUM_H)
			&& (parserData.parserState != ParserState::CHECKSUM_L))
	{
		parserData.crc_expected = crc_accumulate(d, parserData.crc_expected);
	}


	switch(parserData.parserState) {
		case ParserState::INIT:
			if (d == 0xFE) {
				parserData.crc_expected = X25_INIT_CRC;
				parserData.parserState = ParserState::LEN;
			}
			else
			{
				ret = ParserResult::IDLE;
			}
			break;
		case ParserState::LEN:
			parserData.payload_len = d;
			parserData.parserState = ParserState::SEQ;
			break;
		case ParserState::SEQ:
			parserData.parserState = ParserState::SYSID;
			break;
		case ParserState::SYSID:
			parserData.parserState = ParserState::COMID;
			break;
		case ParserState::COMID:
			parserData.parserState = ParserState::MSGID;
			break;
		case ParserState::MSGID:
			parserData.read_cnt = 0;
			parserData.parserState = (parserData.payload_len == 0)? ParserState::CHECKSUM_L:  ParserState::PAYLOAD;
			break;
		case ParserState::PAYLOAD:
			parserData.read_cnt++;
			if (parserData.read_cnt >= parserData.payload_len) {
				parserData.parserState = ParserState::CHECKSUM_L;

			}
			break;
		case ParserState::CHECKSUM_L:
			parserData.crc_l = d;
			parserData.parserState = ParserState::CHECKSUM_H;

			break;

		case ParserState::CHECKSUM_H:
			parserData.crc_h = d;
			uint16_t crc_actual = (parserData.crc_h << 8) | parserData.crc_l;
			parserData.parserState = ParserState::INIT;

			if (crc_actual == parserData.crc_expected)
			{
				ret = ParserResult::HAS_FRAME;
			}
			else
			{
				ret = ParserResult::CRC_ERR;
			}

			break;

//		default:
//			parserData.parserState = ParserState::INIT;
//			break;
	}

	return ret;
}



void mavlink_parser(data_stream_t &din, dout_stream_t &dout)
{
#pragma HLS INTERFACE axis  port=din
#pragma HLS INTERFACE axis  port=dout
#pragma HLS PIPELINE off

	MavlinkParserData parserData;
	data_t d_in;
	uint8_t d;
	ParserResult parserRet;

	uint8_t buff[512];

	ap_uint<9> buff_head = 0;
	ap_uint<9> buff_tail = 0;
	ap_uint<9> buff_stx = 0;

	parser_loop: do {
		if (buff_head == buff_tail)
		{
			d_in = din.read();

			if (d_in.last) {
				return;
			}

			d = d_in.data;

			buff[buff_head] = d;
			buff_head++;
			buff_tail++;
		} else {
			d = buff[buff_tail];
			buff_tail++;
		}


		parserRet = Parser_step(d, parserData);

		if (parserRet  == ParserResult::HAS_FRAME)
		{
			buff_tail = buff_stx;

			export_frame_loop: for (unsigned i = 0; i < parserData.payload_len + 5; i++) {
				buff_tail++;
				uint8_t dout_tmp = buff[buff_tail];
				dout.write(dout_tmp);
			}

			buff_tail++;
			buff_tail++;
			buff_tail++;

			buff_stx = buff_tail;
		}
		else if (parserRet  == ParserResult::CRC_ERR)
		{
			if (buff_stx != buff_head)
			{
				buff_stx++;
				buff_tail = buff_stx;
			}
		}
		else if (parserRet == ParserResult::IDLE)
		{
			if (buff_stx != buff_head)
			{
				buff_stx++;
			}
		}
		else {

		}
	} while (1);

}

#endif
