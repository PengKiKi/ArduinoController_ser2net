#pragma once

#include "CNatives.h"

extern CSocket* g_pSocket;


int n_create_socket(int type)
{
	return g_pSocket->create_socket(type);
}

int n_bind_socket(int socketid, std::string ip)
{
	return g_pSocket->bind_socket(socketid, (char*)ip.c_str());
}

int n_set_max_connections(int socketid, int maxconnections)
{
	return g_pSocket->set_max_connections(socketid, maxconnections);
}

int n_connect_socket(int socketid, std::string address, int port)
{
	return g_pSocket->connect_socket(socketid, (char*)address.c_str(), port);
}

int n_listen_socket(int socketid, int port)
{
	return g_pSocket->listen_socket(socketid, port);
}

int n_stop_listen_socket(int socketid)
{
	return g_pSocket->stop_listen_socket(socketid);
}

int n_destroy_socket(int socketid)
{
	return g_pSocket->destroy_socket(socketid);
}

int n_close_remote_connection(int socketid, int remoteclientid)
{
	return g_pSocket->close_remote_connection(socketid, remoteclientid);
}

int n_send_socket(int socketid, char * data, int size)
{
	return g_pSocket->send_socket(socketid, data, size);
}

int n_sendto_remote_client(int clientid, int remoteclientid, char * data)
{
	int ret_val = g_pSocket->sendto_remote_client(clientid, remoteclientid, data);
	return ret_val;
}

int n_is_remote_client_connected(int socketid, int remoteclientid)
{
	return g_pSocket->is_remote_client_connected(socketid, remoteclientid);
}

int n_is_socket_valid(int socketid)
{
	return g_pSocket->is_socket_valid(socketid);
}

std::string n_get_remote_client_ip(int socketid, int remoteclientid)
{
	return  g_pSocket->get_remote_client_ip(socketid, remoteclientid);
}