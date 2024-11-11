#include "room.h"
#include "utils.h"
#include "config.h"

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

