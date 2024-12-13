#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "server.h"
#include "handler.h"
#include "utils.h"
#include "cJSON.h"
#include "admin.h"
#include <pthread.h>


void server_run() {
    printf("Server initializing...\n");

    // 서버 소켓 생성 및 초기화
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket error");
        exit(1);
    }

    // 소켓 옵션 설정
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt error");
        close(server_socket);
        exit(1);
    }

    // 서버 주소 설정
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
    // 최대 30개의 클라이언트 대기
    if (listen(server_socket, 30) == -1) {
        perror("listen error");
        close(server_socket);
        exit(1);
    }
    printf("Server is running on port %d\n", SERVER_PORT);


    // admin 소켓 생성 및 초기화
    int admin_server_socket;
    struct sockaddr_in admin_server_addr;
    admin_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (admin_server_socket == -1) {
        perror("admin socket error");
        exit(1);
    }

    // 소켓 옵션 설정
    if (setsockopt(admin_server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt error");
        close(admin_server_socket);
        exit(1);
    }

    // admin 서버 주소 설정
    memset(&admin_server_addr, 0, sizeof(admin_server_addr));
    admin_server_addr.sin_family = AF_INET;
    admin_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    admin_server_addr.sin_port = htons(ADMIN_PORT);

    // 소켓 바인딩 및 대기
    if (bind(admin_server_socket, (struct sockaddr*)&admin_server_addr, sizeof(admin_server_addr)) == -1) {
        perror("admin bind error");
        close(admin_server_socket);
        exit(1);
    }
    // 최대 3개의 클라이언트 대기
    if (listen(admin_server_socket, 3) == -1) {
        perror("admin listen error");
        close(admin_server_socket);
        exit(1);
    }
    printf("Admin server is running on port %d\n", ADMIN_PORT);




    while (1) {
        // 클라이언트 소켓과 관리자 소켓을 모두 select로 관리
        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(server_socket, &reads);
        FD_SET(admin_server_socket, &reads);
        int fd_max = (server_socket > admin_server_socket) ? server_socket : admin_server_socket;

        if (select(fd_max + 1, &reads, 0, 0, NULL) == -1) {
            perror("select error");
            break;
        }

        // 클라이언트 소켓 처리
        if (FD_ISSET(server_socket, &reads)) {
            // 클라이언트의 요청 처리
            int *client_socket_ptr = malloc(sizeof(int));
            *client_socket_ptr = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
            if (*client_socket_ptr == -1) {
                perror("accept error");
                free(client_socket_ptr);
                continue;
            }

            // 클라이언트 스레드 생성 및 작업 시작
            pthread_t tid;
            if (pthread_create(&tid, NULL, client_handler, (void *)client_socket_ptr) != 0) {
                perror("pthread_create error");
                close(*client_socket_ptr);
                free(client_socket_ptr);
                continue;
            }

            // 스레드의 작업이 종료되면 자원을 해제
            pthread_detach(tid);
        }

        // 관리자 소켓 처리
        if (FD_ISSET(admin_server_socket, &reads)) {
            // 관리자의 요청 처리
            int *admin_socket_ptr = malloc(sizeof(int));
            *admin_socket_ptr = accept(admin_server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
            if (*admin_socket_ptr == -1) {
                perror("admin accept error");
                free(admin_socket_ptr);
                continue;
            }

            // 관리자 스레드 생성 및 작업 시작
            pthread_t tid;
            if (pthread_create(&tid, NULL, admin_handler, (void *)admin_socket_ptr) != 0) {
                perror("admin pthread_create error");
                close(*admin_socket_ptr);
                free(admin_socket_ptr);
                continue;
            }
            // 스레드의 작업이 종료되면 자원을 해제
            pthread_detach(tid);
        }
    }
}


void *client_handler(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];

    // 클라이언트로부터 요청 수신
    int str_len = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (str_len < 0) {
        perror("read error");
        close(client_socket);
        pthread_exit(NULL);
    }
    buffer[str_len] = 0;

    // 메소드와 경로 추출
    char method[8];
    char path[256];
    sscanf(buffer, "%s %s", method, path);

    printf("Received request: [%s] %s\n", method, path);

    // JSON 파싱
    char *json_start = strstr(buffer, "\r\n\r\n");
    cJSON *json_request = NULL;
    if (json_start) {
        json_start += 4;
        json_request = parse_json(json_start);
    }

    // 각각의 요청에 따라 핸들러 호출
    handle_request(client_socket, method, path, json_request);

    // 요청이 종료되어 JSON 객체가 할당되었다면 메모리 해제
    close(client_socket);
    pthread_exit(NULL);
}

// 관리자 요청 처리 함수
void *admin_handler(void *arg) {
    int admin_socket = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];

    while (1) {
        // 관리자로부터 명령 수신
        int str_len = recv(admin_socket, buffer, BUFFER_SIZE - 1, 0);
        if (str_len <= 0) {
            perror("recv error or admin disconnected");
            close(admin_socket);
            pthread_exit(NULL);
        }
        buffer[str_len] = 0;

        // 명령 파싱
        char *command_str = strtok(buffer, " ");
        if (command_str == NULL) {
            send(admin_socket, "Invalid command format.\n", strlen("Invalid command format.\n"), 0);
            continue;
        }

        int command = atoi(command_str);
        char *params = strtok(NULL, "");

        // 명령에 따라 처리
        switch (command) {
            case 1:
            {
                char *info = get_server_info();
                send(admin_socket, info, strlen(info), 0);
                free(info);
            }
                break;
            case 2:
                manage_rooms(admin_socket, command, NULL);
                break;
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                manage_rooms(admin_socket, command, params);
                break;
            case 8:
                send(admin_socket, "서버를 종료합니다...\n", strlen("서버를 종료합니다...\n"), 0);
                exit(0);
            default:
                send(admin_socket, "알 수 없는 명령입니다.\n", strlen("알 수 없는 명령입니다.\n"), 0);
        }
    }
}