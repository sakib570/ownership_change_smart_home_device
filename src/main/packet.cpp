#include "packet.h"
#include "device_info.h"

struct generic_packet* create_ip_info_packet(){
	struct generic_packet *ip_info_packet = (struct generic_packet*)malloc(sizeof(struct generic_packet));

	ip_info_packet->header.version = PROTOCOL_VERSION;
	ip_info_packet->header.message_type = MSG_IP_UNICAST;
	ip_info_packet->header.reserved = RESERVED;
	ip_info_packet->header.payload_length = htons(sizeof(DEVICE_NAME));
	memset(ip_info_packet->header.sender_ip, '\0', 15);
	strcpy(ip_info_packet->header.sender_ip, get_own_ipadress());
	sprintf(ip_info_packet->header.sender_port,"%d", PORTNUM);
	strcpy(ip_info_packet->payload, DEVICE_NAME);

	return ip_info_packet;
}

struct generic_packet* create_pw_confirmation_packet(){
	struct generic_packet *pw_confirmation_packet = (struct generic_packet*)malloc(sizeof(struct generic_packet));

	pw_confirmation_packet->header.version = PROTOCOL_VERSION;
	pw_confirmation_packet->header.message_type = MSG_PW_CONFIRMATION;
	pw_confirmation_packet->header.reserved = RESERVED;
	pw_confirmation_packet->header.payload_length = htons(sizeof(PW_CONFIRMATION_CODE));
	memset(pw_confirmation_packet->header.sender_ip, '\0', 15);
	strcpy(pw_confirmation_packet->header.sender_ip, get_own_ipadress());
	sprintf(pw_confirmation_packet->header.sender_port,"%d", PORTNUM);
	strcpy(pw_confirmation_packet->payload, PW_CONFIRMATION_CODE);

	return pw_confirmation_packet;
}

struct generic_packet* create_challenge_packet(){
	struct generic_packet *challenge_packet = (struct generic_packet*)malloc(sizeof(struct generic_packet));

	challenge_packet->header.version = PROTOCOL_VERSION;
	challenge_packet->header.message_type = MSG_CHALLENGE;
	challenge_packet->header.reserved = RESERVED;
	challenge_packet->header.payload_length = htons(sizeof(CHALLENGE_REQUEST_CODE));
	memset(challenge_packet->header.sender_ip, '\0', 15);
	strcpy(challenge_packet->header.sender_ip, get_own_ipadress());
	sprintf(challenge_packet->header.sender_port,"%d", PORTNUM);
	strcpy(challenge_packet->payload, CHALLENGE_REQUEST_CODE);

	return challenge_packet;
}
