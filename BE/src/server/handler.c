#include <string.h>
#include <unistd.h>
#include "handler.h"
#include "cJSON.h"
#include "auth.h"
#include "room.h"
#include "game.h"
#include "image.h"
#include "chat.h"

void handle_request(int client_socket, const char *method, const char *path, cJSON *json_request) {

    // CORS 헤더 추가
    if (strcmp(method, "OPTIONS") == 0) {
        const char *cors_response =
                "HTTP/1.1 204 No Content\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "\r\n";
        write(client_socket, cors_response, strlen(cors_response));
        return;
    }


    if (strcmp(method, "POST") == 0 && strcmp(path, "/join") == 0) {
        // 회원가입 처리
        join_handler(client_socket, json_request);
    }
    else if (strcmp(method, "POST") == 0 && strcmp(path, "/login") == 0) {
        // 로그인 처리
        login_handler(client_socket, json_request);
    }
    else if (strcmp(method, "POST") == 0 && strcmp(path, "/room/create") == 0) {
        // 방 생성 처리
        create_room_handler(client_socket, json_request);
    }
    else if (strcmp(method, "POST") == 0 && strcmp(path, "/room/join") == 0) {
        // 방 목록 조회 처리
        join_room_handler(client_socket, json_request);
    }
    else if (strcmp(method, "POST") == 0 && strcmp(path, "/room") == 0) {
        // 방 목록 조회 처리
        get_room_list_handler(client_socket, json_request);
    }
    else if (strcmp(method, "DELETE") == 0 && strcmp(path, "/room") == 0) {
        // 방 목록 조회 처리
        leave_room_handler(client_socket, json_request);
    }
    else if (strcmp(method, "POST") == 0 && strcmp(path, "/room/status") == 0) {
        // 방 목록 조회 처리
        get_room_status_handler(client_socket, json_request);
    }
    // 이미지 업로드 API 핸들러
    else if (strcmp(method, "POST") == 0 && strcmp(path, "/image/upload") == 0) {
        image_upload_handler(client_socket, json_request);
    }
    //게임 API 핸들러
    else if (strcmp(method, "POST") == 0 && strcmp(path, "/game/start_game") == 0) {
        start_game_handler(client_socket, json_request);
    }
    else if (strcmp(method, "POST") == 0 && strcmp(path, "/game/get_game_status") == 0) {
        get_game_status_handler(client_socket, json_request);
    }
    else if (strcmp(method, "POST") == 0 && strcmp(path, "/game/move_piece") == 0) {
        move_piece_handler(client_socket, json_request);
    }
    else if (strcmp(method, "POST") == 0 && strcmp(path, "/game/forfeit") == 0) {
        forfeit_game_handler(client_socket, json_request);
    }
    else if (strcmp(method, "POST") == 0 && strcmp(path, "/chat/send_message") == 0){
        send_message_handler(client_socket, json_request);
    }


    else {
        // 지원하지 않는 요청에 대한 404 응답
        const char *not_found = "HTTP/1.1 404 Not Found\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: 13\r\n\r\n"
                                "404 Not Found";
        write(client_socket, not_found, strlen(not_found));
    }

    // JSON 객체 해제
    if (json_request != NULL) {
        cJSON_Delete(json_request);
    }
}
