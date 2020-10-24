#include "./IniRW.h"
#include <Windows.h>
#define INIVAL_LEN		(256)

IniRW::IniRW(std::string fileName) : m_fileName(fileName)
{

}

IniRW::~IniRW()
{

}

std::string IniRW::read(std::string appName, std::string keyName, std::string defaultValue)
{
	char valBuff[INIVAL_LEN + 1];
	memset(valBuff, 0, sizeof(char)*(INIVAL_LEN + 1));
	DWORD len = GetPrivateProfileString(
		appName.c_str(),
		keyName.c_str(),
		defaultValue.c_str(),
		valBuff, INIVAL_LEN,
		m_fileName.c_str());
	if (len == 0)
	{
		return "";
	}
	return valBuff;
}

int IniRW::read(std::string appName, std::string keyName, int defaultValue)
{
	return GetPrivateProfileInt(
		appName.c_str(), keyName.c_str(), 
		defaultValue, m_fileName.c_str());
}