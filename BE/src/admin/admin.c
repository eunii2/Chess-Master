#include "admin.h"
#include<stdio.h>
#include<string.h>
#include <stdlib.h>

void admin_menu() {

    // 서버 시작 메시지 출력
    printf(" =========================================\n");
    printf("  ██████╗██╗  ██╗███████╗███████╗███████╗\n");
    printf(" ██╔════╝██║  ██║██╔════╝██╔════╝██╔════╝\n");
    printf(" ██║     ███████║█████╗  ███████╗███████╗\n");
    printf(" ██║     ██╔══██║██╔══╝  ╚════██║╚════██║\n");
    printf(" ╚██████╗██║  ██║███████╗███████║███████║\n");
    printf("  ╚═════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝\n");
    printf(" =========================================\n");
    printf(" :: Chess Master Server ::        (v1.0.0)\n\n");

    char input[10];

    while (1) {

        printf("관리자 모드로 들어가시겠습니까? (y/n): ");
        fgets(input, sizeof(input), stdin);

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "n") == 0 || strcmp(input, "N") == 0) {
            printf("서버를 구동합니다...\n\n");
            return;
        } else if (strcmp(input, "y") == 0 || strcmp(input, "Y") == 0) {
            printf("관리자 모드로 진입합니다.\n");
            break;
        } else {
            printf("잘못된 입력입니다. 'y' 또는 'n'으로 입력해주세요.\n");
        }
    }

    // 관리자 메뉴 출력

    int choice;
    while (1) {
        printf("\n========== 관리자 메뉴 ==========\n");
        printf("1. 서버 정보 조회\n");
        printf("2. 방 관리 기능\n");
        printf("3. 시스템 명령 실행\n");
        printf("4. 사용자 강제 퇴장\n");
        printf("5. 서버 종료\n");
        printf("100. 관리자 모드 종료\n");
        printf("=================================\n");
        printf("선택하세요 (1-6): ");

        if (scanf("%d", &choice) != 1) {
            printf("올바른 숫자를 입력해주세요.\n");
            while (getchar() != '\n'); // 입력 버퍼 비우기
            continue;
        }

        switch (choice) {
            case 1:
                get_server_info();
                break;
            case 2:
                manage_rooms();
                break;
            case 3: {
                char command[256];
                printf("실행할 시스템 명령어를 입력하세요(리눅스 명령) : ");
                while (getchar() != '\n');
                fgets(command, sizeof(command), stdin);
                command[strcspn(command, "\n")] = '\0';

                if (strlen(command) > 0) {
                    int result = system(command);
                    if (result == -1)
                        perror("명령 실행 오류");
                } else
                    printf("명령어가 입력되지 않았습니다.\n");

                break;
            }
            case 4:
                printf("강제 퇴장할 사용자를 선택하세요...\n");
                // 사용자 강제 퇴장 기능 구현
                break;
            case 5:
                printf("서버를 종료합니다...\n");
                // 서버 종료 기능 구현
                return;
            case 100:
                printf("관리자 모드를 종료합니다.\n\n");
                return;
            default:
                printf("잘못된 선택입니다. 1-6 사이의 숫자를 입력해주세요.\n");
                break;
        }
    }


}