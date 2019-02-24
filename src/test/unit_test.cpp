#include "unit_test.h"

int main(){

	is_test = true;
	test_correctness_of_ip_info_packet_creation();
	return 0;
}

void test_correctness_of_ip_info_packet_creation(){

	struct generic_packet* ip_info_packet = (struct generic_packet*)malloc(sizeof(struct generic_packet));
	char* test_ip = (char*)"192.168.0.1";

	ip_info_packet->header.version = 9;
	ip_info_packet->header.message_type = 0x02;
	ip_info_packet->header.reserved = 0x0000;
	ip_info_packet->header.payload_length = htons(sizeof("OWCH_IoT_DEVICE"));
	memset(ip_info_packet->header.sender_ip, '\0', 15);
	strcpy(ip_info_packet->header.sender_ip, test_ip);
	sprintf(ip_info_packet->header.sender_port,"%d", 6346);
	strcpy(ip_info_packet->payload, "OWCH_IoT_DEVICE");

	char *correct_ip_info_packet = (char*) ip_info_packet;

	char* ip_info_packet_to_test = (char *)create_ip_info_packet();
	int packet_length = (int)sizeof(PACKET_HEADER) + (int)sizeof(DEVICE_NAME);
	int compare_value = memcmp(correct_ip_info_packet, ip_info_packet_to_test, packet_length);
	if(compare_value == 0)
		printf("Test passed\n");
	else
		printf("Test Failed\n");
	if(DEBUG_LEVEL > 3){
		int i;
		for(i=0;i<packet_length;i++)
			printf("%d ",correct_ip_info_packet[i]);
		printf("\n");
		for(i=0;i<packet_length;i++)
				printf("%d ",ip_info_packet_to_test[i]);
		printf("\n");
	}

}
