cmake_minimum_required(VERSION 3.29)
project(chess_server)

set(CMAKE_C_STANDARD 99)  # C 표준 버전을 설정합니다.
set(CMAKE_CXX_STANDARD 14)

# 소스 디렉토리 추가
include_directories(src)
include_directories(src/auth)
include_directories(src/chat)
include_directories(src/game)
include_directories(src/record)
include_directories(src/server)
include_directories(src/utils)
include_directories(src/admin)
include_directories(libs/cJSON)
include_directories(config)

# 라이브러리 포함 (dd 디렉토리에 라이브러리가 있을 경우)
add_subdirectory(libs/cJSON)  # 소스가 프로젝트 내에 있다고 가정

# 실행 파일 추가
add_executable(chess_server
        src/main.c
        src/auth/auth.c
        src/chat/chat.c
        src/game/game.c
        src/record/record.c
        src/server/server.c
        src/server/handler.c
        src/utils/db.c
        src/utils/utils.c
        src/room/room.c
        src/admin/admin.c
)

# dd 라이브러리 링크
target_link_libraries(chess_server PRIVATE cjson)