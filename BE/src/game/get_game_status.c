#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "cJSON.h"
#include "config.h"
#include "game.h"
#include "utils.h"

// 게임 상태 배열 및 초기화 플래그
static GameState game_states[10];
static int initialized = 0;

// 새로운 GameState 생성 함수
GameState* create_game_state(int room_id) {
    for (int i = 0; i < 10; i++) {
        if (game_states[i].room_id == 0) { // 빈 슬롯 찾기
            GameState* game_state = &game_states[i];
            game_state->room_id = room_id;

            // 체스판 초기화
            for (int row = 0; row < 8; row++) {
                for (int col = 0; col < 8; col++) {
                    game_state->board[row][col] = '.'; // 빈 칸은 '.'으로 표시
                }
            }

            // 기본값 설정
            memset(game_state->player1_token, 0, sizeof(game_state->player1_token));
            memset(game_state->player2_token, 0, sizeof(game_state->player2_token));
            memset(game_state->current_player_token, 0, sizeof(game_state->current_player_token));
            game_state->game_over = 0;       // 초기 상태: 게임 진행 중
            return game_state;
        }
    }

    return NULL; // 빈 슬롯이 없으면 NULL 반환
}

// 특정 방의 게임 상태를 반환
GameState* get_game_state(int room_id) {
    if (!initialized) {
        memset(game_states, 0, sizeof(game_states)); // 초기화
        initialized = 1;
    }

    for (int i = 0; i < 10; i++) {
        if (game_states[i].room_id == room_id) {
            return &game_states[i];
        }
    }
    // 방 ID가 존재하지 않으면 새로운 GameState 생성
    return create_game_state(room_id);
}

// 게임 상태 핸들러 (클라이언트 요청 처리)
void get_game_status_handler(int client_socket, cJSON *json_request) {
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");

    if (!cJSON_IsNumber(room_id_json)) {
        const char *response =
                "HTTP/1.1 400 Bad Request\r\n"
                "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n\r\n"
                "{\"status\":\"error\",\"message\":\"Invalid room_id\"}";
        write(client_socket, response, strlen(response));
        return;
    }

    int room_id = room_id_json->valueint;
    GameState* game_state = get_game_state(room_id);

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

    // 토큰으로 사용자 이름 조회
    char* player1_username = get_user_name_by_token(game_state->player1_token);
    char* player2_username = get_user_name_by_token(game_state->player2_token);

    // 디버깅을 위한 로그 출력
    printf("Player 1 Token: %s\n", game_state->player1_token);
    printf("Player 2 Token: %s\n", game_state->player2_token);
    printf("Player 1 Username: %s\n", player1_username);
    printf("Player 2 Username: %s\n", player2_username);

    // JSON 응답 생성
    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddNumberToObject(response_json, "room_id", game_state->room_id);
    cJSON_AddStringToObject(response_json, "current_player_token", game_state->current_player_token);
    cJSON_AddStringToObject(response_json, "player1_token", game_state->player1_token);
    cJSON_AddStringToObject(response_json, "player2_token", game_state->player2_token);
    cJSON_AddStringToObject(response_json, "player1_username",
                            player1_username ? player1_username : "Player 1");
    cJSON_AddStringToObject(response_json, "player2_username",
                            player2_username ? player2_username : "Player 2");
    cJSON_AddStringToObject(response_json, "player1_username", player1_username);
    cJSON_AddStringToObject(response_json, "player2_username", player2_username);
    cJSON_AddBoolToObject(response_json, "game_over", game_state->game_over);

    // 체스판 상태를 JSON 배열로 추가
    cJSON *board_array = cJSON_CreateArray();
    for (int i = 0; i < 8; i++) {
        char row[9]; // 각 행은 최대 8개의 문자 + NULL
        strncpy(row, game_state->board[i], 8);
        row[8] = '\0'; // 문자열 종료
        cJSON_AddItemToArray(board_array, cJSON_CreateString(row));
    }
    cJSON_AddItemToObject(response_json, "board", board_array);

    // JSON 응답 전송
    char *json_str = cJSON_Print(response_json);
    char response[4096];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Access-Control-Allow-Origin: http://localhost:5173\r\n"
             "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
             "Access-Control-Allow-Headers: Content-Type\r\n"
             "Content-Type: application/json\r\n\r\n"
             "%s", json_str);

    write(client_socket, response, strlen(response));

    // 메모리 해제
    free(json_str);
    cJSON_Delete(response_json);
}
