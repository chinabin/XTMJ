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
	Lstring getUserNameById(Lint iUserId);
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
	void setGonghuiApply(std::map<Lint, std::set<Lint>> gonghuiApply);
	std::vector<Gonghui> getGonghuiInfo();
	Gonghui getGonghuiInfoById(Lint gonghuiId);
	bool isUserInGonghui(Lint gonghuiId, Lint userId);
	std::vector<GonghuiUser> getGonghuiUserInfoById(Lint gonghuiId);
	void addGonghuiPaiju(Lint gonghuiId, PaiJuInfo paijuInfo);
	void addGonghuiApply(Lint gonghuiId, Lint userId);
	Lint delGonghuiPaiju(Lint gonghuiId, Lint roomId);
	void updateGonghuiPaiju(Lint gonghuiId, Lint roomId, Lstring roomState, Lint user[4]);
	std::vector<Gonghui> getUserGonghuiByUserId(Lint userId);
	Lint gonghuiApplyOp(Lint gonghuiId, Lint userId, bool opResult);
	std::vector<GonghuiUser> getGonghuiApplyUser(Lint gonghuiId);

	Lint addGonghuiUser(Lint gonghuiId, Lint userId);
	Lint delGonghuiUser(Lint gognhuiId, Lint userId);

	Lint delGonghui(Lint gonghuiId);

	void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);
	Lint updateGonghuiRoomPolicy(Lint gonghuiId, Lstring roomPolicy, bool isAdd);
	std::string& replace_all_distinct(std::string& str, const std::string& old_value, const std::string& new_value);
private:
	boost::mutex m_mutexUserQueue;

	std::map<Lstring, boost::shared_ptr<User> > m_mapUUID2User;						//只有在线玩家
	std::map<Lint, boost::shared_ptr<User> > m_mapId2User;
private:
	boost::mutex m_mutexUserBaseInfoQueue;
	std::map<Lint, boost::shared_ptr<UserBaseInfo> >  m_mapUserBaseInfo;			//离线玩家也包含
	std::map<Lint, Gonghui> m_gonghuiInfo;
	std::map<Lint, std::set<Lint>> m_gonghuiApplyInfo;
private:
	boost::mutex m_mutexUserLoginInfoQueue;
	std::map<Lint, boost::shared_ptr<UserLoginInfo> > m_mapUserLoginInfo;
};

#define gUserManager UserManager::Instance()

#endif