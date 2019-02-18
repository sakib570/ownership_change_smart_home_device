#ifndef OWNERSHIP_CHANGE_H_
#define OWNERSHIP_CHANGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

void change_device_name(void);
int turn_on_discoverable_mode(void);
void _begin_thread(pthread_t thread_id, void (*function_name(void *)));
char* execute_shell_command(char *cmd, char output[]);

#endif /* OWNERSHIP_CHANGE_H_ */
