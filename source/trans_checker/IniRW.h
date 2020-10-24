#ifndef INIRW_H
#define INIRW_H

#include <string>

class IniRW
{
public:
	IniRW(std::string fileName);
	virtual ~IniRW();

	std::string read(std::string appName, std::string keyName, std::string defaultValue);
	int read(std::string appName, std::string keyName, int defaultValue);
	
private:
	const std::string m_fileName;
};

#endif