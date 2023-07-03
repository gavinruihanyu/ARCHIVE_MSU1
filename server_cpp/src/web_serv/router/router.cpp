#include <router.hpp>
namespace router
{
	int parse_http_request(int bytes_recv, char* request_string, struct http_request* request) //execution of function on Gavins desktop takes approx 41 microseconds per small request (single line headers, bodies etc)
	{
		//can 
		// Initialize request structure
		memset(request, 0, sizeof(struct http_request));

		// Parse the request line
		int ret = sscanf(request_string, "%7s %511s %15s", request->method, request->uri, request->version); //triple string
		if (ret != 3) {
			// Invalid request line
			std::cout << "Invalid request line\n";
			return -1;
		}

		//print out the method, uri and version
		std::cout << "Method: ";
		for (int i = 0; i < sizeof(request->method); i++)
		{
			std::cout << request->method[i];
		}

		std::cout << "\nUri: ";
		for (int i = 0; i < sizeof(request->uri); i++)
		{
			std::cout << request->uri[i];
		}

		std::cout << "\nVersion: ";
		for (int i = 0; i < sizeof(request->version); i++)
		{
			std::cout << request->version[i];
		}
		std::cout << '\n';

		// Find out which http version we are dealing with. https://developer.mozilla.org/en-US/docs/Glossary/QUIC
		if (!strcmp("HTTP/1.1", request->version)) std::cout << "HTTP version 1.1\n";//expected chunked data this is what our server was writte for anyways
		else if (!strcmp("HTTP/2.0", request->version)) std::cout << "HTTP version 2.0\n";//not esxpecting chunked data learn more: https://en.wikipedia.org/wiki/HTTP/2
		else if (!strcmp("HTTP/3.0", request->version)) std::cout << "HTTP version 3.0\n";//not know what to do because im going to commit a crime against 

		// Find the start of the headers
		char* headers_start = strstr(request_string, "\r\n");
		if (headers_start == NULL) {
			// No headers found
			std::cout << "No headers found\n";
			return -1;
		};
		// Find the end of the headers
		char* headers_end = strstr(headers_start + 2, "\r\n\r\n");
		if (headers_end == NULL) {
			// No end of headers found.
			std::cout << "No end of headers found\n";
			return -1; //invalid data, respond accordingly
		}
		// Set the headers and headers length
		request->headers = headers_start + 2;
		request->headers_length = headers_end - headers_start - 2;

		// Set the body and body length
		request->body = headers_end + 4;
		request->body_length = bytes_recv - (request->body - request_string);
		return 0;
	}

	int route(http_request req)
	{
		if (!strcmp(req.uri, "/")) std::cout << "Client made request for \"/\"";
		else if (!strcmp(req.uri, "/auth/login"))
		{
			std::cout << "Client made request for \"/login\"\n";

			struct sockaddr_storage address;
			memset(req.cli->address_buffer, 0x0, req.cli->address_buffer_len);
			getnameinfo((struct sockaddr*)&(req.cli->address), req.cli->address_length, req.cli->address_buffer, req.cli->address_buffer_len, 0, 0, NI_NUMERICHOST | NI_NUMERICSERV);
			std::cout << "Client IP: ";
			for (unsigned int i = 0; i < req.cli->address_buffer_len; i++)
			{
				std::cout << req.cli->address_buffer[i];
			}
			std::cout << '\n';

			std::cout << "Request Body: ";
			for (int i = 0; i < req.body_length; i++)
			{
				std::cout << req.body[i];
			}

			//sanitize the input:
			char* email_start = strstr(req.body, "{\"email\":\"") + 10;
			char* email_end = strstr(req.body, ",\"password\":") -1;
			char* email = new char[email_end - email_start];
			
			strncpy(email, email_start, email_end - email_start);

			std::cout << '\n';
			for (int i = 0; i < (email_end - email_start); i++)
			{
				std::cout << email[i];
			}
			std::cout << '\n';

			char* password_start = strstr(req.body, ",\"password\":\"") + 13;
			char* password_end = &(req.body[req.body_length - 2]);
			char* password = new char[password_end-password_start];

			strncpy(password, password_start, password_end - password_start);

			for (int i = 0; i < (password_end - password_start); i++)
			{
				std::cout << password[i];
			}


			//mongo can handle multiple connections therefore you can instantiate more than one connection to mongodb
			using bsoncxx::builder::stream::close_array;
			using bsoncxx::builder::stream::close_document;
			using bsoncxx::builder::stream::document;
			using bsoncxx::builder::stream::finalize;
			using bsoncxx::builder::stream::open_array;
			using bsoncxx::builder::stream::open_document;

			mongocxx::uri uri("mongodb+srv://mtest202269420:DsmMh3f17t6E9pTv@cluster0.mtpuj95.mongodb.net/?retryWrites=true&w=majority");
			mongocxx::client client(uri);
			if (!client) std::cout << "DB Was not connected! Aborting!\n";

			mongocxx::database db = client["MINGO_TEST"];
			mongocxx::collection coll = db["USER_INFO"];

			bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
				coll.find_one(document{} << "USER" << "gxvinyu@gmail.com" << finalize);
			if (maybe_result) {
				std::cout << bsoncxx::to_json(*maybe_result) << "\n";
			}
			else
			{
				std::cout << "No email found... lets try regestering lol\n";
			}
		}
		else if (!strcmp(req.uri, "/auth/register")) std::cout << "Client made request for \"/register\"\n";
		return 0;
	}
}