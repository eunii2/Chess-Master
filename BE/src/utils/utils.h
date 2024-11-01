#ifndef CHESS_SERVER_UTILS_H
#define CHESS_SERVER_UTILS_H

#include "cJSON.h"

cJSON *parse_json(const char *json_string);
void generate_token(char *token, size_t length);

#endif //CHESS_SERVER_UTILS_H
