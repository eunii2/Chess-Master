#ifndef SERVER_PROJECT_GAME_H
#define SERVER_PROJECT_GAME_H

#include <pthread.h>
#include "cJSON.h"
#include <stdbool.h>

// 게임 로직 함수
void* game_thread(void* arg);
void start_game_in_room(int room_id);
void set_forfeit();

// 핸들러 함수
void start_game_handler(int client_socket, cJSON *json_request);
void forfeit_game_handler(int client_socket, cJSON *json_request);
void get_game_status_handler(int client_socket, cJSON *json_request);
void move_piece_handler(int client_socket, cJSON *json_request);

bool is_valid_rook_move(int from_row, int from_col, int to_row, int to_col);
bool is_valid_bishop_move(int from_row, int from_col, int to_row, int to_col);
bool is_valid_knight_move(int from_row, int from_col, int to_row, int to_col);
bool is_valid_queen_move(int from_row, int from_col, int to_row, int to_col);
bool is_valid_pawn_move(int from_row, int from_col, int to_row, int to_col, char piece);
bool is_valid_king_move(int from_row, int from_col, int to_row, int to_col);

#endif