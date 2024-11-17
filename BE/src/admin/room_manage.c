#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include "server.h"
#include <stdlib.h>

void change_room_permission(int admin_socket);
void list_room_details(int admin_socket);
void show_room_size(int admin_socket);


void manage_rooms(int admin_socket) {
    int choice;
    char buffer[BUFFER_SIZE];
    char room_id[256];
    char new_room_id[256];
    char original_path[512];

    if (getcwd(original_path, sizeof(original_path)) == NULL) {
        perror("Error getting current directory");
        return;
    }

    // 방 관리 메뉴를 클라이언트에 처음에 한 번만 전송
    snprintf(buffer, sizeof(buffer), "\n========== 방 관리 메뉴 ==========\n"
                                     "1. 게임방 아이디 리스트 조회\n"
                                     "2. 게임방 삭제\n"
                                     "3. 게임방 아이디 변경\n"
                                     "4. 게임방 상세 정보 조회\n"
                                     "5. 게임방 접근 권한 변경\n"
                                     "6. 게임방 파일 개수 및 크기 조회\n"
                                     "7. 이전 메뉴로 돌아가기\n"
                                     "=================================\n"
                                     "선택하세요 (1-7): ");
    send(admin_socket, buffer, strlen(buffer), 0);






    while (1) {
        // 클라이언트로부터 선택지 수신
        int str_len = recv(admin_socket, buffer, BUFFER_SIZE - 1, 0);
        if (str_len <= 0) {
            perror("recv error or admin disconnected");
            chdir(original_path);
            close(admin_socket);
            pthread_exit(NULL);
        }
        buffer[str_len] = 0;
        choice = atoi(buffer);

        // 선택지에 따라 작업 수행
        switch (choice) {
            case 1:
                if (chdir("../data/game") != 0) {
                    perror("Error changing to game directory");
                    return;
                }
                DIR *d = opendir(".");
                char buffer[BUFFER_SIZE];

                if (d) {
                    snprintf(buffer, sizeof(buffer), "\n========== 게임방 리스트 ==========\n");
                    send(admin_socket, buffer, strlen(buffer), 0);

                    struct dirent *dir;
                    while ((dir = readdir(d)) != NULL) {
                        if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                            snprintf(buffer, sizeof(buffer), "%s\n", dir->d_name);
                            send(admin_socket, buffer, strlen(buffer), 0);
                        }
                    }
                    closedir(d);
                } else {
                    snprintf(buffer, sizeof(buffer), "Error opening directory: %s\n", strerror(errno));
                    send(admin_socket, buffer, strlen(buffer), 0);
                }
                chdir(original_path);
                break;
            case 2:
                if (chdir("../data/game") != 0) {
                    perror("Error changing to game directory");
                    return;
                }
                snprintf(buffer, sizeof(buffer), "삭제할 게임방 아이디를 입력하세요: ");
                send(admin_socket, buffer, strlen(buffer), 0);
                str_len = recv(admin_socket, room_id, sizeof(room_id) - 1, 0);
                if (str_len <= 0) {
                    perror("recv error or admin disconnected");
                    chdir(original_path);
                    close(admin_socket);
                    pthread_exit(NULL);
                }
                room_id[str_len] = 0;
                if (rmdir(room_id) == 0) {
                    snprintf(buffer, sizeof(buffer), "게임방 '%s'이(가) 삭제되었습니다.\n", room_id);
                } else {
                    snprintf(buffer, sizeof(buffer), "Error deleting room '%s': %s\n", room_id, strerror(errno));
                }
                send(admin_socket, buffer, strlen(buffer), 0);
                chdir(original_path);
                break;
            case 3:
                if (chdir("../data/game") != 0) {
                    perror("Error changing to game directory");
                    return;
                }
                snprintf(buffer, sizeof(buffer), "변경할 게임방 아이디를 입력하세요: ");
                send(admin_socket, buffer, strlen(buffer), 0);
                str_len = recv(admin_socket, room_id, sizeof(room_id) - 1, 0);
                if (str_len <= 0) {
                    perror("recv error or admin disconnected");
                    chdir(original_path);
                    close(admin_socket);
                    pthread_exit(NULL);
                }
                room_id[str_len] = 0;

                snprintf(buffer, sizeof(buffer), "새로운 게임방 아이디를 입력하세요: ");
                send(admin_socket, buffer, strlen(buffer), 0);
                str_len = recv(admin_socket, new_room_id, sizeof(new_room_id) - 1, 0);
                if (str_len <= 0) {
                    perror("recv error or admin disconnected");
                    chdir(original_path);
                    close(admin_socket);
                    pthread_exit(NULL);
                }
                new_room_id[str_len] = 0;

                DIR *check_dir = opendir(new_room_id);
                if (check_dir != NULL) {
                    snprintf(buffer, sizeof(buffer), "이미 존재하는 게임방 아이디 '%s'입니다. 다른 이름을 입력하세요.\n", new_room_id);
                    closedir(check_dir);
                } else if (rename(room_id, new_room_id) == 0) {
                    snprintf(buffer, sizeof(buffer), "게임방 아이디가 '%s'에서 '%s'으로 변경되었습니다.\n", room_id, new_room_id);
                } else {
                    snprintf(buffer, sizeof(buffer), "Error renaming room '%s' to '%s': %s\n", room_id, new_room_id, strerror(errno));
                }
                send(admin_socket, buffer, strlen(buffer), 0);
                chdir(original_path);
                break;
            case 4:
                if (chdir("../data/game") != 0) {
                    perror("Error changing to game directory");
                    return;
                }
                list_room_details(admin_socket);
                chdir(original_path);
                break;
            case 5:
                if (chdir("../data/game") != 0) {
                    perror("Error changing to game directory");
                    return;
                }
                change_room_permission(admin_socket);
                chdir(original_path);
                break;
            case 6:
                if (chdir("../data/game") != 0) {
                    perror("Error changing to game directory");
                    return;
                }
                show_room_size(admin_socket);
                chdir(original_path);
                break;
            case 7:
                return;
            default:
                snprintf(buffer, sizeof(buffer), "잘못된 입력입니다. 다시 시도해주세요.\n");
                send(admin_socket, buffer, strlen(buffer), 0);
                break;
        }
    }
}


void list_room_details(int admin_socket) {
    DIR *d = opendir(".");
    char buffer[BUFFER_SIZE];
    if (d) {
        snprintf(buffer, sizeof(buffer), "\n========== 게임방 상세 리스트 ==========\n");
        send(admin_socket, buffer, strlen(buffer), 0);
        struct dirent *dir;
        struct stat file_stat;

        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                if (stat(dir->d_name, &file_stat) == 0) {
                    snprintf(buffer, sizeof(buffer), "방 ID: %s\n  - 생성 시간: %s  - 마지막 수정 시간: %s  - 접근 권한: %o\n",
                             dir->d_name, ctime(&file_stat.st_ctime), ctime(&file_stat.st_mtime), file_stat.st_mode & 0777);
                    send(admin_socket, buffer, strlen(buffer), 0);
                } else {
                    snprintf(buffer, sizeof(buffer), "Error getting file details for '%s': %s\n", dir->d_name, strerror(errno));
                    send(admin_socket, buffer, strlen(buffer), 0);
                }
            }
        }
        closedir(d);
    } else {
        snprintf(buffer, sizeof(buffer), "Error opening directory: %s\n", strerror(errno));
        send(admin_socket, buffer, strlen(buffer), 0);
    }
}

void change_room_permission(int admin_socket) {
    char room_id[256];
    int permission;
    char buffer[BUFFER_SIZE];

    snprintf(buffer, sizeof(buffer), "권한을 변경할 게임방 아이디를 입력하세요: ");
    send(admin_socket, buffer, strlen(buffer), 0);
    int str_len = recv(admin_socket, room_id, sizeof(room_id) - 1, 0);
    if (str_len <= 0) {
        perror("recv error or admin disconnected");
        close(admin_socket);
        pthread_exit(NULL);
    }
    room_id[str_len] = 0;

    snprintf(buffer, sizeof(buffer), "새로운 권한 (8진수 형식, 예: 755): ");
    send(admin_socket, buffer, strlen(buffer), 0);
    str_len = recv(admin_socket, buffer, sizeof(buffer) - 1, 0);
    if (str_len <= 0) {
        perror("recv error or admin disconnected");
        close(admin_socket);
        pthread_exit(NULL);
    }
    buffer[str_len] = 0;
    permission = strtol(buffer, NULL, 8);

    if (chmod(room_id, permission) == 0) {
        snprintf(buffer, sizeof(buffer), "게임방 '%s'의 권한이 %o로 변경되었습니다.\n", room_id, permission);
    } else {
        snprintf(buffer, sizeof(buffer), "Error changing permissions for room '%s': %s\n", room_id, strerror(errno));
    }
    send(admin_socket, buffer, strlen(buffer), 0);
}

void show_room_size(int admin_socket) {
    char room_id[256];
    DIR *d;
    struct dirent *dir;
    struct stat file_stat;
    long total_size = 0;
    int file_count = 0;
    char buffer[BUFFER_SIZE];

    snprintf(buffer, sizeof(buffer), "크기를 확인할 게임방 아이디를 입력하세요: ");
    send(admin_socket, buffer, strlen(buffer), 0);
    int str_len = recv(admin_socket, room_id, sizeof(room_id) - 1, 0);
    if (str_len <= 0) {
        perror("recv error or admin disconnected");
        close(admin_socket);
        pthread_exit(NULL);
    }
    room_id[str_len] = 0;

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
        snprintf(buffer, sizeof(buffer), "게임방 '%s'의 파일 개수: %d\n총 크기: %ld bytes\n", room_id, file_count, total_size);
    } else {
        snprintf(buffer, sizeof(buffer), "Error opening room '%s': %s\n", room_id, strerror(errno));
    }
    send(admin_socket, buffer, strlen(buffer), 0);
}