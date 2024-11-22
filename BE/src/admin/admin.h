#ifndef CHESS_SERVER_ADMIN_H
#define CHESS_SERVER_ADMIN_H

char *get_server_info();
void manage_rooms();


void list_rooms(int admin_socket);
void delete_room(int admin_socket, char *room_id);
void rename_room(int admin_socket, char *room_data);
void room_details(int admin_socket, char *room_id);
void change_permissions(int admin_socket, char *room_data);
void room_size(int admin_socket, char *room_id);

#endif //CHESS_SERVER_ADMIN_H
