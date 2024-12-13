#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "cJSON.h"
#include "config.h"
#include "game.h"
#include "utils.h"
#include "image.h"

// 게임 상태 배열 및 초기화 플래그
static GameState game_states[10];
static int initialized = 0;

// 특정 방의 게임 상태를 초기화하고 저장. 체스판 상태와 기본값을 설정
GameState* create_game_state(int room_id) {
    for (int i = 0; i < 10; i++) {
        if (game_states[i].room_id == 0) {
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
            game_state->game_over = 0;
            return game_state;
        }
    }

    return NULL;
}

// 요청된 방의 게임 상태를 반환하거나 새로운 상태를 생성
GameState* get_game_state(int room_id) {
    if (!initialized) {
        memset(game_states, 0, sizeof(game_states));
        initialized = 1;
    }

    for (int i = 0; i < 10; i++) {
        if (game_states[i].room_id == room_id) {
            return &game_states[i];
        }
    }
    return create_game_state(room_id);
}

// 게임 상태 핸들러 (클라이언트 요청 처리)
void get_game_status_handler(int client_socket, cJSON *json_request) {
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");

    if (!cJSON_IsNumber(room_id_json)) {
        // 유효하지 않은 방 ID 요청 처리
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
        // 요청된 방 ID가 존재하지 않을 경우
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

    // 토큰으로 유저 ID 조회
    int player1_id = get_user_id_by_token(game_state->player1_token);
    int player2_id = get_user_id_by_token(game_state->player2_token);

    // 프로필 이미지 URL 조회
    UserList user_list = {NULL, 0, 0};
    load_user_list(&user_list);

    const char *player1_image = NULL;
    const char *player2_image = NULL;

    // player1의 이미지 찾기
    for (size_t i = 0; i < user_list.size; i++) {
        if (user_list.entries[i].user_id == player1_id) {
            player1_image = user_list.entries[i].image_address;
            break;
        }
    }
    // player2의 이미지 찾기
    for (size_t i = 0; i < user_list.size; i++) {
        if (user_list.entries[i].user_id == player2_id) {
            player2_image = user_list.entries[i].image_address;
            break;
        }
    }

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
    cJSON_AddBoolToObject(response_json, "game_over", game_state->game_over);

    // 게임이 종료된 경우 승자와 종료 이유 추가
    if (game_state->game_over) {
        cJSON_AddStringToObject(response_json, "winner_token", game_state->winner_token);
        // 기권으로 인한 종료인지 확인
        if (game_state->forfeit_token[0] != '\0') {
            cJSON_AddStringToObject(response_json, "game_over_reason", "forfeit");
        } else {
            cJSON_AddStringToObject(response_json, "game_over_reason", "king_captured");
        }
    }

    // 체스판 상태를 JSON 배열로 추가
    cJSON *board_array = cJSON_CreateArray();
    for (int i = 0; i < 8; i++) {
        char row[9];
        strncpy(row, game_state->board[i], 8);
        row[8] = '\0';
        cJSON_AddItemToArray(board_array, cJSON_CreateString(row));
    }
    cJSON_AddItemToObject(response_json, "board", board_array);

    // 프로필 이미지 URL 추가
    cJSON_AddStringToObject(response_json, "player1_profile_image", 
                           player1_image ? player1_image : "");
    cJSON_AddStringToObject(response_json, "player2_profile_image", 
                           player2_image ? player2_image : "");

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

    free_user_list(&user_list);
    free(json_str);
    cJSON_Delete(response_json);
}
