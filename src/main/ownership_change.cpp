#include "ownership_change.h"
#include "packet.h"
#include "device_info.h"

char buf[BUFLEN], ssid_buf[BUFLEN];
char active_profile[11];;
char socket_buf[BUFLEN];
char *changed_context;
char known_context[MAX_ARRAY_SIZE][BUFLEN];
char profile_list[BUFLEN][MAX_PROFILE_NAME_SIZE];
pthread_t device_info_thread, server_thread, check_ssid_thread;
pthread_t bt_device_search_thread;
int sockfd, send_sockfd, counter = 0;
int max_rsp, num_rsp;
int dev_id, sock, len, flags, response;
struct sockaddr_in serv_addr, client_addr, dest_addr;
socklen_t client_sock_len = sizeof(client_addr);
device_info *master_device, *new_master_device;
inquiry_info *ii = NULL;
bool is_device_configured = false, is_identity_required = false;
bool new_control_device_found = false, is_search_finished = false;
bool is_master_device_info_updated = false, is_master_device_found = false;
bool is_trusted_device_identity_update_required =false;
bool is_profile_list_sent =false, change_detected = false;
bool challenge_response_recieved = false, is_new_profile_creation_required = false;

int main(void){

	int response;

	change_device_name();

	response = turn_on_discoverable_mode();
	if(response == 0)
		printf("Discoverable Mode On...\n");
	else
		printf("*****Failed to Set Discoverable Mode On*****\n");

	_begin_thread(device_info_thread, get_paired_device);
	_begin_thread(server_thread, create_server);

	while(1);

	return 0;
}

void _begin_thread(pthread_t thread_id, void (*function_name(void *))){
	if(pthread_create(&thread_id, NULL, function_name, NULL) < 0){
		    perror("could not create thread");
	}
}

void change_device_name(void){
	int response;

	response = system("hciconfig hci0 name 'OWCH_BT_Device' > /dev/null 2>&1");
	response = system("hciconfig hci0 down > /dev/null 2>&1");
	response = system("hciconfig hci0 up > /dev/null 2>&1");

	if(response != 0)
		printf("Failed to change the device name..\n");
	else
		printf("Device name changed..\n");
}

int turn_on_discoverable_mode(void){
	return system("hciconfig hci0 piscan");
}

char* execute_shell_command(char *cmd, char output[]){
	FILE *fp;
	char tmp_buf[128];
	int count = 0;
	char *response;

	if ((fp = popen(cmd, "r")) == NULL) {
		printf("Error opening pipe!\n");
	}

	while(1){
		if(feof(fp) || count >= 1){
			break;
		}
		memset(tmp_buf, 0 ,128);
		response = fgets(tmp_buf, 128, fp);

		if(strstr(tmp_buf, "Controller"))
			continue;
		memcpy((output + (count*128)),tmp_buf, 128);
		count++;
	}
	if(pclose(fp))  {
		printf("Command not found or exited with error status. %s\n",response);
	}

	return output;
}


void* get_paired_device(void *){

	char *cmd = (char *)"echo list | bluetoothctl";
	//printf("%s\n", execute_shell_command(cmd, buf));
	execute_shell_command(cmd, buf);

	pthread_exit(0);
	return NULL;
}

int receieve_packet(void){
	int receieved_packet_lenght;
	while(1){
		memset(socket_buf, 0, BUFLEN);
		receieved_packet_lenght = recvfrom(sockfd, socket_buf, BUFLEN, 0, (struct sockaddr *)&client_addr, &client_sock_len);
		if (receieved_packet_lenght == -1)
			perror("Receive Failed!!");
		if(DEBUG_LEVEL > 1)
			printf("Received packet from %s:%d\nLen = %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), receieved_packet_lenght);
		}
	return receieved_packet_lenght;
}

int create_socket() {
	return socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

void* create_server(void *){

	sockfd = create_socket();
	if (sockfd < 0)
		perror("ERROR opening socket");
	printf("Server Started\n");
	memset((char *) &serv_addr,0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORTNUM);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		perror("ERROR on binding");
	if(receieve_packet())
		parser(socket_buf,len);
	return NULL;
}

void send_packet(char send_buf[], in_addr dest_ip, int length){
	send_sockfd = create_socket();
	if (send_sockfd < 0)
		perror("ERROR opening socket");
	memset((char *) &dest_addr,0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr.s_addr = dest_ip.s_addr;
	dest_addr.sin_port = htons(PORTNUM);
	if(DEBUG_LEVEL > 1){
		int i;
		for(i=0;i<length;i++)
			printf("%d ",send_buf[i]);
		printf("\n");
	}

	if(sendto(send_sockfd, send_buf, length, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1)
		perror("Sending Failed");
	if(DEBUG_LEVEL > 1)
		printf("Packet Sent\n");

}


void parser(char rcv_buf[], int length){
	if(DEBUG_LEVEL > 1){
		int i;
		for(i=0;i<length;i++)
				printf("%d ",rcv_buf[i]);
		printf("\n");
	}

	struct generic_packet* rcv_packet = (struct generic_packet *)rcv_buf;

	if(rcv_packet->header.version != PROTOCOL_VERSION)
		perror("Invalid Protocol Version\n");
	else if(rcv_packet->header.message_type == MSG_IP_BROADCAST){
			if(!is_device_configured){
				printf("Configuration Received\n");
				/*master_device = parse_device_info(buf);
				printf("Trusted Device Name: %s\nTrusted Device Address: %s\n",master_device->device_name, master_device->bt_address);*/
				get_master_device_info(rcv_packet);
				send_packet((char*)create_ip_info_packet(),master_device->ip, (int)((int)sizeof(PACKET_HEADER)+(int)sizeof(DEVICE_NAME)));
			}
		}
	else if(rcv_packet->header.message_type == MSG_PASSWORD){
			if(!is_device_configured){
				printf("Profile Information Received\n");
				if(strcmp(rcv_packet->header.sender_ip, inet_ntoa(master_device->ip)) != 0){
					printf("Not the Master Device!!!");
				}
				else{
					save_password(rcv_packet, length);
					save_context(FIRST_CONTEXT);
					save_trusted_device();
					_begin_thread(check_ssid_thread, check_wifi_ssid);
					send_packet((char*)create_pw_confirmation_packet(),master_device->ip, (int)((int)sizeof(PACKET_HEADER)+(int)sizeof(PW_CONFIRMATION_CODE)));
					is_device_configured = true;
					printf("Profile %s Activated\n",active_profile);
					printf("Ownership Change Detection Started\n");
				}
			}
			else if(is_new_profile_creation_required){
				printf("Profile Information Received\n");
				save_password(rcv_packet, length);
				save_context(FIRST_CONTEXT);
				save_trusted_device();
				_begin_thread(check_ssid_thread, check_wifi_ssid);
				send_packet((char*)create_pw_confirmation_packet(),master_device->ip, (int)((int)sizeof(PACKET_HEADER)+(int)sizeof(PW_CONFIRMATION_CODE)));
				is_device_configured = true;
				printf("Profile %s Activated\n",active_profile);
				printf("Ownership Change Detection Started\n");
			}

		}
	else if(rcv_packet->header.message_type == MSG_IDENTITY_BRDCAST){
			if(is_identity_required){
				if(DEBUG_LEVEL>2)
					printf("Parsing Control Device Identity\n");
				get_control_device_identity(rcv_packet);
			}
			else if(is_trusted_device_identity_update_required)
				get_trusted_device_identity_for_new_profile(rcv_packet);
			else{
				if(DEBUG_LEVEL > 1)
					printf("Ignoring Identity Broadcast packet\n");
			}
		}
	else if(rcv_packet->header.message_type == MSG_CHALLENGE_RESPONSE){
			if(strcmp(rcv_packet->header.sender_ip, inet_ntoa(master_device->ip)) != 0){
				printf("Unauthorised Access!!\n");
			}
			else{
				if(strcmp(rcv_packet->payload, CHALLENGE_RESPONSE_CODE) != 0)
					printf("Challenge Not Satisfied.\n");
				else{
					printf("Challenge Satisfied..New Known Context Created\n");
					save_context(NOT_FIRST_CONTEXT);
					_begin_thread(check_ssid_thread, check_wifi_ssid);
					challenge_response_recieved = true;
					/*memcpy(known_context[counter], changed_context, BUFLEN);
					counter++;
					challenge_response_recieved = true;
					if(DEBUG_LEVEL > 1){
						printf("Current Known Contexts..\n");
						for(int i=0;i<counter;i++)
							printf("%d. %s\n", i+1, known_context[i]);
					}*/
				}
			}
		}
	else if(rcv_packet->header.message_type == MSG_PASSWORD_RESPONSE){
			char password[BUFLEN];
			get_owner_password(password);
			if(strcmp(rcv_packet->payload, password) == 0){
				printf("Challenge Satisfied..New Known Context Created\n");
				memcpy(known_context[counter], changed_context, BUFLEN);
				counter++;
			}
			else{
				printf("Owner Authentication Failed\n");
				printf("New user profile created..Previous Owner Profile Encrypted\n");
			}
			challenge_response_recieved = true;
		}
	else if(rcv_packet->header.message_type == MSG_PROFILE_REQUEST){
			if(is_device_configured){
				in_addr dest_ip;
				if(active_profile[0] != '\0'){
					printf("Currently Active profile: %s Encrypted\n", active_profile);
					memset(active_profile, '\0', sizeof(active_profile));
				}
				pthread_cancel(check_ssid_thread);
				dest_ip.s_addr = inet_addr(rcv_packet->header.sender_ip);
				send_profile_list(dest_ip);
			}

		}
	else if(rcv_packet->header.message_type == MSG_PROFILE_CHOICE_RESPONSE){
			if(is_device_configured && is_profile_list_sent){
				challenge_response_recieved = true;
				authenticate_owner(rcv_packet);
			}

		}
	else if(rcv_packet->header.message_type == MSG_CREATE_NEW_PROFILE_REQUEST){
			is_trusted_device_identity_update_required = true;
			is_new_profile_creation_required = true;
		}
}

void get_master_device_info(struct generic_packet *rcv_packet){
	if(strcmp(rcv_packet->payload, master_device->device_name))
		printf("Not the Master Device!!!\n");
	else{
		master_device->ip.s_addr = inet_addr(rcv_packet->header.sender_ip);
		master_device->port = atoi(rcv_packet->header.sender_port);
		if(DEBUG_LEVEL>2)
			printf("Device IP: %s\nDevice Port: %d\n", inet_ntoa(master_device->ip), atoi(rcv_packet->header.sender_port));
	}
}

void save_password(struct generic_packet* rcv_packet, int length){

	//FILE *fp = fopen("Password.txt", "w");
	FILE *fp;

	profile_info *p_info = (profile_info*)rcv_packet->payload;
	//printf("Payload: %s",rcv_packet->payload);
	if(p_info->passsword[strlen(p_info->passsword) - 1] == '\n')
	{
		p_info->passsword[strlen(p_info->passsword) - 1] = '\0';
	}

	printf("Profile Name: %s Password: %s\n", p_info->profile_name, p_info->passsword);
	strcpy(active_profile, p_info->profile_name);

	struct stat st = {0};

	if (stat(active_profile, &st) == -1) {
	    mkdir(active_profile, 0777);
	}
	/*char file_path[BUFLEN];
	sprintf(file_path,"%s/%s",active_profile,"Password.txt");*/

	fp = fopen("Password.txt", "a+");
	if (fp == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	//fprintf(fp, "%s,%s\n", master_device->device_name, rcv_packet->payload);
	fprintf(fp, "%s %s\n", p_info->profile_name, p_info->passsword);
	fclose(fp);
}

void save_context(int isFirstContext){
	char *cmd = (char *)"iwconfig 2>/dev/null | grep ESSID | cut -d: -f2";
	execute_shell_command(cmd, ssid_buf);
	char *current_ssid = (char *)malloc(BUFLEN);
	memcpy(current_ssid, ssid_buf, BUFLEN);
	char file_path[BUFLEN];
    sprintf(file_path,"%s/%s",active_profile,"known_context.txt");

	FILE *fp = fopen(file_path, "a+");
	if (fp == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	if(isFirstContext == FIRST_CONTEXT){

		if(current_ssid[strlen(current_ssid) - 1] == '\n')
		{
			current_ssid[strlen(current_ssid) - 1] = '\0';
		}
		printf("Context %s stored as Known Context\n", current_ssid);
		fprintf(fp, "%s\n", current_ssid);
	}
	else{
		//printf("Context %s stored as Known Context\n", current_ssid);
		if(changed_context[strlen(changed_context) - 1] == '\n')
		{
			changed_context[strlen(changed_context) - 1] = '\0';
		}
		printf("Storing %s in %s", changed_context, file_path);
		fprintf(fp, "%s\n", changed_context);
	}
	fclose(fp);
}

void save_trusted_device(){

	char file_path[BUFLEN];
    sprintf(file_path,"%s/%s",active_profile,"trusted_device.txt");

	FILE *fp = fopen(file_path, "a+");
	if (fp == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	printf("Trusted Device Identity stored\n");
	fprintf(fp, "%s %s\n", master_device->device_name, master_device->bt_address);
	fclose(fp);
}

void get_known_context_list(){
	counter = 0;
	char line[BUFLEN];
	char file_path[BUFLEN];
	sprintf(file_path,"%s/%s",active_profile,"known_context.txt");
	FILE *fp = fopen(file_path, "r");
	if (fp == NULL){
		printf("Error opening file!\n");
		exit(1);
	}
	while(fgets(line, BUFLEN, (FILE*) fp)){
		memcpy(known_context[counter], line, BUFLEN);
		counter++;
	}
}

void get_control_device_identity(struct generic_packet *rcv_packet){
	extra_info *info = (extra_info *)rcv_packet->payload;
	if(DEBUG_LEVEL>2)
		printf("Device Name: %s Device Address: %s\n", info->device_name, info->bt_address);

	if(strcmp(info->device_name, master_device->device_name)){
		new_master_device = (device_info *)malloc(sizeof(device_info*));
		new_master_device->ip.s_addr = inet_addr(rcv_packet->header.sender_ip);
		new_master_device->port = atoi(rcv_packet->header.sender_port);
		strcpy(new_master_device->bt_address, info->bt_address);
		strcpy(new_master_device->device_name, info->device_name);
		//printf("Device Name: %s Device Address: %s\n", new_master_device->device_name, new_master_device->bt_address);
		//printf("Device IP: %s\nDevice Port: %d\n", inet_ntoa(new_master_device->ip), new_master_device->port);
		new_control_device_found = true;
		is_identity_required = false;
	}
	else{
		printf("Trusted Device Found.\n");
		printf("Trusted Device information Updated\n");
		master_device->ip.s_addr = inet_addr(rcv_packet->header.sender_ip);
		master_device->port = atoi(rcv_packet->header.sender_port);
		printf("Trusted device IP: %s\nDevice Port: %d\n", inet_ntoa(master_device->ip), atoi(rcv_packet->header.sender_port));
		is_master_device_info_updated = true;
		is_master_device_found = true;
		is_identity_required = false;
	}
}

void get_trusted_device_identity_for_new_profile(struct generic_packet *rcv_packet){
	extra_info *info = (extra_info *)rcv_packet->payload;
	if(DEBUG_LEVEL>2)
		printf("Device Name: %s Device Address: %s\n", info->device_name, info->bt_address);

	master_device->ip.s_addr = inet_addr(rcv_packet->header.sender_ip);
	master_device->port = atoi(rcv_packet->header.sender_port);
	strcpy(master_device->bt_address, info->bt_address);
	strcpy(master_device->device_name, info->device_name);
	printf("Trusted Device Identity Updated\n");
	is_trusted_device_identity_update_required = false;
}

void get_owner_password(char password[]){
	FILE *fp = fopen("Password.txt", "r");
	if (fp == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	fgets(password, BUFLEN, (FILE*) fp);

}

void send_profile_list(in_addr dest_ip){
	int count = 0;
	char line[BUFLEN];
	FILE *fp = fopen("Password.txt", "r");
	if (fp == NULL){
		printf("Error opening file!\n");
		exit(1);
	}
	while(fgets(line, BUFLEN, (FILE*) fp))
	{
		char e = sscanf(line, "%s", profile_list[count]);
		//printf("%s\n",profile_list[count]);
		count++;
	}
	fclose(fp);
	send_packet((char*)create_profile_list_packet(profile_list, count) ,dest_ip, (int)((int)sizeof(PACKET_HEADER)+(int)(count*11)));
	is_profile_list_sent = true;
}

void* check_wifi_ssid(void *){
	bool is_known_context = false;
	char *cmd = (char *)"iwconfig 2>/dev/null | grep ESSID | cut -d: -f2";
	changed_context = (char*)malloc(BUFLEN);
	execute_shell_command(cmd, ssid_buf);
	char *current_ssid = (char *)malloc(BUFLEN);
	memcpy(current_ssid, ssid_buf, BUFLEN);
	if(current_ssid[strlen(current_ssid) - 1] == '\n')
	{
		current_ssid[strlen(current_ssid) - 1] = '\0';
	}
	if(DEBUG_LEVEL>2)
		printf("Current SSID: %s", current_ssid);
	//memcpy(known_context[counter], current_ssid, BUFLEN);
	//counter++;
	get_known_context_list();
	char *new_ssid;
	while(1){
		is_known_context = false;
		sleep(20);
		memset(ssid_buf, 0, BUFLEN);
		execute_shell_command(cmd, ssid_buf);
		new_ssid = ssid_buf;
		if(new_ssid[strlen(new_ssid) - 1] == '\n')
		{
			new_ssid[strlen(new_ssid) - 1] = '\0';
		}

		if(strncmp(new_ssid, (char *)current_ssid, strlen(new_ssid)) == 0){
			continue;
		}
		else if(strncmp(new_ssid, "off/any", strlen("off/any")) == 0){
			if(DEBUG_LEVEL>2)
				printf("off/any Detected\n");
			continue;
		}
		else{
			for(int i=0;i<counter;i++){
				if(strncmp(new_ssid, known_context[i], strlen(new_ssid)) == 0){
					printf("Change to Known Context Detected..\n");
					is_known_context = true;
					break;
				}
			}

			memset(current_ssid,0,BUFLEN);
			memcpy(current_ssid, new_ssid, BUFLEN);

			if(is_known_context)
				continue;
			else{
				memset(changed_context,0, BUFLEN);
				memcpy(changed_context, current_ssid, BUFLEN);
				change_detected = true;
				pthread_cancel(check_ssid_thread);
			}
		}
	}
	return NULL;
}

void authenticate_owner(struct generic_packet *rcv_packet){
	char line[BUFLEN];
	char profile_name[11];
	char password_hash[BUFLEN];
	in_addr dest_ip;

	profile_info *p_info = (profile_info*)rcv_packet->payload;
	dest_ip.s_addr = inet_addr(rcv_packet->header.sender_ip);
	if(p_info->passsword[strlen(p_info->passsword) - 1] == '\n')
	{
		p_info->passsword[strlen(p_info->passsword) - 1] = '\0';
	}

	if(DEBUG_LEVEL>2)
		printf("Received Profile Name: %s Password: %s Length: %d\n", p_info->profile_name, p_info->passsword, (int)strlen(p_info->passsword));
	FILE *fp = fopen("Password.txt", "r");
	if (fp == NULL){
		printf("Error opening file!\n");
		exit(1);
	}
	while(fgets(line, BUFLEN, (FILE*) fp))
	{
		char e = sscanf(line, "%s %s", profile_name, password_hash);
		if(DEBUG_LEVEL>2)
			printf("Name: %s Password %s Length %d\n", profile_name, password_hash, (int)strlen(password_hash));
		if(strcmp(profile_name, p_info->profile_name) == 0){
			if(strcmp(password_hash, p_info->passsword) == 0){
				printf("Profile %s Activated\n", p_info->profile_name);
				strcpy(active_profile, p_info->profile_name);
				save_context(FIRST_CONTEXT);
				_begin_thread(check_ssid_thread, check_wifi_ssid);
				send_packet((char*)create_profile_authetication_response_packet(1), dest_ip, (int)((int)sizeof(PACKET_HEADER)+(int)sizeof(AUTHENTICATION_SUCCESS)));
			}
			else{
				send_packet((char*)create_profile_authetication_response_packet(0), dest_ip, (int)((int)sizeof(PACKET_HEADER)+(int)sizeof(AUTHENTICATION_SUCCESS)));
				//printf("Password Mismatch %d", strcmp(password_hash, p_info->passsword));
				printf("Authentication Failure!! Unable Activate Profile\n");
			}
			break;
		}
	}
	fclose(fp);

}

void* search_bt_device(void *){
	int i;
	char addr[19] = { 0 };
	char name[248] = { 0 };

	printf("Ownership Change Detected\n");
	printf("Searching for Trusted Device\n");
	is_identity_required = true;
	dev_id = hci_get_route(NULL);
	sock = hci_open_dev( dev_id );
	if (dev_id < 0 || sock < 0) {
		perror("opening socket");
		exit(1);
	}

	len  = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if( num_rsp < 0 ) perror("hci_inquiry");
	if(DEBUG_LEVEL > 2)
		printf("Number Of Device Found: %d\n",num_rsp);

	for (i = 0; i < num_rsp; i++) {
		if(DEBUG_LEVEL > 2)
			printf("Printing Device\n");
		ba2str(&(ii+i)->bdaddr, addr);
		memset(name, 0, sizeof(name));
		if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name),name, 0) < 0)
			strcpy(name, "[unknown]");
		if(DEBUG_LEVEL > 2)
			printf("%s  %s\n", addr, name);
		if(strcmp(master_device->bt_address, addr) == 0 && strcmp(master_device->device_name, name) == 0){
			//printf("Trusted Device Found.\n");
			is_master_device_found = true;
		}
	}

	is_search_finished = true;
	free(ii);
	close(sock);

	return NULL;
}

bool ready_to_send_challege(){
	if(is_master_device_found && is_master_device_info_updated)
		return true;
	else
		return false;
}

void* verify_ownership_change(void *){

	if(DEBUG_LEVEL>2)
		printf("Waiting For Ownership Change\n");
	while(1){
		sleep(10);
		if(change_detected == true){
			change_detected = false;
			is_search_finished = false;
			_begin_thread(bt_device_search_thread, search_bt_device);
			challenge_response_recieved = false;
			is_master_device_found = false;
			is_master_device_info_updated = false;
			new_control_device_found = false;

			while(1){
				//printf("Inside While\n");
				sleep(30);
				if(is_search_finished){
					//printf("Inside Search Finished\n");
					if(!challenge_response_recieved){
						//printf("Inside Response not received\n");
						if(ready_to_send_challege()){
							printf("Sending Challenge to Trusted Device!!!\n");
							send_packet((char*)create_challenge_packet(),master_device->ip, (int)((int)sizeof(PACKET_HEADER)+(int)sizeof(CHALLENGE_REQUEST_CODE)));
						}
						else if(new_control_device_found && !is_master_device_found && !is_master_device_info_updated){
							printf("Unable to Find Trusted Device!!!\n");
							printf("New Control Device Found!!!\n");
							printf("Sending Available Profile List!!!\n");
							send_profile_list(new_master_device->ip);
							printf("Waiting for Response..\n");
							//printf("Requesting Password from New Control Device!!!\n");
							//send_packet((char*)create_pw_request_packet(), new_master_device->ip, (int)((int)sizeof(PACKET_HEADER)));
						}
					}
					else
						break;
				}
				else
					continue;
			}

		pthread_cancel(bt_device_search_thread);

		}
		else
			continue;

	}
	return NULL;
}


