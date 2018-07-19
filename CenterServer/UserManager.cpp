#include "UserManager.h"
#include "DbServer.h"
#include "LLog.h"
#include "LMsgS2S.h"
#include "Work.h"

#include "InsideNet.h"
#include "LDBSession.h"
#include "Config.h"


#define LOADUSER_COUNT			500000
#define LOADUSER_THREAD			16
#define LOADUSER_ALLOC_SIZE		50000

UserManager::UserManager()
{
	m_bLoadUserSuccess = false;
}

UserManager::~UserManager()
{

}

bool UserManager::Init()
{
	_loadUserInfoFromDB();
	_loadGonghuiInfoFromDB();
	//_loadTestData();
	return true;
}

void UserManager::addUser(boost::shared_ptr<DUser> user)
{
	if(user.get() == NULL)
	{
		return;
	}

	boost::mutex::scoped_lock l(m_mutexQueue);

	_addUser(user);
}

void UserManager::delUser(const Lstring& strUserId)
{
	boost::mutex::scoped_lock l(m_mutexQueue);

	auto itUser = m_mapUUID2User.find(strUserId);
	if(itUser != m_mapUUID2User.end())
	{
		m_mapId2User.erase(itUser->second->m_usert.m_id);
		m_mapUUID2User.erase(itUser);
	}
}

void UserManager::delUser(Lint iUserId)
{
	boost::mutex::scoped_lock l(m_mutexQueue);

	auto itUser = m_mapId2User.find(iUserId);
	if(itUser != m_mapId2User.end())
	{
		m_mapUUID2User.erase(itUser->second->m_usert.m_unioid);
		m_mapId2User.erase(itUser);
	}
}

boost::shared_ptr<CSafeUser> UserManager::getUser(const Lstring& strUserId)
{
	boost::shared_ptr<DUser> user;

	{
		boost::mutex::scoped_lock l(m_mutexQueue);
		auto itUser = m_mapUUID2User.find(strUserId);
		if(itUser != m_mapUUID2User.end())
		{
			user = itUser->second;
		}
	}
	
	boost::shared_ptr<CSafeUser> safeUser;
	if(user)
	{
		safeUser.reset(new CSafeUser(user));
	}

	return safeUser;
}

boost::shared_ptr<CSafeUser> UserManager::getUser(Lint iUserId)
{
	boost::shared_ptr<DUser> user;

	{
		boost::mutex::scoped_lock l(m_mutexQueue);
		auto itUser = m_mapId2User.find(iUserId);
		if(itUser != m_mapId2User.end())
		{
			user = itUser->second;
		}
	}

	boost::shared_ptr<CSafeUser> safeUser;
	if(user)
	{
		safeUser.reset(new CSafeUser(user));
	}

	return safeUser;
}

void UserManager::loadGonghuiInfo(Lint serverID)
{
	boost::mutex::scoped_lock l(m_mutexQueue);
	LMsgCe2LGonghuiInfo sendMsg;
	for (auto it = m_mapId2Gonghui.begin(); it != m_mapId2Gonghui.end(); it++)
	{
		sendMsg.m_gonghui.push_back(it->second);
	}
	sendMsg.m_count = m_mapId2Gonghui.size();
	sendMsg.m_hasSentAll = 1;

	gWork.SendMsgToLogic(sendMsg, serverID, 2000);
	LLOG_ERROR("complete load gonghui info, size=%d.", sendMsg.m_count);
}

void UserManager::LoadUserIdInfo(Lint serverID)
{
	boost::mutex::scoped_lock l(m_mutexQueue);

	LTime cur;

	int iUserCount = 0;
 	auto it = m_mapId2User.begin();
	if(it != m_mapId2User.end())
	{
		while(it != m_mapId2User.end())
		{
			int iCount = 0;
			LMsgCe2LUserIdInfo send;
			for(; it != m_mapId2User.end() && iCount < 50;++it)
			{
				UserIdInfo info;
				info.m_id = it->second->m_usert.m_id;
				info.m_nike = it->second->m_usert.m_nike;
				info.m_headImageUrl = it->second->m_usert.m_headImageUrl;
				info.m_openId = it->second->m_usert.m_openid;
				info.m_sex = it->second->m_usert.m_sex;
				info.m_agentPower = it->second->m_usert.m_agentPower;
				send.m_info.push_back(info);

				++iCount;
				++iUserCount;
			}

			send.m_count = iCount;
			send.m_hasSentAll = iUserCount == m_mapId2User.size() ? 1 : 0;

			gWork.SendMsgToLogic(send, serverID, 2000);
		}
	}
	else
	{
		LMsgCe2LUserIdInfo send;
		send.m_count = 0;
		send.m_hasSentAll = 1;

		gWork.SendMsgToLogic(send, serverID);
	}

	LTime now;
	LLOG_ERROR("Complete load user info. time = %ld, UserCount = %d", now.Secs() - cur.Secs(), iUserCount);
}

Lstring generateKey(int iLen)
{
	static Lstring strSeed = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*(){}[];,.?|";
	Lstring strKey;
	for(int i = 0; i < iLen; ++i)
	{
		int iRand = L_Rand(0, strSeed.length() - 1);
		strKey += strSeed.at(iRand);
	}

	return strKey;
}

void UserManager::_loadTestData()
{
	std::map<std::string, DUser*> mapUser;
	for(int i = 0; i < 2000000; ++i)
	{
		boost::shared_ptr<DUser> user(new DUser);
		user->m_usert.m_id = i;
		user->m_usert.m_openid = generateKey(32);
		user->m_usert.m_nike = generateKey(20);
		user->m_usert.m_sex = 0;
		user->m_usert.m_provice = "beijing";
		user->m_usert.m_city = "beijing";
		user->m_usert.m_country = "china";
		user->m_usert.m_headImageUrl = "";
		user->m_usert.m_unioid = user->m_usert.m_openid;
		user->m_usert.m_numOfCard1s = 0;
		user->m_usert.m_numOfCard2s = 50;
		user->m_usert.m_numOfCard3s = 0;
		user->m_usert.m_lastLoginTime = 5643131;
		user->m_usert.m_regTime = 5643131;
		user->m_usert.m_new = 0;
		user->m_usert.m_gm = 0;
		user->m_usert.m_totalbuynum = 0;
		user->m_usert.m_totalplaynum = 0;
		user->m_usert.m_status = 1;
		_addUser(user);

		if(mapUser.size() < 40000)
		{
			mapUser[user->m_usert.m_unioid] = user.get();
		}
	}

// 	LTime cur;
// 
// 	LTime now;
// 	LLOG_ERROR("miao %ld", now.MSecs() - cur.MSecs());

	//插入消息
	for(int i = 0; i < 4; ++i)
	for(auto it = mapUser.begin(); it != mapUser.end();++it)
	{

		LMsgC2SMsg* pMsg = new LMsgC2SMsg;

		pMsg->m_openId = it->second->m_usert.m_openid;
		pMsg->m_nike	   = it->second->m_usert.m_nike;
		pMsg->m_sign	   = "";
		pMsg->m_plate  = "wechat";
		pMsg->m_accessToken = "";
		pMsg->m_refreshToken = "";
		pMsg->m_md5		= "";
		pMsg->m_severID	= 15001;
		pMsg->m_uuid		= it->second->m_usert.m_unioid;
		pMsg->m_sex		= it->second->m_usert.m_sex;
		pMsg->m_imageUrl= it->second->m_usert.m_headImageUrl;
		pMsg->m_nikename= it->second->m_usert.m_nike;


		LMsgFromLoginServer* pTest = new LMsgFromLoginServer;
		pTest->m_userMsgId = MSG_C_2_S_MSG;
		pTest->m_userMsg   = pMsg;

		gWork.Push(pTest);
	}
}

bool UserManager::_loadGonghuiInfoFromDB()
{
	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	bool m_bLoadGonhuiSuccess = true;
	std::stringstream sql2;
	sql2 << "SELECT id, name, adminId, roomPolicy FROM gonghui";

	if (mysql_real_query(dbSession2.GetMysql(), sql2.str().c_str(), sql2.str().size()))
	{
		LLOG_ERROR("UserManager::LoadGonghuiInfo sql error %s", mysql_error(dbSession2.GetMysql()));
		return false;
	}

	MYSQL_RES* res2 = mysql_store_result(dbSession2.GetMysql());
	if (res2 == NULL)
	{
		LLOG_ERROR("Fail to store gonghui result. Error = %s", mysql_error(dbSession2.GetMysql()));
		return false;
	}

	MYSQL_ROW row2 = mysql_fetch_row(res2);
	if (!row2)
	{
		mysql_free_result(res2);
		return false;
	}

	while (row2)
	{
		Gonghui gonghui;
		gonghui.m_gonghuiId = atoi(*row2++);
		gonghui.m_gonghuiName = *row2++;
		gonghui.m_adminUserId = atoi(*row2++);
		gonghui.m_roomPolicy = *row2++;
		LLOG_ERROR("gonghui info: id=%d, name=%s, adminUserId=%d, roomPolciy=%s.", gonghui.m_gonghuiId, gonghui.m_gonghuiName.c_str(), gonghui.m_adminUserId, gonghui.m_roomPolicy.c_str());
		std::vector<PaiJuInfo> paijuInfo = _getPaiJuByGonghuiId(gonghui.m_gonghuiId);
		gonghui.m_paijuInfo = paijuInfo;
		gonghui.m_paijuCount = paijuInfo.size();

		std::vector<GonghuiUser> userInfo = _getUserInfoByGonghuiId(gonghui.m_gonghuiId);
		gonghui.m_userCount = userInfo.size();
		gonghui.m_userInfo = userInfo;
		m_mapId2Gonghui[gonghui.m_gonghuiId] = gonghui;
		row2 = mysql_fetch_row(res2);
	}

	mysql_free_result(res2);
}

std::vector<GonghuiUser>  UserManager::_getUserInfoByGonghuiId(Lint gonghuiId)
{
	std::vector<GonghuiUser> userInfo;
	LDBSession dbSession;
	if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return userInfo;
	}

	std::stringstream sql;
	sql << "SELECT a.userId, b.Nike FROM r_gonghui_user a, user b where a.userId = b.Id and a.gonghuiId = ";
	sql << gonghuiId;
	LLOG_ERROR("sql str=%s.", sql.str().c_str());
	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("Fail to query gonghui user info. Error = %s", mysql_error(dbSession.GetMysql()));
		return userInfo;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("Fail to store gonghui userinfo result. Error = %s", mysql_error(dbSession.GetMysql()));
		return userInfo;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("No data from user info db. Error = %s", mysql_error(dbSession.GetMysql()));
		return userInfo;
	}

	while (row)
	{
		GonghuiUser gonghuiUser;
		gonghuiUser.id = atoi(*row++);
		gonghuiUser.name = *row++;
		userInfo.push_back(gonghuiUser);
		row = mysql_fetch_row(res);
	}
	LLOG_ERROR("complete load gonghui user info, size=%d.", userInfo.size());
	mysql_free_result(res);
}

std::vector<PaiJuInfo> UserManager::_getPaiJuByGonghuiId(Lint gonghuiId)
{
	std::vector<PaiJuInfo> paijuList;
	LDBSession dbSession;
	if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return paijuList;
	}

	std::stringstream sql;
	sql << "SELECT roomId, roomCount, score, type, state, user1, user2, user3, user4 FROM room where gonghuiId = " ;
	sql << gonghuiId;

	if (mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("Fail to query paiJu. Error = %s", mysql_error(dbSession.GetMysql()));
		return paijuList;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if (res == NULL)
	{
		LLOG_ERROR("Fail to store paiJu result. Error = %s", mysql_error(dbSession.GetMysql()));
		return paijuList;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		LLOG_ERROR("No data from paiJu db. Error = %s", mysql_error(dbSession.GetMysql()));
		return paijuList;
	}

	while (row)
	{
		PaiJuInfo paiJu;
		paiJu.m_roomId = atoi(*row++);
		paiJu.m_roomCounts = atoi(*row++);
		paiJu.m_roomScore = atoi(*row++);
		paiJu.m_roomType = *row++;
		paiJu.m_roomState = *row++;
		paiJu.m_user1 = *row++;
		paiJu.m_user2 = *row++;
		paiJu.m_user3 = *row++;
		paiJu.m_user4 = *row++;

		paijuList.push_back(paiJu);
		row = mysql_fetch_row(res);
	}

	mysql_free_result(res);
}

bool UserManager::_loadUserInfoFromDB()
{
	LDBSession dbSession;
	if(!dbSession.Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	std::stringstream sql;
	sql << "SELECT COUNT(*) FROM user";

	if(mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("Fail to query. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if(res == NULL)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if(!row)
	{
		LLOG_ERROR("No data from db. Error  = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	int iUserCount = atoi(*row);
	mysql_free_result(res);

	LLOG_ERROR("%d users will be loaded", iUserCount);

	int iUserCountTmp = iUserCount;
	int iUserCountOnThread[LOADUSER_THREAD] = {0};
	int iNeedThread = iUserCountTmp / LOADUSER_COUNT;
	if(iNeedThread < LOADUSER_THREAD)
	{
		int iIndex = 0;
		while(iUserCountTmp > 0 && iIndex < LOADUSER_THREAD)
		{
			iUserCountOnThread[iIndex] = iUserCountTmp > LOADUSER_COUNT ? LOADUSER_COUNT : iUserCountTmp;
			++iIndex;
			iUserCountTmp = iUserCountTmp - LOADUSER_COUNT;
		}
	}
	else
	{
		int iAverageCount = iUserCountTmp / LOADUSER_THREAD;
		int iLastRemain = iUserCountTmp - (iAverageCount * LOADUSER_THREAD);
		for(int i = 0; i < LOADUSER_THREAD; ++i)
		{
			iUserCountOnThread[i] = iAverageCount;
		}
		iUserCountOnThread[LOADUSER_THREAD - 1] += iLastRemain;
	}

	//start to load user
	m_bLoadUserSuccess = true;
	boost::shared_ptr<boost::thread> loadThread[LOADUSER_THREAD];
	int iBeginIndex = 0;
	for(int i = 0; i < LOADUSER_THREAD; ++i)
	{
		if(iUserCountOnThread[i] == 0)
		{
			break;
		}

		loadThread[i] = boost::shared_ptr<boost::thread> (new boost::thread(boost::bind(&UserManager::_loadUserInfoFromDB,this, iBeginIndex, iUserCountOnThread[i])));
		iBeginIndex += iUserCountOnThread[i];
	}

	for(int i = 0; i < LOADUSER_THREAD; ++i)
	{
		if(loadThread[i])
		{
			loadThread[i]->join();
		}
	}
	
	if(!m_bLoadUserSuccess)
	{
		LLOG_ERROR("Fail to load user");
		return false;
	}

	LLOG_ERROR("Load users successfuly. User count %d:%d", iUserCount, m_mapUUID2User.size());

	return true;
}

void UserManager::_loadUserInfoFromDB(int iBeginIndex, int iCount)
{
	if(iBeginIndex < 0 || iCount <= 0)
	{
		LLOG_ERROR("The value is wrong. BeginIndex = %d, Count = %d", iBeginIndex, iCount);
		return;
	}

	LDBSession dbSession;
	if(!dbSession.Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session. BeginIndex = %d, Count = %d", iBeginIndex, iCount);
		m_bLoadUserSuccess = false;
		return;
	}

	LLOG_ERROR("Load user from %d. Count = %d", iBeginIndex, iCount);

	int iRemainUserCount = iCount;
	while(iRemainUserCount > 0)
	{
		int iLoadCount = iRemainUserCount > LOADUSER_COUNT ? LOADUSER_COUNT : iRemainUserCount;
		std::stringstream sql;
		sql << "select Id,OpenId,Nike,Sex,Provice,City,Country,HeadImageUrl,UnionId,NumsCard1,NumsCard2,NumsCard3,LastLoginTime,RegTime,New,Gm,TotalCardNum,TotalPlayNum,Coins,Credits,AgentPower,AgentId,status FROM user ORDER BY Id DESC LIMIT ";
		sql << iBeginIndex << "," << iLoadCount;

		LTime loadBegin;

		if(mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
		{
			LLOG_ERROR("UserManager::LoadUserIdInfo sql error %s", mysql_error(dbSession.GetMysql()));
			m_bLoadUserSuccess = false;
			break;
		}

		MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
		if(res == NULL)
		{
			LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
			m_bLoadUserSuccess = false;
			break;
		}

		MYSQL_ROW row = mysql_fetch_row(res);
		if(!row)
		{
			mysql_free_result(res);
			break;
		}

		LTime loadEnd;
		LLOG_ERROR("Spend %d seconds to load user from db(%d:%d)", loadEnd.Secs() - loadBegin.Secs(), iBeginIndex, iLoadCount);

		iRemainUserCount = iRemainUserCount - iLoadCount;

		//Start to fill user info
		int iRemainLoadCount = iLoadCount; 
		
		int iArraySize = iRemainLoadCount > LOADUSER_ALLOC_SIZE ? LOADUSER_ALLOC_SIZE : iRemainLoadCount;
		int iArrayIndex = 0;
		DUser* pArrayUser = new DUser[iArraySize];
		while(row && iRemainLoadCount > 0)
		{
			++iBeginIndex;

			DUser* user = &pArrayUser[iArrayIndex];

			user->m_usert.m_id = atoi(*row++);
			user->m_usert.m_openid = *row++;
			user->m_usert.m_nike = *row++;
			user->m_usert.m_sex = atoi(*row++);
			user->m_usert.m_provice = *row++;
			user->m_usert.m_city = *row++;
			user->m_usert.m_country = *row++;
			user->m_usert.m_headImageUrl = *row++;
			user->m_usert.m_unioid = *row++;
			user->m_usert.m_numOfCard1s = atoi(*row++);
			user->m_usert.m_numOfCard2s = atoi(*row++);
			user->m_usert.m_numOfCard3s = atoi(*row++);
			user->m_usert.m_lastLoginTime = atoi(*row++);
			user->m_usert.m_regTime = atoi(*row++);
			user->m_usert.m_new = atoi(*row++);
			user->m_usert.m_gm = atoi(*row++);
			user->m_usert.m_totalbuynum = atoi(*row++);
			user->m_usert.m_totalplaynum = atoi(*row++);
			user->m_usert.m_coins = atoi(*row++);
			user->m_usert.m_creditValue = atoi(*row++);
			user->m_usert.m_agentPower = atoi(*row++);
			user->m_usert.m_agentId = atoi(*row++);
			user->m_usert.m_status = atoi(*row++);

			++iArrayIndex;

			if(iArrayIndex == iArraySize)
			{
				boost::mutex::scoped_lock l(m_mutexQueue);
				for(int i = 0; i < iArrayIndex; ++i)
				{
					boost::shared_ptr<DUser> ptrUser(&pArrayUser[i]);
					m_mapUUID2User[pArrayUser[i].m_usert.m_unioid] = ptrUser;
					m_mapId2User[pArrayUser[i].m_usert.m_id] = ptrUser;
				}

				iRemainLoadCount = iRemainLoadCount - iArraySize;
				iArrayIndex = 0;

				if(iRemainLoadCount > 0)
				{
					iArraySize = iRemainLoadCount > LOADUSER_ALLOC_SIZE ? LOADUSER_ALLOC_SIZE : iRemainLoadCount;
					pArrayUser = new DUser[iArraySize];
				}
			}

			row = mysql_fetch_row(res);
		}

		mysql_free_result(res);

		for(int i = 0; i < iArrayIndex; ++i)
		{
			boost::shared_ptr<DUser> ptrUser(&pArrayUser[i]);
			m_mapUUID2User[pArrayUser[i].m_usert.m_unioid] = ptrUser;
			m_mapId2User[pArrayUser[i].m_usert.m_id] = ptrUser;
		}
	}
}

void UserManager::_addUser(boost::shared_ptr<DUser> user)
{
	auto itUser = m_mapId2User.find(user->m_usert.m_id);
	if(itUser == m_mapId2User.end())
	{
		m_mapId2User[user->m_usert.m_id] = user;
		m_mapUUID2User[user->m_usert.m_unioid] = user;
	}
}

bool UserManager::Final()
{
	return true;
}

void UserManager::SaveUser(DUser* user)
{
	//查询数据库
	std::stringstream ss;
	ss << "UPDATE user SET ";
	ss << "Nike='"<<  user->m_usert.m_nike <<"',";
	ss << "Sex='"<<  user->m_usert.m_sex <<"',";
	ss << "Provice='"<<  user->m_usert.m_provice <<"',";
	ss << "City='"<<  user->m_usert.m_city <<"',";
	ss << "Country='"<<  user->m_usert.m_country<<"',";
	ss << "HeadImageUrl='"<<  user->m_usert.m_headImageUrl<<"',";
	ss << "NumsCard1='"<<  user->m_usert.m_numOfCard1s<<"',";
	ss << "NumsCard2='"<<  user->m_usert.m_numOfCard2s<<"',";
	ss << "NumsCard3='"<<  user->m_usert.m_numOfCard3s<<"',";
	ss << "LastLoginTime='"<<  user->m_usert.m_lastLoginTime<<"',";
	ss << "TotalCardNum='" << user->m_usert.m_totalbuynum << "',";
	ss << "New='"<<  user->m_usert.m_new<< "',";
	ss << "TotalPlayNum='"<<  user->m_usert.m_totalplaynum<<"',";
	ss << "Coins='"<<  user->m_usert.m_coins<<"',";
	ss << "status='" << user->m_usert.m_status<<"',";
	ss << "Credits='" << user->m_usert.m_creditValue<<"'";
	ss << " WHERE UnionId='" << user->m_usert.m_unioid << "'";

	LLOG_DEBUG("UserManager::SaveUser sql =%s", ss.str().c_str());

	Lstring* sql = new Lstring(ss.str());
	gDbServerManager.Push(sql,user->m_usert.m_id);
}


void UserManager::SaveUserLastLogin(DUser* user)
{
	//查询数据库
	std::stringstream ss;
	ss << "UPDATE user SET ";
	ss << "Nike='"<<  user->m_usert.m_nike <<"',";
	ss << "Sex='"<<  user->m_usert.m_sex <<"',";
	ss << "Provice='"<<  user->m_usert.m_provice <<"',";
	ss << "City='"<<  user->m_usert.m_city <<"',";
	ss << "Country='"<<  user->m_usert.m_country<<"',";
	ss << "HeadImageUrl='"<<  user->m_usert.m_headImageUrl<<"',";
	ss << "NumsCard1='"<<  user->m_usert.m_numOfCard1s<<"',";
	ss << "NumsCard2='"<<  user->m_usert.m_numOfCard2s<<"',";
	ss << "NumsCard3='"<<  user->m_usert.m_numOfCard3s<<"',";
	ss << "LastLoginTime='"<<  user->m_usert.m_lastLoginTime<<"',";
	ss << "LastLoginIP='" << user->m_loginIP << "',";
	ss << "New='"<<  user->m_usert.m_new<< "',";
	ss << "TotalCardNum='" << user->m_usert.m_totalbuynum << "',";
	ss << "TotalPlayNum='"<<  user->m_usert.m_totalplaynum<<"',";
	ss << "Coins='"<<  user->m_usert.m_coins<<"',";
	ss << "status='" << user->m_usert.m_status<<"',";
	ss << "Credits='" << user->m_usert.m_creditValue << "'";
	ss << " WHERE UnionId='" << user->m_usert.m_unioid << "'";

	LLOG_DEBUG("UserManager::SaveUserLastLogin sql =%s", ss.str().c_str());

	Lstring* sql = new Lstring(ss.str());
	gDbServerManager.Push(sql,user->m_usert.m_id);
}

void UserManager::SaveCardInfo(DUser* user,Lint cardType, Lint count, Lint operType, const Lstring& admin, const Lstring& remark)
{
	if ( user == NULL )
	{
		return;
	}
	std::stringstream ss;
	ss << "INSERT INTO charge (Time,CardType,CardNum,Type,UserId,Admin,TotalCardNum,Remark) VALUES (";
	ss << "'" << gWork.GetCurTime().Secs() << "',";
	ss << "'" << cardType << "',";
	ss << "'" << count << "',";
	ss << "'" << operType << "',";
	ss << "'" << user->m_usert.m_id << "',";
	ss << "'" << admin << "',";
	ss << "'" << user->m_usert.m_numOfCard2s << "',";
	ss << "'" << remark << "')";

	Lstring* sql = new Lstring(ss.str());
	gDbServerManager.Push(sql,user->m_usert.m_id);
}

