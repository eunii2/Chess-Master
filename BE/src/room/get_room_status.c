#include "room.h"
#include "utils.h"
#include "config.h"
#include "game.h"

void get_room_status_handler(int client_socket, cJSON *json_request) {
    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(json_request, "token");
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");

    if (!cJSON_IsString(token_json) || !cJSON_IsNumber(room_id_json)) {
        char error_response[512];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 400 Bad Request\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"Invalid input\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    const char *token = token_json->valuestring;
    int room_id = room_id_json->valueint;
    GameState *game_state = get_game_state(room_id);
    int game_started = (game_state != NULL && game_state->game_started);

    int user_id = get_user_id_by_token(token);
    if (user_id == -1) {
        char error_response[512];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 401 Unauthorized\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"Invalid token or user not found\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 게임이 시작된 경우
    if (game_started) {
        int is_creator = (strcmp(game_state->player1_token, token) == 0);
        char success_response[512];
        snprintf(success_response, sizeof(success_response),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"success\",\"game_started\":true,\"is_creator\":%s}",
                 cors_headers,
                 is_creator ? "true" : "false");
        write(client_socket, success_response, strlen(success_response));
        return;
    }

    // 게임이 시작되지 않은 경우 room_list.txt 확인
    FILE *file = fopen(ROOM_LIST_FILE, "r");
    if (!file) {
        perror("fopen error");
        char error_response[512];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 500 Internal Server Error\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"Failed to access room data\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    char buffer[512];
    int room_found = 0;
    int is_creator = 0;
    int has_joined_members = 0;
    char room_name[100] = "";

    while (fgets(buffer, sizeof(buffer), file)) {
        int current_room_id, creator_id;
        if (sscanf(buffer, "Room ID: %d, Room Name: %[^,], Created By: %d", &current_room_id, room_name, &creator_id) == 3 && current_room_id == room_id) {
            room_found = 1;
            if (creator_id == user_id) {
                is_creator = 1;
            }
            if (strstr(buffer, "joined:")) {
                has_joined_members = 1;
            }
            break;
        }
    }
    fclose(file);

    if (!room_found) {
        char error_response[512];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 404 Not Found\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"Room not found\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    char success_response[512];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "%s\r\n"
             "{\"status\":\"success\",\"room_name\":\"%s\",\"is_creator\":%s,\"has_joined_members\":%s,\"game_started\":%s}",
             cors_headers, room_name,
             is_creator ? "true" : "false",
             has_joined_members ? "true" : "false",
             game_started ? "true" : "false");  // 게임 시작 상태 추가
    write(client_socket, success_response, strlen(success_response));
}