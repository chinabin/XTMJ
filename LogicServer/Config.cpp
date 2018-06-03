#include "Config.h"
#include "LLog.h"

bool Config::Init()
{
	m_ini.LoadFile("logic_config.ini");
	m_addRobot = m_ini.GetInt("AddRobot", 0);
	m_DebugMod = m_ini.GetInt("DebugModel", 0);
	m_ServerID = m_ini.GetInt("ServerID", -1);
	return true;
}

bool Config::Final()
{
	return true;
}

Lint Config::GetLogLevel()
{
	//默认debug log
	return m_ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
}

Lstring Config::GetLogicManagerIp()
{
	return m_ini.GetString("LogicManagerIp", "127.0.0.1");
}

Lshort Config::GetLogicManagerPort()
{
	return m_ini.GetShort("LogicManagerPort", 9001);
}

Lstring Config::GetCenterKey()
{
	return m_ini.GetString("CenterKey", "gj2ag3j54acbfbaa1a2f7a70d766c1d");
}

Lstring Config::GetDBIp()
{
	return m_ini.GetString("DBIp", "127.0.0.1");
}

Lshort Config::GetDBPort()
{
	return m_ini.GetShort("DBPort", 7001);
}

Lstring Config::GetDBKey()
{
	return m_ini.GetString("DBKey", "gj2ag3j54acbfbaa1a2f7a70d766c1d");
}

Lstring Config::GetInsideIp()
{
	return m_ini.GetString("InsideIp", "127.0.0.1");
}

Lshort Config::GetInsidePort()
{
	return m_ini.GetShort("InsidePort", 6001);
}

bool Config::GetIfAddRobot()
{
	return (m_addRobot != 0);
}

bool Config::GetDebugModel()
{
	return (m_DebugMod != 0);
}

Lint Config::GetServerID()
{
	return m_ServerID;
}

Lstring Config::GetServerName()
{
	return m_ini.GetString("ServerName", "Unknow Server");
}

void Config::Reload()
{
	LIniConfig	temp;
	temp.LoadFile("logic_config.ini");

	Lint oldDebugMod = m_DebugMod;
	m_DebugMod = temp.GetInt("DebugModel", 0);

	Lint logLevel = temp.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
	LLog::Instance().SetLogLevel(logLevel);

	LLOG_DEBUG("Config::Reload debugMod:%d->%d LogLevel:%d", oldDebugMod, m_DebugMod, logLevel);
}
