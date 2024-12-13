#ifndef SERVER_PROJECT_GAME_H
#define SERVER_PROJECT_GAME_H

#include <pthread.h>
#include "cJSON.h"
#include <stdbool.h>
#include "../config/config.h"

// GameState 구조체 정의
typedef struct {
    int room_id;
    char board[8][8];
    int current_player;
    int remaining_time;
    int game_over;
    char player1_username[50];
    char player2_username[50];
    char player1_token[TOKEN_LENGTH + 1];
    char player2_token[TOKEN_LENGTH + 1];
    char current_player_token[TOKEN_LENGTH + 1];
    int game_started;
    char winner_token[TOKEN_LENGTH+1];
    char game_over_reason[32];
    char forfeit_token[TOKEN_LENGTH];
} GameState;

// 체스판 관련 전역 변수
extern char chessboard[8][8];

// 게임 로직 함수
void* game_thread(void* arg);
void start_game_in_room(int room_id);
void set_forfeit();
void setup_initial_board(char board[8][8]);
GameState* get_game_state(int room_id);

// 핸들러 함수
void start_game_handler(int client_socket, cJSON *json_request);
void forfeit_game_handler(int client_socket, cJSON *json_request);
void get_game_status_handler(int client_socket, cJSON *json_request);
void move_piece_handler(int client_socket, cJSON *json_request);

bool is_valid_rook_move(int from_row, int from_col, int to_row, int to_col, char chessboard[8][8]);
bool is_valid_bishop_move(int from_row, int from_col, int to_row, int to_col, char piece, char chessboard[8][8]);
bool is_valid_knight_move(int from_row, int from_col, int to_row, int to_col);
bool is_valid_queen_move(int from_row, int from_col, int to_row, int to_col, char piece, char chessboard[8][8]);
bool is_valid_pawn_move(int from_row, int from_col, int to_row, int to_col, char piece, char chessboard[8][8]);
bool is_valid_king_move(int from_row, int from_col, int to_row, int to_col);

void get_game_history_handler(int client_socket, cJSON *json_request);
void get_user_game_history_handler(int client_socket, cJSON *json_request);

#endif