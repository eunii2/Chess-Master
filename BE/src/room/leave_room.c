#include "room.h"
#include "utils.h"
#include "config.h"

void leave_room_handler(int client_socket, cJSON *json_request) {
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
    char *lines[1024];
    int line_count = 0;
    int room_found = 0;
    int user_is_creator = 0;
    int user_joined = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        lines[line_count] = strdup(buffer);
        if (!lines[line_count]) {
            perror("strdup error");
            fclose(file);
            return;
        }

        int current_room_id, creator_id;
        char room_name[100];
        if (sscanf(buffer, "Room ID: %d, Room Name: %[^,], Created By: %d", &current_room_id, room_name, &creator_id) == 3 && current_room_id == room_id) {
            room_found = 1;
            if (creator_id == user_id) {
                user_is_creator = 1;
                break;
            }

            char joined_check[64];
            snprintf(joined_check, sizeof(joined_check), ", joined: %d", user_id);

            char *joined_pos = strstr(buffer, joined_check);
            if (joined_pos) {
                user_joined = 1;
                char temp_line[512];
                strncpy(temp_line, buffer, joined_pos - buffer);
                temp_line[joined_pos - buffer] = '\0';
                strcat(temp_line, joined_pos + strlen(joined_check));
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
            break;
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
        for (int i = 0; i <= line_count; i++) {
            free(lines[i]);
        }
        return;
    }

    if (!user_joined && !user_is_creator) {
        char error_response[512];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 409 Conflict\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"User is not joined in the room\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        for (int i = 0; i <= line_count; i++) {
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
        for (int i = 0; i <= line_count; i++) {
            free(lines[i]);
        }
        return;
    }

    // 수정된 내용을 파일에 다시 씀
    for (int i = 0; i <= line_count; i++) {
        if (user_is_creator && strstr(lines[i], "Room ID: %d,")) {
            // 방 생성자라면 해당 방 정보를 완전히 제거함
            free(lines[i]);
            continue;
        } else if (user_joined) {
            // joined된 사용자는 수정된 lines[i]를 그대로 기록
            fputs(lines[i], file);
            free(lines[i]);
        }
    }

    fclose(file);

    char success_response[512];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "%s\r\n"
             "{\"status\":\"success\",\"message\":\"Left room successfully\"}",
             cors_headers);
    write(client_socket, success_response, strlen(success_response));
}
