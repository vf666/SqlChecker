#include "./FileRW.h"
#include "./IniRW.h"
#include "./SqlConnector.h"
#include <thread>

#define ERR_FILE	("./err.info")

class SqlTransChecker
{
public:
	SqlTransChecker(
		const std::string& fileOne,
		const std::string& fileTwo)
	{
		FileRW rwOne(fileOne);
		rwOne.read(m_oneSqls);

		FileRW rwTwo(fileTwo);
		rwTwo.read(m_twoSqls);

		FileRW errRW(ERR_FILE);
		errRW.rebuild();
	}
	~SqlTransChecker()
	{

	}

	void execute() 
	{
		std::vector<SqlTransChecker::SqlRow> composes;
		composes.resize(m_oneSqls.size() + m_twoSqls.size());
		check(0, 0, composes);
	}
private:
	struct SqlRow
	{
		int			transNum;
		std::string sql;
		std::string err;
	};
	void check(int oneBegin, int twoBegin, std::vector<SqlTransChecker::SqlRow>& composes)
	{
		if (oneBegin >= m_oneSqls.size() && twoBegin >= m_twoSqls.size()) 
		{
			runTest(composes);
			return;
		}
		if (oneBegin < m_oneSqls.size()) 
		{
			SqlRow sr;
			sr.sql = m_oneSqls[oneBegin];
			sr.transNum = (oneBegin + 1 == m_oneSqls.size() ? -1 : 1);
			composes[oneBegin + twoBegin] = sr;
			check(oneBegin + 1, twoBegin, composes);
		}
		if (twoBegin < m_twoSqls.size())
		{
			SqlRow sr;
			sr.sql = m_twoSqls[twoBegin];
			sr.transNum = (twoBegin + 1 == m_twoSqls.size() ? -2 : 2);
			composes[oneBegin + twoBegin] = sr;
			check(oneBegin, twoBegin + 1, composes);
		}
	}

	void runTest(std::vector<SqlTransChecker::SqlRow>& composes) 
	{
		if (composes.size() <= 0)
		{
			saveErrorComposes(composes, "composes is empty");
			return;
		}
		printf("++++++ run test begin:\n");
		std::vector<ISqlConnector*> connectors;
		connectors.resize(2, NULL);
		SqlConnectorFactory cf(SqlConnectorFactory::MySqlType);
		for (int i = 0; i < connectors.size(); i++)
		{
			connectors[i] = cf.create();
			if (!connectors[i]) 
			{
				char err[501] = { 0 };
				sprintf_s(err, 500, "open db fault at trans num = %d!", i);
				saveErrorComposes(composes, err);
				return release(cf, connectors, i);
			}
			if (!connectors[i]->startTrans()) 
			{
				char err[501] = { 0 };
				sprintf_s(err, 500, "start trans fault at trans num = %d!", i);
				saveErrorComposes(composes, err);
				return release(cf, connectors, i);
			}
		}
		for (int i = 0; i < composes.size(); i++)
		{
			
			int transNum = abs(composes[i].transNum) - 1;
			printf("###### begin run trans num = %d\n sql : %s\n", transNum + 1, composes[i].sql.c_str());
			//如果卡住，说明产生了死锁！！！
			if (!connectors[transNum]->query(composes[i].sql.c_str(), composes[i].err))
			{
				saveErrorComposes(composes, "");
				return release(cf, connectors, connectors.size());
			}
			//按理最后应该提交事务。但是，多个组合情况运行时，
			//前一个组合序列成功提交后，后续组合序列可能与之前的组合序列提交的结果有冲突而无法运行；
			//所以不能真正提交，否则每次运行完一种可能组合序列，就需要清理数据进行状态还原
			/*
			if (composes[i].transNum < 0 && !connectors[i]->commitTrans())
			{
				char err[501] = { 0 };
				sprintf_s(err, 500, "commit trans fault at trans num = %d!", transNum);
				saveErrorComposes(composes, err);
				return release(cf, connectors, connectors.size());
			}
			*/
		}
		release(cf, connectors, connectors.size());
	}
	void saveErrorComposes(
		std::vector<SqlTransChecker::SqlRow>& composes, std::string globalErr)
	{
		FileRW errRW(ERR_FILE);
		FileRW::Writer* wr = errRW.createWriter();

		std::string begin = "++++++ Error Composes Begin ++++++";
		wr->append(begin);

		for (int i = 0; i < composes.size(); i++)
		{
			char transInfo[501] = { 0 };
			sprintf_s(transInfo, 500, "###### trans num = %d", composes[i].transNum);
			std::string transMsg = transInfo;
			wr->append(transMsg);

			std::string sqlMsg = "sql : ";
			sqlMsg += composes[i].sql;
			wr->append(transMsg);

			if (composes[i].err.size() <= 0)
			{
				continue;
			}
			std::string errMsg = "err : ";
			errMsg += composes[i].err;
			wr->append(errMsg);
		}

		if (globalErr.size() > 0)
		{
			std::string headMsg = "@@@@@@ have global error!";
			wr->append(headMsg);

			std::string errMsg = "err : ";
			errMsg += globalErr;
			wr->append(errMsg);
		}

		std::string end = "------ Error Composes End ------\n";
		wr->append(end);

		errRW.releaseWriter(wr);
	}
	void release(SqlConnectorFactory& cf, std::vector<ISqlConnector*> connectors, int rollbackUp)
	{
		for (int i = 0; i < connectors.size(); i++)
		{
			if (connectors[i])
			{
				if (i < rollbackUp)
				{
					connectors[i]->rollbackTrans();
				}
				cf.release(connectors[i]);
			}
		}
		printf("++++++ run test over!\n\n");
	}
private:
	std::vector<std::string> m_oneSqls;
	std::vector<std::string> m_twoSqls;
};

int main(int argc, char* argv[])
{
	std::string fileOne = (argc >= 1 ? argv[1] : "");
	std::string fileTwo = (argc >= 2 ? argv[2] : "");
	SqlTransChecker stc(fileOne, fileTwo);

	stc.execute();

	std::this_thread::sleep_for(std::chrono::seconds(2));
	return 0;
}