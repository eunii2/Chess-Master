#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "handler.h"
#include "cJSON.h"

void handle_request(int client_socket, const char *method, const char *path, cJSON *json_request) {
    printf("Handling request: [%s] %s\n", method, path);

    if (strcmp(method, "POST") == 0 && strcmp(path, "/login") == 0) {
        // 로그인 처리
//        login_handler(client_socket, json_request);
    }
    else if (strcmp(method, "GET") == 0 && strcmp(path, "/profile") == 0) {
        // 프로필 조회 처리
//        profile_handler(client_socket, json_request);
    }
        // 추가적인 경로 및 메서드에 대한 처리
    else {
        // 지원하지 않는 요청에 대한 404 응답
        const char *not_found = "HTTP/1.1 404 Not Found\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: 13\r\n\r\n"
                                "404 Not Found";
        write(client_socket, not_found, strlen(not_found));
    }

    // JSON 객체 해제
    if (json_request != NULL) {
        cJSON_Delete(json_request);
    }
}