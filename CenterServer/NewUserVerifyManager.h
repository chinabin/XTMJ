#ifndef _DB_NEWUSER_GM_H_
#define _DB_NEWUSER_GM_H_

#include "LBase.h"
#include "LSingleton.h"
#include "NewUserVerify.h"

#define  NEWUSERVERIFY_NUM   4

//db服务器
class NewUserVerifyManager : public LSingleton<NewUserVerifyManager>
{
public:
	virtual bool		Init();
	virtual bool		Final();

	virtual void		Clear();

	void                Start();

	void                Join();

	void                Stop();

	void                Push(const Lstring& strUUID, LMsg* pMsg);


private:

	NewUserVerify      m_dbpool[NEWUSERVERIFY_NUM];
};


#define gDbNewUserManager NewUserVerifyManager::Instance()

#endif
