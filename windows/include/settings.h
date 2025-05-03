#pragma once

#include <fstream>
#include <map>
#include <string>

class Settings
{
public:
	static void load()
	{
		std::ifstream f(dir() + "\\settings.cfg");
		
		std::string line;
		while (std::getline(f, line))
		{
			size_t pos = line.find("=");
			std::string name = line.substr(0, pos);
			int value = std::atoi(line.substr(pos + 1).c_str());
			
			settings[name] = value;
		}

		f.close();
	}

	static void save()
	{
		std::ofstream f(dir() + "\\settings.cfg");

		for (auto& entry : settings)
		{
			f << entry.first << "=" << entry.second << std::endl;
		}

		f.close();
	}

	static int get(std::string name)
	{
		auto res = settings.find(name);
		if (res != settings.end())
		{
			return res->second;
		}
		else
		{
			return -1;
		}
	}

	static const std::map<std::string, int>& get()
	{
		return settings;
	}

	static void set(std::string name, int value)
	{
		settings[name] = value;
	}

private:
	inline static std::map<std::string, int> settings;

	static std::string dir()
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);

		std::string path(buffer);
		return path.substr(0, path.find_last_of("\\/"));
	}
};
