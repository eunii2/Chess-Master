#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "cJSON.h"

// 게임 상태 구조체 정의
typedef struct {
    int room_id;               // 방 ID
    char board[8][8];          // 체스판 상태
    int current_player;        // 현재 턴 플레이어 (1 또는 2)
    int remaining_time;        // 남은 시간 (초)
    int game_over;             // 게임 종료 여부 (0: 진행 중, 1: 종료)
} GameState;

// 게임 상태 배열 및 초기화 플래그
static GameState game_states[10];
static int initialized = 0;

// 게임 상태 초기화 함수
void initialize_game_states() {
    if (initialized) return;

    for (int i = 0; i < 10; i++) {
        game_states[i].room_id = i + 1;
        memset(game_states[i].board, '.', sizeof(game_states[i].board));
        game_states[i].current_player = 1;
        game_states[i].remaining_time = 60;
        game_states[i].game_over = 0;
    }

    initialized = 1;
}

// 특정 방의 게임 상태를 반환
GameState* get_game_state(int room_id) {
    initialize_game_states();

    for (int i = 0; i < 10; i++) {
        if (game_states[i].room_id == room_id) {
            return &game_states[i];
        }
    }

    return NULL; // 방 ID가 존재하지 않으면 NULL 반환
}

// 게임 상태 핸들러 (클라이언트 요청 처리)
void get_game_status_handler(int client_socket, cJSON *json_request) {
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");

    if (!cJSON_IsNumber(room_id_json)) {
        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid room_id";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    int room_id = room_id_json->valueint;
    GameState* game_state = get_game_state(room_id);

    if (!game_state) {
        const char *error_response =
                "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRoom not found";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // JSON 응답 생성
    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddNumberToObject(response_json, "room_id", game_state->room_id);
    cJSON_AddNumberToObject(response_json, "current_player", game_state->current_player);
    cJSON_AddNumberToObject(response_json, "remaining_time", game_state->remaining_time);
    cJSON_AddBoolToObject(response_json, "game_over", game_state->game_over);

    // 체스판 상태를 JSON 배열로 추가
    cJSON *board_array = cJSON_CreateArray();
    for (int i = 0; i < 8; i++) {
        cJSON_AddItemToArray(board_array, cJSON_CreateString(game_state->board[i]));
    }
    cJSON_AddItemToObject(response_json, "board", board_array);

    // JSON 응답 전송
    char *response_string = cJSON_Print(response_json);
    char response[1024];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n%s",
             response_string);

    write(client_socket, response, strlen(response));

    // 메모리 해제
    cJSON_Delete(response_json);
    free(response_string);
}
