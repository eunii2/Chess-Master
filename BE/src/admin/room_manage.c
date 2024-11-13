#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

void manage_rooms() {
    int choice;
    char room_id[256];
    char new_room_id[256];
    char original_path[512];

    if (getcwd(original_path, sizeof(original_path)) == NULL) {
        perror("Error getting current directory");
        return;
    }

    if (chdir("../data/game") != 0) {
        perror("Error changing to game directory");
        return;
    }

    while (1) {
        printf("\n========== 방 관리 메뉴 ==========\n");
        printf("1. 게임방 아이디 리스트 조회\n");
        printf("2. 게임방 삭제\n");
        printf("3. 게임방 아이디 변경\n");
        printf("4. 이전 메뉴로 돌아가기\n");
        printf("=================================\n");
        printf("선택하세요 (1-4): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                DIR *d = opendir(".");
                if (d) {
                    printf("\n========== 게임방 리스트 ==========\n");
                    struct dirent *dir;
                    while ((dir = readdir(d)) != NULL) {
                        if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                            printf("%s\n", dir->d_name);
                        }
                    }
                    closedir(d);
                } else {
                    fprintf(stderr, "Error opening directory: %s\n", strerror(errno));
                }
                break;
            }
            case 2:
                printf("삭제할 게임방 아이디를 입력하세요: ");
                scanf("%s", room_id);
                if (rmdir(room_id) == 0) {
                    printf("게임방 '%s'이(가) 삭제되었습니다.\n", room_id);
                } else {
                    fprintf(stderr, "Error deleting room '%s': %s\n", room_id, strerror(errno));
                }
                break;
            case 3:
                printf("변경할 게임방 아이디를 입력하세요: ");
                scanf("%s", room_id);
                printf("새로운 게임방 아이디를 입력하세요: ");
                scanf("%s", new_room_id);

                DIR *check_dir = opendir(new_room_id);
                if (check_dir != NULL) {
                    printf("이미 존재하는 게임방 아이디 '%s'입니다. 다른 이름을 입력하세요.\n", new_room_id);
                    closedir(check_dir);
                } else if (rename(room_id, new_room_id) == 0) {
                    printf("게임방 아이디가 '%s'에서 '%s'으로 변경되었습니다.\n", room_id, new_room_id);
                } else {
                    fprintf(stderr, "Error renaming room '%s' to '%s': %s\n", room_id, new_room_id, strerror(errno));
                }
                break;
            case 4:
                // 원래 디렉토리로 돌아가기
                chdir(original_path);
                return;
            default:
                printf("잘못된 입력입니다. 다시 시도해주세요.\n");
                break;
        }
    }
}