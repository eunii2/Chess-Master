#ifndef CHESS_SERVER_HANDLER_H
#define CHESS_SERVER_HANDLER_H

#include "cJSON.h"

void handle_request(int client_socket, const char *method, const char *path, cJSON *json_request);

#endif //CHESS_SERVER_HANDLER_H
