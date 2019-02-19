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

struct generic_packet* create_pw_request_packet(){
	struct generic_packet *pw_request_packet = (struct generic_packet*)malloc(sizeof(struct generic_packet));

	pw_request_packet->header.version = PROTOCOL_VERSION;
	pw_request_packet->header.message_type = MSG_PASSWORD_REQUEST;
	pw_request_packet->header.reserved = RESERVED;
	pw_request_packet->header.payload_length = htons(sizeof(0));
	memset(pw_request_packet->header.sender_ip, '\0', 15);
	strcpy(pw_request_packet->header.sender_ip, get_own_ipadress());
	sprintf(pw_request_packet->header.sender_port,"%d", PORTNUM);

	return pw_request_packet;
}

struct generic_packet* create_profile_list_packet(char profile_list[1024][11], int total_count){
	struct generic_packet *profile_list_packet = (struct generic_packet*)malloc(sizeof(struct generic_packet));
	//printf("Total Count Received: %d\n",total_count);
	profile_list_packet->header.version = PROTOCOL_VERSION;
	profile_list_packet->header.message_type = MSG_PROFILE_LIST_RESPONSE;
	profile_list_packet->header.reserved = RESERVED;
	profile_list_packet->header.payload_length = htons(((total_count)*11));
	memset(profile_list_packet->header.sender_ip, '\0', 15);
	strcpy(profile_list_packet->header.sender_ip, get_own_ipadress());
	sprintf(profile_list_packet->header.sender_port,"%d", PORTNUM);
	for(int i = 0; i<total_count; i++){
		memset(profile_list_packet->payload+(i*11), '\0', 11);
		strcpy(profile_list_packet->payload+(i*11), profile_list[i]);
	}
	//printf("Payload: %s\n",profile_list_packet->payload);
	return profile_list_packet;
}

struct generic_packet* create_profile_authetication_response_packet(int result){
	struct generic_packet *profile_authetication_response_packet = (struct generic_packet*)malloc(sizeof(struct generic_packet));
	profile_authetication_response_packet->header.version = PROTOCOL_VERSION;
	profile_authetication_response_packet->header.message_type = MSG_PROFILE_AUTHENTICATION_RESPONSE;
	profile_authetication_response_packet->header.reserved = RESERVED;
	profile_authetication_response_packet->header.payload_length = htons(sizeof(AUTHENTICATION_SUCCESS));
	memset(profile_authetication_response_packet->header.sender_ip, '\0', 15);
	strcpy(profile_authetication_response_packet->header.sender_ip, get_own_ipadress());
	sprintf(profile_authetication_response_packet->header.sender_port,"%d", PORTNUM);
	if(result == 1)
		strcpy(profile_authetication_response_packet->payload, AUTHENTICATION_SUCCESS);
	else
		strcpy(profile_authetication_response_packet->payload, AUTHENTICATION_FAIL);
	return profile_authetication_response_packet;
}

