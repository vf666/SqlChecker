#include <fstream>
#include <io.h>
#include "./FileRW.h"

class WriterImpl : public FileRW::Writer
{
public:
	WriterImpl(const char* fileName)
		: m_out(fileName, std::ios::binary | std::ios::app){ }
	~WriterImpl()
	{
		if (!m_out)
		{
			m_out.close();
		}
	}
	virtual long append(const std::string& row)
	{
		if (!m_out)
		{
			return -1;
		}
		m_out.write(row.c_str(), row.size());
		m_out.write("\n", strlen("\n"));
		return row.size() + strlen("\n");
	}
private:
	std::ofstream m_out;
};

FileRW::FileRW(const std::string& fileName) : m_fileName(fileName)
{

}

FileRW::~FileRW()
{

}

bool FileRW::rebuild()
{
	std::ofstream out(
		m_fileName.c_str(), std::ios::trunc);
	if (!out)
	{
		return false;
	}
	else
	{
		out.close();
		return true;
	}
}

long FileRW::read(std::vector<std::string>& rows) const
{
	std::ifstream in(
		m_fileName.c_str(), std::ios::binary | std::ios::in);
	if (!in)
	{
		return -1;
	}
	in.seekg(0, std::ios::beg);
	std::string line; 
	while (std::getline(in, line))
	{
		rows.push_back(line);
	}
	return rows.size();
}

FileRW::Writer* FileRW::createWriter() const
{
	return new WriterImpl(m_fileName.c_str());
}

void FileRW::releaseWriter(Writer* wr) const
{
	WriterImpl* impl = dynamic_cast<WriterImpl*>(wr);
	if (impl)
	{
		delete impl;
	}
}
