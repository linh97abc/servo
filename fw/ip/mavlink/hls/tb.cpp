//#include <gen_parser/src/mavlink_parser.h>
#include "mavlink_parser.h"

int main()
{
	hls::stream<data_t, 4096> din;
	dout_stream_t dout;

	for (unsigned i = 0; i < 100; i++) {
		din.write({0xFE, 0});
		din.write({0x02, 0});
		din.write({0x01, 0});
		din.write({0x03, 0});
		din.write({0x04, 0});
		din.write({0x05, 0});

	//	din.write(0xfe);
	//	din.write(0xf0);
	//	din.write(21);
	//	din.write(7);

		din.write({0xFE, 0});
		din.write({0x03, 0});
		din.write({0x04, 0});
		din.write({0x05, 0});
		din.write({0x06, 0});
		din.write({0x07, 0});
		din.write({0xFE, 0});
		din.write({0xF0, 0});
		din.write({0x05, 0});

		din.write({158, 0});
		din.write({86, 0});

	}


	din.write({86, 1});

	mavlink_parser(din, dout);

	return 0;
}
