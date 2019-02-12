#include "ownership_change.h"

int main(void){
	change_device_name();
	return 0;
}

void change_device_name(void){
	int response;

	response = system("hciconfig hci0 name 'OWCH_BT_Device'");
	response = system("hciconfig hci0 down");
	response = system("hciconfig hci0 up");
	if(response == 0)
		printf("Device Name Changed..\n");
}
