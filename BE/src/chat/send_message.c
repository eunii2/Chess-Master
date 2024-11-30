#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "config.h"
#include "game.h"
#include "cJSON.h"
#include "utils.h"
#include "chat.h"

// 채팅 메시지 처리 핸들러
void send_message_handler(int client_socket, cJSON *json_request) {
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");
    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(json_request, "token");
    const cJSON *message_json = cJSON_GetObjectItemCaseSensitive(json_request, "message");

    // JSON 유효성 검사
    if (!cJSON_IsNumber(room_id_json) || !cJSON_IsString(token_json) || !cJSON_IsString(message_json)) {
        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Invalid input\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    int room_id = room_id_json->valueint;
    const char *token = token_json->valuestring;
    const char *message = message_json->valuestring;

    // 게임 상태 확인
    GameState *game_state = get_game_state(room_id);
    if (!game_state) {
        const char *error_response =
                "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Room not found\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // **방에 참여한 플레이어인지 확인**
    const char *username = NULL;
    if (strcmp(game_state->player1_token, token) == 0) {
        username = get_user_name_by_token(token); // player1의 username 가져오기
    } else if (strcmp(game_state->player2_token, token) == 0) {
        username = get_user_name_by_token(token); // player2의 username 가져오기
    } else {
        const char *error_response =
                "HTTP/1.1 403 Forbidden\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Token not authorized for this room\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    if (!username) {
        const char *error_response =
                "HTTP/1.1 500 Internal Server Error\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Failed to retrieve username\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    char chat_file_path[256];
    snprintf(chat_file_path, sizeof(chat_file_path), "../data/game/%d/chat.txt", room_id);

    FILE *chat_file = fopen(chat_file_path, "a");
    if (!chat_file) {
        perror("fopen error");
        const char *error_response =
                "HTTP/1.1 500 Internal Server Error\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Failed to save message\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    fprintf(chat_file, "%s: %s\n", username, message);
    fclose(chat_file);

    // 성공 응답 반환
    const char *success_response =
            "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"status\":\"success\",\"message\":\"Message sent\"}";
    write(client_socket, success_response, strlen(success_response));

    // 서버 로그 출력
    printf("Message from %s in room %d: %s\n", username, room_id, message);
}
