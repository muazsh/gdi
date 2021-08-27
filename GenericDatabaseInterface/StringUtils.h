#pragma once
#include <algorithm>
#include <string>

namespace gdi 
{
	void ToLower(std::string& str)
	{
		std::transform(str.begin(), str.end(),
			str.begin(), [](unsigned char c) { return std::tolower(c); });
	}

	bool IsEmptyOrWhitespaces(const std::string& str)
	{
		return std::find_if_not(str.begin(), str.end(), std::isspace) == str.end();
	}
}
