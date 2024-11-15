#include <stdio.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "admin.h"
#include<stdlib.h>


char *get_server_info() {
    struct utsname sys_info;
    char *info_buffer = calloc(1024, sizeof(char));
    if (info_buffer == NULL) {
        perror("Memory allocation error");
        return NULL;
    }

    if (uname(&sys_info) == 0) {
        // 서버 정보를 문자열로 포맷하여 info_buffer에 저장
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
    return info_buffer;  // 포맷된 문자열 반환
}