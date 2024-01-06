#ifndef SOCKSET_FRAME_INCLUDE_H
#define SOCKSET_FRAME_INCLUDE_H
#include <iostream>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#define SHUT_RDWR 2
#else
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define MAX_BYTE 0xff
typedef unsigned long long SOCKET;
#endif
#define INTERNET_PORT 10086
#define INVALID_SOCKET 100000
void Recv(int __fd, void *__buf, size_t __n, int __flags);
void Send(int __fd, const void *__buf, size_t __n, int __flags);
// void Send(int __fd, const GameMessage *__buf, size_t __n, int __flags);
#endif