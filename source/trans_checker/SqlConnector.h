#ifndef ISQLCONNECTOR_H
#define ISQLCONNECTOR_H

#include <string>

struct ISqlConnector
{
	virtual bool connect(
		const char* host, int port, const char* db,
		const char* user, const char* pw) = 0;
	virtual bool startTrans() = 0; 
	virtual bool query(const char* sql, std::string& err) = 0;
	virtual bool rollbackTrans() = 0;
	virtual bool commitTrans() = 0;
};

class SqlConnectorFactory
{
public:
	enum SqlType
	{
		MySqlType,
	};
	SqlConnectorFactory(SqlType type);
	~SqlConnectorFactory();
	ISqlConnector* create();
	bool release(ISqlConnector* connector);
};

#endif