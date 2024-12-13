#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "game.h"
#include "config.h"
#include "cJSON.h"
#include "utils.h"

// 게임을 기권으로 종료하는 함수
void set_forfeit() {
    is_game_over = true; // 게임 종료 상태 설정
    printf("Player has forfeited the game.\n");
}

// 클라이언트로부터의 기권 요청을 처리하는 함수
void forfeit_game_handler(int client_socket, cJSON *json_request) {
    // 요청에서 room_id와 token 추출
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");
    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(json_request, "token");

    // JSON 유효성 검사
    if (!cJSON_IsNumber(room_id_json) || !cJSON_IsString(token_json)) {
        // 유효하지 않은 요청에 대한 응답
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

    // 토큰을 통해 플레이어 검증
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

    // 게임 종료 상태와 기권 정보 업데이트
    game_state->game_over = 1;
    strncpy(game_state->forfeit_token, token, TOKEN_LENGTH);
    strncpy(game_state->game_over_reason, "forfeit", sizeof(game_state->game_over_reason));

    // 상대 플레이어를 승리자로 설정
    char *winner = NULL;
    if (strcmp(game_state->player1_token, token) == 0) {
        winner = get_user_name_by_token(game_state->player2_token);
        snprintf(game_state->winner_token, sizeof(game_state->winner_token), "%s", game_state->player2_token);

    } else {
        winner = get_user_name_by_token(game_state->player1_token);
        snprintf(game_state->winner_token, sizeof(game_state->winner_token), "%s", game_state->player1_token);

    }

    // 기권 기록을 히스토리 파일에 저장
    char history_file[256];
    snprintf(history_file, sizeof(history_file), "../data/game/%d/history.txt", room_id);
    FILE *history = fopen(history_file, "a");
    if (history) {
        fprintf(history, "Game Over! The opponent has forfeited. Winner: %s\n", winner ? winner : "Unknown");
        fclose(history);
    }

    // 성공 응답 반환
    char success_response[512];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 200 OK\r\n"
             "Access-Control-Allow-Origin: http://localhost:5173\r\n"
             "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
             "Access-Control-Allow-Headers: Content-Type\r\n"
             "Content-Type: application/json\r\n\r\n"
             "{\"status\":\"success\",\"message\":\"Game forfeited\","
             "\"game_over\":true,\"winner_token\":\"%s\","
             "\"game_over_reason\":\"forfeit\"}",
             game_state->winner_token);

    write(client_socket, success_response, strlen(success_response));

    // 서버 로그 출력
    printf("Game in room %d forfeited by %s. Winner: %s\n", room_id, player_name, winner);

}