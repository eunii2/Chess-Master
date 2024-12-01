#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include "game.h"
#include "cJSON.h"
#include "utils.h"
#include "chat.h"

void get_messages_handler(int client_socket, cJSON *json_request) {
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");
    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(json_request, "token");

    // JSON 유효성 검사
    if (!cJSON_IsNumber(room_id_json) || !cJSON_IsString(token_json)) {
        const char *response =
                "HTTP/1.1 400 Bad Request\r\n"
                "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n\r\n"
                "{\"status\":\"error\",\"message\":\"Invalid input\"}";
        write(client_socket, response, strlen(response));
        return;
    }

    int room_id = room_id_json->valueint;
    const char *token = token_json->valuestring;

    // 게임 상태 확인
    GameState *game_state = get_game_state(room_id);
    if (!game_state) {
        const char *response =
                "HTTP/1.1 404 Not Found\r\n"
                "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n\r\n"
                "{\"status\":\"error\",\"message\":\"Room not found\"}";
        write(client_socket, response, strlen(response));
        return;
    }

    // 권한 확인
    if (strcmp(game_state->player1_token, token) != 0 && strcmp(game_state->player2_token, token) != 0) {
        const char *error_response =
                "HTTP/1.1 403 Forbidden\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Not authorized\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 채팅 파일 경로
    char chat_file_path[256];
    snprintf(chat_file_path, sizeof(chat_file_path), "../data/game/%d/chat.txt", room_id);

    // 채팅 메시지 읽기
    FILE *chat_file = fopen(chat_file_path, "r");
    if (!chat_file) {
        // 채팅 파일이 없는 경우 빈 메시지 배열 반환
        const char *empty_response =
                "HTTP/1.1 200 OK\r\n"
                "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n\r\n"
                "{\"status\":\"success\",\"messages\":[]}";
        write(client_socket, empty_response, strlen(empty_response));
        return;
    }

    // JSON 응답 생성
    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON *messages_array = cJSON_AddArrayToObject(response_json, "messages");

    char line[1024];
    while (fgets(line, sizeof(line), chat_file)) {
        // 줄바꿈 문자 제거
        line[strcspn(line, "\n")] = 0;

        // username과 message 분리
        char *separator = strchr(line, ':');
        if (separator) {
            *separator = '\0';
            char *username = line;
            char *message = separator + 2;  // ': ' 다음부터가 메시지

            cJSON *message_obj = cJSON_CreateObject();
            cJSON_AddStringToObject(message_obj, "username", username);
            cJSON_AddStringToObject(message_obj, "content", message);
            cJSON_AddItemToArray(messages_array, message_obj);
        }
    }

    fclose(chat_file);

    // JSON을 문자열로 변환
    char *response_str = cJSON_Print(response_json);

    // HTTP 응답 헤더 작성
    char response_header[1024];
    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 200 OK\r\n"
             "Access-Control-Allow-Origin: http://localhost:5173\r\n"
             "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
             "Access-Control-Allow-Headers: Content-Type\r\n"
             "Content-Type: application/json\r\n"
             "Content-Length: %zu\r\n\r\n",
             strlen(response_str));

    // 응답 전송
    write(client_socket, response_header, strlen(response_header));
    write(client_socket, response_str, strlen(response_str));

    // 메모리 해제
    free(response_str);
    cJSON_Delete(response_json);
}