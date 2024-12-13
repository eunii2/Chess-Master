#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "game.h"

// 클라이언트로부터 요청받은 게임 히스토리를 처리하는 함수
void get_game_history_handler(int client_socket, cJSON *json_request) {
    // 요청에서 room_id 추출
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");
    if (!cJSON_IsNumber(room_id_json)) { // room_id가 숫자가 아닌 경우 에러 응답
        const char *error_response = "HTTP/1.1 400 Bad Request\r\n"
                                     "Content-Type: application/json\r\n\r\n"
                                     "{\"status\":\"error\",\"message\":\"Invalid room ID\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 유효한 room_id 가져오기
    int room_id = room_id_json->valueint;

    // 히스토리 파일 경로 생성
    char game_log_path[256];
    snprintf(game_log_path, sizeof(game_log_path), "../data/game/%d/history.txt", room_id);

    // 히스토리 파일 열기
    FILE *log_file = fopen(game_log_path, "r");
    if (!log_file) { // 파일이 없는 경우 에러 응답
        const char *error_response = "HTTP/1.1 400 Bad Request\r\n"
                                     "Content-Type: application/json\r\n\r\n"
                                     "{\"status\":\"error\",\"message\":\"Invalid room ID\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    fseek(log_file, 0, SEEK_END);
    long file_size = ftell(log_file);
    fseek(log_file, 0, SEEK_SET);

    // 파일 내용 읽기
    char *history_content = malloc(file_size + 1);
    size_t read_size = fread(history_content, 1, file_size, log_file);
    history_content[read_size] = '\0';
    fclose(log_file);

    // JSON 응답 생성
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", "success");
    cJSON_AddStringToObject(response, "history", history_content);

    char *json_str = cJSON_Print(response);

    // HTTP 응답 헤더 생성
    char response_header[512];
    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "Access-Control-Allow-Origin: http://localhost:5173\r\n"
             "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
             "Access-Control-Allow-Headers: Content-Type\r\n"
             "Content-Length: %zu\r\n\r\n",
             strlen(json_str));

    // HTTP 응답 전송
    write(client_socket, response_header, strlen(response_header));
    write(client_socket, json_str, strlen(json_str));

    // 메모리 해제
    free(history_content);
    free(json_str);
    cJSON_Delete(response);
}