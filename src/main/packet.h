#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>

struct PACKET_HEADER{
	uint8_t version;
	uint8_t message_type;
	uint16_t reserved;
	uint16_t payload_length;
	char sender_port[5];
	char sender_ip[15];
};

struct generic_packet{
	struct PACKET_HEADER header;
	char payload[128];
};

#endif /* PACKET_H_ */
