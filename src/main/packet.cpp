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

