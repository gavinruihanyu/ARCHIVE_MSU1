#include "../external_headers/mongo_include.h"

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

#include <client.hpp>

namespace router
{
	class http_request {
	public:
		char method[8];
		char uri[512];
		char version[16];
		char* headers;
		int headers_length;
		char* body;
		int body_length;
		client* cli;
	};
	class request
	{
	public:
		int type;
		/*
		1: send
		2: idk
		3: add more here bruh
		*/

		request* prev = nullptr;
		request* next = nullptr;

		struct http_request http_req;
	};
	int parse_http_request(int bytes_recv, char* request_string, struct http_request* request); //execution of function on Gavins desktop takes approx 41 microseconds per small request (single line headers, bodies etc)
	int route(http_request req);
}