#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>

#define MSG_IP_BROADCAST 				 		0x01
#define MSG_IP_UNICAST					 		0x02
#define MSG_PASSWORD					 		0x03
#define MSG_PW_CONFIRMATION				 		0x04
#define MSG_IDENTITY_BRDCAST					0x05
#define MSG_CHALLENGE			 		 		0x06
#define MSG_CHALLENGE_RESPONSE			 		0x07
#define MSG_PASSWORD_REQUEST			 		0x08
#define MSG_PASSWORD_RESPONSE			 		0x09
#define MSG_PROFILE_LIST_RESPONSE		 		0x13
#define MSG_PROFILE_REQUEST              		0x14
#define MSG_CREATE_NEW_PROFILE_REQUEST          0x15
#define MSG_PROFILE_CHOICE_RESPONSE      		0x18
#define MSG_PROFILE_AUTHENTICATION_RESPONSE		0x19


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
