#pragma once

#include "SFML/Window.hpp"
#include <string>

class FileDialogs
{
public:
	static std::string OpenFile(sf::WindowHandle handle, const char* filter);
	static std::string SaveFile(sf::WindowHandle handle, const char* filter);
};