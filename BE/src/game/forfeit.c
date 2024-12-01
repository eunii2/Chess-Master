#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "game.h"
#include "config.h"
#include "cJSON.h"
#include "utils.h"

// 기권 처리 함수
void set_forfeit() {
    is_game_over = true; // 게임 종료 상태 설정
    printf("Player has forfeited the game.\n");
}

// 기권 요청 처리 핸들러
void forfeit_game_handler(int client_socket, cJSON *json_request) {
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

    /// 특정 방의 게임 상태 가져오기
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

    // 토큰 검증 및 플레이어 확인
    const char *player_name = NULL;
    if (strcmp(game_state->player1_token, token) == 0) {
        player_name = get_user_name_by_token(game_state->player1_token);
    } else if (strcmp(game_state->player2_token, token) == 0) {
        player_name = get_user_name_by_token(game_state->player2_token);
    } else {
        const char *response =
                "HTTP/1.1 401 Unauthorized\r\n"
                "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n\r\n"
                "{\"status\":\"error\",\"message\":\"Invalid token\"}";
        write(client_socket, response, strlen(response));
        return;
    }

    // 게임 종료 상태 설정
    game_state->game_over = 1;

    // 상대방의 승리자 확인
    char *winner = NULL;
    if (strcmp(game_state->player1_token, token) == 0) {
        winner = get_user_name_by_token(game_state->player2_token);
    } else if (strcmp(game_state->player2_token, token) == 0) {
        winner = get_user_name_by_token(game_state->player1_token);
    }

    // 승자 기록을 history.txt에 추가
    char game_log_path[256];
    snprintf(game_log_path, sizeof(game_log_path), GAME_HISTORY, room_id);
    FILE *log_file = fopen(game_log_path, "a");
    if (log_file) {
        fprintf(log_file, "Game Over! Winner by forfeit: %s\n", winner ? winner : "Unknown");
        fclose(log_file);
    }

    // 성공 응답 반환
    char success_response[512];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 200 OK\r\n"
             "Access-Control-Allow-Origin: http://localhost:5173\r\n"
             "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
             "Access-Control-Allow-Headers: Content-Type\r\n"
             "Content-Type: application/json\r\n\r\n"
             "{\"status\":\"success\",\"message\":\"%s forfeited the game. Winner: %s\",\"room_id\":%d}",
             player_name, winner ? winner : "Unknown", room_id);
    write(client_socket, success_response, strlen(success_response));

    // 서버 로그 출력
    printf("Game in room %d forfeited by %s. Winner: %s\n", room_id, player_name, winner);

}