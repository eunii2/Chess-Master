#include "room.h"
#include "utils.h"
#include "config.h"
#include "game.h"

// 방 생성 API 핸들러
void create_room_handler(int client_socket, cJSON *json_request) {
    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(json_request, "token");
    const cJSON *room_name_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_name");

    // JSON 유효성 검사
    if (!cJSON_IsString(token_json) || !cJSON_IsString(room_name_json)) {
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
    const char *room_name = room_name_json->valuestring;

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

    // 사용자 이름 가져오기
    char *username = get_user_name_by_token(token);
    if (!username) {
        char error_response[512];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 500 Internal Server Error\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"Failed to get username\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 파일을 읽기 모드로 열기
    FILE *file = fopen(ROOM_LIST_FILE, "r");
    if (!file) {
        // 파일이 없어서 열 수 없는 경우, 생성 시도
        file = fopen(ROOM_LIST_FILE, "w");
        if (!file) {
            perror("fopen error");
            char error_response[512];
            snprintf(error_response, sizeof(error_response),
                     "HTTP/1.1 500 Internal Server Error\r\n"
                     "Content-Type: application/json\r\n"
                     "%s\r\n"
                     "{\"status\":\"error\",\"message\":\"Failed to retrieve room list\"}",
                     cors_headers);
            write(client_socket, error_response, strlen(error_response));
            return;
        }
        fclose(file);
        // 파일이 생성되었으므로 다시 읽기 모드로 열기
        file = fopen(ROOM_LIST_FILE, "r");
        if (!file) {
            perror("fopen error");
            char error_response[512];
            snprintf(error_response, sizeof(error_response),
                     "HTTP/1.1 500 Internal Server Error\r\n"
                     "Content-Type: application/json\r\n"
                     "%s\r\n"
                     "{\"status\":\"error\",\"message\":\"Failed to retrieve room list\"}",
                     cors_headers);
            write(client_socket, error_response, strlen(error_response));
            return;
        }
    }

    // 사용자가 이미 방을 만들었는지 확인
    char buffer[512];
    int room_exists = 0;
    while (fgets(buffer, sizeof(buffer), file)) {
        int existing_user_id;
        if (sscanf(buffer, "Room ID: %*d, Room Name: %*[^,], Created By: %d", &existing_user_id) == 1) {
            if (existing_user_id == user_id) {
                room_exists = 1;
                break;
            }
        }
    }
    fclose(file);

    // 이미 방이 있는 경우 에러 응답
    if (room_exists) {
        char error_response[512];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 409 Conflict\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"User already has a created room\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    static int last_room_id = 0;
    int room_id = ++last_room_id;

    // 방 생성
    GameState *game_state = get_game_state(room_id);
    if (game_state) {
        strncpy(game_state->player1_token, token, TOKEN_LENGTH);
        game_state->player1_token[TOKEN_LENGTH] = '\0';
    }

    file = fopen(ROOM_LIST_FILE, "a");
    if (!file) {
        perror("fopen error");
        char error_response[512];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 500 Internal Server Error\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"Failed to create room\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        return;
    }
    fprintf(file, "Room ID: %d, Room Name: %s, Created By: %d, Creator Username: %s\n", 
            room_id, room_name, user_id, username);
    fclose(file);

    char success_response[512];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 201 Created\r\n"
             "Content-Type: application/json\r\n"
             "%s\r\n"
             "{\"status\":\"success\","
             "\"message\":\"Room created successfully\","
             "\"room_id\":%d,"
             "\"host_token\":\"%s\","
             "\"creator_username\":\"%s\"}",
             cors_headers, room_id, token, username);
    write(client_socket, success_response, strlen(success_response));
}

