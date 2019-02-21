#ifndef OWNERSHIP_CHANGE_H_
#define OWNERSHIP_CHANGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>


#define BUFLEN  			1024
#define PORTNUM				6346
#define DEBUG_LEVEL			   1

#define FIRST_CONTEXT              1
#define NOT_FIRST_CONTEXT          0


void change_device_name(void);
int turn_on_discoverable_mode(void);
void _begin_thread(pthread_t thread_id, void (*function_name(void *)));
char* execute_shell_command(char *cmd, char output[]);
void* get_paired_device(void *);
void* create_server(void *);
void parser(char rcv_buf[], int length);
void get_master_device_info(struct generic_packet *rcv_packet);
void save_password(struct generic_packet* rcv_packet, int length);
void save_context(int isFirstContext);
void save_trusted_device();

#endif /* OWNERSHIP_CHANGE_H_ */
