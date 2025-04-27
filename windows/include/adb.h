#pragma once

#include <windows.h>
#include <string>

#include "logger.h"

namespace adb
{
	/*
	* Get the local directory of the program
	*/
	std::string dir()
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);

		std::string path(buffer);
		return path.substr(0, path.find_last_of("\\/"));
	}

	/*
	* Reverse the given tcp port.
	* adb reverse tcp:<port> tcp:<port>
	*/
	bool start(int port)
	{
		std::string path = dir();
		std::string command = path + "\\adb\\adb.exe " + "reverse tcp:" + std::to_string(port) + " tcp:" + std::to_string(port);

		if (system(command.c_str()) == 0)
		{
			logger << "[ADB:" << port << "] Started " << std::endl;
			return true;
		}
		else
		{
			logger << "[ADB:" << port << "] Failed to start " << std::endl;
			return false;
		}
	}

	/*
	* Removes the reversed given tcp port.
	* adb reverse --remove tcp:<port>
	*/
	bool kill(int port)
	{
		std::string path = dir();
		std::string command = path + "\\adb\\adb.exe " + "reverse --remove tcp:" + std::to_string(port);

		if (system(command.c_str()) == 0)
		{
			logger << "[ADB:" << port << "] Stopped " << std::endl;
			return true;
		}
		else
		{
			logger << "[ADB:" << port << "] Failed to stop " << std::endl;
			return false;
		}
	}
}