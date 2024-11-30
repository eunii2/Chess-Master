#ifndef SERVER_PROJECT_CHAT_H
#define SERVER_PROJECT_CHAT_H

void send_message_handler(int client_socket, cJSON *json_request);

#endif //SERVER_PROJECT_CHAT_H
