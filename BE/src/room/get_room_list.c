#include "room.h"
#include "utils.h"
#include "config.h"
#include "image.h"

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

    // 유저 리스트 로드
    UserList user_list = {NULL, 0, 0};
    load_user_list(&user_list);



    // 응답 JSON 생성
    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");


    // 본인 프로필 사진 확인
    const char *user_profile_image = NULL;
    for (size_t i = 0; i < user_list.size; i++) {
        if (user_list.entries[i].user_id == user_id) {
            user_profile_image = user_list.entries[i].image_address;
            break;
        }
    }

    // 본인의 프로필 사진 추가
    if (user_profile_image) {
        printf("user_profile_image: %s\n", user_profile_image);
        cJSON_AddStringToObject(response_json, "profile_image", user_profile_image);
    } else {
        cJSON_AddNullToObject(response_json, "profile_image");
    }


    // 방 리스트 파일 읽기
    FILE *file = fopen(ROOM_LIST_FILE, "r");
    if (!file) {
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
            free_user_list(&user_list);
            return;
        }
        fclose(file);
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
            free_user_list(&user_list);
            return;
        }
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
            if (num_fields == 4) {
                joined_status = 1;
            }

            cJSON *room_json = cJSON_CreateObject();
            cJSON_AddNumberToObject(room_json, "room_id", room_id);
            cJSON_AddStringToObject(room_json, "room_name", room_name);
            cJSON_AddBoolToObject(room_json, "joined", joined_status);

            // 방을 만든 사람의 프로필 사진 및 이름 추가
            const char *profile_image = NULL;
            for (size_t i = 0; i < user_list.size; i++) {
                if (user_list.entries[i].user_id == created_by) {
                    profile_image = user_list.entries[i].image_address;
                    break;
                }
            }

            if (profile_image) {
                cJSON_AddStringToObject(room_json, "profile_image", profile_image);
            } else {
                cJSON_AddNullToObject(room_json, "profile_image");
            }

            cJSON_AddItemToArray(room_list_json, room_json);
        }
    }
    fclose(file);

    // 유저 리스트 메모리 해제
    free_user_list(&user_list);



    cJSON_AddItemToObject(response_json, "rooms", room_list_json);

    const char *response_string = cJSON_PrintUnformatted(response_json);
    char http_response[2048];
    snprintf(http_response, sizeof(http_response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "%s\r\n"
             "%s", cors_headers, response_string);

    write(client_socket, http_response, strlen(http_response));

    cJSON_Delete(response_json);
    free((void *)response_string);
}