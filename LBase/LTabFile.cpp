
#include "LTabFile.h"
#include <string.h>
#include "LTool.h"
LTabFile::LTabFile():m_index(0)
{
}
LTabFile::~LTabFile()
{
}

bool LTabFile::Load(const char* FileName)
{
	if ( FileName == NULL )
	{
		return false;
	}
	clear();
	char line_content[MAX_TXT_FILE];
	std::vector<Lstring> des;
	std::ifstream ifs(FileName);
	int i = 0;
	while(ifs)
	{
		des.clear();
		ifs.getline(line_content,MAX_TXT_FILE);
		std::string s = string_To_UTF8(line_content);
		L_ParseString(s, des, "\t");
		if (des.size() > 0)
		{
			fileItems.push_back(des) ;
		}
	}
	if(ifs)
		ifs.close();
	return true;
}

void LTabFile::clear()
{
	m_index = 1;		//去掉最上面一行注释
	fileItems.clear();
}

std::vector< std::string >& LTabFile::getTapFileLine()
{
	if (m_index >= fileItems.size())
	{
		static std::vector< std::string > nullItem;
		return nullItem;
	}
	return fileItems[m_index ++];
}
