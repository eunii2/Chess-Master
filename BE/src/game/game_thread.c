#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "config.h"
#include "game.h"

// 게임 진행을 관리하는 스레드 함수
void* game_thread(void* arg) {
    int room_id = ((int*)arg)[0];
    int* forfeit = &((int*)arg)[1];
    free(arg);

    char log_file_path[256];
    snprintf(log_file_path, sizeof(log_file_path), LOG_FILE_PATH_FORMAT, room_id);

    FILE *log_file = fopen(log_file_path, "a");
    if (!log_file) {
        perror("fopen failed");
        return NULL;
    }

    // 프로세스 ID와 방 정보를 기록
    pid_t process_id = getpid();
    fprintf(log_file, "Game started in room %d by process %d\n", room_id, process_id);
    fflush(log_file);

    // 게임 상태를 지속적으로 업데이트
    while (1) {
        printf("Updating game state in room %d\n", room_id);

        // 포기 상태 확인 및 게임 종료 처리
        if (*forfeit) {
            fprintf(log_file, "Player forfeited. Game in room %d ended by process %d\n", room_id, process_id);
            fflush(log_file);
            break;
        }

        // 말 이동 이벤트 기록
        fprintf(log_file, "Player moved piece from A to B by process %d\n", process_id);
        fflush(log_file);

        sleep(1);
    }

    fclose(log_file);
    return NULL;
}
