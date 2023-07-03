//Include file for standard system include files
//or project specific include files
#pragma once
//Networking header files and libraries
#if defined (_WIN32) //Winsock 
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <conio.h>
#pragma comment (lib, "ws2_32.lib")
#else //Berkeley sockets
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <signal.h>
#endif
//abstraction of types across diffrent operating systems
#if !defined (_WIN32)//for unix systems
#define SOCKET int
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define GETSOCKETERRNO() (errno)
#define PCSTR const char*
#define SIOCTL(socket, cmd, argp) ioctl(socket, cmd, argp)
#define POLL(fds, nfds, timout) poll(fds, nfds, timeout)
#endif 
#if defined (_WIN32)//for windows
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())
#define SIOCTL(socket, cmd, argp) ioctlsocket(socket, cmd, argp)
#define POLL(fds, nfds, timeout) WSAPoll(fds, nfds, timeout)
#endif

#if !defined(IPV6_V6ONLY)
#define IPV6_V6ONLY 27
#endif
// TODO: Reference additional headers your program requires here.
