#include "room.h"
#include "utils.h"
#include "config.h"
#include "game.h"

// 방 상태 조회 API 핸들러
void get_room_status_handler(int client_socket, cJSON *json_request) {
    pid_t pid = getpid(); // 현재 프로세스 ID
    printf("Process %d: Received /room/status request\n", pid);

    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(json_request, "token");
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");

    if (!cJSON_IsString(token_json) || !cJSON_IsNumber(room_id_json)) {
        printf("Process %d: Invalid input\n", pid);
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

    // 토큰과 방 ID 추출
    const char *token = token_json->valuestring;
    int room_id = room_id_json->valueint;
    printf("Process %d: Checking room status for room_id=%d, token=%s\n", pid, room_id, token);

    GameState *game_state = get_game_state(room_id);
    int game_started = (game_state != NULL && game_state->game_started);

    int user_id = get_user_id_by_token(token);
    if (user_id == -1) {
        printf("Process %d: Invalid token for user\n", pid);
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
        printf("Process %d: Game started for room_id=%d\n", pid, room_id);
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
        printf("Process %d: Failed to open room list file\n", pid);
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

    printf("Process %d: Searching for room_id=%d in room list file\n", pid, room_id);
    char buffer[512];
    int room_found = 0;
    int is_creator = 0;
    int has_joined_members = 0;
    char room_name[100] = "";
    char creator_username[100] = "";
    char joined_username[100] = "";

    // room_list.txt 파일에서 방 정보 찾기
    while (fgets(buffer, sizeof(buffer), file)) {
        int current_room_id, creator_id;
        if (sscanf(buffer, "Room ID: %d, Room Name: %[^,], Created By: %d", &current_room_id, room_name, &creator_id) == 3 && current_room_id == room_id) {
            room_found = 1;
            if (creator_id == user_id) {
                is_creator = 1;
            }
            char *creator_name_pos = strstr(buffer, "Creator Username: ");
            if (creator_name_pos) {
                sscanf(creator_name_pos + strlen("Creator Username: "), "%[^,\n]", creator_username);
            }
            char *joined_name_pos = strstr(buffer, "joined_username: ");
            if (joined_name_pos) {
                has_joined_members = 1;
                sscanf(joined_name_pos + strlen("joined_username: "), "%[^,\n]", joined_username);
            }
            break;
        }
    }
    fclose(file);

    // 방을 찾지 못한 경우 404 응답
    if (!room_found) {
        printf("Process %d: Room not found for room_id=%d\n", pid, room_id);
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

    // 방을 찾은 경우 200 응답
    printf("Process %d: Room found. Sending success response.\n", pid);
    char success_response[1024];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "%s\r\n"
             "{\"status\":\"success\","
             "\"room_name\":\"%s\","
             "\"is_creator\":%s,"
             "\"has_joined_members\":%s,"
             "\"game_started\":%s,"
             "\"creator_username\":\"%s\","
             "\"joined_username\":\"%s\"}",
             cors_headers, room_name,
             is_creator ? "true" : "false",
             has_joined_members ? "true" : "false",
             game_started ? "true" : "false",
             creator_username,
             joined_username);

    write(client_socket, success_response, strlen(success_response));
}
