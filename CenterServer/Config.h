#ifndef _CONFIG_H_
#define _CONFIG_H_


#include "LIni.h"
#include "LSingleton.h"

class Config :public LSingleton<Config>
{
public:
	virtual	bool	Init();

	virtual	bool	Final();


	Lint	GetLogLevel();

	Lstring	GetDbHost();
	Lstring	GetDbUser();
	Lstring	GetDbPass();
	Lstring	GetDbName();
	Lshort	GetDbPort();

	Lstring	GetInsideIp();
	Lshort	GetInsidePort();

	Lstring GetRedisIp();
	Lshort	GetRedisPort();

	Lstring	GetLocalLogin();//本地登录

	Lstring GetGMIp();
	Lshort  GetGMPot();

	Lstring	GetCenterKey();

	Lint    GetTime();

	Lshort    GetWechatVerify();
private:
	LIniConfig	m_ini;
};

#define gConfig Config::Instance()

#endif
