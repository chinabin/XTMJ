#include "Config.h"
#include "LLog.h"

bool Config::Init()
{
	m_ini.LoadFile("logic_manager_config.ini");
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

Lstring Config::GetCenterIp()
{
	return m_ini.GetString("CenterIp", "127.0.0.1");
}

Lshort Config::GetCenterPort()
{
	return m_ini.GetShort("CenterPort", 5001);
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

Lint Config::GetTime()
{
	return m_ini.GetInt("StartTime", 0);
}

Lint Config::GetRemoteLogStart()
{
	return m_ini.GetInt("RemoteLogStart", 0);
}

Lstring Config::GetRemoteLogUrl()
{
	return m_ini.GetString("RemoteLogUrl", "");
}

Lint Config::GetMaxCachedLogSize()
{
	// default value: 20 * 1024
	return m_ini.GetInt("MaxCachedLogSize", 20480);
}

Lshort Config::GetMaxCachedLogNum()
{
	// default value: 100
	return m_ini.GetShort("MaxCachedLogNum", 100);
}

Lstring Config::GetHorseInfo()
{
	return m_ini.GetString("HorseInfo", "欢迎来到爱玩游戏，请玩家文明娱乐，严禁赌博，如发现有赌博行为，将封停账号，并向公安机关举报！");
}

Lstring Config::GetLogicDBIp()
{
	return m_ini.GetString("LogicDBIp", "127.0.0.1");
}

Lshort Config::GetLogicDBPort()
{
	return m_ini.GetShort("LogicDBPort", 7001);
}

Lstring Config::GetLogicDBKey()
{
	return m_ini.GetString("LogicDBKey", "gj2ag3j54acbfbaa1a2f7a70d766c1d");
}

Lstring	Config::GetDbHost()
{
	return m_ini.GetString("DbHost", "192.168.1.139");
}

Lstring	Config::GetDbUser()
{
	return m_ini.GetString("DbUser", "root");
}

Lstring	Config::GetDbPass()
{
	return m_ini.GetString("DbPass", "");
}

Lstring	Config::GetDbName()
{
	return m_ini.GetString("DbName", "test");
}

Lshort	Config::GetDbPort()
{
	return m_ini.GetShort("DbPort", 3306);
}

Lstring Config::GetRedisIp()
{
	return m_ini.GetString("RedisIp","127.0.0.1");
}

Lshort Config::GetRedisPort()
{
	return m_ini.GetShort("RedisPort", 6379);
}
