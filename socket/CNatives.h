#pragma once

#include "main.h"
int n_create_socket(int type);
int n_bind_socket(int socketid, std::string ip);
int n_set_max_connections(int socketid, int maxconnections);
int n_connect_socket(int socketid, std::string address, int port);
int n_listen_socket(int socketid, int port);
int n_stop_listen_socket(int socketid);
int n_destroy_socket(int socketid);
int n_close_remote_connection(int socketid, int remoteclientid);
int n_send_socket(int socketid, char * data, int size);
int n_sendto_remote_client(int clientid, int remoteclientid, char * data);
int n_is_remote_client_connected(int socketid, int remoteclientid);
int n_is_socket_valid(int socketid);
std::string n_get_remote_client_ip(int socketid, int remoteclientid);