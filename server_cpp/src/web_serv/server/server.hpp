#include "../external_headers/mongo_include.h"
#include "../external_headers/networking_include.h"
#include "../external_headers/boost_include.h"
#include "../external_headers/openssl_include.h"

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

#include <router.hpp>
#include <helpers.hpp>

/*
There is an optimal size to a buffer.
Too small a buffer can trigger more system calls than necessary,
while too big a buffer can trigger unnecessary reloads of the CPU cache.
The best way to answer this question for your specific situation is to use a profiler.*/

#define BACKLOG 10
#define newcli_LOCKFREE_CAPACITY 1024
#define BUFFER_SIZE 1024
#define MAX_REQUEST_LENGTH 2048
#define MAX_HEADER_LENGTH 512


namespace server
{
#if defined (_WIN32)
	int wininit();
	void winfinish();
#endif
	int start(PCSTR serv);
}

