#include <stdio.h>
#include <unistd.h>
#include "game.h"
#include "cJSON.h"

void set_forfeit(int* forfeit) {
    *forfeit = 1;
    printf("Player has forfeited the game.\n");
}

void forfeit_game_handler(int client_socket, cJSON *json_request) {
    const cJSON *room_id_json = cJSON_GetObjectItemCaseSensitive(json_request, "room_id");

    if (!cJSON_IsNumber(room_id_json)) {
        const char *error_response =
                "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid room_id";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    int room_id = room_id_json->valueint;

    int forfeit_status = 0;
    set_forfeit(&forfeit_status);

    char response[512];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n\r\n"
             "{\"status\":\"success\",\"message\":\"Game forfeited\",\"room_id\":%d}",
             room_id);
    write(client_socket, response, strlen(response));
}
