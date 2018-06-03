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
