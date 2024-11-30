#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "game.h"
#include "cJSON.h"
#include "utils.h"

// 체스판 데이터
char chessboard[8][8];

void start_game_in_room(int room_id) {
    pthread_t thread;
    int* args = malloc(2 * sizeof(int));
    if (!args) {
        perror("malloc failed");
        return;
    }
    args[0] = room_id;
    args[1] = 0;

    if (pthread_create(&thread, NULL, game_thread, args) != 0) {
        perror("pthread_create failed");
        free(args);
        return;
    }

    // 스레드를 분리하여 실행
    pthread_detach(thread);
    printf("Game thread for room %d started\n", room_id);
}

void setup_initial_board(char board[8][8]) {
    const char initial_board[8][8] = {
            {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
            {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
            {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
            {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };
    memcpy(board, initial_board, sizeof(initial_board));
}

void start_game_handler(int client_socket, cJSON *json_request) {
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");

    if (!cJSON_IsNumber(room_id_json)) {
        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid room_id";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    int room_id = room_id_json->valueint;

    // 게임 상태 업데이트
    GameState *game_state = get_game_state(room_id);
    if (!game_state) {
        const char *error_response =
                "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRoom not found";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // **방장 토큰 확인 추가**
    if (strlen(game_state->player1_token) == 0) {
        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nHost token not set";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // **두 명의 플레이어가 참가했는지 확인**
    if (strlen(game_state->player1_token) == 0 || strlen(game_state->player2_token) == 0) {
        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n"
                "{\"status\":\"error\",\"message\":\"Not enough players to start the game\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 체스판 초기 상태 설정
    setup_initial_board(game_state->board);

    // 게임 디렉토리 생성
    char game_dir[256];
    snprintf(game_dir, sizeof(game_dir), "../data/game/%d", room_id);
    create_directory(game_dir);

    // history 및 chat 파일 생성
    char history_file[256], chat_file[256];
    snprintf(history_file, sizeof(history_file), "%s/history.txt", game_dir);
    snprintf(chat_file, sizeof(chat_file), "%s/chat.txt", game_dir);
    FILE *history = fopen(history_file, "w");
    FILE *chat = fopen(chat_file, "w");
    if (history) fclose(history);
    if (chat) fclose(chat);

    // **디버깅 메시지 추가**
    printf("Debug: Starting game in room %d. Host token: %s\n", room_id, game_state->player1_token);

    // 방장이 선공하도록 설정
    strncpy(game_state->current_player_token, game_state->player1_token, TOKEN_LENGTH);
    game_state->current_player_token[TOKEN_LENGTH] = '\0';

    // 성공 응답 반환
    char response[512];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n\r\n"
             "{\"status\":\"success\",\"message\":\"Game started\",\"room_id\":%d}",
             room_id);
    write(client_socket, response, strlen(response));
}
