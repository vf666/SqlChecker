#include "./SqlConnector.h"
#include "./IniRW.h"
#include <WinSock2.h>
#include <stdio.h>
#include <mysql.h>
#include <string.h>

class MySqlConnector : public ISqlConnector
{
public:
	MySqlConnector()
	{
		m_sqlHandle = mysql_init(NULL);
	}
	~MySqlConnector()
	{
		if (m_sqlHandle)
		{
			mysql_close(m_sqlHandle);
		}
	}
	virtual bool connect(
		const char* host, int port, const char* db, const char* user, const char* pw)
	{
		if (!m_sqlHandle)
		{
			return false;
		}
		m_sqlHandle = mysql_real_connect(m_sqlHandle, host, user, pw, db, port, NULL, 0);
		if (m_sqlHandle == NULL)
		{
			printf("Failed to connect to database: Error: %s\n", mysql_error(m_sqlHandle));
			return false;
		} 
		mysql_set_character_set(m_sqlHandle, "utf8");
		return true;
	}
	virtual bool startTrans()
	{
		if (!m_sqlHandle)
		{
			return false;
		}
		int ret = mysql_query(m_sqlHandle, "SET AUTOCOMMIT=0");
		if (ret != 0) {
			printf("mysql_OperationTran query set err: %s\n", mysql_error(m_sqlHandle));
			return false;
		}
		ret = mysql_query(m_sqlHandle, "start transaction");
		if (ret != 0) {
			printf("mysql_OperationTran query start err: %s\n", mysql_error(m_sqlHandle));
			return false;
		}
		
		return true;
	}
	virtual bool query(const char* sql, std::string& err)
	{
		if (!m_sqlHandle)
		{
			err = "invalid sql handle!";
			return false;
		}
		int ret = mysql_query(m_sqlHandle, sql);
		if (ret != 0) {
			err = mysql_error(m_sqlHandle);
			printf("query err: %s\n", err.c_str());
			return false;
		}
		return true;
	}
	virtual bool rollbackTrans()
	{
		if (!m_sqlHandle)
		{
			return false;
		}
		int ret = mysql_query(m_sqlHandle, "ROLLBACK");
		if (ret != 0) {
			printf("rollback err: %s\n", mysql_error(m_sqlHandle));
			return false;
		}
		return true;
	}
	virtual bool commitTrans()
	{
		if (!m_sqlHandle)
		{
			return false;
		}
		int ret = mysql_query(m_sqlHandle, "COMMIT");
		if (ret != 0) {
			printf("commit err: %s\n", mysql_error(m_sqlHandle));
			return false;
		}
		return true;
	}
private:
	MYSQL* m_sqlHandle;
};

SqlConnectorFactory::SqlConnectorFactory(SqlType /*type*/)
{

}

SqlConnectorFactory::~SqlConnectorFactory()
{

}

ISqlConnector* SqlConnectorFactory::create()
{
	MySqlConnector* connector = new MySqlConnector;
	IniRW irw("./db_info.ini");
	std::string host = irw.read("DB_INFO", "HOST", "");
	int port = irw.read("DB_INFO", "PORT", 3306);
	std::string db = irw.read("DB_INFO", "DB", "sql_checker");
	std::string user = irw.read("DB_INFO", "USER", "root");
	std::string pw = irw.read("DB_INFO", "PW", "123");
	if (!connector->connect(host.c_str(), port, db.c_str(), user.c_str(), pw.c_str()))
	{
		delete connector;
		return NULL;
	}
	return connector;
}

bool SqlConnectorFactory::release(ISqlConnector* connector)
{
	MySqlConnector* impl = dynamic_cast<MySqlConnector*>(connector);
	if (impl)
	{
		delete impl;
		return true;
	}
	else 
	{
		return false;
	}
}