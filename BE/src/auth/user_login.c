#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"
#include "config.h"
#include "utils.h"

// 로그인 처리 함수
void login_handler(int client_socket, cJSON *json_request) {
    const cJSON *username = cJSON_GetObjectItemCaseSensitive(json_request, "username");
    const cJSON *password = cJSON_GetObjectItemCaseSensitive(json_request, "password");

    if (!cJSON_IsString(username) || !cJSON_IsString(password)) {
        char error_response[512];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 400 Bad Request\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"Invalid input\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    if (access(USER_LIST_FILE, F_OK) == -1) {
        perror("access error");
        char error_response[512];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 500 Internal Server Error\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"User data not found\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));
        return;
    }

    FILE *file = fopen(USER_LIST_FILE, "r");
    if (!file) {
        perror("fopen error");
        return;
    }

    char *lines[1024];
    int line_count = 0;
    int user_found = 0;
    int matched_line_index = -1;
    char buffer[512];

    while (fgets(buffer, sizeof(buffer), file)) {
        lines[line_count] = strdup(buffer);
        if (!lines[line_count]) {
            perror("strdup error");
            fclose(file);
            return;
        }

        char *username_start = strstr(buffer, "Username: ");
        char *password_start = strstr(buffer, "Password: ");
        if (username_start && password_start) {
            char existing_username[256];
            username_start += strlen("Username: ");
            sscanf(username_start, "%[^, \n]", existing_username);

            char existing_password[256];
            password_start += strlen("Password: ");
            sscanf(password_start, "%[^, \n]", existing_password);

            if (strcmp(existing_username, username->valuestring) == 0 &&
                strcmp(existing_password, password->valuestring) == 0) {
                user_found = 1;
                matched_line_index = line_count;
            }
        }
        line_count++;
    }

    fclose(file);

    if (!user_found) {
        char error_response[512];
        snprintf(error_response, sizeof(error_response),
                 "HTTP/1.1 401 Unauthorized\r\n"
                 "Content-Type: application/json\r\n"
                 "%s\r\n"
                 "{\"status\":\"error\",\"message\":\"Invalid username or password\"}",
                 cors_headers);
        write(client_socket, error_response, strlen(error_response));

        for (int i = 0; i < line_count; i++) {
            free(lines[i]);
        }
        return;
    }

    char token[TOKEN_LENGTH + 1];
    generate_token(token, TOKEN_LENGTH);

    char *line = lines[matched_line_index];
    char temp_line[512];

    char *token_pos = strstr(line, ", Token: ");
    if (token_pos) {
        *token_pos = '\0';
    } else {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
    }

    snprintf(temp_line, sizeof(temp_line), "%s, Token: %s\n", line, token);
    free(lines[matched_line_index]);
    lines[matched_line_index] = strdup(temp_line);
    if (!lines[matched_line_index]) {
        perror("strdup error");
        for (int i = 0; i < line_count; i++) {
            if (i != matched_line_index) {
                free(lines[i]);
            }
        }
        return;
    }

    file = fopen(USER_LIST_FILE, "w");
    if (!file) {
        perror("fopen error");
        for (int i = 0; i < line_count; i++) {
            free(lines[i]);
        }
        return;
    }

    for (int i = 0; i < line_count; i++) {
        fputs(lines[i], file);
    }

    fclose(file);

    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }

    char success_response[512];
    snprintf(success_response, sizeof(success_response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "%s\r\n"
             "{\"status\":\"success\",\"message\":\"Login successful\",\"token\":\"%s\"}",
             cors_headers, token);
    write(client_socket, success_response, strlen(success_response));
}