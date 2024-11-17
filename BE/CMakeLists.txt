cmake_minimum_required(VERSION 3.1)
project(chess_server)

set(CMAKE_C_STANDARD 99)  # C 표준 버전을 설정
set(CMAKE_CXX_STANDARD 14)

find_package(Threads REQUIRED)


# 소스 디렉토리 추가
include_directories(src)
include_directories(src/auth)
include_directories(src/chat)
include_directories(src/game)
include_directories(src/record)
include_directories(src/room)
include_directories(src/server)
include_directories(src/utils)
include_directories(src/admin)
include_directories(libs/cJSON)
include_directories(config)

# 라이브러리 포함
add_subdirectory(libs/cJSON)

# 실행 파일 추가
add_executable(chess_server
        src/main.c
        src/chat/chat.c
        src/record/record.c
        src/server/server.c
        src/server/handler.c
        src/utils/db.c
        src/utils/utils.c
        src/room/create_room.c
        src/room/join_room.c
        src/room/get_room_list.c
        src/room/leave_room.c
        src/room/get_room_status.c
        src/auth/user_join.c
        src/auth/user_login.c
        src/admin/server_info.c
        src/admin/room_manage.c
        src/game/forfeit.c
        src/game/start_game.c
        src/game/game_thread.c
        src/game/game.h
        src/game/move_piece.c
        src/game/get_game_status.c
)

# 라이브러리 링크
target_link_libraries(chess_server PRIVATE cjson ${CMAKE_THREAD_LIBS_INIT})