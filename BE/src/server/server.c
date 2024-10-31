// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "server.h"
#include "handler.h"
#include "utils.h"
#include "cJSON.h"

void server_run() {
    printf("Server initializing...\n");

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // 서버 소켓 생성 및 초기화
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket error");
        exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    // 소켓 바인딩 및 대기
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind error");
        close(server_socket);
        exit(1);
    }
    if (listen(server_socket, 5) == -1) {
        perror("listen error");
        close(server_socket);
        exit(1);
    }
    printf("Server is running on port %d\n", SERVER_PORT);

    // 클라이언트 연결 처리
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
        if (client_socket == -1) {
            perror("accept error");
            continue;
        }

        // 데이터 수신
        int str_len = read(client_socket, buffer, BUFFER_SIZE - 1);
        if (str_len < 0) {
            perror("read error");
            close(client_socket);
            continue;
        }
        buffer[str_len] = 0;  // null-terminate

        // HTTP 메서드와 경로 추출
        char method[8];
        char path[256];
        sscanf(buffer, "%s %s", method, path);

        printf("Received request: [%s] %s\n", method, path);

        // JSON 본문 추출
        char *json_start = strstr(buffer, "\r\n\r\n");
        cJSON *json_request = NULL;
        if (json_start) {
            json_start += 4;  // "\r\n\r\n" 이후로 이동
            json_request = parse_json(json_start);  // JSON 파싱
        }

        // 요청 핸들링
        handle_request(client_socket, method, path, json_request);

        close(client_socket);  // 클라이언트 소켓 닫기
    }


}