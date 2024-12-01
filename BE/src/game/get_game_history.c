#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "game.h"

void get_game_history_handler(int client_socket, cJSON *json_request) {
    // room_id 가져오기
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");
    if (!cJSON_IsNumber(room_id_json)) {
        const char *error_response = "HTTP/1.1 400 Bad Request\r\n"
                                     "Content-Type: application/json\r\n\r\n"
                                     "{\"status\":\"error\",\"message\":\"Invalid room ID\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    int room_id = room_id_json->valueint;

    // 히스토리 파일 경로 생성
    char game_log_path[256];
    snprintf(game_log_path, sizeof(game_log_path), "../data/game/%d/history.txt", room_id);

    // 파일 읽기
    FILE *log_file = fopen(game_log_path, "r");
    if (!log_file) {
        const char *error_response = "HTTP/1.1 400 Bad Request\r\n"
                                     "Content-Type: application/json\r\n\r\n"
                                     "{\"status\":\"error\",\"message\":\"Invalid room ID\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 파일 크기 확인
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

    // HTTP 응답 헤더
    char response_header[512];
    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "Access-Control-Allow-Origin: http://localhost:5173\r\n"
             "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
             "Access-Control-Allow-Headers: Content-Type\r\n"
             "Content-Length: %zu\r\n\r\n",
             strlen(json_str));

    // 응답 전송
    write(client_socket, response_header, strlen(response_header));
    write(client_socket, json_str, strlen(json_str));

    // 메모리 해제
    free(history_content);
    free(json_str);
    cJSON_Delete(response);
}