#ifndef _GATE_MANAGER_H_
#define _GATE_MANAGER_H_

#include "LBase.h"
#include "User.h"
#include "LSingleton.h"

class UserManager :public LSingleton<UserManager>
{
public:
	virtual	bool Init();
	virtual	bool Final();

	User* GetUserByGateIdAndUUID(Lint gateId, const Lstring& strUUID);

	User* GetUserbyDataId(Lint dataId);

	void  AddUser(User* user);

	void  DelUser(User* user);

	void  BoadCast(LMsg& msg);
private:
	std::map<Lstring, User*>	m_mapUUID2User;
	std::map<Lint, User*>		m_mapId2User;
};

#define gUserManager UserManager::Instance()

#endif