#include <iostream>
#include <string>
#include <fstream>
#include <regex>

std::string get_input();
bool yn_prompt(const char* prompt);

int get_salt(char* salt);

struct credentials
{
	char salt[512];
	char mongo_uri[512];
};

int load_credentials(const char* file_path, char* salt);

