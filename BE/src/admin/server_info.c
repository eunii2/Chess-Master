#include <stdio.h>
#include <sys/utsname.h>
#include <unistd.h>
#include "admin.h"

void get_server_info() {
    struct utsname sys_info;
    if (uname(&sys_info) == 0) {
        printf("\n========== 서버 정보 ==========\n");
        printf("시스템 이름: %s\n", sys_info.sysname);
        printf("노드 이름: %s\n", sys_info.nodename);
        printf("릴리즈: %s\n", sys_info.release);
        printf("버전: %s\n", sys_info.version);
        printf("머신: %s\n", sys_info.machine);
        printf("현재 폴더 위치: %s\n", getcwd(NULL, 0));
        printf("==============================\n\n");
    } else {
        perror("uname");
    }
}