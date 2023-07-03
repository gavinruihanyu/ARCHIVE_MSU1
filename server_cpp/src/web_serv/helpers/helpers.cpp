#include "helpers.hpp"

std::string get_input() {
	std::string input;
	std::getline(std::cin, input);
	return input;
}

bool yn_prompt(const char* prompt)
{
	/*will add (y/n): at the end, so no need to call function with it also adds newline*/
	std::cout << prompt << " (y/n): ";
	std::string str;
	std::regex regexp("[^yYnN]");
	std::regex yes_resp("[yY]");
	std::regex no_resp("[nN]");

	bool invalid_response = true;
	while (invalid_response)
	{
		std::getline(std::cin, str);
		if (std::regex_search(str, regexp, std::regex_constants::match_any) || size(str) > 1)
		{
			std::cout << "Please enter in a valid response (y/n):";
		}
		else { invalid_response = false; }
	}
	if (std::regex_search(str, yes_resp, std::regex_constants::match_any)) { return true; }
	else if (std::regex_search(str, no_resp, std::regex_constants::match_any)) { return false; }
}

int get_salt(char* salt)//gets the salt file
{
	std::string salt_str;
	std::string line;
	//std::cout << "Please specify salt.txt path\n> ";
	std::ifstream salt_file("salt.txt");//get_input());
	if (salt_file.is_open())
	{
		while (std::getline(salt_file, line))
		{
			salt_str += line;
		}
		salt_file.close();
		int length = salt_str.length();
		strcpy(salt, salt_str.c_str());
		salt[length] = 0;
		return length;
	}

	else {
		std::cout << "Unable to open salt file. ";
		bool yn_response = yn_prompt("Start server without salt? (Should only be done for testing only)");
		if (!yn_response) {
			std::cout << "Server Aborted.\n";
			exit(-1);
		}
	}

}

int load_credentials(const char* file_path, char* salt)
{
	std::string salt_str;
	std::string line;
	//std::cout << "Please specify salt.txt path\n> ";
	std::ifstream salt_file(file_path);//get_input());
	if (salt_file.is_open())
	{
		while (std::getline(salt_file, line))
		{
			salt_str += line;
		}
		salt_file.close();
		int length = salt_str.length();
		strcpy(salt, salt_str.c_str());
		salt[length] = 0;
		return length;
	}

	else std::cout << "Unable to open salt file";
	exit(-1);
}