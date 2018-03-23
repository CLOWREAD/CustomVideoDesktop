#include "ReadConfig.h"

#include <fstream>

void ReadConfig::Read()
{
	std::ifstream ifs;
	ifs.open("config.d");
	char str[256];
	
	while (!ifs.eof())
	{
		ifs.getline(str, 256);
		if (strcmp(str, "VIDEO_FILE_PATH") == 0)
		{
			ifs.getline(str, 256);
			m_VideoFilePath = new char[strlen(str) + 1];
			strcpy_s(m_VideoFilePath,256, str);
		}


	}
	




}

ReadConfig::ReadConfig()
{
}


ReadConfig::~ReadConfig()
{
}
