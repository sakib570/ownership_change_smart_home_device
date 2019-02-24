#include "ownership_change.h"
#include "packet.h"
#include "device_info.h"

int main(void){

	int response;

	change_device_name();

	response = turn_on_discoverable_mode();
	if(response == 0)
		printf("Discoverable Mode On...\n");
	else
		printf("*****Failed to Set Discoverable Mode On*****\n");

	call_threads();

	while(1);

	return 0;
}
