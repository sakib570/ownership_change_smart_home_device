#ifndef DEVICE_INFO_H_
#define DEVICE_INFO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>



typedef struct {
	char device_name[1024];
	char bt_address[20];
	in_addr ip;
	uint16_t port;
}device_info;

char* get_own_ipadress();

#endif /* DEVICE_INFO_H_ */
