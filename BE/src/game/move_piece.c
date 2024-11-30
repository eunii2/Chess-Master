#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "game.h"
#include "config.h"
#include "cJSON.h"
#include "utils.h"

void print_board() {
    printf("Current Chessboard:\n");
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            printf("%c ", chessboard[row][col]);
        }
        printf("\n");
    }
}

// 게임 종료 여부
bool is_game_over = false;

bool is_king_captured(char chessboard[8][8]) {
    bool white_king_exists = false;
    bool black_king_exists = false;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (chessboard[row][col] == 'K') white_king_exists = true;
            if (chessboard[row][col] == 'k') black_king_exists = true;
        }
    }

    // 두 킹 중 하나가 없으면 true 반환
    return !(white_king_exists && black_king_exists);
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

// 폰 이동 규칙 (전진 및 대각선 공격)
bool is_valid_pawn_move(int from_row, int from_col, int to_row, int to_col, char piece, char chessboard[8][8]) {
    int direction = (piece == 'P') ? -1 : 1; // 백폰: 위로(-1), 흑폰: 아래로(+1)
    printf("Debug: Pawn at (%d, %d), target (%d, %d), direction: %d\n", from_row, from_col, to_row, to_col, direction);

    // 직진 이동
    if (from_col == to_col) {
        if (to_row - from_row == direction && chessboard[to_row][to_col] == ' ') {
            printf("Debug: Valid one-step forward move.\n");
            return true;
        }

        if ((from_row == 6 && piece == 'P' && to_row - from_row == 2 * direction) ||
            (from_row == 1 && piece == 'p' && to_row - from_row == 2 * direction)) {
            if (chessboard[from_row + direction][from_col] == ' ' && chessboard[to_row][to_col] == ' ') {
                printf("Debug: Valid two-step forward move.\n");
                return true;
            }
        }
    }

    // **대각선 공격** 추가됨
    if (abs(from_col - to_col) == 1 && to_row - from_row == direction) {
        char target = chessboard[to_row][to_col];
        if (target != ' ' &&
            ((piece == 'P' && target >= 'a' && target <= 'z') || // 백폰이 흑말 공격
             (piece == 'p' && target >= 'A' && target <= 'Z'))) { // 흑폰이 백말 공격
            printf("Debug: Valid diagonal attack move. Capturing '%c'.\n", target);
            return true;
        }
    }

    printf("Error: Pawn move from (%d, %d) to (%d, %d) is invalid.\n", from_row, from_col, to_row, to_col);
    return false;
}


// 이동 유효성 확인 함수
bool is_valid_move(int from_row, int from_col, int to_row, int to_col, char piece, char chessboard[8][8]) {
    char target_piece = chessboard[to_row][to_col];
    if ((piece >= 'A' && piece <= 'Z' && target_piece >= 'A' && target_piece <= 'Z') ||
        (piece >= 'a' && piece <= 'z' && target_piece >= 'a' && target_piece <= 'z')) {
        // 디버깅 출력: 이동 정보 출력
        printf("Debug: Checking move for piece '%c' from (%d, %d) to (%d, %d), target: '%c'\n",
               piece, from_row, from_col, to_row, to_col, target_piece);

        return false; // 같은 팀이면 이동 불가
    }

    switch (piece) {
        case 'Q': case 'q': return is_valid_queen_move(from_row, from_col, to_row, to_col);
        case 'R': case 'r': return is_valid_rook_move(from_row, from_col, to_row, to_col);
        case 'B': case 'b': return is_valid_bishop_move(from_row, from_col, to_row, to_col);
        case 'N': case 'n': return is_valid_knight_move(from_row, from_col, to_row, to_col);
        case 'K': case 'k': return is_valid_king_move(from_row, from_col, to_row, to_col);
        case 'P': case 'p': return is_valid_pawn_move(from_row, from_col, to_row, to_col, piece, chessboard);
        default: return false;
    }
}

void move_piece_handler(int client_socket, cJSON *json_request) {
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");
    const cJSON *from_position_json = cJSON_GetObjectItemCaseSensitive(json_request, "from_position");
    const cJSON *to_position_json = cJSON_GetObjectItemCaseSensitive(json_request, "to_position");
    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(json_request, "token");

    // JSON 유효성 검사
    if (!cJSON_IsNumber(room_id_json) || !cJSON_IsString(from_position_json) ||
        !cJSON_IsString(to_position_json) || !cJSON_IsString(token_json)) {
        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Invalid input\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    int room_id = room_id_json->valueint;
    const char *token = token_json->valuestring;

    // 토큰으로 user_id 가져오기
    int player_id = get_user_id_by_token(token);
    if (player_id == -1) {
        const char *error_response =
                "HTTP/1.1 401 Unauthorized\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Invalid token\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 게임 상태 가져오기
    GameState *game_state = get_game_state(room_id);
    if (!game_state) {
        const char *error_response =
                "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Room not found\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // **토큰 유효성 확인**
    if (strcmp(game_state->player1_token, token) != 0 && strcmp(game_state->player2_token, token) != 0) {
        const char *error_response =
                "HTTP/1.1 401 Unauthorized\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Invalid token\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    if (strcmp(game_state->current_player_token, token) != 0) {
        printf("Error: It is not the turn for token '%s'. Current turn token is '%s'\n",
               token, game_state->current_player_token);

        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Not your turn\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    printf("Debug: Player 1 Token: %s\n", game_state->player1_token);
    printf("Debug: Player 2 Token: %s\n", game_state->player2_token);
    printf("Debug: Current Player Token: %s\n", game_state->current_player_token);
    printf("Debug: Provided Token: %s\n", token);

    // 게임 종료 여부 확인
    if (game_state->game_over) {
        const char *error_response =
                "HTTP/1.1 403 Forbidden\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Game is already over\"}";
        write(client_socket, error_response, strlen(error_response));
        printf("Move attempt in room %d rejected: Game is already over.\n", room_id);
        return;
    }

    char (*chessboard)[8] = game_state->board;

    // 체스판 위치 변환
    const char *from_position = from_position_json->valuestring;
    const char *to_position = to_position_json->valuestring;

    int from_row = 8 - (from_position[1] - '0');
    int from_col = from_position[0] - 'a';
    int to_row = 8 - (to_position[1] - '0');
    int to_col = to_position[0] - 'a';

    char piece = chessboard[from_row][from_col];
    printf("Attempting to move piece '%c' from %s to %s\n", piece, from_position, to_position);

    // 유효하지 않은 소스 확인
    if (piece == ' ') {
        printf("Error: No piece at the source position (%s)\n", from_position);
        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"No piece at source\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 현재 플레이어 토큰 확인
    if (strcmp(game_state->current_player_token, token) != 0) {
        printf("Error: It is not the turn for token '%s'. Current turn token is '%s'\n",
               token, game_state->current_player_token);

        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Not your turn\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 이동 유효성 검사
    if (!is_valid_move(from_row, from_col, to_row, to_col, piece, chessboard)) {
        printf("Error: Invalid move from (%d, %d) to (%d, %d)\n", from_row, from_col, to_row, to_col);
        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"status\":\"error\",\"message\":\"Invalid move\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 체스판 상태 업데이트
    chessboard[to_row][to_col] = piece;
    chessboard[from_row][from_col] = ' ';

    // 현재 플레이어의 사용자 이름 가져오기
    char *player_name = get_user_name_by_token(token);

    // **history.txt에 기록 추가**
    char history_file[256];
    snprintf(history_file, sizeof(history_file), GAME_HISTORY, room_id);

    FILE *history = fopen(history_file, "a");
    if (history) {
        fprintf(history, "Player: %s moved %c from %s to %s\n",
                player_name ? player_name : "Unknown", piece, from_position, to_position);

        fprintf(history, "Board state after move:\n");
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                fprintf(history, "%c ", game_state->board[row][col]);
            }
            fprintf(history, "\n");
        }
        fprintf(history, "--------------------\n");
        fclose(history);
    }

    // 킹이 잡혔는지 확인
    if (is_king_captured(chessboard)) {
        game_state->game_over = 1;

        const char *end_response =
                "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"status\":\"success\",\"message\":\"King captured. Game over\"}";
        write(client_socket, end_response, strlen(end_response));
        printf("Game over: A king has been captured.\n");
        return;
    }

    // 턴 변경: 현재 플레이어 토큰 전환
    if (strcmp(game_state->current_player_token, game_state->player1_token) == 0) {
        strncpy(game_state->current_player_token, game_state->player2_token, TOKEN_LENGTH);
    } else {
        strncpy(game_state->current_player_token, game_state->player1_token, TOKEN_LENGTH);
    }

    // 성공 응답 반환
    char success_response[512];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"status\":\"success\",\"message\":\"Move successful\"}");
    write(client_socket, success_response, strlen(success_response));
}