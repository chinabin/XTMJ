#include "LLog.h"
#include "LDBSession.h"

LDBSession::LDBSession()
	:m_Port(0)
	,m_Mysql(NULL)
{

}

LDBSession::~LDBSession()
{
	if(m_Mysql)
	{
		mysql_close(m_Mysql);
	}
}

bool LDBSession::Init( const std::string& host, const std::string& user, const std::string& pass, const std::string& dbname, const std::string& character, Lshort port)
{
	m_Host = host;
	m_User = user;
	m_Pass = pass;
	m_DBName = dbname;
	m_Port = port;
	m_Character = character;

	//初始化mysql
	if((m_Mysql = mysql_init(NULL)) == 0)
	{
		LLOG_ERROR("mysql_init() error %s");
		return false;
	}

	//设置mysql自动重连
	char value = 1;
	if(mysql_options(m_Mysql, MYSQL_OPT_RECONNECT, (char*)&value) != 0)
	{
		LLOG_ERROR("mysql_options error %s",mysql_error(m_Mysql));
		return false;
	}

	//连接数据库
	if(mysql_real_connect(m_Mysql,m_Host.c_str(),m_User.c_str(),m_Pass.c_str(),dbname.c_str(),m_Port,NULL,0) == 0)
	{
		LLOG_ERROR("mysql_real_connect error %s",mysql_error(m_Mysql));
		return false;
	}

	//设置mysql字符集
	if(mysql_set_character_set(m_Mysql, m_Character.c_str()) != 0)
	{
		LLOG_ERROR("mysql_set_character_set error %s",mysql_error(m_Mysql));
		return false;
	}

	return true;
}

MYSQL* LDBSession::GetMysql()
{
	mysql_ping(m_Mysql);
	return m_Mysql;
}
