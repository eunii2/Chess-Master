#ifndef SERVER_PROJECT_IMAGE_H
#define SERVER_PROJECT_IMAGE_H

#include "cJSON.h"
#include <netinet/in.h>

#define MAX_BUFFER_SIZE 1024

typedef struct UserEntry {
    int user_id;
    char *image_address;
} UserEntry;

typedef struct {
    UserEntry *entries;
    size_t size;
    size_t capacity;
} UserList;


void image_upload_handler(int client_socket, cJSON *json_request);
void load_user_list(UserList *user_list);
void save_user_list(UserList *user_list);
void free_user_list(UserList *user_list);
int compare_user_entries(const void *a, const void *b);

#endif //SERVER_PROJECT_IMAGE_H
