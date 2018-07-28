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

Lstring UserManager::getUserNameById(Lint iUserId)
{
	auto itUser = m_mapUserBaseInfo.find(iUserId);
	if (itUser != m_mapUserBaseInfo.end())
	{
		LLOG_ERROR("Find user, user id=%d, nike=%s.", iUserId, itUser->second->m_nike.c_str());
		return itUser->second->m_nike;
	}
	LLOG_ERROR("cannot find user, user id=%d", iUserId);
	return "";
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

std::vector<GonghuiUser> UserManager::getGonghuiUserInfoById(Lint gonghuiId)
{
	std::vector<GonghuiUser> userVector;
	std::map<Lint, Gonghui>::iterator iter = m_gonghuiInfo.find(gonghuiId);
	if (iter != m_gonghuiInfo.end())
	{
		userVector = iter->second.m_userInfo;
	}
	return userVector;
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

Gonghui UserManager::getGonghuiInfoById(Lint gonghuiId)
{
	Gonghui gonghui;
	std::map<Lint, Gonghui>::iterator iter = m_gonghuiInfo.find(gonghuiId);
	if (iter != m_gonghuiInfo.end())
	{
		gonghui = iter->second;
	}
	return gonghui;
}

void UserManager::setGonghuiInfo(std::vector<Gonghui> gonghuiInfo)
{
	for (Gonghui gonghui : gonghuiInfo)
	{
		m_gonghuiInfo[gonghui.m_gonghuiId] = gonghui;
	}
}

void UserManager::updateGonghuiPaiju(Lint gonghuiId, Lint roomId, Lstring roomState, Lstring user[4])
{
	LLOG_ERROR("update paiju, gonghuiId:%d, roomState=%s, roomId=%d, user=%s,%s,%s,%s.", gonghuiId, roomState.c_str(), roomId, user[0].c_str(), user[1].c_str(), user[2].c_str(), user[3].c_str());

	std::map<Lint, Gonghui>::iterator iter = m_gonghuiInfo.find(gonghuiId);
	if (iter != m_gonghuiInfo.end())
	{
		std::vector<PaiJuInfo> paijuInfo = iter->second.m_paijuInfo;

		std::vector<PaiJuInfo>::iterator paijuIter = paijuInfo.begin();
		for (; paijuIter != paijuInfo.end(); )
		{
			if (paijuIter->m_roomId == roomId)
			{
				paijuIter->m_roomState = roomState;
				paijuIter->m_user1 = user[1];
				paijuIter->m_user2 = user[2];
				paijuIter->m_user3 = user[3];
				paijuIter->m_user4 = user[4];
				break;
			}
			++paijuIter;
		}
	}
}

Lint UserManager::delGonghuiPaiju(Lint gonghuiId, Lint roomId)
{
	LLOG_ERROR("delete paiju, gonghuiId:%d, roomId=%d.", gonghuiId, roomId);
	std::map<Lint, Gonghui>::iterator iter = m_gonghuiInfo.find(gonghuiId);

	Lint roomCounts = 0;
	if (iter != m_gonghuiInfo.end())
	{
		std::vector<PaiJuInfo> paijuInfo = iter->second.m_paijuInfo;
		
		std::vector<PaiJuInfo>::iterator paijuIter = paijuInfo.begin();
		for (; paijuIter != paijuInfo.end(); )
		{
			if (paijuIter->m_roomId == roomId)
			{
				paijuIter = paijuInfo.erase(paijuIter);
				iter->second.m_paijuCount--;
				roomCounts = paijuIter->m_roomCounts;
			}
			else
			{
				++paijuIter;
			}
		}
		iter->second.m_paijuInfo = paijuInfo;
	}

	return roomCounts;
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

void UserManager::addGonghuiApply(Lint gonghuiId, Lint userId, Lstring userName)
{
	LLOG_ERROR("add gonghui apply, gonghuiId=%d, userId=%d, userName=%s.", gonghuiId, userId, userName.c_str());
	
	std::map<Lint, std::set<Lint>>::iterator iter = m_gonghuiApplyInfo.find(gonghuiId);
	if (iter != m_gonghuiApplyInfo.end())
	{
		std::set<Lint> x = iter->second;
		x.insert(userId);
		iter->second = x;
	}
	else
	{
		std::set<Lint> x;
		x.insert(userId);
		m_gonghuiApplyInfo.insert(std::map<Lint, std::set<Lint>>::value_type(gonghuiId, x));
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

std::vector<GonghuiUser> UserManager::getGonghuiApplyUser(Lint gonghuiId)
{
	std::vector<GonghuiUser> applyUser;
	std::map<Lint, std::set<Lint>>::iterator iter = m_gonghuiApplyInfo.find(gonghuiId);
	if (iter == m_gonghuiApplyInfo.end())
	{
		LLOG_ERROR("Error, gonghui: %d no apply user.", gonghuiId);
		return applyUser;
	}

	std::set<Lint> applyUserList = iter->second;

	for (Lint userId : applyUserList)
	{
		GonghuiUser user;
		user.id = userId;
		user.name = getUserNameById(userId);
		LLOG_ERROR("Get gonghui apply user, userId:%d, userName:%s.", userId, user.name.c_str());
		applyUser.push_back(user);
	}
	
	return applyUser;
}

Lint UserManager::gonghuiApplyOp(Lint gonghuiId, Lint userId, bool opResult)
{
	std::map<Lint, std::set<Lint>>::iterator iter = m_gonghuiApplyInfo.find(gonghuiId);
	if (iter == m_gonghuiApplyInfo.end())
	{
		LLOG_ERROR("Error, gonghui: %d does not exist.", gonghuiId);
		return -1;
	}

	std::set<Lint> applyUserList = iter->second;
	
	std::set<Lint>::iterator setIter = applyUserList.find(userId);
	if (setIter == applyUserList.end())
	{
		LLOG_ERROR("Error, user:%d does not apply to add gonghui: %d.", userId, gonghuiId);
		return -2;
	}

	// 不管是同意还是不同意，都删除掉申请记录中的数据
	applyUserList.erase(setIter);
	iter->second = applyUserList;

	// TODO 增加对应的数据库操作
	if (opResult)
	{
		// 用户加入工会
		return addGonghuiUser(gonghuiId, userId);
	}
	else
	{
		// 不同意用户加入工会，需要更新删除数据库中的申请记录
	}

	return 0;
}

Lint UserManager::addGonghuiUser(Lint gonghuiId, Lint userId)
{
	std::vector<GonghuiUser> userVector;
	std::map<Lint, Gonghui>::iterator iter = m_gonghuiInfo.find(gonghuiId);
	if (iter == m_gonghuiInfo.end())
	{
		LLOG_ERROR("Error, gonghui: %d does not exist.", gonghuiId);
		return -1;
	}

	userVector = iter->second.m_userInfo;

	GonghuiUser user;
	user.id = userId;
	user.name = getUserNameById(userId);
	userVector.push_back(user);
	iter->second.m_userInfo = userVector;

	// TODO 带补充数据库中新增用户的处理逻辑

	return 0;
}

Lint UserManager::delGonghuiUser(Lint gonghuiId, Lint userId)
{
	std::vector<GonghuiUser> userVector;
	std::map<Lint, Gonghui>::iterator iter = m_gonghuiInfo.find(gonghuiId);
	if (iter == m_gonghuiInfo.end())
	{
		LLOG_ERROR("Error, gonghui: %d does not exist.", gonghuiId);
		return -1;
	}

	userVector = iter->second.m_userInfo;

	bool find = false;
	std::vector<GonghuiUser>::iterator userIter = userVector.begin();
	for (; userIter != userVector.end(); ++userIter)
	{
		if (userIter->id == userId)
		{
			find = true;
			userVector.erase(userIter);
			break;
		}
	}

	LLOG_ERROR("find user=%d.", find);
	if (find)
	{
		iter->second.m_userInfo = userVector;
		iter->second.m_userCount--;

		// TODO 待补充数据库中删除工会用户的逻辑
	}
	return 0;
}

void UserManager::SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

std::string& UserManager::replace_all_distinct(std::string& str, const std::string& old_value, const std::string& new_value)
{
	for (std::string::size_type pos = 0; pos != std::string::npos; pos += new_value.length())
	{
		if ((pos = str.find(old_value, pos)) != std::string::npos)
		{
			str.replace(pos, old_value.length(), new_value);
		}
		else
		{
			break;
		}
	}
	return   str;
}

Lint UserManager::updateGonghuiRoomPolicy(Lint gonghuiId, Lstring roomPolicy, bool isAdd)
{
	std::map<Lint, Gonghui>::iterator iter = m_gonghuiInfo.find(gonghuiId);
	if (iter == m_gonghuiInfo.end())
	{
		LLOG_ERROR("Error, gonghui: %d does not exist.", gonghuiId);
		return -1;
	}

	Lstring curPolicy = iter->second.m_roomPolicy;
	Lstring newPolicy;
	
	if (isAdd)
	{
		newPolicy = (curPolicy[curPolicy.length() - 1] == ';') ? (curPolicy + roomPolicy) : (curPolicy + ";" + roomPolicy);
	}
	else
	{
		newPolicy = replace_all_distinct(curPolicy, roomPolicy, "");
	}
	iter->second.m_roomPolicy = newPolicy;

	// TODO 检测policy的合法性，并且调用创建工会房间的接口，创建房间
	// TODO 待补充数据库入库信息
}