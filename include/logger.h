#pragma once

#include <ostream>
#include <iostream>
#include <fstream>

/*
* Provides custom logging service.
* https://stackoverflow.com/a/17597747
*/
class Logger
{
public:
	Logger(std::ostream& outstream = std::cout): outstream(outstream)
	{
	}

	// Overload for using std::endl
	// https://stackoverflow.com/a/10059337
	Logger& operator<<(std::ostream& (*f)(std::ostream&))
	{
		outstream << std::endl;
		return *this;
	}

	template<typename T>
	Logger& operator<<(const T& data)
	{
		outstream << data;
		return *this;
	}

private:
	std::ostream& outstream;
};

extern Logger logger;