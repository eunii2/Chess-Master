#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "config.h"
#include <string.h>
#include <sys/stat.h>
#include <errno.h>


// JSON 파싱 및 출력 함수
cJSON *parse_json(const char *json_string) {
    cJSON *json = cJSON_Parse(json_string);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "JSON Parse Error before: %s\n", error_ptr);
        }
        return NULL;
    }

    // 파싱된 JSON 객체를 문자열로 변환하여 출력
    char *json_printed = cJSON_Print(json);
    if (json_printed != NULL) {
        printf("%s\n\n", json_printed);
        free(json_printed);  // 문자열 메모리 해제
    } else {
        printf("Failed to print JSON.\n");
    }

    return json;
}


// 랜덤 토큰 생성 함수
void generate_token(char *token, size_t length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t charset_size = sizeof(charset) - 1;

    srand(time(NULL) ^ getpid());
    for (size_t i = 0; i < length; i++) {
        token[i] = charset[rand() % charset_size];
    }
    token[length] = '\0';
}


// 토큰으로 userID를 찾는 함수. 실패시 -1 반환
int get_user_id_by_token(const char* token) {
    if (!token) return -1;

    // user_list.txt 파일 열기
    FILE *file = fopen(USER_LIST_FILE, "r");
    if (!file) {
        perror("fopen error");
        return -1;
    }

    char buffer[512];
    int user_id = -1;

    while (fgets(buffer, sizeof(buffer), file)) {
        // Token이 있는 라인 찾기
        char *token_pos = strstr(buffer, "Token: ");
        if (token_pos) {
            // 해당 라인에서 토큰 추출
            char current_token[TOKEN_LENGTH + 1];
            sscanf(token_pos + strlen("Token: "), "%s", current_token);

            // 토큰이 일치하면 user_id 추출
            if (strcmp(current_token, token) == 0) {
                // User ID: 부분 찾기
                char *id_pos = strstr(buffer, "User ID: ");
                if (id_pos) {
                    sscanf(id_pos + strlen("User ID: "), "%d", &user_id);
                    break;
                }
            }
        }
    }

    fclose(file);
    return user_id;
}

// 폴더 생성 함수
void create_directory(const char *path) {
    if (mkdir(path, 0777) == -1) {
        if (errno == EEXIST) {
            printf("Directory %s already exists\n", path);
        } else {
            perror("mkdir failed");
        }
    } else {
        printf("Directory %s created\n", path);
    }
}

// 데이터 폴더 구조 초기화
void initialize_data_directories() {
    create_directory("../data");
    create_directory("../data/user");
    create_directory("../data/rooms");
    create_directory("../data/game");
    create_directory("../data/chat");
}