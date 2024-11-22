#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include "server.h"
#include "admin.h"


void manage_rooms(int admin_socket, int command, char *params) {
    switch (command) {
        case 2:
            list_rooms(admin_socket);
            break;
        case 3:
            delete_room(admin_socket, params);
            break;
        case 4:
            rename_room(admin_socket, params);
            break;
        case 5:
            room_details(admin_socket, params);
            break;
        case 6:
            change_permissions(admin_socket, params);
            break;
        case 7:
            room_size(admin_socket, params);
            break;
        default:
            send(admin_socket, "알 수 없는 명령입니다.\n", strlen("알 수 없는 명령입니다.\n"), 0);
    }
}

void list_rooms(int admin_socket) {
    // Increase buffer size as needed to accommodate all data
    char buffer[BUFFER_SIZE * 10]; // Assuming 10x the normal size
    char temp[BUFFER_SIZE];
    DIR *d;
    struct dirent *dir;

    buffer[0] = '\0'; // Initialize the buffer

    if ((d = opendir("../data/game")) != NULL) {
        snprintf(temp, sizeof(temp), "\n========== 게임방 리스트 ==========\n");
        strncat(buffer, temp, sizeof(buffer) - strlen(buffer) - 1);

        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                snprintf(temp, sizeof(temp), "%s\n", dir->d_name);
                strncat(buffer, temp, sizeof(buffer) - strlen(buffer) - 1);
            }
        }
        closedir(d);

        // Send the entire buffer at once
        send(admin_socket, buffer, strlen(buffer), 0);
    } else {
        snprintf(buffer, sizeof(buffer), "Error: Unable to open game directory: %s\n", strerror(errno));
        send(admin_socket, buffer, strlen(buffer), 0);
    }
}

void delete_room(int admin_socket, char *params) {
    char buffer[BUFFER_SIZE];
    char room_id[256];

    if (params == NULL || sscanf(params, "%s", room_id) != 1) {
        snprintf(buffer, sizeof(buffer), "Error: Room ID is required.\n");
    } else {
        char path[512];
        snprintf(path, sizeof(path), "../data/game/%s", room_id);

        if (rmdir(path) == 0) {
            snprintf(buffer, sizeof(buffer), "게임방 '%s'이(가) 삭제되었습니다.\n", room_id);
        } else {
            snprintf(buffer, sizeof(buffer), "Error deleting room '%s': %s\n", room_id, strerror(errno));
        }
    }
    send(admin_socket, buffer, strlen(buffer), 0);
}

void rename_room(int admin_socket, char *params) {
    char buffer[BUFFER_SIZE];
    char old_name[256], new_name[256];

    if (params == NULL || sscanf(params, "%s %s", old_name, new_name) != 2) {
        snprintf(buffer, sizeof(buffer), "Error: Old and new room IDs are required.\n");
    } else {
        char old_path[512], new_path[512];
        snprintf(old_path, sizeof(old_path), "../data/game/%s", old_name);
        snprintf(new_path, sizeof(new_path), "../data/game/%s", new_name);

        if (rename(old_path, new_path) == 0) {
            snprintf(buffer, sizeof(buffer), "게임방 아이디가 '%s'에서 '%s'으로 변경되었습니다.\n", old_name, new_name);
        } else {
            snprintf(buffer, sizeof(buffer), "Error renaming room '%s' to '%s': %s\n", old_name, new_name, strerror(errno));
        }
    }
    send(admin_socket, buffer, strlen(buffer), 0);
}

void room_details(int admin_socket, char *params) {
    char buffer[BUFFER_SIZE];
    char room_id[256];
    struct stat file_stat;
    char path[512];

    if (params == NULL || sscanf(params, "%s", room_id) != 1) {
        snprintf(buffer, sizeof(buffer), "Error: Room ID is required.\n");
    } else {
        snprintf(path, sizeof(path), "../data/game/%s", room_id);

        if (stat(path, &file_stat) == 0) {
            snprintf(buffer, sizeof(buffer),
                     "방 ID: %s\n- 생성 시간: %s- 마지막 수정 시간: %s- 접근 권한: %o\n",
                     room_id, ctime(&file_stat.st_ctime), ctime(&file_stat.st_mtime), file_stat.st_mode & 0777);
        } else {
            snprintf(buffer, sizeof(buffer), "Error getting details for room '%s': %s\n", room_id, strerror(errno));
        }
    }
    send(admin_socket, buffer, strlen(buffer), 0);
}


void change_permissions(int admin_socket, char *params) {
    char buffer[BUFFER_SIZE];
    char room_id[256];
    int permissions;

    if (params == NULL || sscanf(params, "%s %o", room_id, &permissions) != 2) {
        snprintf(buffer, sizeof(buffer), "Error: Room ID and permissions (octal) are required.\n");
    } else {
        char path[512];
        snprintf(path, sizeof(path), "../data/game/%s", room_id);

        if (chmod(path, permissions) == 0) {
            snprintf(buffer, sizeof(buffer), "게임방 '%s'의 권한이 %o로 변경되었습니다.\n", room_id, permissions);
        } else {
            snprintf(buffer, sizeof(buffer), "Error changing permissions for room '%s': %s\n", room_id, strerror(errno));
        }
    }
    send(admin_socket, buffer, strlen(buffer), 0);
}


void room_size(int admin_socket, char *params) {
    char buffer[BUFFER_SIZE];
    char room_id[256];
    char path[512];
    DIR *d;
    struct dirent *dir;
    struct stat file_stat;
    long total_size = 0;
    int file_count = 0;

    if (params == NULL || sscanf(params, "%s", room_id) != 1) {
        snprintf(buffer, sizeof(buffer), "Error: Room ID is required.\n");
    } else {
        snprintf(path, sizeof(path), "../data/game/%s", room_id);

        if ((d = opendir(path)) != NULL) {
            while ((dir = readdir(d)) != NULL) {
                if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                    char file_path[512];
                    snprintf(file_path, sizeof(file_path), "%s/%s", path, dir->d_name);

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
    }
    send(admin_socket, buffer, strlen(buffer), 0);
}