#include "LLog.h"
#include "LTime.h"

#define LLOG_SIZE		2048

LLog::LLog() :	LRunnable(),
				LSingleton<LLog>(),
				m_level(LLOG_LEVEL_DEBUG)
{

}

LLog::~LLog()
{

}

bool LLog::Init()
{
	return true;
}

bool LLog::Final()
{
	Clear();
	return true;
}

void LLog::Clear()
{

}

void LLog::Run()
{
	while(!m_stop)
	{
		LLogData* pLog = (LLogData*)Pop();

		if(pLog == NULL)
		{
			Sleep(1);
			continue;
		}

		_outToConsole(pLog);

		std::string strFormat = _formatLog(pLog);
		WriteFile(strFormat.c_str());
		
		delete pLog;
	}
	m_file.close();
}

void LLog::SetFileName(const char* fileName)
{
	m_fileName = fileName;
	m_fileName += ".log";

	Lstring	filename = LTimeTool::GetLocalDateInString(m_curTime);
	filename += "_";
	filename += m_fileName;
	m_file.open(filename.c_str(), std::ios::app | std::ios::out);

	Start();
}

void LLog::SetLogLevel(Lint logLevel)
{
	m_level = logLevel;
}

void LLog::Log(LLOG_LEVEL level, const char* pchFuncName, int iLineNumber, const char* pchLog, ...)
{
	if(level > m_level || pchFuncName == NULL || pchLog == NULL)
	{
		return;
	}

	LLogData* pLogData = new LLogData;
	pLogData->m_logLevel		= level;
	pLogData->m_strFuncName = pchFuncName;
	pLogData->m_strTime		= LTimeTool::GetLocalTimeInString();
	pLogData->m_iLineNumber	= iLineNumber;

	try
	{
		char chLog[LLOG_SIZE + 1] = {0};

		va_list ap;
		va_start(ap, pchLog);
		if(vsnprintf(chLog, LLOG_SIZE - 1, pchLog, ap) > 0)
		{
			pLogData->m_strLog = chLog;
		}
		va_end(ap);
	}
	catch(...)
	{
		pLogData->m_strLog = "Exception on log";
	}

	Push(pLogData);
}

// void LLog::WriteFile(char const* content)
// {
// 	Lstring	file = LTimeTool::GetLocalDateInString();
// 	file += "_";
// 	file += m_fileName;
// 	std::ofstream ofs(file.c_str(), std::ios::app | std::ios::out);
// 	if(ofs)
// 	{
// 		ofs << content << std::endl;
// 		ofs.close();
// 	}
// }

void LLog::WriteFile(char const* content)
{
	LTime now;
	if(now.GetDate() == m_curTime.GetDate())
	{
		m_file << content << std::endl;
	}
	else
	{
		m_file.close();

		m_curTime = now;
		Lstring	filename = LTimeTool::GetLocalDateInString(m_curTime);
		filename += "_";
		filename += m_fileName;
		m_file.open(filename.c_str(), std::ios::app | std::ios::out);
		m_file << content << std::endl;
	}
}

std::string LLog::_formatLog(LLogData* pLogData)
{
	if(pLogData == NULL)
	{
		return "";
	}

	std::string strLevel;
	switch(pLogData->m_logLevel)
	{
	case LLOG_LEVEL_NULL:
		strLevel = "Null ";
		break;
	case LLOG_LEVEL_INFO:
		strLevel = "Info ";
		break;
	case LLOG_LEVEL_ERROR:
		strLevel = "Error";
		break;
	case LLOG_LEVEL_WARN:
		strLevel = "Warn ";
		break;
	case LLOG_LEVEL_DEBUG:
		strLevel = "Debug";
		break;
	default:
		strLevel = "Unknown";
		break;
	}

	char chNumber[20] = {0};
	sprintf(chNumber, "%d", pLogData->m_iLineNumber);

	std::string strFormat;

	strFormat += pLogData->m_strTime;
	strFormat += "[";
	strFormat += strLevel;
	strFormat += "][";
	strFormat += pLogData->m_strFuncName;
	strFormat += ":";
	strFormat += chNumber;
	strFormat += "] ";
	strFormat += pLogData->m_strLog;

	return strFormat;
}

void LLog::_outToConsole(LLogData* pLogData)
{
	if(pLogData == NULL)
	{
		return;
	}

	std::string strOut;
	strOut = pLogData->m_strTime + " " + pLogData->m_strLog;
	std::cout << strOut.c_str() << std::endl;

#if ((defined(WIN32) || defined(WIN64)) && defined(_DEBUG))
	OutputDebugStringA(strOut.c_str());
	OutputDebugStringA("\n");
#endif
}