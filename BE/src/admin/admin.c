#include "admin.h"
#include<stdio.h>
#include<string.h>

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
        printf("1. 방 목록 조회\n");
        printf("2. 방 삭제\n");
        printf("3. 사용자 목록 조회\n");
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
                printf("방 목록을 조회합니다...\n");
                // 방 목록 조회 기능 구현
                break;
            case 2:
                printf("삭제할 방을 선택하세요...\n");
                // 방 삭제 기능 구현
                break;
            case 3:
                printf("사용자 목록을 조회합니다...\n");
                // 사용자 목록 조회 기능 구현
                break;
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