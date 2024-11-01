#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


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