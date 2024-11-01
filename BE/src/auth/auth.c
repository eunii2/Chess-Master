#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "cJSON.h"
#include "config.h"
#include "utils.h"

// 회원가입 처리 함수
void join_handler(int client_socket, cJSON *json_request) {
    const cJSON *username = cJSON_GetObjectItemCaseSensitive(json_request, "username");
    const cJSON *password = cJSON_GetObjectItemCaseSensitive(json_request, "password");

    // 유효성 검사
    if (!cJSON_IsString(username) || !cJSON_IsString(password)) {
        const char *error_response = "HTTP/1.1 400 Bad Request\r\n"
                                     "Content-Type: application/json\r\n\r\n"
                                     "{\"status\":\"error\",\"message\":\"Invalid input\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 디렉터리 생성
    if (access(USER_DATA_DIR, F_OK) == -1) {
        if (mkdir(USER_DATA_DIR, 0777) == -1) {
            perror("mkdir error");
            return;
        }
    }

    // user_list.txt 파일 열기
    FILE *file = fopen(USER_LIST_FILE, "a+");
    if (!file) {
        perror("fopen error");
        return;
    }

    /// 중복 사용자 검사 및 마지막 user_id 찾기
    char buffer[256];
    int user_exists = 0;
    int last_user_id = 0;
    rewind(file);  // 파일 포인터를 파일의 시작으로 이동
    while (fgets(buffer, sizeof(buffer), file)) {
        // user_id 파싱
        int current_user_id;
        if (sscanf(buffer, "User ID: %d,", &current_user_id) == 1) {
            if (current_user_id > last_user_id) {
                last_user_id = current_user_id;  // 가장 큰 user_id 갱신
            }
        }

        // "Username: "으로 시작하는 라인에서 정확히 일치하는지 확인
        char *username_start = strstr(buffer, "Username: ");
        if (username_start) {
            username_start += strlen("Username: ");
            // 사용자 이름 추출
            char existing_username[256];
            if (sscanf(username_start, "%[^, \n]", existing_username) == 1) {
                if (strcmp(existing_username, username->valuestring) == 0) {
                    user_exists = 1;
                    printf("User already exists\n");
                    break;
                }
            }
        }
    }

    // 중복 사용자가 있을 경우 실패 응답 전송 후 종료
    if (user_exists) {
        const char *error_response = "HTTP/1.1 409 Conflict\r\n"
                                     "Content-Type: application/json\r\n\r\n"
                                     "{\"status\":\"error\",\"message\":\"Username already exists\"}";
        write(client_socket, error_response, strlen(error_response));
        fclose(file);  // 파일 닫기
        return;
    }

    // 새 user_id 생성
    int new_user_id = last_user_id + 1;

    // 가입 시간 기록
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *timeinfo = localtime(&tv.tv_sec);
    char signup_date[64];
    strftime(signup_date, sizeof(signup_date), "%Y-%m-%d %H:%M:%S", timeinfo);

    // 사용자 정보 저장을 위한 메모리 할당
    char *user_info = calloc(1, 256);
    if (!user_info) {
        perror("calloc error");
        fclose(file);
        return;
    }

    // 사용자 정보 파일에 저장 (user_id 포함)
    snprintf(user_info, 256, "User ID: %d, Username: %s, Password: %s, Signup Date: %s\n",
             new_user_id, username->valuestring, password->valuestring, signup_date);
    fprintf(file, "%s", user_info);
    fflush(file);  // 데이터가 출력 버퍼에 남아있지 않도록

    // 메모리 해제 및 파일 닫기
    free(user_info);
    fclose(file);

    // 성공 응답 전송
    const char *success_response = "HTTP/1.1 201 Created\r\n"
                                   "Content-Type: application/json\r\n\r\n"
                                   "{\"status\":\"success\",\"message\":\"Signup successful\"}";
    write(client_socket, success_response, strlen(success_response));
}

// 로그인 처리 함수
void login_handler(int client_socket, cJSON *json_request) {
    const cJSON *username = cJSON_GetObjectItemCaseSensitive(json_request, "username");
    const cJSON *password = cJSON_GetObjectItemCaseSensitive(json_request, "password");

    // 유효성 검사
    if (!cJSON_IsString(username) || !cJSON_IsString(password)) {
        const char *error_response = "HTTP/1.1 400 Bad Request\r\n"
                                     "Content-Type: application/json\r\n\r\n"
                                     "{\"status\":\"error\",\"message\":\"Invalid input\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // 사용자 데이터 파일 접근 확인
    if (access(USER_LIST_FILE, F_OK) == -1) {
        perror("access error");
        const char *error_response = "HTTP/1.1 500 Internal Server Error\r\n"
                                     "Content-Type: application/json\r\n\r\n"
                                     "{\"status\":\"error\",\"message\":\"User data not found\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    // user_list.txt 파일 열기
    FILE *file = fopen(USER_LIST_FILE, "r");
    if (!file) {
        perror("fopen error");
        return;
    }

    // 파일의 모든 라인을 읽어 메모리에 저장
    char *lines[1024];  // 최대 1024라인 가정
    int line_count = 0;
    int user_found = 0;
    int matched_line_index = -1;
    char buffer[512];

    while (fgets(buffer, sizeof(buffer), file)) {
        lines[line_count] = strdup(buffer);  // 라인을 복사하여 저장
        if (!lines[line_count]) {
            perror("strdup error");
            fclose(file);
            return;
        }

        // 사용자 검증
        char *username_start = strstr(buffer, "Username: ");
        char *password_start = strstr(buffer, "Password: ");
        if (username_start && password_start) {
            // 사용자 이름 추출
            char existing_username[256];
            username_start += strlen("Username: ");
            sscanf(username_start, "%[^, \n]", existing_username);

            // 비밀번호 추출
            char existing_password[256];
            password_start += strlen("Password: ");
            sscanf(password_start, "%[^, \n]", existing_password);

            // 사용자 이름과 비밀번호 일치 확인
            if (strcmp(existing_username, username->valuestring) == 0 &&
                strcmp(existing_password, password->valuestring) == 0) {
                user_found = 1;
                matched_line_index = line_count;
            }
        }

        line_count++;
    }

    fclose(file);

    if (!user_found) {
        const char *error_response = "HTTP/1.1 401 Unauthorized\r\n"
                                     "Content-Type: application/json\r\n\r\n"
                                     "{\"status\":\"error\",\"message\":\"Invalid username or password\"}";
        write(client_socket, error_response, strlen(error_response));
        // 메모리 해제
        for (int i = 0; i < line_count; i++) {
            free(lines[i]);
        }
        return;
    }

    // 랜덤 토큰 생성
    char token[TOKEN_LENGTH + 1];
    generate_token(token, TOKEN_LENGTH);

    // 해당 라인을 수정
    char *line = lines[matched_line_index];
    char temp_line[512];

    // 기존 토큰 제거
    char *token_pos = strstr(line, ", Token: ");
    if (token_pos) {
        *token_pos = '\0';  // 기존 토큰과 앞의 쉼표 제거
    } else {
        // 개행 문자 제거
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
    }

    // 수정된 라인 생성
    snprintf(temp_line, sizeof(temp_line), "%s, Token: %s\n", line, token);

    // 기존 라인 메모리 해제하고 업데이트된 라인 저장
    free(lines[matched_line_index]);
    lines[matched_line_index] = strdup(temp_line);
    if (!lines[matched_line_index]) {
        perror("strdup error");
        // 메모리 해제
        for (int i = 0; i < line_count; i++) {
            if (i != matched_line_index) {
                free(lines[i]);
            }
        }
        return;
    }

    // 파일에 모든 라인 쓰기
    file = fopen(USER_LIST_FILE, "w");
    if (!file) {
        perror("fopen error");
        // 메모리 해제
        for (int i = 0; i < line_count; i++) {
            free(lines[i]);
        }
        return;
    }

    for (int i = 0; i < line_count; i++) {
        fputs(lines[i], file);
    }

    fclose(file);

    // 메모리 해제
    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }

    // 토큰을 포함한 성공 응답 전송
    char success_response[512];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n\r\n"
             "{\"status\":\"success\",\"message\":\"Login successful\",\"token\":\"%s\"}",
             token);
    write(client_socket, success_response, strlen(success_response));
}