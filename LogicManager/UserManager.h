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
public:
	void addUser(boost::shared_ptr<User> user);
	void delUser(Lint iUserId);

	boost::shared_ptr<CSafeResourceLock<User> > getUserByGateIdAndUUID(Lint iGateId, const Lstring& strUUID);
	boost::shared_ptr<CSafeResourceLock<User> > getUserbyUserId(Lint iUserId);
	boost::shared_ptr<CSafeResourceLock<User> > getUserbyUserUUID(const Lstring& uuid);

public:
	void addUserBaseInfo(boost::shared_ptr<UserBaseInfo> userBaseInfo);
	boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > getUserBaseInfo(Lint iUserId);
public:
	void addUserLoginInfo(boost::shared_ptr<UserLoginInfo> userLoginInfo);
	void delUserLoginInfo(Lint iUserId);
	boost::shared_ptr<CSafeResourceLock<UserLoginInfo> > getUserLoginInfo(Lint iUserId);
	void setGonghuiInfo(std::vector<Gonghui> gonghuiInfo);
	std::vector<Gonghui> getGonghuiInfo();
	void addGonghuiPaiju(Lint gonghuiId, PaiJuInfo paijuInfo);
	std::vector<Gonghui> getUserGonghuiByUserId(Lint userId);
private:
	boost::mutex m_mutexUserQueue;

	std::map<Lstring, boost::shared_ptr<User> > m_mapUUID2User;						//只有在线玩家
	std::map<Lint, boost::shared_ptr<User> > m_mapId2User;
private:
	boost::mutex m_mutexUserBaseInfoQueue;
	std::map<Lint, boost::shared_ptr<UserBaseInfo> >  m_mapUserBaseInfo;			//离线玩家也包含
	std::map<Lint, Gonghui> m_gonghuiInfo;
private:
	boost::mutex m_mutexUserLoginInfoQueue;
	std::map<Lint, boost::shared_ptr<UserLoginInfo> > m_mapUserLoginInfo;
};

#define gUserManager UserManager::Instance()

#endif