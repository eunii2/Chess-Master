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

// 토큰으로부터 사용자 이름을 가져오는 함수
const char* get_username_from_token(const char* token);

// 방 ID에 따라 게임 스레드를 생성하는 함수
void start_game_in_room(int room_id) {
    pthread_t thread;
    int* args = malloc(2 * sizeof(int));
    if (!args) {
        perror("malloc failed");
        return;
    }
    args[0] = room_id;
    args[1] = 0;

    // 게임 스레드 생성
    if (pthread_create(&thread, NULL, game_thread, args) != 0) {
        perror("pthread_create failed");
        free(args);
        return;
    }

    // 스레드를 분리하여 실행
    pthread_detach(thread);
    printf("Game thread for room %d started\n", room_id);
}

// 체스판의 초기 상태를 설정하는 함수
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

// 게임 시작 요청을 처리하는 함수
void start_game_handler(int client_socket, cJSON *json_request) {
    // JSON 요청에서 방 ID와 토큰을 추출
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");
    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(json_request, "token");

    printf("Received start game request - room_id: %s, token: %s\n",
           cJSON_Print(room_id_json),
           cJSON_GetStringValue(token_json));

    // JSON 유효성 검증
    if (!cJSON_IsNumber(room_id_json) || !cJSON_IsString(token_json)) {
        const char *response =
                "HTTP/1.1 400 Bad Request\r\n"
                "Access-Control-Allow-Origin: http://localhost:5173\r\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Type: application/json\r\n"
                "Content-Length: 47\r\n"
                "\r\n"
                "{\"status\":\"error\",\"message\":\"Invalid input\"}";
        write(client_socket, response, strlen(response));
        return;
    }

    int room_id = room_id_json->valueint;
    const char *token = token_json->valuestring;

    // 게임 상태 업데이트
    GameState *game_state = get_game_state(room_id);

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

    printf("Comparing tokens - Player1: '%s', Received: '%s'\n",
           game_state->player1_token, 
           token);

    // 방장 토큰 확인
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
    game_state->game_started = 1;

    // 응답 JSON 생성
    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "message", "Game started");
    cJSON_AddNumberToObject(response_json, "room_id", room_id);
    
    char *json_str = cJSON_Print(response_json);
    int json_len = strlen(json_str);

    char response_buffer[1024];
    int response_len = snprintf(response_buffer, sizeof(response_buffer),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Access-Control-Allow-Origin: http://localhost:5173\r\n"
        "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "\r\n"
        "%s", json_len, json_str);

    write(client_socket, response_buffer, response_len);
    
    cJSON_Delete(response_json);
    free(json_str);

    printf("Response sent: %s\n", response_buffer);

    // 두 명의 플레이어가 참가했는지 확인
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
    char history_file[256], chat_file[256], info_file[256];
    snprintf(history_file, sizeof(history_file), "%s/history.txt", game_dir);
    snprintf(chat_file, sizeof(chat_file), "%s/chat.txt", game_dir);
    snprintf(info_file, sizeof(info_file), "%s/info.txt", game_dir);

    FILE *history = fopen(history_file, "w");
    FILE *chat = fopen(chat_file, "w");
    FILE *info = fopen(info_file, "w");

    if (info) {
        char* player1_username = get_user_name_by_token(game_state->player1_token);
        char* player2_username = get_user_name_by_token(game_state->player2_token);

        // room_list.txt에서 방 이름 가져오기
        char room_name[256] = "";
        FILE *room_list = fopen(ROOM_LIST_FILE, "r");
        if (room_list) {
            char line[512];
            while (fgets(line, sizeof(line), room_list)) {
                int current_room_id;
                char current_room_name[256];
                if (sscanf(line, "Room ID: %d, Room Name: %[^,]", &current_room_id, current_room_name) == 2) {
                    if (current_room_id == room_id) {
                        strcpy(room_name, current_room_name);
                        break;
                    }
                }
            }
            fclose(room_list);
        }

        fprintf(info, "Room Name: %s\n", room_name);
        fprintf(info, "Username: %s, Token: %s\n", player1_username, game_state->player1_token);
        fprintf(info, "Username: %s, Token: %s\n", player2_username, game_state->player2_token);
        fclose(info);
    }

    if (history) fclose(history);
    if (chat) fclose(chat);

    printf("Debug: Starting game in room %d. Host token: %s\n", room_id, game_state->player1_token);

    // 방장이 선공하도록 설정
    strncpy(game_state->current_player_token, game_state->player1_token, TOKEN_LENGTH);
    game_state->current_player_token[TOKEN_LENGTH] = '\0';

    // 시작된 게임은 Room List에서 방 제거 로직
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

    if (remove(ROOM_LIST_FILE) != 0) {
        perror("Error removing room_list.txt");
    }
    if (rename("../data/temp_room_list.txt", ROOM_LIST_FILE) != 0) {
        perror("Error renaming temp_room_list.txt to room_list.txt");
    }
}
