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
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>


#define BUFLEN  			1024
#define PORTNUM				6346
#define DEBUG_LEVEL			   1

#define FIRST_CONTEXT              1
#define NOT_FIRST_CONTEXT          0
#define MAX_ARRAY_SIZE		 	 100
#define MAX_PROFILE_NAME_SIZE	  11


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
void get_known_context_list();
void get_control_device_identity(struct generic_packet *rcv_packet);
void get_trusted_device_identity_for_new_profile(struct generic_packet *rcv_packet);
void get_owner_password(char password[]);
void send_profile_list(in_addr dest_ip);
void* check_wifi_ssid(void *);
void authenticate_owner(struct generic_packet *rcv_packet);
void* search_bt_device(void *);
void* verify_ownership_change(void *);
int receieve_packet(void);
int create_socket();
bool ready_to_send_challege();
bool ready_to_send_profile_list(void);

#endif /* OWNERSHIP_CHANGE_H_ */
