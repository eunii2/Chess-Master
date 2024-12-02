#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "config.h"
#include "game.h"
#include "utils.h"

void get_user_game_history_handler(int client_socket, cJSON *json_request) {
    const cJSON *token_json = cJSON_GetObjectItemCaseSensitive(json_request, "token");
    if (!cJSON_IsString(token_json)) {
        const char *error_response = "HTTP/1.1 400 Bad Request\r\n"
                                   "Content-Type: application/json\r\n\r\n"
                                   "{\"status\":\"error\",\"message\":\"Invalid token\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    const char* user_token = token_json->valuestring;
    const char* username = get_user_name_by_token(user_token);
    if (!username) {
        const char *error_response = "HTTP/1.1 401 Unauthorized\r\n"
                                   "Content-Type: application/json\r\n\r\n"
                                   "{\"status\":\"error\",\"message\":\"Invalid token\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }
    
    DIR *dir;
    struct dirent *entry;
    cJSON *games_array = cJSON_CreateArray();
    
    dir = opendir("../data/game");
    if (dir == NULL) {
        const char *error_response = "HTTP/1.1 500 Internal Server Error\r\n"
                                   "Content-Type: application/json\r\n\r\n"
                                   "{\"status\":\"error\",\"message\":\"Cannot access game directory\"}";
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char info_path[256];
            snprintf(info_path, sizeof(info_path), "../data/game/%s/info.txt", entry->d_name);
            
            FILE *info_file = fopen(info_path, "r");
            if (info_file) {
                char line[256];
                int is_user_game = 0;
                char room_name[256] = "";
                
                while (fgets(line, sizeof(line), info_file)) {
                    if (strncmp(line, "Room Name: ", 11) == 0) {
                        strncpy(room_name, line + 11, sizeof(room_name) - 1);
                        room_name[strcspn(room_name, "\n")] = 0;
                    } else if (strncmp(line, "Username: ", 10) == 0) {
                        char current_username[256];
                        if (sscanf(line, "Username: %[^,]", current_username) == 1) {
                            if (strcmp(current_username, username) == 0) {
                                is_user_game = 1;
                            }
                        }
                    }
                }

                if (is_user_game) {
                    cJSON *game_obj = cJSON_CreateObject();
                    cJSON_AddStringToObject(game_obj, "roomId", entry->d_name);
                    cJSON_AddStringToObject(game_obj, "roomName", room_name);

                    char history_path[256];
                    snprintf(history_path, sizeof(history_path), "../data/game/%s/history.txt", entry->d_name);
                    struct stat st;

                    if (stat(history_path, &st) == 0) {
                        char date[64];
                        strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", localtime(&st.st_mtime));
                        cJSON_AddStringToObject(game_obj, "createdAt", date);
                    }

                    cJSON_AddItemToArray(games_array, game_obj);
                }
                fclose(info_file);
            }
        }
    }
    closedir(dir);

    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", "success");
    cJSON_AddItemToObject(response, "games", games_array);

    char *json_str = cJSON_Print(response);

    char response_header[512];
    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "Access-Control-Allow-Origin: http://localhost:5173\r\n"
             "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE\r\n"
             "Access-Control-Allow-Headers: Content-Type\r\n"
             "Content-Length: %zu\r\n\r\n",
             strlen(json_str));

    write(client_socket, response_header, strlen(response_header));
    write(client_socket, json_str, strlen(json_str));

    free(json_str);
    cJSON_Delete(response);
}
