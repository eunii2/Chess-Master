#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define LOG_FILE_PATH_FORMAT "../data/game/%d/game_log.txt"

// 게임 스레드 함수
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

    fprintf(log_file, "Game started in room %d\n", room_id);
    fflush(log_file);

    // 게임 진행 루프
    while (1) {
        printf("Updating game state in room %d\n", room_id);

        // 게임 종료 조건 확인 (기권 여부)
        if (*forfeit) {
            fprintf(log_file, "Player forfeited. Game in room %d ended\n", room_id);
            fflush(log_file);
            break;
        }
        // 예: 말을 이동하는 로직 처리 및 로그 기록
        fprintf(log_file, "Player moved piece from A to B\n");
        fflush(log_file);

        sleep(1);
    }

    fclose(log_file);
    printf("Game in room %d has ended\n", room_id);

    pthread_exit(NULL);
}

// 게임 시작 함수
void start_game_in_room(int room_id) {
    pthread_t thread;
    int* args = malloc(2 * sizeof(int));
    args[0] = room_id;
    args[1] = 0;

    if (pthread_create(&thread, NULL, game_thread, args) != 0) {
        perror("pthread_create failed");
        free(args);
        return;
    }

    pthread_detach(thread);
    printf("Game thread for room %d started\n", room_id);
}

void set_forfeit(int* forfeit) {
    *forfeit = 1; // 기권 상태 설정
    printf("Player has forfeited the game.\n");
}