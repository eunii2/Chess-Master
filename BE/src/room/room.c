#include "room.h"
#include "utils.h"
#include "config.h"


// 방 생성 핸들러
void create_room_handler(int client_socket, cJSON *json_request) {
    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(json_request, "token");
    const cJSON *room_name_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_name");

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
    fprintf(file, "Room ID: %d, Room Name: %s, Created By: %d\n", room_id, room_name, user_id);
    fclose(file);

    char success_response[512];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 201 Created\r\n"
             "Content-Type: application/json\r\n"
             "%s\r\n"
             "{\"status\":\"success\",\"message\":\"Room created successfully\",\"room_id\":%d}",
             cors_headers, room_id);
    write(client_socket, success_response, strlen(success_response));
}


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

void get_room_list_handler(int client_socket, cJSON *json_request) {
    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(json_request, "token");

    if (!cJSON_IsString(token_json)) {
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
                 "{\"status\":\"error\",\"message\":\"Failed to retrieve room list\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    cJSON *room_list_json = cJSON_CreateArray();
    char buffer[512];

    while (fgets(buffer, sizeof(buffer), file)) {
        int room_id, created_by, joined_user_id;
        char room_name[100];
        int joined_status = 0;

        // Parse the line, including the 'joined' field if present
        int num_fields = sscanf(buffer, "Room ID: %d, Room Name: %99[^,], Created By: %d, joined: %d",
                                &room_id, room_name, &created_by, &joined_user_id);

        if (num_fields >= 3) {
            // Set 'joined' status to true if the 'joined' field is present
            if (num_fields == 4) {
                joined_status = 1;
            }

            cJSON *room_json = cJSON_CreateObject();
            cJSON_AddNumberToObject(room_json, "room_id", room_id);
            cJSON_AddStringToObject(room_json, "room_name", room_name);
            cJSON_AddBoolToObject(room_json, "joined", joined_status);

            cJSON_AddItemToArray(room_list_json, room_json);
        }
    }
    fclose(file);

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddItemToObject(response_json, "rooms", room_list_json);

    const char *response_string = cJSON_PrintUnformatted(response_json);
    char http_response[1024];
    snprintf(http_response, sizeof(http_response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "%s\r\n"
             "%s", cors_headers, response_string);

    write(client_socket, http_response, strlen(http_response));

    cJSON_Delete(response_json);
    free((void *)response_string);
}

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
             "{\"status\":\"success\",\"room_name\":\"%s\",\"is_creator\":%s,\"has_joined_members\":%s}",
             cors_headers, room_name, is_creator ? "true" : "false", has_joined_members ? "true" : "false");
    write(client_socket, success_response, strlen(success_response));
}