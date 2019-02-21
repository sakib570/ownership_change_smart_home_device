#include "ownership_change.h"
#include "packet.h"
#include "device_info.h"

char buf[BUFLEN], ssid_buf[BUFLEN];
char active_profile[11];;
char socket_buf[BUFLEN];
char *changed_context;
char known_context[MAX_ARRAY_SIZE][BUFLEN];
pthread_t device_info_thread, server_thread;
int sockfd, send_sockfd, counter = 0;
struct sockaddr_in serv_addr, client_addr, dest_addr;
socklen_t client_sock_len = sizeof(client_addr);
device_info *master_device, *new_master_device;
bool is_device_configured = false, is_identity_required = false;
bool new_control_device_found = false;
bool is_master_device_info_updated = false, is_master_device_found = false;
bool is_trusted_device_identity_update_required =false;


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

void* create_server(void *){

	int len;
	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd < 0)
		perror("ERROR opening socket");
	printf("Server Started\n");
	memset((char *) &serv_addr,0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORTNUM);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		perror("ERROR on binding");
	while(1){
		memset(socket_buf, 0, BUFLEN);
		if ((len = recvfrom(sockfd, socket_buf, BUFLEN, 0, (struct sockaddr *)&client_addr, &client_sock_len))==-1)
			perror("Receive Failed!!");
		//parser(socket_buf,len);
		if(DEBUG_LEVEL > 1)
			printf("Received packet from %s:%d\nLen = %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), len);
	}

}

void send_packet(char send_buf[], in_addr dest_ip, int length){
	send_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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


