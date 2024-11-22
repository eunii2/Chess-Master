#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"
#include "config.h"
#include "image.h"
#include "utils.h"


void image_upload_handler(int client_socket, cJSON *json_request) {
    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(json_request, "token");
    const cJSON *image_address_json = cJSON_GetObjectItemCaseSensitive(json_request, "image_address");

    // JSON 유효성 검사
    if (!cJSON_IsString(token_json) || !cJSON_IsString(image_address_json)) {
        char error_response[MAX_BUFFER_SIZE];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 400 Bad Request\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"Invalid user_id or image_address\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    const char *token = token_json->valuestring;
    const char *image_address = image_address_json->valuestring;

    // 토큰으로 유저 ID 가져오기
    int user_id = get_user_id_by_token(token);
    if (user_id <= 0) {
        char error_response[MAX_BUFFER_SIZE];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 401 Unauthorized\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"Invalid token\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 유저 리스트 로드
    UserList user_list = {NULL, 0, 0};
    load_user_list(&user_list);

    // user_id를 기반으로 유저 찾기
    int user_found = 0;
    for (size_t i = 0; i < user_list.size; i++) {
        if (user_list.entries[i].user_id == user_id) {
            // 이미지 주소 업데이트
            free(user_list.entries[i].image_address);
            user_list.entries[i].image_address = strdup(image_address);
            user_found = 1;
            break;
        }
    }

    // 유저가 없으면 새로 추가
    if (!user_found) {
        // 리스트 크기 확장
        if (user_list.size == user_list.capacity) {
            size_t new_capacity = user_list.capacity == 0 ? 10 : user_list.capacity * 2;
            UserEntry *new_entries = realloc(user_list.entries, new_capacity * sizeof(UserEntry));
            if (!new_entries) {
                perror("realloc error");
                free_user_list(&user_list);
                char error_response[MAX_BUFFER_SIZE];
                snprintf(error_response, sizeof(error_response),
                         "HTTP/1.1 500 Internal Server Error\r\n"
                         "Content-Type: application/json\r\n"
                         "%s\r\n"
                         "{\"status\":\"error\",\"message\":\"Memory allocation failed\"}",
                         cors_headers);
                write(client_socket, error_response, strlen(error_response));
                return;
            }
            user_list.entries = new_entries;
            user_list.capacity = new_capacity;
        }

        // 새로운 유저 추가
        user_list.entries[user_list.size].user_id = user_id;
        user_list.entries[user_list.size].image_address = strdup(image_address);
        user_list.size++;
    }

    // 유저 리스트 정렬 (user_id의 오름차순)
    qsort(user_list.entries, user_list.size, sizeof(UserEntry), compare_user_entries);

    // 유저 리스트 저장
    save_user_list(&user_list);

    // 메모리 해제
    free_user_list(&user_list);

    // 성공 응답
    char success_response[MAX_BUFFER_SIZE];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "%s\r\n"
             "{\"status\":\"success\",\"message\":\"User information updated successfully\"}",
             cors_headers);
    write(client_socket, success_response, strlen(success_response));
}

// 유저 리스트 로드 함수
void load_user_list(UserList *user_list) {
    FILE *file = fopen(USER_IMAGE_FILE, "r");
    if (!file) {
        // 파일이 없으면 새로 생성
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // 개행 문자 제거
        line[strcspn(line, "\n")] = '\0';

        // user_id와 이미지 주소 분리
        char *delimiter = strchr(line, ':');
        if (!delimiter) continue;

        *delimiter = '\0';
        int user_id = atoi(line);
        char *image_address = delimiter + 1;

        // 공백 제거
        while (*image_address == ' ') image_address++;

        // 리스트 크기 확장 필요 시 처리
        if (user_list->size == user_list->capacity) {
            size_t new_capacity = user_list->capacity == 0 ? 10 : user_list->capacity * 2;
            UserEntry *new_entries = realloc(user_list->entries, new_capacity * sizeof(UserEntry));
            if (!new_entries) {
                perror("realloc error");
                break;
            }
            user_list->entries = new_entries;
            user_list->capacity = new_capacity;
        }

        // 새로운 엔트리 추가
        user_list->entries[user_list->size].user_id = user_id;
        user_list->entries[user_list->size].image_address = strdup(image_address);
        user_list->size++;
    }

    fclose(file);
}

// 유저 리스트 저장 함수
void save_user_list(UserList *user_list) {
    FILE *file = fopen(USER_IMAGE_FILE, "w");
    if (!file) {
        perror("fopen error");
        return;
    }

    for (size_t i = 0; i < user_list->size; i++) {
        fprintf(file, "%d : %s\n", user_list->entries[i].user_id, user_list->entries[i].image_address);
    }

    fclose(file);
}

// 유저 리스트 메모리 해제 함수
void free_user_list(UserList *user_list) {
    for (size_t i = 0; i < user_list->size; i++) {
        free(user_list->entries[i].image_address);
    }
    free(user_list->entries);
}

// 유저 엔트리 비교 함수 (오름차순 정렬용)
int compare_user_entries(const void *a, const void *b) {
    UserEntry *entry_a = (UserEntry *)a;
    UserEntry *entry_b = (UserEntry *)b;
    return (entry_a->user_id - entry_b->user_id);
}