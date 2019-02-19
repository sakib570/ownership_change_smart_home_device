#ifndef DEVICE_INFO_H_
#define DEVICE_INFO_H_

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

#endif /* DEVICE_INFO_H_ */
