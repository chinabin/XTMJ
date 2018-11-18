#include "UserManager.h"
#include "GonghuiManager.h"

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

bool UserManager::isUserInGonghui(Lint gonghuiId, Lint userId)
{
	std::vector<GonghuiUser> userList = getGonghuiUserInfoById(gonghuiId);

	for (GonghuiUser user : userList)
	{
		if (userId == user.id)
		{
			return true;
		}
	}
	return false;
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

void UserManager::setGonghuiApply(std::map<Lint, std::set<Lint>> gonghuiApply)
{
	m_gonghuiApplyInfo = gonghuiApply;
}

void UserManager::setGonghuiInfo(std::vector<Gonghui> gonghuiInfo)
{
	for (Gonghui gonghui : gonghuiInfo)
	{
		m_gonghuiInfo[gonghui.m_gonghuiId] = gonghui;
	}
}

void UserManager::updateGonghuiPaiju(Lint gonghuiId, Lint roomId, Lstring roomState, Lint user[4])
{
	LLOG_ERROR("update paiju, gonghuiId:%d, roomState=%s, roomId=%d, user=%d,%d,%d,%d.", gonghuiId, roomState.c_str(), roomId, user[0], user[1], user[2], user[3]);
	
	std::map<Lint, Gonghui>::iterator iter = m_gonghuiInfo.find(gonghuiId);
	if (iter != m_gonghuiInfo.end())
	{
		std::vector<PaiJuInfo> paijuInfo = iter->second.m_paijuInfo;

		std::vector<PaiJuInfo>::iterator paijuIter = paijuInfo.begin();
		for (; paijuIter != paijuInfo.end(); )
		{
			if (paijuIter->m_roomId == roomId)
			{
				if ("1" == roomState)
				{
					paijuIter->m_roomState = "WAITING";
				}
				else if ("2" == roomState)
				{
					paijuIter->m_roomState = "PLAYING";
				}
				else if ("3" == roomState)
				{
					paijuIter->m_roomState = "END";
				}
				else if ("4" == roomState)
				{
					paijuIter->m_roomState = "Abort";
				}
				else
				{
					paijuIter->m_roomState = "IDLE";
				}
				
				paijuIter->m_user1 = getUserNameById(user[0]);
				paijuIter->m_user2 = getUserNameById(user[1]);
				paijuIter->m_user3 = getUserNameById(user[2]);
				paijuIter->m_user4 = getUserNameById(user[3]);
				break;
			}
			++paijuIter;
		}

		iter->second.m_paijuInfo = paijuInfo;
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
	LLOG_ERROR("add paiju, gonghuiId:%d, roomId=%d,counts=%d,roomType=%d,baseScore=%d.", gonghuiId, paijuInfo.m_roomId, paijuInfo.m_roomCounts, paijuInfo.m_roomType, paijuInfo.m_roomScore);
	std::map<Lint, Gonghui>::iterator iter;
	iter = m_gonghuiInfo.find(gonghuiId);
	if (iter != m_gonghuiInfo.end())
	{
		iter->second.m_paijuInfo.push_back(paijuInfo);
		iter->second.m_paijuCount = iter->second.m_paijuCount + 1;
	}
}

void UserManager::addGonghuiApply(Lint gonghuiId, Lint userId)
{
	LLOG_ERROR("add gonghui apply, gonghuiId=%d, userId=%d.", gonghuiId, userId);
	
	std::map<Lint, std::set<Lint>>::iterator iter = m_gonghuiApplyInfo.find(gonghuiId);
	if (iter != m_gonghuiApplyInfo.end())
	{
		std::set<Lint> x = iter->second;
		for (Lint id : x)
		{
			if (id == userId)
			{
				return;
			}
		}
		x.insert(userId);
		iter->second = x;
		gGonghuiManager.insertGonghuiApply(gonghuiId, userId);
	}
	else
	{
		std::set<Lint> x;
		x.insert(userId);
		m_gonghuiApplyInfo.insert(std::map<Lint, std::set<Lint>>::value_type(gonghuiId, x));
		gGonghuiManager.insertGonghuiApply(gonghuiId, userId);
	}
}

std::vector<Gonghui> UserManager::getUserGonghuiByUserId(Lint userId)
{
	//LLOG_ERROR("begin to get user gonghui info, userId=%d, gonghui size=%d.", userId, m_gonghuiInfo.size());
	std::vector<Gonghui> gonghuiInfo;
	std::map<Lint, Gonghui>::iterator iter;
	for (iter = m_gonghuiInfo.begin(); iter != m_gonghuiInfo.end(); iter++)
	{
		Gonghui gonghui = iter->second;
		std::vector<GonghuiUser> userList = gonghui.m_userInfo;

		//LLOG_ERROR("gonghui info, gonghuiId=%d, gonghuiName=%s, adminUserId=%d.", gonghui.m_gonghuiId, gonghui.m_gonghuiName.c_str(), gonghui.m_adminUserId);

		for (GonghuiUser tmpUser : userList)
		{
			//LLOG_DEBUG("user id=%d, name=%s.", tmpUser.id, tmpUser.name.c_str());
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
		gGonghuiManager.updateGonghuiApply(gonghuiId, userId, "Approved");
		return addGonghuiUser(gonghuiId, userId);
	}
	else
	{
		// 不同意用户加入工会，需要更新删除数据库中的申请记录
		if (!gGonghuiManager.updateGonghuiApply(gonghuiId, userId, "Refused"))
		{
			LLOG_ERROR("Error, failed to delete user %d apply info from db.", userId);
			return 0;
		}
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
	if (!gGonghuiManager.addGonghuiUser(gonghuiId, userId))
	{
		LLOG_ERROR("Error, add user %d to database failed.", userId);
		return -7;
	}
	return 0;
}

Lint UserManager::delGonghui(Lint gonghuiId)
{
	m_gonghuiInfo.erase(gonghuiId);
	m_gonghuiApplyInfo.erase(gonghuiId);
	if (gGonghuiManager.deleteGonghui(gonghuiId))
	{
		LLOG_ERROR("delete gonghui:%d success.", gonghuiId);
		return 0;
	}

	LLOG_ERROR("delete gonghui:%d failed.", gonghuiId);
	return -11;
}

std::vector<Lint> UserManager::getGonghuiIdByAdminUser(Lint adminUserId)
{
	std::vector<Lint> vec;
	std::map<Lint, Gonghui>::iterator iter = m_gonghuiInfo.begin();
	for (; iter != m_gonghuiInfo.end(); iter++)
	{
		Gonghui gonghui = iter->second;
		if (adminUserId == gonghui.m_adminUserId)
		{
			vec.push_back(gonghui.m_gonghuiId);
		}
	}
	return vec;
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

		if (!gGonghuiManager.delGonghuiUser(gonghuiId, userId))
		{
			LLOG_ERROR("Error, delete user %d from database failed.", userId);
			return -8;
		}
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
		if (curPolicy.find(roomPolicy.substr(0, roomPolicy.length() -1)) != std::string::npos)
		{
			LLOG_ERROR("Error, roomPolicy:%s had already exist.", roomPolicy.c_str());
			return -12;
		}
		newPolicy = (curPolicy[curPolicy.length() - 1] == ';') ? (curPolicy + roomPolicy) : (curPolicy + ";" + roomPolicy);
		LLOG_DEBUG("curPolicy=%s,newPolicy=%s.", curPolicy.c_str(), newPolicy.c_str());
	}
	else
	{
		newPolicy = replace_all_distinct(curPolicy, roomPolicy, "");
		newPolicy = replace_all_distinct(curPolicy, ";;", ";");
		std::vector<PaiJuInfo> newPaijuVec;
		std::vector<PaiJuInfo> tmpPaijuVec = iter->second.m_paijuInfo;
		for (PaiJuInfo paiju : tmpPaijuVec)
		{
			std::stringstream ss1,ss2;
			ss1 << paiju.m_roomType << "," << paiju.m_roomScore << "," << paiju.m_roomCounts << ",1";
			ss2 << paiju.m_roomType << "," << paiju.m_roomScore << "," << paiju.m_roomCounts << ",2";
			
			Lstring roomPolicy1 = ss1.str();
			Lstring roomPolicy2 = ss2.str();
			LLOG_DEBUG("roomPolicy=%s,roomPolicy1=%s,roomPolicy2=%s.", roomPolicy.c_str(), roomPolicy1.c_str(), roomPolicy2.c_str());
			if (roomPolicy1 != roomPolicy && roomPolicy2 != roomPolicy)
			{
				newPaijuVec.push_back(paiju);
			}
		}
		iter->second.m_paijuInfo = newPaijuVec;
		iter->second.m_paijuCount = newPaijuVec.size();
	}

	iter->second.m_roomPolicy = newPolicy;

	// TODO 检测policy的合法性，并且调用创建工会房间的接口，创建房间
	// TODO 待补充数据库入库信息
	gGonghuiManager.updateGonghuiPolicy(gonghuiId, newPolicy);
	return 0;
}