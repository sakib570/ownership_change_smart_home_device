#include "ownership_change.h"

char buf[BUFLEN];
pthread_t device_info_thread;

int main(void){

	int response;

	change_device_name();

	response = turn_on_discoverable_mode();
	if(response == 0)
		printf("Discoverable Mode On...\n");
	else
		printf("*****Failed to Set Discoverable Mode On*****\n");

	_begin_thread(device_info_thread, get_paired_device);

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

	char *cmd = (char *)"bluetoothctl";
	//printf("%s\n", execute_shell_command(cmd));
	execute_shell_command(cmd, buf);

	pthread_exit(0);
	return NULL;
}

