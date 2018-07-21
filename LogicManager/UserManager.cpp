#include "UserManager.h"

bool UserManager::Init()
{
	return true;
}

bool UserManager::Final()
{
	return true;
}

void UserManager::addUser(boost::shared_ptr<User> user)
{
	if(user.get() == NULL)
	{
		return;
	}

	boost::mutex::scoped_lock l(m_mutexUserQueue);
	if(m_mapId2User.find(user->m_userData.m_id) == m_mapId2User.end())
	{
		m_mapId2User[user->m_userData.m_id] = user;
		m_mapUUID2User[user->m_userData.m_unioid] = user;
	}
}

void UserManager::delUser(Lint iUserId)
{
	boost::mutex::scoped_lock l(m_mutexUserQueue);

	auto itUser = m_mapId2User.find(iUserId);
	if(itUser != m_mapId2User.end())
	{
		m_mapUUID2User.erase(itUser->second->m_userData.m_unioid);
		m_mapId2User.erase(itUser);
	}
}

boost::shared_ptr<CSafeResourceLock<User> > UserManager::getUserByGateIdAndUUID(Lint iGateId, const Lstring& strUUID)
{
	boost::shared_ptr<User> user;

	do
	{
		boost::mutex::scoped_lock l(m_mutexUserQueue);

		auto itUser = m_mapUUID2User.find(strUUID);
		if(itUser == m_mapUUID2User.end())
		{
			break;
		}

		if(itUser->second->getUserGateID() != iGateId)
		{
			break;
		}

		user = itUser->second;

	}while(false);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser;
	if(user)
	{
		safeUser.reset(new CSafeResourceLock<User>(user));
	}

	return safeUser;
}

boost::shared_ptr<CSafeResourceLock<User> > UserManager::getUserbyUserId(Lint iUserId)
{
	boost::shared_ptr<User> user;

	do 
	{
		boost::mutex::scoped_lock l(m_mutexUserQueue);

		auto itUser = m_mapId2User.find(iUserId);
		if(itUser == m_mapId2User.end())
		{
			break;
		}

		user = itUser->second;

	}while(false);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser;
	if(user)
	{
		safeUser.reset(new CSafeResourceLock<User>(user));
	}

	return safeUser;
}

boost::shared_ptr<CSafeResourceLock<User> > UserManager::getUserbyUserUUID(const Lstring& uuid)
{
	boost::shared_ptr<User> user;

	do 
	{
		boost::mutex::scoped_lock l(m_mutexUserQueue);

		auto itUser = m_mapUUID2User.find(uuid);
		if(itUser == m_mapUUID2User.end())
		{
			break;
		}

		user = itUser->second;

	}while(false);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser;
	if(user)
	{
		safeUser.reset(new CSafeResourceLock<User>(user));
	}

	return safeUser;
}


void UserManager::addUserBaseInfo(boost::shared_ptr<UserBaseInfo> userBaseInfo)
{
	if(userBaseInfo.get() == NULL)
	{
		return;
	}

	boost::mutex::scoped_lock l(m_mutexUserBaseInfoQueue);
	m_mapUserBaseInfo[userBaseInfo->m_id] = userBaseInfo;
}

boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > UserManager::getUserBaseInfo(Lint iUserId)
{
	boost::shared_ptr<UserBaseInfo> userBaseInfo;
	do 
	{
		boost::mutex::scoped_lock l(m_mutexUserBaseInfoQueue);
		auto itBaseInfo = m_mapUserBaseInfo.find(iUserId);
		if(itBaseInfo == m_mapUserBaseInfo.end())
		{
			break;
		}

		userBaseInfo = itBaseInfo->second;

	}while(false);

	boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo;
	if(userBaseInfo)
	{
		safeBaseInfo.reset(new CSafeResourceLock<UserBaseInfo>(userBaseInfo));
	}

	return safeBaseInfo;
}

void UserManager::addUserLoginInfo(boost::shared_ptr<UserLoginInfo> userLoginInfo)
{
	if(userLoginInfo.get() == NULL)
	{
		return;
	}

	 boost::shared_ptr<UserLoginInfo> userLoginInfoBefore;

	 {
		 boost::mutex::scoped_lock l(m_mutexUserLoginInfoQueue);
		 auto itLoginInfo = m_mapUserLoginInfo.find(userLoginInfo->m_user.m_id);
		 if(itLoginInfo == m_mapUserLoginInfo.end())
		 {
			 m_mapUserLoginInfo[userLoginInfo->m_user.m_id] = userLoginInfo;
		 }
		 else
		 {
			 userLoginInfoBefore = itLoginInfo->second;
		 }

	 }

	 if(userLoginInfoBefore)  //如果存在则修改内容
	 {
		 boost::shared_ptr<CSafeResourceLock<UserLoginInfo> > safeLoginInfo(new CSafeResourceLock<UserLoginInfo>(userLoginInfoBefore)); //对单个加锁

		 safeLoginInfo->getResource()->m_seed = userLoginInfo->m_seed;
		 safeLoginInfo->getResource()->m_time = userLoginInfo->m_time;
		 safeLoginInfo->getResource()->m_user = userLoginInfo->m_user;
	 }
}

void UserManager::delUserLoginInfo(Lint iUserId)
{
	boost::mutex::scoped_lock l(m_mutexUserLoginInfoQueue);
	m_mapUserLoginInfo.erase(iUserId);
}

std::vector<Gonghui> UserManager::getGonghuiInfo()
{
	std::vector<Gonghui> tmpGonghuiInfo;
	
	std::map<Lint, Gonghui>::iterator iter;
	for (iter = m_gonghuiInfo.begin(); iter != m_gonghuiInfo.end(); iter++)
	{
		tmpGonghuiInfo.push_back(iter->second);
	}

	return tmpGonghuiInfo;
}

void UserManager::setGonghuiInfo(std::vector<Gonghui> gonghuiInfo)
{
	for (Gonghui gonghui : gonghuiInfo)
	{
		m_gonghuiInfo[gonghui.m_gonghuiId] = gonghui;
	}
}

void UserManager::addGonghuiPaiju(Lint gonghuiId, PaiJuInfo paijuInfo)
{
	LLOG_ERROR("add paiju, gonghuiId:%d, roomId=%d,counts=%d,roomType=%s,baseScore=%d.", gonghuiId, paijuInfo.m_roomId, paijuInfo.m_roomCounts, paijuInfo.m_roomType.c_str(), paijuInfo.m_roomScore);
	std::map<Lint, Gonghui>::iterator iter;
	iter = m_gonghuiInfo.find(gonghuiId);
	if (iter != m_gonghuiInfo.end())
	{
		iter->second.m_paijuInfo.push_back(paijuInfo);
		iter->second.m_paijuCount = iter->second.m_paijuCount + 1;
	}
}

std::vector<Gonghui> UserManager::getUserGonghuiByUserId(Lint userId)
{
	LLOG_ERROR("begin to get user gonghui info, userId=%d, gonghui size=%d.", userId, m_gonghuiInfo.size());
	std::vector<Gonghui> gonghuiInfo;
	std::map<Lint, Gonghui>::iterator iter;
	for (iter = m_gonghuiInfo.begin(); iter != m_gonghuiInfo.end(); iter++)
	{
		Gonghui gonghui = iter->second;
		std::vector<GonghuiUser> userList = gonghui.m_userInfo;

		LLOG_ERROR("gonghui info, gonghuiId=%d, gonghuiName=%s, adminUserId=%d.", gonghui.m_gonghuiId, gonghui.m_gonghuiName.c_str(), gonghui.m_adminUserId);

		for (GonghuiUser tmpUser : userList)
		{
			LLOG_DEBUG("user id=%d, name=%s.", tmpUser.id, tmpUser.name.c_str());
			if (tmpUser.id == userId)
			{
				LLOG_ERROR("find gonghui info, userId=%d, gonghuiId=%d, gonghuiName=%s, adnmin=%d.", userId, gonghui.m_gonghuiId, gonghui.m_gonghuiName.c_str(), gonghui.m_adminUserId);
				gonghuiInfo.push_back(gonghui);
			}
		}
	}
	return gonghuiInfo;
}

boost::shared_ptr<CSafeResourceLock<UserLoginInfo> > UserManager::getUserLoginInfo(Lint iUserId)
{
	boost::shared_ptr<UserLoginInfo> userLoginInfo;
	do 
	{
		boost::mutex::scoped_lock l(m_mutexUserLoginInfoQueue);
		auto itLoginInfo = m_mapUserLoginInfo.find(iUserId);
		if(itLoginInfo == m_mapUserLoginInfo.end())
		{
			break;
		}

		userLoginInfo = itLoginInfo->second;

	}while(false);

	boost::shared_ptr<CSafeResourceLock<UserLoginInfo> > safeLoginInfo;
	if(userLoginInfo)
	{
		safeLoginInfo.reset(new CSafeResourceLock<UserLoginInfo>(userLoginInfo));
	}

	return safeLoginInfo;
}
