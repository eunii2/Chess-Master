// admin.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define ADMIN_PORT 9090

int main() {
    int sock;
    struct sockaddr_in server_addr;

    char *server_ip = "127.0.0.1";  // Server IP
    char buffer[BUFFER_SIZE];

    // Create socket
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket error");
        exit(1);
    }

    // Server address configuration
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(ADMIN_PORT);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect error");
        close(sock);
        exit(1);
    }

    printf("관리자 모드에 접속하였습니다.\n");

    while (1) {
        // Display the menu
        printf("\n========== 관리자 메뉴 ==========\n");
        printf("1. 서버 정보 조회\n");
        printf("2. 게임방 리스트 조회\n");
        printf("3. 게임방 삭제 (예: 3 방ID)\n");
        printf("4. 게임방 아이디 변경 (예: 4 기존ID 새로운ID)\n");
        printf("5. 게임방 상세 정보 조회 (예: 5 방ID)\n");
        printf("6. 게임방 권한 변경 (예: 6 방ID 권한(8진수))\n");
        printf("7. 게임방 파일 크기 및 개수 조회 (예: 7 방ID)\n");
        printf("8. 서버 종료\n");
        printf("100. 관리자 모드 종료\n");
        printf("=================================\n");
        printf("명령을 입력하세요: ");

        // Read the entire command line
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            printf("입력 오류.\n");
            continue;
        }
        buffer[strcspn(buffer, "\n")] = 0;  // Remove newline character

        if (strcmp(buffer, "100") == 0) {
            printf("관리자 모드를 종료합니다.\n");
            break;
        }

        // Send the command to the server
        if (send(sock, buffer, strlen(buffer), 0) == -1) {
            perror("send error");
            break;
        }

        // Receive and display the server's response
        int str_len = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (str_len <= 0) {
            perror("recv error or server closed");
            break;
        }
        buffer[str_len] = 0;
        printf("서버 응답: %s\n", buffer);
    }

    close(sock);
    return 0;
}