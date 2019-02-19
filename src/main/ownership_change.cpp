#include "ownership_change.h"

char buf[BUFLEN];
char socket_buf[BUFLEN];
pthread_t device_info_thread, server_thread;
int sockfd, send_sockfd;
struct sockaddr_in serv_addr, client_addr;
socklen_t client_sock_len = sizeof(client_addr);

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


