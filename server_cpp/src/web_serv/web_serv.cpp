#include "external_headers/mongo_include.h" //mongo must be referenced before winsock
#include "external_headers/networking_include.h"
#include "external_headers/boost_include.h"
#include "external_headers/openssl_include.h"

//#include <server.hpp>
#include <server.hpp>

mongocxx::instance instance{}; // This should be done only once.

int main()
{
	//remove pre preocessor block before execution to see if it makes a difference
#if defined (_WIN32)
	server::wininit();
#endif
//main process
	server::start("3000");
	std::cout << "Finished.\n";
	std::system("Pause");
#if defined (_WIN32)
	server::winfinish();
#endif
	return 0;
}