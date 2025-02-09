#ifndef CHESS_SERVER_UTILS_H
#define CHESS_SERVER_UTILS_H

#include "cJSON.h"

cJSON *parse_json(const char *json_string);
void generate_token(char *token, size_t length);

void create_directory(const char *path);

// 토큰으로 userID를 찾는 함수. 실패시 -1 반환
int get_user_id_by_token(const char* token);
char* get_user_name_by_token(const char* token);

void send_cors_response(int client_socket, const char *status_code, const char *content_type, const char *body);

#endif //CHESS_SERVER_UTILS_H
