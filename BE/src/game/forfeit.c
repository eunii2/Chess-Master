#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "game.h"
#include "config.h"
#include "cJSON.h"

// 기권 처리 함수
void set_forfeit() {
    is_game_over = true; // 게임 종료 상태 설정
    printf("Player has forfeited the game.\n");
}

// 기권 요청 처리 핸들러
void forfeit_game_handler(int client_socket, cJSON *json_request) {
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");
    const cJSON *player_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "player_id");

    // JSON 유효성 검사
    if (!cJSON_IsNumber(room_id_json) || !cJSON_IsNumber(player_id_json)) {
        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid room_id or player_id";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    int room_id = room_id_json->valueint;
    int player_id = player_id_json->valueint;

    // 특정 방의 게임 상태 가져오기
    GameState *game_state = get_game_state(room_id);
    if (!game_state) {
        const char *error_response =
                "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRoom not found";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 게임 종료 상태 설정
    game_state->game_over = 1;

    // 성공 응답 반환
    char response[512];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n\r\n"
             "{\"status\":\"success\",\"message\":\"Player %d forfeited the game\",\"room_id\":%d}",
             player_id, room_id);
    write(client_socket, response, strlen(response));

    // 서버 로그 출력
    printf("Player %d forfeited the game in room %d.\n", player_id, room_id);
}
