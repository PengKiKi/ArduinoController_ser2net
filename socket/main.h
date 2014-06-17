#pragma once

#pragma warning (disable:4005 4700 996)

#if (defined(WIN32) || defined(_WIN32) || defined(_WIN64))
	#include "winsock2.h"
	#include "windows.h"
	#include "ws2tcpip.h"
	#pragma comment(lib, "ws2_32.lib")
#else
	#include "sys/types.h"
	#include "sys/socket.h"
	#include "arpa/inet.h"
	#include "netdb.h"
	#include "fcntl.h"
	#include "unistd.h"
	#include "pthread.h"
#endif
#include "queue"
#include "list"
#include "string.h"
#include "CThread.h"
#include "CSocket.h"
#include "CNatives.h"

#define VERSION "0.1b"

#ifdef WIN32
	#define SLEEP(x) { Sleep(x); }
#else
	#define SLEEP(x) { usleep(x * 1000); }
	#define SOCKET_ERROR (-1)
	typedef unsigned long DWORD;
	typedef unsigned int UINT;
#endif

typedef void (*logprintf_t)(char* format, ...);


struct remoteConnect {
	int socketid;
	char* remote_client;
	int remote_clientid;
};

struct remoteDisconnect {
	int socketid;
	int remote_clientid;
};

struct receiveData {
	int socketid;
	int remote_clientid;
	char* data;
	int data_len;
};

struct socketAnswer {
	int socketid;
	char* data;
	int data_len;
};

struct socketClose {
	int socketid;
};

struct socketUDP {
	int socketid;
	char* data;
	int data_len;
	char* remote_ip;
	int remote_port;
};

extern std::queue<remoteConnect> onRemoteConnect;
extern std::queue<remoteDisconnect> onRemoteDisconnect;
extern std::queue<receiveData> onSocketReceiveData;
extern std::queue<socketAnswer> onSocketAnswer;
extern std::queue<socketClose> onSocketClose;
extern std::queue<socketUDP> onUDPReceiveData;
