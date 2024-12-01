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
    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(json_request, "token");

    // 디버그 로그 추가
    printf("Received start game request - room_id: %s, token: %s\n",
           cJSON_Print(room_id_json),
           cJSON_GetStringValue(token_json));

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

    // 게임 상태 업데이트
    GameState *game_state = get_game_state(room_id);

    // 게임 상태 디버그 로그
    if (game_state) {
        printf("Game state found - player1_token: %s, player2_token: %s\n",
               game_state->player1_token,
               game_state->player2_token);
    } else {
        printf("Game state not found for room_id: %d\n", room_id);
    }

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

    // 토큰이 방장의 토큰인지 확인
    if (strcmp(game_state->player1_token, token) != 0) {
        const char *response =
                "HTTP/1.1 403 Forbidden\r\n"
                "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n\r\n"
                "{\"status\":\"error\",\"message\":\"Only room creator can start the game\"}";
        write(client_socket, response, strlen(response));
        return;
    }

    // 게임 시작 상태 업데이트
    game_state->game_started = 1;  // 게임 시작 상태 설정

    // 참가자에게 게임 시작 알림 전송
    const char *response = "{\"status\":\"success\",\"message\":\"Game started\"}";
    write(client_socket, response, strlen(response));

    // **두 명의 플레이어가 참가했는지 확인**
    if (strlen(game_state->player1_token) == 0 || strlen(game_state->player2_token) == 0) {
        const char *response =
                "HTTP/1.1 400 Bad Request\r\n"
                "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n\r\n"
                "{\"status\":\"error\",\"message\":\"Waiting for players to join\"}";
        write(client_socket, response, strlen(response));
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

    // Room List에서 방 제거 로직 추가
    FILE *file = fopen(ROOM_LIST_FILE, "r");
    if (!file) {
        const char *response =
                "HTTP/1.1 500 Internal Server Error\r\n"
                "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n\r\n"
                "{\"status\":\"error\",\"message\":\"Failed to update room list\"}";
        write(client_socket, response, strlen(response));
        return;
    }

    FILE *temp_file = fopen("../data/temp_room_list.txt", "w");
    if (!temp_file) {
        perror("fopen error");
        fclose(file);
        const char *error_response =
                "HTTP/1.1 500 Internal Server Error\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Failed to update room list\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    char buffer[512];
    while (fgets(buffer, sizeof(buffer), file)) {
        int current_room_id;
        sscanf(buffer, "Room ID: %d,", &current_room_id);
        if (current_room_id != room_id) {
            fputs(buffer, temp_file);
        } else {
            printf("Debug: Removing room ID %d from room_list.txt\n", current_room_id);
        }
    }

    fclose(file);
    fclose(temp_file);

    // 원본 파일을 업데이트
    if (remove(ROOM_LIST_FILE) != 0) {
        perror("Error removing room_list.txt");
    }
    if (rename("../data/temp_room_list.txt", ROOM_LIST_FILE) != 0) {
        perror("Error renaming temp_room_list.txt to room_list.txt");
    }

    // 성공 응답 반환
    char success_response[512];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 200 OK\r\n"
             "Access-Control-Allow-Origin: http://localhost:5173\r\n"
             "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
             "Access-Control-Allow-Headers: Content-Type\r\n"
             "Content-Type: application/json\r\n\r\n"
             "{\"status\":\"success\",\"message\":\"Game started\",\"room_id\":%d}",
             room_id);
    write(client_socket, success_response, strlen(success_response));
}
