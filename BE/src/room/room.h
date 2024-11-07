#ifndef CHESS_SERVER_ROOM_H
#define CHESS_SERVER_ROOM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h" // 유저 ID 확인 함수가 정의된 헤더 파일
#include "cJSON.h" // JSON 파싱을 위한 헤더 파일
#include <unistd.h>  // POSIX 호환 함수들

void create_room_handler(int client_socket, cJSON *json_request);
void join_room_handler(int client_socket, cJSON *json_request);
void get_room_list_handler(int client_socket, cJSON *json_request);
void leave_room_handler(int client_socket, cJSON *json_request);
void get_room_status_handler(int client_socket, cJSON *json_request);

#endif
