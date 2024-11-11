#include "room.h"
#include "utils.h"
#include "config.h"

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
        // 개행 문자 제거
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

            // 'joined' 정보 추가
            char temp_line[512];
            snprintf(temp_line, sizeof(temp_line), "%s, joined: %d", buffer, user_id);
            free(lines[line_count]);
            lines[line_count] = strdup(temp_line);
            if (!lines[line_count]) {
                perror("strdup error");
                fclose(file);
                for (int i = 0; i <= line_count; i++) {
                    free(lines[i]);
                }
                return;
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