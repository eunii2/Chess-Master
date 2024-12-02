#include "room.h"
#include "utils.h"
#include "config.h"
#include "game.h"

void join_room_handler(int client_socket, cJSON *json_request) {
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

    char *lines[1024];
    int line_count = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        lines[line_count] = strdup(buffer);
        if (!lines[line_count]) {
            perror("strdup error");
            fclose(file);
            return;
        }

        int current_room_id;
        if (sscanf(buffer, "Room ID: %d,", &current_room_id) == 1 && current_room_id == room_id) {
            room_found = 1;

            // 이미 참여한 사용자인지 확인
            char joined_check[64];
            snprintf(joined_check, sizeof(joined_check), "joined: %d", user_id);
            if (strstr(buffer, joined_check) != NULL) {
                char error_response[512];
                snprintf(error_response, sizeof(error_response),
                         "HTTP/1.1 409 Conflict\r\n"
                         "Content-Type: application/json\r\n"
                         "%s\r\n"
                         "{\"status\":\"error\",\"message\":\"User already joined the room\"}",
                         cors_headers);
                write(client_socket, error_response, strlen(error_response));

                for (int i = 0; i <= line_count; i++) {
                    free(lines[i]);
                }
                fclose(file);
                return;
            }

            // 사용자 이름 가져오기
            char *username = get_user_name_by_token(token);
            if (!username) {
                // 에러 처리
                return;
            }

            // 'joined' 정보에 사용자 이름도 추가
            char temp_line[512];
            snprintf(temp_line, sizeof(temp_line), "%s, joined: %d, joined_username: %s", 
                     buffer, user_id, username);
            free(lines[line_count]);
            lines[line_count] = strdup(temp_line);

            // GameState에 후공 플레이어의 토큰과 이름 추가
            GameState *game_state = get_game_state(room_id);
            if (game_state) {
                strncpy(game_state->player2_token, token, TOKEN_LENGTH);
                game_state->player2_token[TOKEN_LENGTH] = '\0';
            }
        }

        line_count++;
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
        for (int i = 0; i < line_count; i++) {
            free(lines[i]);
        }
        return;
    }

    file = fopen(ROOM_LIST_FILE, "w");
    if (!file) {
        perror("fopen error");
        char error_response[512];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 500 Internal Server Error\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"Failed to update room data\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        for (int i = 0; i < line_count; i++) {
            free(lines[i]);
        }
        return;
    }

    // 수정된 내용을 파일에 다시 씀
    for (int i = 0; i < line_count; i++) {
        fprintf(file, "%s\n", lines[i]);
        free(lines[i]);
    }

    fclose(file);

    char success_response[512];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "%s\r\n"
             "{\"status\":\"success\",\"message\":\"Joined room successfully\",\"room_id\":%d}",
             cors_headers, room_id);
    write(client_socket, success_response, strlen(success_response));
}