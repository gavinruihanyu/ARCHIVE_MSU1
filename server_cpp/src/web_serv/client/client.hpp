#include "../external_headers/networking_include.h"
#include <iostream>
#include <string>
#include <chrono>
#include <regex>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stdlib.h>
#include <fstream>

#define BACKLOG 10
#define newcli_LOCKFREE_CAPACITY 1024
#define BUFFER_SIZE 1024
#define MAX_REQUEST_LENGTH 2048
#define MAX_HEADER_LENGTH 512

class client
{
public:
	SOCKET socket = INVALID_SOCKET;
	struct sockaddr_storage address;
	socklen_t address_length = sizeof(address);
	char address_buffer[128];
	int address_buffer_len = 128;
	client* prev = nullptr;
	client* next = nullptr;

	int operation;

	char buffer[BUFFER_SIZE];
	int buff_size = BUFFER_SIZE;
	int bytes_recv = 0;

	~client()
	{
		socket = INVALID_SOCKET;
	}
	void reset()
	{
		socket = INVALID_SOCKET;
		memset(address_buffer, 0x0, sizeof(address_buffer));
	}
};