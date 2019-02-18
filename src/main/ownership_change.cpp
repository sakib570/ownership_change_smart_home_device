#include "ownership_change.h"

int main(void){

	int response;

	change_device_name();

	response = turn_on_discoverable_mode();
	if(response == 0)
		printf("Discoverable Mode On...\n");
	else
		printf("*****Failed to Set Discoverable Mode On*****\n");

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

