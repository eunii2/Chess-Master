#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

void change_room_permission();
void list_room_details();
void show_room_size();

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
        printf("4. 게임방 상세 정보 조회\n");
        printf("5. 게임방 접근 권한 변경\n");
        printf("6. 게임방 파일 개수 및 크기 조회\n");
        printf("7. 이전 메뉴로 돌아가기\n");
        printf("=================================\n");
        printf("선택하세요 (1-7): ");
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
                } else
                    fprintf(stderr, "Error opening directory: %s\n", strerror(errno));
                break;
            }
            case 2:
                printf("삭제할 게임방 아이디를 입력하세요: ");
                scanf("%s", room_id);
                if (rmdir(room_id) == 0) {
                    printf("게임방 '%s'이(가) 삭제되었습니다.\n", room_id);
                } else
                    fprintf(stderr, "Error deleting room '%s': %s\n", room_id, strerror(errno));
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
                } else if (rename(room_id, new_room_id) == 0)
                    printf("게임방 아이디가 '%s'에서 '%s'으로 변경되었습니다.\n", room_id, new_room_id);
                 else
                    fprintf(stderr, "Error renaming room '%s' to '%s': %s\n", room_id, new_room_id, strerror(errno));
                break;
            case 4:
                list_room_details();
                break;
            case 5:
                change_room_permission();
                break;
            case 6:
                show_room_size();
                break;
            case 7:
                chdir(original_path);
                return;
            default:
                printf("잘못된 입력입니다. 다시 시도해주세요.\n");
                break;
        }
    }
}

void list_room_details() {
    DIR *d = opendir(".");
    if (d) {
        printf("\n========== 게임방 상세 리스트 ==========\n");
        struct dirent *dir;
        struct stat file_stat;

        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                if (stat(dir->d_name, &file_stat) == 0) {
                    printf("방 ID: %s\n", dir->d_name);
                    printf("  - 생성 시간: %s", ctime(&file_stat.st_ctime));
                    printf("  - 마지막 수정 시간: %s", ctime(&file_stat.st_mtime));
                    printf("  - 접근 권한: %o\n", file_stat.st_mode & 0777);
                } else
                    perror("Error getting file details");
            }
        }
        closedir(d);
    } else
        fprintf(stderr, "Error opening directory: %s\n", strerror(errno));
}


void change_room_permission() {
    char room_id[256];
    int permission;

    printf("권한을 변경할 게임방 아이디를 입력하세요: ");
    scanf("%s", room_id);
    printf("새로운 권한 (8진수 형식, 예: 755): ");
    scanf("%o", &permission);

    if (chmod(room_id, permission) == 0) {
        printf("게임방 '%s'의 권한이 %o로 변경되었습니다.\n", room_id, permission);
    } else
        fprintf(stderr, "Error changing permissions for room '%s': %s\n", room_id, strerror(errno));

}

void show_room_size() {
    char room_id[256];
    DIR *d;
    struct dirent *dir;
    struct stat file_stat;
    long total_size = 0;
    int file_count = 0;

    printf("크기를 확인할 게임방 아이디를 입력하세요: ");
    scanf("%s", room_id);

    d = opendir(room_id);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                char file_path[512];
                snprintf(file_path, sizeof(file_path), "%s/%s", room_id, dir->d_name);

                if (stat(file_path, &file_stat) == 0) {
                    total_size += file_stat.st_size;
                    file_count++;
                }
            }
        }
        closedir(d);
        printf("게임방 '%s'의 파일 개수: %d\n", room_id, file_count);
        printf("총 크기: %ld bytes\n", total_size);
    } else
        fprintf(stderr, "Error opening room '%s': %s\n", room_id, strerror(errno));

}