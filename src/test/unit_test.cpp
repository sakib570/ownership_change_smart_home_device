#include "unit_test.h"

int main(){

	is_test = true;
	test_correctness_of_ip_info_packet_creation();
	test_correctness_of_pw_confirmation_packet_creation();
	test_correctness_of_challenge_packet_creation();
	test_correctness_of_pw_request_packet_creation();
	test_correctness_of_profile_list_packet_creation();
	test_correctness_of_profile_authetication_response_packet_creation();
	test_condition_correctness_of_ready_to_send_challenge();
	test_condition_correctness_of_ready_to_send_profile_list();
	test_correctness_of_owner_password_retrieval_from_file();
	return 0;
}

void test_correctness_of_ip_info_packet_creation(void){

	struct generic_packet* ip_info_packet = (struct generic_packet*)malloc(sizeof(struct generic_packet));
	char* test_ip = (char*)"192.168.0.1";

	printf("-> Testing correctness of ip_info_packet creation...\n");

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
	printf("\t->Test Result: ");
	if(compare_value == 0)
		printf("Passed\n\n");
	else
		printf("Failed\n\n");
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

void test_correctness_of_pw_confirmation_packet_creation(void){

	struct generic_packet* pw_confirmation_packet = (struct generic_packet*)malloc(sizeof(struct generic_packet));
	char* test_ip = (char*)"192.168.0.1";

	printf("-> Testing correctness of pw_confirmation_packet creation...\n");

	pw_confirmation_packet->header.version = 9;
	pw_confirmation_packet->header.message_type = 0x04;
	pw_confirmation_packet->header.reserved = 0x0000;
	pw_confirmation_packet->header.payload_length = htons(sizeof("0xF9F9"));
	memset(pw_confirmation_packet->header.sender_ip, '\0', 15);
	strcpy(pw_confirmation_packet->header.sender_ip, test_ip);
	sprintf(pw_confirmation_packet->header.sender_port,"%d", 6346);
	strcpy(pw_confirmation_packet->payload, "0xF9F9");

	char *correct_pw_confirmation_packet = (char*) pw_confirmation_packet;

	char* pw_confirmation_packet_to_test = (char *)create_pw_confirmation_packet();
	int packet_length = (int)sizeof(PACKET_HEADER) + (int)sizeof(PW_CONFIRMATION_CODE);
	int compare_value = memcmp(correct_pw_confirmation_packet, pw_confirmation_packet_to_test, packet_length);
	printf("\t->Test Result: ");
	if(compare_value == 0)
		printf("Passed\n\n");
	else
		printf("Failed\n\n");
	if(DEBUG_LEVEL > 3){
		int i;
		for(i=0;i<packet_length;i++)
			printf("%d ",correct_pw_confirmation_packet[i]);
		printf("\n");
		for(i=0;i<packet_length;i++)
				printf("%d ",pw_confirmation_packet_to_test[i]);
		printf("\n");
	}

}

void test_correctness_of_challenge_packet_creation(void){

	struct generic_packet* challenge_packet = (struct generic_packet*)malloc(sizeof(struct generic_packet));
	char* test_ip = (char*)"192.168.0.1";

	printf("-> Testing correctness of challenge_packet creation...\n");

	challenge_packet->header.version = 9;
	challenge_packet->header.message_type = 0x06;
	challenge_packet->header.reserved = 0x0000;
	challenge_packet->header.payload_length = htons(sizeof("0x1391"));
	memset(challenge_packet->header.sender_ip, '\0', 15);
	strcpy(challenge_packet->header.sender_ip, test_ip);
	sprintf(challenge_packet->header.sender_port,"%d", 6346);
	strcpy(challenge_packet->payload, "0x1391");

	char *correct_challenge_packet = (char*) challenge_packet;

	char* challenge_packet_to_test = (char *)create_challenge_packet();
	int packet_length = (int)sizeof(PACKET_HEADER) + (int)sizeof(CHALLENGE_REQUEST_CODE);
	int compare_value = memcmp(correct_challenge_packet, challenge_packet_to_test, packet_length);
	printf("\t->Test Result: ");
	if(compare_value == 0)
		printf("Passed\n\n");
	else
		printf("Failed\n\n");
	if(DEBUG_LEVEL > 3){
		int i;
		for(i=0;i<packet_length;i++)
			printf("%d ",correct_challenge_packet[i]);
		printf("\n");
		for(i=0;i<packet_length;i++)
				printf("%d ",challenge_packet_to_test[i]);
		printf("\n");
	}

}

void test_correctness_of_pw_request_packet_creation(void){

	struct generic_packet* pw_request_packet = (struct generic_packet*)malloc(sizeof(struct generic_packet));
	char* test_ip = (char*)"192.168.0.1";

	printf("-> Testing correctness of pw_request_packet creation...\n");

	pw_request_packet->header.version = 9;
	pw_request_packet->header.message_type = 0x08;
	pw_request_packet->header.reserved = 0x0000;
	pw_request_packet->header.payload_length = htons(sizeof(0));
	memset(pw_request_packet->header.sender_ip, '\0', 15);
	strcpy(pw_request_packet->header.sender_ip, test_ip);
	sprintf(pw_request_packet->header.sender_port,"%d", 6346);

	char *correct_pw_request_packet = (char*) pw_request_packet;

	char* pw_request_packet_to_test = (char *)create_pw_request_packet();
	int packet_length = (int)sizeof(PACKET_HEADER);
	int compare_value = memcmp(correct_pw_request_packet, pw_request_packet_to_test, packet_length);
	printf("\t->Test Result: ");
	if(compare_value == 0)
		printf("Passed\n\n");
	else
		printf("Failed\n\n");
	if(DEBUG_LEVEL > 3){
		int i;
		for(i=0;i<packet_length;i++)
			printf("%d ",correct_pw_request_packet[i]);
		printf("\n");
		for(i=0;i<packet_length;i++)
				printf("%d ",pw_request_packet_to_test[i]);
		printf("\n");
	}

}

void test_correctness_of_profile_list_packet_creation(void){

	struct generic_packet* profile_list_packet = (struct generic_packet*)malloc(sizeof(struct generic_packet));
	char* test_ip = (char*)"192.168.0.1";
	char mock_profile_list[1024][11];

	printf("-> Testing correctness of profile_list_packet creation...\n");

	strcpy(mock_profile_list[1], "Profile 1");
	strcpy(mock_profile_list[1], "Profile 2");
	int number_of_mock_profiles = 2;

	profile_list_packet->header.version = 9;
	profile_list_packet->header.message_type = 0x13;
	profile_list_packet->header.reserved = 0x0000;
	profile_list_packet->header.payload_length = htons(number_of_mock_profiles*11);
	memset(profile_list_packet->header.sender_ip, '\0', 15);
	strcpy(profile_list_packet->header.sender_ip, test_ip);
	sprintf(profile_list_packet->header.sender_port,"%d", 6346);

	for(int i = 0; i<number_of_mock_profiles; i++){
		memset(profile_list_packet->payload+(i*11), '\0', 11);
		strcpy(profile_list_packet->payload+(i*11), mock_profile_list[i]);
	}

	char *correct_profile_list_packet = (char*) profile_list_packet;

	char* profile_list_packet_to_test = (char *)create_profile_list_packet(mock_profile_list, number_of_mock_profiles);
	int packet_length = (int)sizeof(PACKET_HEADER) + (int)(number_of_mock_profiles*11);
	int compare_value = memcmp(correct_profile_list_packet, profile_list_packet_to_test, packet_length);
	printf("\t->Test Result: ");
	if(compare_value == 0)
		printf("Passed\n\n");
	else
		printf("Failed\n\n");
	if(DEBUG_LEVEL > 3){
		int i;
		for(i=0;i<packet_length;i++)
			printf("%d ",correct_profile_list_packet[i]);
		printf("\n");
		for(i=0;i<packet_length;i++)
				printf("%d ",profile_list_packet_to_test[i]);
		printf("\n");
	}

}

void test_correctness_of_profile_authetication_response_packet_creation(void){

	struct generic_packet* profile_authetication_response_packet = (struct generic_packet*)malloc(sizeof(struct generic_packet));
	char* test_ip = (char*)"192.168.0.1";

	printf("-> Testing correctness of profile_authentication_response_packet creation...\n");

	profile_authetication_response_packet->header.version = 9;
	profile_authetication_response_packet->header.message_type = 0x19;
	profile_authetication_response_packet->header.reserved = 0x0000;
	profile_authetication_response_packet->header.payload_length = htons(sizeof("0x01"));
	memset(profile_authetication_response_packet->header.sender_ip, '\0', 15);
	strcpy(profile_authetication_response_packet->header.sender_ip, test_ip);
	sprintf(profile_authetication_response_packet->header.sender_port,"%d", 6346);
	strcpy(profile_authetication_response_packet->payload, "0x01");

	char *correct_profile_authetication_response_packet = (char*) profile_authetication_response_packet;

	char* profile_authetication_response_packet_to_test = (char *)create_profile_authetication_response_packet(1);
	int packet_length = (int)sizeof(PACKET_HEADER) + (int)sizeof(AUTHENTICATION_SUCCESS);
	int compare_value = memcmp(correct_profile_authetication_response_packet, profile_authetication_response_packet_to_test, packet_length);
	printf("\t->Test Result: ");
	if(compare_value == 0)
		printf("Passed\n\n");
	else
		printf("Failed\n\n");
	if(DEBUG_LEVEL > 3){
		int i;
		for(i=0;i<packet_length;i++)
			printf("%d ",correct_profile_authetication_response_packet[i]);
		printf("\n");
		for(i=0;i<packet_length;i++)
				printf("%d ",profile_authetication_response_packet_to_test[i]);
		printf("\n");
	}

}

void test_condition_correctness_of_ready_to_send_challenge(void){

	printf("-> Testing condition correctness of ready_send_challenge() function...\n");
	extern bool is_master_device_found, is_master_device_info_updated;
	int test_pass_count= 0;

	/* if both booleans are true the function should return true */
	is_master_device_found = true;
	is_master_device_info_updated = true;
	if(ready_to_send_challege())
		test_pass_count++;

	/* if either of the booleans is false function should return false */
	is_master_device_found = false;
	is_master_device_info_updated = true;
	if(!ready_to_send_challege())
		test_pass_count++;

	/* if either of the booleans is false function should return false */
	is_master_device_found = true;
	is_master_device_info_updated = false;
	if(!ready_to_send_challege())
		test_pass_count++;

	/* if both booleans are false false function should return false */
	is_master_device_found = false;
	is_master_device_info_updated = false;
	if(!ready_to_send_challege())
		test_pass_count++;
	printf("\t->Test Result: ");
	if(test_pass_count == 4)
		printf("Passed\n\n");
	else
		printf("Failed\n\n");
}

void test_condition_correctness_of_ready_to_send_profile_list(void){

	printf("-> Testing condition correctness of ready_send_profile_list() function...\n");
	extern bool is_master_device_found, is_master_device_info_updated, is_new_control_device_found;
	int test_pass_count= 0;

	/* if is_new_control device is true and other two are false the function should return true */
	is_master_device_found = false;
	is_master_device_info_updated = false;
	is_new_control_device_found = true;
	if(ready_to_send_profile_list())
		test_pass_count++;

	/* for all other combination of the boolean values the function should return false */
	is_master_device_found = true;
	is_master_device_info_updated = true;
	is_new_control_device_found = true;
	if(!ready_to_send_profile_list())
		test_pass_count++;

	/* for all other combination of the boolean values the function should return false */
	is_master_device_found = true;
	is_master_device_info_updated = false;
	is_new_control_device_found = true;
	if(!ready_to_send_profile_list())
		test_pass_count++;

	/* for all other combination of the boolean values the function should return false */
	is_master_device_found = true;
	is_master_device_info_updated = false;
	is_new_control_device_found = false;
	if(!ready_to_send_profile_list())
		test_pass_count++;

	/* for all other combination of the boolean values the function should return false */
	is_master_device_found = true;
	is_master_device_info_updated = true;
	is_new_control_device_found = false;
	if(!ready_to_send_profile_list())
		test_pass_count++;

	/* for all other combination of the boolean values the function should return false */
	is_master_device_found = false;
	is_master_device_info_updated = true;
	is_new_control_device_found = false;
	if(!ready_to_send_profile_list())
		test_pass_count++;

	/* for all other combination of the boolean values the function should return false */
	is_master_device_found = false;
	is_master_device_info_updated = true;
	is_new_control_device_found = true;
	if(!ready_to_send_profile_list())
		test_pass_count++;

	/* for all other combination of the boolean values the function should return false */
	is_master_device_found = false;
	is_master_device_info_updated = false;
	is_new_control_device_found = false;
	if(!ready_to_send_profile_list())
		test_pass_count++;

	printf("\t->Test Result: ");
	if(test_pass_count == 8)
		printf("Passed\n\n");
	else
		printf("Failed\n\n");
}

void test_correctness_of_owner_password_retrieval_from_file(void){

	profile_info *mock_profile = (profile_info*)malloc(sizeof(profile_info));
	memset(mock_profile, '\0', sizeof(profile_info));
	memcpy(mock_profile->passsword, "123456", sizeof("123456"));
	FILE *fp = fopen("Password.txt", "w");

	printf("-> Testing condition correctness owner password retrieval from file...\n");

	if (fp == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	fprintf(fp, "%s", mock_profile->passsword);
	fclose(fp);

	char retrieved_password[BUFLEN];
	get_owner_password(retrieved_password);

	printf("\t->Test Result: ");
	if(memcmp(retrieved_password, mock_profile->passsword, sizeof("123456")) == 0)
		printf("Passed\n\n");
	else
		printf("Failed\n\n");
}

