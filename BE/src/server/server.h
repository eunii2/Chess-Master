#ifndef SERVER_PROJECT_SERVER_H
#define SERVER_PROJECT_SERVER_H

#define BUFFER_SIZE 2048
#include "config.h"

void server_run();
void *client_handler(void *arg);
void *admin_handler(void *arg);

#endif //SERVER_PROJECT_SERVER_H
