#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 1024
#define ADMIN_PORT 9090

int main() {
    int sock;
    struct sockaddr_in server_addr;
    struct hostent *host;

    char *server_ip = "127.0.0.1";  // 서버 IP

    // 호스트 이름을 IP 주소로 변환
    if ((host = gethostbyname(server_ip)) == NULL) {
        perror("gethostbyname error");
        exit(1);
    }

    // 소켓 생성
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket error");
        exit(1);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = ((struct in_addr*)(host->h_addr))->s_addr;
    server_addr.sin_port = htons(ADMIN_PORT);

    // 서버에 연결
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect error");
        close(sock);
        exit(1);
    }

    printf("관리자 모드에 접속하였습니다.\n");

    char buffer[BUFFER_SIZE];
    int choice;

    while (1) {
        // 클라이언트는 관리자 메뉴만 출력하고 서버로 명령어 전송
        printf("\n========== 관리자 메뉴 ==========\n");
        printf("1. 서버 정보 조회\n");
        printf("2. 방 관리 기능\n");
        printf("5. 서버 종료\n");
        printf("100. 관리자 모드 종료\n");
        printf("=================================\n");
        printf("선택하세요 (1-5): ");

        if (scanf("%d", &choice) != 1) {
            printf("올바른 숫자를 입력해주세요.\n");
            while (getchar() != '\n'); // 입력 버퍼 비우기
            continue;
        }
        while (getchar() != '\n'); // 입력 버퍼 비우기

        if (choice == 100) {
            printf("관리자 모드를 종료합니다.\n");
            break;
        }

        // 선택한 명령을 서버로 전송
        snprintf(buffer, sizeof(buffer), "%d", choice);
        if (send(sock, buffer, strlen(buffer), 0) == -1) {
            perror("send error");
            break;
        }

        // 서버로부터 응답 수신 및 출력
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