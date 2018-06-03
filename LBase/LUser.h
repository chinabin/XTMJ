#ifndef _L_USER_H_
#define _L_USER_H_ 

#include "LBase.h"
#include "LBuff.h"


class LUser
{
public:
	LUser();
	~LUser();

	void	Read(LBuff& buff);
	void	Write(LBuff& buff);

public:
	Lint		m_id;
	Lstring		m_openid;
	Lstring		m_nike;
	Lint		m_sex;
	Lstring		m_provice;
	Lstring		m_city;
	Lstring		m_country;
	Lstring		m_headImageUrl;
	Lstring		m_unioid;
	Lint		m_numOfCard1s;
	Lint		m_numOfCard2s;
	Lint		m_numOfCard3s;
	Lint		m_regTime;
	Lint		m_lastLoginTime;
	Lint		m_new;
	Lint		m_gm;
	Lint		m_totalbuynum;
	Lint		m_totalplaynum;
	Lint        m_creditValue;
	Lint        m_count;
	Lint        m_coins;  //金币
	Lint		m_agentPower;		//代理权 0没有代理权
	Lint		m_agentId;			//绑定的代理人 对应m_id
	Lint		m_status;			//账号状态 0删除 1正常 2封号
};

#endif
