#ifndef _D_USER_H_
#define _D_USER_H_


#include "LUser.h"

class CSafeLock
{
public:
	CSafeLock()
	{
		m_uLockedSum = 0;
	}
	virtual ~CSafeLock(){}
protected:
	boost::thread::id m_lockedThreadId;
	unsigned int m_uLockedSum;
};

class DUser : public CSafeLock
{
	friend class CSafeUser;
public:
	DUser();
	virtual ~DUser();

	// 添加房卡
	void	AddCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin, const Lstring& remark = "");
	//删除房卡
	void	DelCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin);

	// 添加金币
	void	AddCoinsCount(Lint count, Lint operType);
	// 删除金币
	void	DelCoinsCount(Lint count, Lint operType);

	void    ExchangeCardCoin(Lint add,Lint del,Lint operType,Lint cardType,const Lstring& admin);

	void    AddCreditValue();   //只会加信用

	// 增加玩家打牌的次数
	void	AddPlayCount();

	// 修改new状态 value见LMsgL2CeModifyUserNew中的注释
	void	ModifyNew( Lint value );

	void	Save();
	
public:
	LUser m_usert;
	Lint  m_lastReqTime;	//最近一次请求登录的时间
	Lint  m_logicID;		//玩家当前所在服务器Index
	Lstring m_loginIP;		//玩家登陆IP
};

class CSafeUser
{
public:
	CSafeUser(boost::shared_ptr<DUser> user);
	~CSafeUser();
public:
	boost::shared_ptr<DUser>& getUser();
	bool isValid();
protected:
	void _lockUser();
	void _unlockUser();
private:
	boost::shared_ptr<DUser> m_User;
	bool m_bLocked;
private:
	static boost::recursive_mutex m_mutexLockUser;
private:
	CSafeUser(){}
};

#endif
