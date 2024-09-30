#pragma once

#include <string>

class Asset {
public:
	Asset() {};

	static const char* get_real_path(const char* vpath);
	static const char* get_real_path(std::string vpath);

};