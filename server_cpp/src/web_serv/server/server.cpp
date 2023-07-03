#include "server.hpp"
//cannot be non blocking on initialization, try non blocking after thread is created...
//it works lmfao

//the seceret is to first accept the socket THEN set the socket to non blockig...

namespace sp //thread pool things
{
	class pool //thread pool
	{
	public:
		int thread_number;
		boost::atomic<int> clients = 0;

		pool* next = nullptr;
		pool* prev = nullptr;

		boost::thread thread;
	
		client* cli_in = nullptr;//we will handle the joining of the client within the tp thread to avoid any race conditions with the tp thread or externally
		boost::atomic<bool> new_cli = true; //will be altered after performing actions on the client in.
		boost::atomic<bool> full = false;

		client* cli_a = nullptr; //client anchor node
		client* cli_f = nullptr; //client focus node

		router::request* req_a = nullptr; //request queue anchor node 
		router::request* req_f = nullptr; //request queue focus node
	};

	void process(pool* s_p)
	{
		std::cout << "Server pool [" << s_p->thread_number << "]: " << "Launched.\n";
		client* cli_i = s_p->cli_a;
		while (true)//compiler generates tight loop do not check for a condition or else performance degration (i know its very small buuttt....)
		{
			if (!s_p->new_cli) //client in true
			{
				//std::cout << "Server pool [" << s_p->thread_number << "]: " << "Client incoming.\n";
				if (s_p->cli_a) //anchor node exists
				{
					//std::cout << "Server pool [" << s_p->thread_number << "]: " << "Client appended.\n";
					s_p->cli_f->next = s_p->cli_in;
					s_p->cli_in->prev = s_p->cli_f;

					s_p->cli_f = s_p->cli_in;
					s_p->cli_in = nullptr;

					s_p->new_cli = true;
				}
				else if (!(s_p->cli_a)) //anchor node does not exist
				{
					//std::cout << "Server pool [" << s_p->thread_number << "]: " << "Base client added.\n";
					s_p->cli_a = s_p->cli_in;
					cli_i = s_p->cli_a;
					s_p->cli_f = cli_i;

					s_p->cli_in = nullptr;

					s_p->new_cli = true;
				}
				s_p->clients += 1;
			}

			if (!cli_i) continue;
			while (true)
			{
				int bytes_recv = recv(cli_i->socket, cli_i->buffer + cli_i->bytes_recv, (cli_i->buff_size - cli_i->bytes_recv), 0x0);
				if (bytes_recv < 0)
				{
					if (GETSOCKETERRNO() != EWOULDBLOCK && GETSOCKETERRNO() != 10035) //winsock for will block
					{
						if (GETSOCKETERRNO() == 10054) //hangup
						{
							std::cout << "Server pool [" << s_p->thread_number << "]: " << "Client hangup.\n";
							if ((cli_i == s_p->cli_a) && (!cli_i->next))
							{
								s_p->cli_a = nullptr;
								delete(cli_i);
								cli_i = s_p->cli_a;

								s_p->clients -= 1;
							}
							//anchor client with next
							else if ((cli_i == s_p->cli_a) && (cli_i->next))
							{
								s_p->cli_a = cli_i->next;
								cli_i->next->prev = nullptr;
								delete(cli_i);
								cli_i = s_p->cli_a;

								s_p->clients -= 1;
							}
							//focus client with previous
							else if ((cli_i == s_p->cli_f) && (cli_i->prev))
							{
								s_p->cli_f = cli_i->prev;
								cli_i->prev->next = nullptr;
								delete(cli_i);
								cli_i = s_p->cli_f;

								s_p->clients -= 1;
							}
							//middle client (with previous and next)
							else if ((cli_i->next) && (cli_i->prev))
							{
								cli_i->next->prev = cli_i->prev;
								cli_i->prev->next = cli_i->next;
								client* cli_j = cli_i->prev;
								delete(cli_i);
								cli_i = cli_j;

								s_p->clients -= 1;
							}
						}
						else 
						{
							std::cout << "Recv failed, critical error, cannot recover! Aborting server runtime!\n";
							std::cout << GETSOCKETERRNO();
							exit(-1);
						}
					}//wsa hangup
					if (cli_i->next) cli_i = cli_i->next;
					else if (cli_i == s_p->cli_f) cli_i = s_p->cli_a;
					break;
				}
				else if (bytes_recv == 0)
				{
					//std::cout << "Server pool [" << s_p->thread_number << "]: " << "Client disconnect.\n";
					CLOSESOCKET(cli_i->socket);
					//anchor client without next
					if ((cli_i == s_p->cli_a) && (!cli_i->next))
					{
						s_p->cli_a = nullptr;
						delete cli_i;
						cli_i = s_p->cli_a;

						s_p->clients -= 1;
					}
					//anchor client with next
					else if ((cli_i == s_p->cli_a) && (cli_i->next))
					{
						s_p->cli_a = cli_i->next;
						cli_i->next->prev = nullptr;
						delete cli_i;
						cli_i = s_p->cli_a;

						s_p->clients -= 1;
					}
					//focus client with previous
					else if ((cli_i == s_p->cli_f) && (cli_i->prev))
					{
						s_p->cli_f = cli_i->prev;
						cli_i->prev->next = nullptr;
						delete cli_i;
						cli_i = s_p->cli_f;

						s_p->clients -= 1;
					}
					//middle client (with previous and next)
					else if ((cli_i->next) && (cli_i->prev))
					{
						cli_i->next->prev = cli_i->prev;
						cli_i->prev->next = cli_i->next;
						client* cli_j = cli_i->prev;
						delete(cli_i);
						cli_i = cli_j;

						s_p->clients -= 1;
					}
					break;
				}
				else
				{
					
					cli_i->bytes_recv += bytes_recv;

					for (int i = cli_i->bytes_recv; i < cli_i->bytes_recv; i++)
					{
						std::cout << cli_i->buffer[i];
					}
					
					
					char* p = strstr(cli_i->buffer, "/clients");
					if (p)
					{
						std::cout << "\nThere are currently [" << s_p->clients << "] clients connected to thread [" << s_p->thread_number << "].\n";
						memset(cli_i->buffer, 1, cli_i->buff_size);
						cli_i->bytes_recv = 0; //breaks on second req
					}

					char* http_firstline = strstr(cli_i->buffer, "HTTP");
					char* header_end = strstr(cli_i->buffer, "\r\n\r\n");
					if (http_firstline && header_end)
					{
						std::cout << "\n\nClient made an http request!\n";

						struct router::http_request request;
						if (router::parse_http_request(cli_i->bytes_recv, cli_i->buffer, &request))
						{
							std::cout << "parse failed!";
							//send error response
						}
						else
						{
							request.cli = cli_i;
							//route the response
							router::route(request);
						}
						memset(cli_i->buffer, 1, cli_i->buff_size);
						cli_i->bytes_recv = 0; //breaks on second req
					}
					//search for end of http request

					if (cli_i->next) cli_i = cli_i->next;
					else if (cli_i == s_p->cli_f) cli_i = s_p->cli_a;
					break;
				}
			}
		}
	}
}

void simple_send_recv(client* cli_in)
{

	if (!ISVALIDSOCKET(cli_in->socket))
	{
		if (GETSOCKETERRNO() != EWOULDBLOCK && GETSOCKETERRNO() != 10035) //winsock for will block
		{
			std::cout << "Accept failed, critical error, cannot recover! Aborting server runtime!\n";
			std::cout << GETSOCKETERRNO();
			exit(-1);
		}
		std::cout << "not a valid socket";
	}

	//test
	char hi[7] = { 'h', 'e','l','l', 'o', '\n', '\0' };
	send(cli_in->socket, hi, 7, 0);
	if (GETSOCKETERRNO() != EWOULDBLOCK && GETSOCKETERRNO() != 10035) //winsock for will block
	{
		std::cout << "Send failed, critical error, cannot recover!Aborting server runtime!\n";
		std::cout << GETSOCKETERRNO();
		exit(-1);
	}

	while (true) {
		int bytes_recv = recv(cli_in->socket, cli_in->buffer, cli_in->buff_size, 0x0);
		if (bytes_recv < 0)
		{
			if (GETSOCKETERRNO() != EWOULDBLOCK && GETSOCKETERRNO() != 10035) //winsock for will block
			{
				std::cout << "Recv failed, critical error, cannot recover! Aborting server runtime!\n";
				std::cout << GETSOCKETERRNO();
				exit(-1);
			}
			//std::cout << "non blocking...";
		}
		else if (bytes_recv == 0)
		{
			std::cout << "Client Disconnect.\n";
			break;
		}
		else
		{
			for (int i = 0; i < bytes_recv; i++)
			{
				std::cout << cli_in->buffer[i];
			}
			std::cout << '\n';
		}
	}
	return;
}

void m_core(SOCKET* socket_listen, char* salt, int saltlen)
{
	sp::pool* sp_a = nullptr;
	sp::pool* sp_f = nullptr;
	sp::pool* sp_i = nullptr;

	//scan through all current pools, if all are full or cannot accept, create a new one
	//create new thread pool
	client* cli_i = new(client);

	int sp_threads = 0;
	while (true)
	{

	//client adding
		cli_i->socket = accept(*socket_listen, ((struct sockaddr*)&cli_i->address), &(cli_i->address_length));
		if (!ISVALIDSOCKET(cli_i->socket))
		{
			if (GETSOCKETERRNO() != EWOULDBLOCK && GETSOCKETERRNO() != 10035) //winsock for will block
			{
				std::cout << "Accept failed, critical error, cannot recover! Aborting server runtime!\n";
				std::cout << GETSOCKETERRNO();
				exit(-1);
			}
			std::cout << "fkdifkos";
			continue; //replace with goto replace
		}
#if defined(_WIN32)
		u_long block_off = 1;
#else
		int block_off_int = 1;
		char* block_off = (char*)&block_off_int;
#endif
		if (SIOCTL(cli_i->socket, FIONBIO, &block_off) < 0)
		{
			perror("ioctl() failed");
			CLOSESOCKET(cli_i->socket);
			std::exit(1);
		}
		//std::cout << "Client accepted.\n";

		if (!sp_a)
		{
			std::cout << "No server pools! Creating a server node...\n";
			sp_a = new (sp::pool);
			sp_threads += 1;
			sp_i = sp_a;
			sp_i->thread_number = sp_threads;
			sp_i->thread = boost::thread(sp::process, sp_i);
		}

		//look for suitable server process
		while (sp_i != sp_f)
		{
			//std::cout << "Looking for available server pool...\n";
			if (!sp_i->full) 
			{
				while (true)
				{
					if (sp_i->new_cli)//current sp_i is accepting clients, append client to list
					{
						//std::cout << "Found server pool, appending client...\n";
						sp_i->cli_in = cli_i;
						cli_i = new(client);
						sp_i->new_cli = false;

						break;
					}
				}
				break;
			}
			else if (sp_i->next)
			{
				sp_i = sp_i->next;
			}
		}

	}
}

void sha256_string(char* string, char* hashed)
{
	char hashed_buf[64];
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, string, strlen(string));
	SHA256_Final(hash, &sha256);
	int i = 0;
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(hashed + (i * 2), "%02x", hash[i]);
	}
}

void test_mongo()
{
	char salt[100];
	memset(salt, 0, sizeof(salt));
	get_salt(salt);

	std::cout << "Salt is: ";
	for (int i = 0; i < 100; i++)
	{
		if (salt[i] == 0) break;
		std::cout << salt[i];
	}
	std::cout << '\n';

	char rand_str[100] = "Password";//length is 9

	char to_be_hashed[1000] = "hi";
	char hashed[256];

	strcat(to_be_hashed, salt);

	sha256_string(to_be_hashed, hashed);

	std::cout << "Hashed: ";
	for (int i = 0; i < 64; i++)
	{
		std::cout << hashed[i];
	}
	std::cout << '\n';

	using bsoncxx::builder::stream::close_array;
	using bsoncxx::builder::stream::close_document;
	using bsoncxx::builder::stream::document;
	using bsoncxx::builder::stream::finalize;
	using bsoncxx::builder::stream::open_array;
	using bsoncxx::builder::stream::open_document;
	try {
		mongocxx::uri uri("mongodb://localhost:27017");
		mongocxx::client client(uri);
		mongocxx::database db = client["MINGO_TEST"];
		mongocxx::collection coll = db["USER_INFO"];

		/*
	auto builder = bsoncxx::builder::stream::document{};
	bsoncxx::document::value doc_value = builder
		<< "name" << "MongoDB"
		<< "type" << "database"
		<< "count" << 1
		<< "versions" << bsoncxx::builder::stream::open_array
		<< "v3.2" << "v3.0" << "v2.6"
		<< close_array
		<< "info" << bsoncxx::builder::stream::open_document
		<< "Email" << 203
		<< "Password" << 123
		<< bsoncxx::builder::stream::close_document
		<< bsoncxx::builder::stream::finalize;
		*/
		auto builder = bsoncxx::builder::stream::document{};
		bsoncxx::document::value doc_value = builder
			<< "USER" << "gxvinyu@gmail.com"
			<< bsoncxx::builder::stream::finalize;
		//moving the data_wrote to data view so we can work with it post creation
		bsoncxx::document::view data = doc_value.view();
		//insert data to collection and store the result in result 
		try
		{
			bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
				coll.insert_one(data);
		}
		catch (mongocxx::bulk_write_exception& err) {

			std::cout << "Mongodb Error:\n";
			std::cout << "code=" << err.code() << std::endl;
			std::cout << "message=" << err.what() << std::endl;

			if (err.raw_server_error()) {
				std::cout << "error reply=" << bsoncxx::to_json(*err.raw_server_error()) << std::endl;
			}

		}
	}
	catch (...) {
		std::cout << "DB Was not connected! Aborting!\n";
		exit(-1);
	}

	
}

#if defined (_WIN32)
int server::wininit()
{
#if defined(_WIN32)
	WSADATA d;
	WORD VERSION = MAKEWORD(2, 2);

	if (WSAStartup(VERSION, &d))
	{
		std::cout << "Failed to initialize winsock. Quitting...\n";
		return 1;
	}
	return 0;
#endif
}
void server::winfinish()
{
#if defined (_WIN32)
	WSACleanup();
#endif
}
#endif
int server::start(PCSTR serv)
{

	test_mongo();

	char salt[100];
	get_salt(salt);

	//initialize listening socket_________________________________________________
	int re_attempts = 0;
	bool func_failure = true;

	std::cout << "Configuring local address...\n";
	struct addrinfo hints;
	struct addrinfo* bind_address;

	memset(&hints, 0, sizeof(hints));


	while (func_failure)
	{
		bool* is_failing = &func_failure;
		if (re_attempts >= 3)
		{
			std::cout << "Failed to configure local address.";
			return -1;
		}

		memset(&hints, 0, sizeof(hints));

		hints.ai_family = AF_INET6;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		*is_failing = (getaddrinfo(0, serv, &hints, &bind_address) != 0);
		if (func_failure)
		{
			std::cout << "getaddrinfo failed with error: " << GETSOCKETERRNO() << ". ";
			bool yn_response = yn_prompt("Try again?");
			if (yn_response) { re_attempts++; }
			else if (!yn_response) { re_attempts += 3; }
		}
	}

	char addr_buff[100];
	char serv_buff[100];
	getnameinfo(bind_address->ai_addr, bind_address->ai_addrlen, addr_buff, sizeof(addr_buff), serv_buff, sizeof(serv_buff), NI_NUMERICHOST | NI_NUMERICSERV);

	std::cout << "Creating a socket on: " << addr_buff << ":" << serv_buff << "...\n";

	func_failure = true;
	SOCKET socket_listen = NULL;
	while (func_failure)
	{
		bool* is_failing = &func_failure;
		if (re_attempts >= 3)
		{
			std::cout << "Failed to configure socket.";
			std::exit(1);
		}

		SOCKET* sock_ptr = &socket_listen;
		*sock_ptr = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
		*is_failing = !ISVALIDSOCKET(socket_listen);
		if (func_failure)
		{
			std::cout << "socket() failed with error: " << GETSOCKETERRNO() << ". ";
			bool yn_response = yn_prompt("Try again?");
			if (yn_response) { re_attempts++; }
			else if (!yn_response) { re_attempts += 3; }
		}
	}

	int ipv_exclsv_off = 0;
	if (setsockopt(socket_listen, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&ipv_exclsv_off, sizeof(ipv_exclsv_off)))
	{
		std::cout << "setsocketopt() failed.\n" << errno << GETSOCKETERRNO();
		CLOSESOCKET(socket_listen);
		std::exit(1);
	}

	std::cout << "Binding socket...\n";

	func_failure = true;
	while (func_failure)
	{
		bool* is_failing = &func_failure;

		if (re_attempts >= 3)
		{
			std::cout << "Failed to configure socket.";
			freeaddrinfo(bind_address);
			std::exit(1);
		}
		*is_failing = bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen);
		if (func_failure)
		{
			std::cout << "bind() failed with error: " << GETSOCKETERRNO() << ". ";
			bool yn_response = yn_prompt("Try again?");
			if (yn_response) { re_attempts++; }
			else if (!yn_response) { re_attempts += 3; }
		}
	}
	freeaddrinfo(bind_address);

	std::cout << "Listening...\n";
	func_failure = true;
	while (func_failure)
	{
		bool* is_failing = &func_failure;

		if (re_attempts >= 3)
		{
			std::cout << "Failed to configure socket.";
			freeaddrinfo(bind_address);
			std::exit(1);
		}
		*is_failing = (listen(socket_listen, BACKLOG) < 0); //err handling for all (bad idea) but i odnt have time bruh
		if (func_failure)
		{
			std::cout << "listen() failed with error: " << GETSOCKETERRNO() << ". ";
			bool yn_response = yn_prompt("Try again?");
			if (yn_response) { re_attempts++; }
			else if (!yn_response) { re_attempts += 3; }
		}
	}
	//allow socket descriptor to be reusable
	int sock_desc_reuse_on = 1;
	if (setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, (char*)&sock_desc_reuse_on, sizeof(sock_desc_reuse_on)) < 0)
	{
		std::cout << "setsocketopt() failed.\n";
		CLOSESOCKET(socket_listen);
		std::exit(1);
	}
	m_core(&socket_listen, salt, sizeof(salt));
}