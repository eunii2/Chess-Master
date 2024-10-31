#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "cJSON.h"
#include "config.h"


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