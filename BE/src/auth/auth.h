#ifndef SERVER_PROJECT_AUTH_H
#define SERVER_PROJECT_AUTH_H

#include "cJSON.h"

void join_handler(int client_socket, cJSON *json_request);
void login_handler(int client_socket, cJSON *json_request);

#endif //SERVER_PROJECT_AUTH_H