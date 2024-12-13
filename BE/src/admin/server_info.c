#include <stdio.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

// 서버 정보를 문자열로 포맷하여 반환하는 함수
char *get_server_info() {
    struct utsname sys_info;
    char *info_buffer = calloc(1024, sizeof(char));
    if (info_buffer == NULL) {
        perror("Memory allocation error");
        return NULL;
    }

    if (uname(&sys_info) == 0) {
        snprintf(info_buffer, 1024,
                 "\n========== 서버 정보 ==========\n"
                 "시스템 이름: %s\n"
                 "노드 이름: %s\n"
                 "릴리즈: %s\n"
                 "버전: %s\n"
                 "머신: %s\n"
                 "현재 폴더 위치: %s\n"
                 "==============================\n\n",
                 sys_info.sysname, sys_info.nodename, sys_info.release,
                 sys_info.version, sys_info.machine, getcwd(NULL, 0));
    } else {
        snprintf(info_buffer, 1024, "Error getting system information: %s\n", strerror(errno));
    }
    return info_buffer;
}

void fork_and_print_server_info() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        char *info = get_server_info();
        if (info) {
            printf("%s", info);
            free(info);
        }
        exit(EXIT_SUCCESS);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        } else {
            printf("Child process did not exit normally\n");
        }
    }
}

#ifdef TEST_SERVER_INFO
int main() {
    printf("Forking to fetch server info...\n");
    fork_and_print_server_info();
    printf("Back in the main process.\n");
    return 0;
}
#endif
