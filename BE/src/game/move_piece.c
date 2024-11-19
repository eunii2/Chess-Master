#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "game.h"
#include "config.h"
#include "cJSON.h"

// 체스판 데이터 (8x8)
static char chessboard[8][8] = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
};

// 게임 종료 여부
bool is_game_over = false;

// 킹 잡힘 여부 확인 함수
bool is_king_captured() {
    bool white_king_exists = false;
    bool black_king_exists = false;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (chessboard[row][col] == 'K') white_king_exists = true;
            if (chessboard[row][col] == 'k') black_king_exists = true;
        }
    }

    return !(white_king_exists && black_king_exists); // 킹 둘 중 하나라도 없으면 true 반환
}

// 퀸 이동 규칙 (대각선, 직선)
bool is_valid_queen_move(int from_row, int from_col, int to_row, int to_col) {
    return is_valid_rook_move(from_row, from_col, to_row, to_col) ||
           is_valid_bishop_move(from_row, from_col, to_row, to_col);
}

// 룩 이동 규칙 (직선)
bool is_valid_rook_move(int from_row, int from_col, int to_row, int to_col) {
    if (from_row == to_row) { // 가로 이동
        for (int col = (from_col < to_col ? from_col + 1 : from_col - 1); col != to_col; col += (from_col < to_col ? 1 : -1)) {
            if (chessboard[from_row][col] != ' ') return false;
        }
        return true;
    } else if (from_col == to_col) { // 세로 이동
        for (int row = (from_row < to_row ? from_row + 1 : from_row - 1); row != to_row; row += (from_row < to_row ? 1 : -1)) {
            if (chessboard[row][from_col] != ' ') return false;
        }
        return true;
    }
    return false;
}

// 비숍 이동 규칙 (대각선)
bool is_valid_bishop_move(int from_row, int from_col, int to_row, int to_col) {
    if (abs(from_row - to_row) != abs(from_col - to_col)) return false;
    int row_step = (to_row > from_row) ? 1 : -1;
    int col_step = (to_col > from_col) ? 1 : -1;

    for (int row = from_row + row_step, col = from_col + col_step; row != to_row; row += row_step, col += col_step) {
        if (chessboard[row][col] != ' ') return false;
    }
    return true;
}

// 나이트 이동 규칙 (L자 이동)
bool is_valid_knight_move(int from_row, int from_col, int to_row, int to_col) {
    int row_diff = abs(from_row - to_row);
    int col_diff = abs(from_col - to_col);
    return (row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff == 2);
}

// 킹 이동 규칙 (1칸 이동)
bool is_valid_king_move(int from_row, int from_col, int to_row, int to_col) {
    return abs(from_row - to_row) <= 1 && abs(from_col - to_col) <= 1;
}

// 폰 이동 규칙 (전진 및 대각선 이동)
bool is_valid_pawn_move(int from_row, int from_col, int to_row, int to_col, char piece) {
    int direction = (piece == 'P') ? -1 : 1; // 백폰은 위로, 흑폰은 아래로
    if (from_col == to_col) { // 전진
        if (to_row - from_row == direction && chessboard[to_row][to_col] == ' ') return true;
        if ((from_row == 1 && piece == 'p' && to_row - from_row == 2 * direction) ||
            (from_row == 6 && piece == 'P' && to_row - from_row == 2 * direction)) {
            return chessboard[from_row + direction][from_col] == ' ' &&
                   chessboard[to_row][to_col] == ' ';
        }
    } else if (abs(from_col - to_col) == 1 && to_row - from_row == direction) { // 대각선 공격
        return chessboard[to_row][to_col] != ' ';
    }
    return false;
}

// 이동 유효성 확인 함수
bool is_valid_move(int from_row, int from_col, int to_row, int to_col, char piece) {
    char target_piece = chessboard[to_row][to_col];
    if ((piece >= 'A' && piece <= 'Z' && target_piece >= 'A' && target_piece <= 'Z') ||
        (piece >= 'a' && piece <= 'z' && target_piece >= 'a' && target_piece <= 'z')) {
        return false; // 같은 팀이면 이동 불가
    }

    switch (piece) {
        case 'Q': case 'q': return is_valid_queen_move(from_row, from_col, to_row, to_col);
        case 'R': case 'r': return is_valid_rook_move(from_row, from_col, to_row, to_col);
        case 'B': case 'b': return is_valid_bishop_move(from_row, from_col, to_row, to_col);
        case 'N': case 'n': return is_valid_knight_move(from_row, from_col, to_row, to_col);
        case 'K': case 'k': return is_valid_king_move(from_row, from_col, to_row, to_col);
        case 'P': case 'p': return is_valid_pawn_move(from_row, from_col, to_row, to_col, piece);
        default: return false;
    }
}

void move_piece_handler(int client_socket, cJSON *json_request) {
    const cJSON *from_position_json = cJSON_GetObjectItemCaseSensitive(json_request, "from_position");
    const cJSON *to_position_json = cJSON_GetObjectItemCaseSensitive(json_request, "to_position");

    if (!cJSON_IsString(from_position_json) || !cJSON_IsString(to_position_json)) {
        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Invalid input\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    const char *from_position = from_position_json->valuestring;
    const char *to_position = to_position_json->valuestring;

    int from_row = 8 - (from_position[1] - '0');
    int from_col = from_position[0] - 'a';
    int to_row = 8 - (to_position[1] - '0');
    int to_col = to_position[0] - 'a';

    char piece = chessboard[from_row][from_col];
    if (piece == ' ') {
        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"No piece at source\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    if (!is_valid_move(from_row, from_col, to_row, to_col, piece)) {
        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Invalid move\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 체스판 상태 업데이트
    chessboard[to_row][to_col] = piece;
    chessboard[from_row][from_col] = ' ';

    // 킹이 잡혔는지 확인
    if (is_king_captured()) {
        is_game_over = true;

        const char *end_response =
                "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"status\":\"success\",\"message\":\"King captured. Game over\"}";
        write(client_socket, end_response, strlen(end_response));
        printf("Game over: A king has been captured.\n");
        return;
    }

    // 성공 응답 반환
    char success_response[512];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"status\":\"success\",\"message\":\"Move successful\"}");
    write(client_socket, success_response, strlen(success_response));
}
