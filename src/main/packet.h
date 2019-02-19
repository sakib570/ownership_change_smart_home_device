#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>

#define PORTNUM							 		6346
#define PROTOCOL_VERSION		   				   9
#define RESERVED					   		  0x0000

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

#define PW_CONFIRMATION_CODE           		"0xF9F9"
#define CHALLENGE_REQUEST_CODE	  	   		"0x1391"
#define CHALLENGE_RESPONSE_CODE	  	   		"0xE7B7"
#define AUTHENTICATION_SUCCESS           	  "0x01"
#define AUTHENTICATION_FAIL              	  "0x02"

#define DEVICE_NAME			  	   "OWCH_IoT_DEVICE"



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

struct generic_packet* create_ip_info_packet();
struct generic_packet* create_pw_confirmation_packet();
struct generic_packet* create_challenge_packet();
struct generic_packet* create_pw_request_packet();
struct generic_packet* create_profile_list_packet(char profile_list[1024][11], int total_count);
struct generic_packet* create_profile_authetication_response_packet(int result);

#endif /* PACKET_H_ */
