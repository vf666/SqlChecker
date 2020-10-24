#ifndef FILERW_H
#define FILERW_H

#include <string>
#include <vector>

class FileRW
{
public:
	struct Writer 
	{
		virtual long append(const std::string& row) = 0;
	};
	FileRW(const std::string& fileName);
	virtual ~FileRW();

	bool rebuild(); 

	long read(std::vector<std::string>& rows) const;
	Writer* createWriter() const;
	void releaseWriter(Writer* wr) const;

private:
	const std::string	m_fileName;

};

#endif