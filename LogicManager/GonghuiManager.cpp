#include "GonghuiManager.h"
#include "LDBSession.h"
#include "Config.h"
#include "DbServer.h"
#include "DbServerManager.h"

bool GonghuiManager::Init()
{
	 // 去除环境上已保存的，无用的数据
	_clearDirtyPaijuInfoFromDB();
	// 加载工会信息，工会对应的用户信息
	_loadGonghuiInfoFromDB();
	// 加载用户申请加入工会的列表
	_loadGonghuiUserApply();

	return true;
}

bool GonghuiManager::Final()
{
	return true;
}

bool GonghuiManager::_loadGonghuiInfoFromDB()
{
	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	bool m_bLoadGonhuiSuccess = true;
	std::stringstream sql2;
	sql2 << "SELECT id, name, adminId, roomPolicy FROM gonghui where status != 'disable'";

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
		m_gonghuiUser[gonghui.m_gonghuiId] = userInfo;

		gonghui.m_userCount = userInfo.size();
		gonghui.m_userInfo = userInfo;
		m_gonghuiInfo[gonghui.m_gonghuiId] = gonghui;
		row2 = mysql_fetch_row(res2);
	}

	mysql_free_result(res2);
}

std::vector<GonghuiUser>  GonghuiManager::_getUserInfoByGonghuiId(Lint gonghuiId)
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

std::vector<PaiJuInfo> GonghuiManager::_getPaiJuByGonghuiId(Lint gonghuiId)
{
	std::vector<PaiJuInfo> paijuList;
	LDBSession dbSession;
	if (!dbSession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return paijuList;
	}

	std::stringstream sql;
	sql << "SELECT roomId, roomType, baseScoreType, playType, deskState, user1, user2, user3, user4 FROM room where gonghuiId = ";
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
		paiJu.m_roomType = atoi(*row++);
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

bool GonghuiManager::_clearDirtyPaijuInfoFromDB()
{
	std::stringstream ss;
	ss << "DELETE FROM room WHERE deskState in ('IDLE', 'PLAYING')";

	LLOG_DEBUG("GonghuiManager::_clearDirtyPaijuInfoFromDB sql =%s", ss.str().c_str());

	Lstring* sql = new Lstring(ss.str());
	gDbServerManager.Push(sql);
}

bool GonghuiManager::_loadAllUserFromDB()
{
	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	bool m_bLoadGonhuiSuccess = true;
	std::stringstream sql2;
	sql2 << "SELECT Id, Nike FROM user";

	if (mysql_real_query(dbSession2.GetMysql(), sql2.str().c_str(), sql2.str().size()))
	{
		LLOG_ERROR("GonghuiManager::_loadAllUserFromDB sql error %s", mysql_error(dbSession2.GetMysql()));
		return false;
	}

	MYSQL_RES* res2 = mysql_store_result(dbSession2.GetMysql());
	if (res2 == NULL)
	{
		LLOG_ERROR("Fail to store user result. Error = %s", mysql_error(dbSession2.GetMysql()));
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
		Lint userId = atoi(*row2++);
		Lstring userName = *row2++;

		m_allUserInfo[userId] = userName;
		row2 = mysql_fetch_row(res2);
	}

	mysql_free_result(res2);
}

Lstring GonghuiManager::getUserNameById(Lint userId)
{
	std::map<Lint, Lstring>::iterator iter = m_allUserInfo.find(userId);
	if (iter != m_allUserInfo.end())
	{
		return iter->second;
	}
	return "";
}

bool GonghuiManager::_loadGonghuiUserApply()
{
	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	bool m_bLoadGonhuiSuccess = true;
	std::stringstream sql2;
	sql2 << "SELECT gonghuiId, userId FROM r_gonghui_apply where opStatus = 'apply'";

	if (mysql_real_query(dbSession2.GetMysql(), sql2.str().c_str(), sql2.str().size()))
	{
		LLOG_ERROR("GonghuiManager::_loadGonghuiUserApply sql error %s", mysql_error(dbSession2.GetMysql()));
		return false;
	}

	MYSQL_RES* res2 = mysql_store_result(dbSession2.GetMysql());
	if (res2 == NULL)
	{
		LLOG_ERROR("Fail to store gonghui apply result. Error = %s", mysql_error(dbSession2.GetMysql()));
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
		Lint gonghuiId = atoi(*row2++);
		Lint userId = atoi(*row2++);
		LLOG_ERROR("gonghui apply info: gonghuiId=%d, userId=%d.", gonghuiId, userId);
		
		m_gonghuiApplyInfo[gonghuiId].insert(userId);
		row2 = mysql_fetch_row(res2);
	}

	mysql_free_result(res2);
}

Lint GonghuiManager::getUserCardsById(Lint userId)
{
	Lint userCards = 0;

	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return userCards;
	}

	bool m_bLoadGonhuiSuccess = true;
	std::stringstream sql2;
	sql2 << "SELECT NumsCard2 FROM user where id = " << userId;
	LLOG_ERROR("sqlstr=%s.", sql2.str().c_str());

	if (mysql_real_query(dbSession2.GetMysql(), sql2.str().c_str(), sql2.str().size()))
	{
		LLOG_ERROR("GonghuiManager::_loadGonghuiUserApply sql error %s", mysql_error(dbSession2.GetMysql()));
		return userCards;
	}

	MYSQL_RES* res2 = mysql_store_result(dbSession2.GetMysql());
	if (res2 == NULL)
	{
		LLOG_ERROR("Fail to store user card result. Error = %s", mysql_error(dbSession2.GetMysql()));
		return userCards;
	}

	MYSQL_ROW row2 = mysql_fetch_row(res2);
	if (!row2)
	{
		mysql_free_result(res2);
		return userCards;
	}

	while (row2)
	{
		userCards = atoi(*row2++);
		row2 = mysql_fetch_row(res2);
	}

	mysql_free_result(res2);
	return userCards;
}

std::map<Lint, std::set<Lint>> GonghuiManager::getGonghuiApplyInfo()
{
	return m_gonghuiApplyInfo;
}

std::vector<Gonghui> GonghuiManager::getGonghuiInfo()
{
	std::vector<Gonghui> x;
	std::map<Lint, Gonghui>::iterator iter = m_gonghuiInfo.begin();
	for (; iter != m_gonghuiInfo.end(); iter++)
	{
		x.push_back(iter->second);
	}
	return x;
}

bool GonghuiManager::addGonghuiUser(Lint gonghuiId, Lint userId)
{
	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	bool m_bLoadGonhuiSuccess = true;
	std::stringstream sql2;
	sql2 << "INSERT INTO r_gonghui_user(gonghuiId, userId) values(" << gonghuiId << ", " << userId << ")";
	LLOG_ERROR("addGonghuiUser sqlStr=%s", sql2.str().c_str());
	if (mysql_real_query(dbSession2.GetMysql(), sql2.str().c_str(), sql2.str().size()))
	{
		LLOG_ERROR("GonghuiManager::addGonghuiUser sql error %s", mysql_error(dbSession2.GetMysql()));
		return false;
	}

	return true;
}

Lint GonghuiManager::delGonghuiUser(Lint gonghuiId, Lint userId)
{
	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	bool m_bLoadGonhuiSuccess = true;
	std::stringstream sql2;
	sql2 << "DELETE FROM r_gonghui_user where gonghuiId =" << gonghuiId << " and userId = " << userId;

	LLOG_ERROR("delGonghuiUser sqlStr=%s", sql2.str().c_str());
	if (mysql_real_query(dbSession2.GetMysql(), sql2.str().c_str(), sql2.str().size()))
	{
		LLOG_ERROR("GonghuiManager::delGonghuiUser sql error %s", mysql_error(dbSession2.GetMysql()));
		return false;
	}

	return true;
}

bool GonghuiManager::updateGonghuiPolicy(Lint gonghuiId, Lstring gonghuiPolicy)
{
	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	bool m_bLoadGonhuiSuccess = true;
	std::stringstream sql2;
	sql2 << "UPDATE gonghui SET roomPolicy = '" << gonghuiPolicy << "' where id = " << gonghuiId;
	LLOG_ERROR("updateGonghuiPolicy sqlStr=%s", sql2.str().c_str());
	
	if (mysql_real_query(dbSession2.GetMysql(), sql2.str().c_str(), sql2.str().size()))
	{
		LLOG_ERROR("GonghuiManager::updateGonghuiPolicy sql error %s", mysql_error(dbSession2.GetMysql()));
		return false;
	}

	return true;
}

// 申请加入工会，在调用此方法之前需要判断用户是否已经在工会
bool GonghuiManager::insertGonghuiApply(Lint gonghuiId, Lint userId)
{
	m_gonghuiApplyInfo[gonghuiId].insert(userId);

	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	bool m_bLoadGonhuiSuccess = true;
	std::stringstream sql2;
	sql2 << "INSERT INTO r_gonghui_apply(gonghuiId, userId, opType, opStatus) VALUES(" << gonghuiId << ", " << userId << ", 1, 'Apply')";
	LLOG_ERROR("insertGonghuiApply sqlStr=%s", sql2.str().c_str());

	if (mysql_real_query(dbSession2.GetMysql(), sql2.str().c_str(), sql2.str().size()))
	{
		LLOG_ERROR("GonghuiManager::insertGonghuiApply sql error %s", mysql_error(dbSession2.GetMysql()));
		return false;
	}
	return true;
}

bool GonghuiManager::updateGonghuiApply(Lint gonghuiId, Lint userId, Lstring opRet)
{
	m_gonghuiApplyInfo[gonghuiId].erase(userId);

	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	bool m_bLoadGonhuiSuccess = true;
	std::stringstream sql2;
	sql2 << "UPDATE r_gonghui_apply SET opStatus = '" << opRet << "' where opType = 1 and gonghuiId = " << gonghuiId << " and userId = " << userId;
	LLOG_ERROR("updateGonghuiApply sqlStr=%s", sql2.str().c_str());

	if (mysql_real_query(dbSession2.GetMysql(), sql2.str().c_str(), sql2.str().size()))
	{
		LLOG_ERROR("GonghuiManager::updateGonghuiApply sql error %s", mysql_error(dbSession2.GetMysql()));
		return false;
	}
	return true;
}

Gonghui GonghuiManager::getGonghuiInfoById(Lint gonghuiId)
{
	Gonghui gonghui;
	std::map<Lint, Gonghui>::iterator iter = m_gonghuiInfo.find(gonghuiId);
	if (iter != m_gonghuiInfo.end())
	{
		gonghui = iter->second;
	}
	return gonghui;
}

bool GonghuiManager::isUserInGonghui(Lint gonghuiId, Lint userId)
{
	std::vector<GonghuiUser> userList = m_gonghuiUser[gonghuiId];
	for (GonghuiUser user : userList)
	{
		if (user.id == userId)
		{
			return true;
		}
	}
	return false;
}

bool GonghuiManager::isUserInGonghuiApply(Lint gonghuiId, Lint userId)
{
	std::set<Lint> userIdList = m_gonghuiApplyInfo[gonghuiId];
	for (Lint id : userIdList)
	{
		if (id == userId)
		{
			return true;
		}
	}
	return false;
}

Lint GonghuiManager::gonghuiApplyOp(Lint gonghuiId, Lint userId, bool opResult)
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

bool GonghuiManager::decreaseCardCount(Lint userId, Lint isAdd, Lint count)
{
	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	std::stringstream ss;
	if (isAdd)
	{
		ss << "UPDATE user SET NumsCard2 = NumsCard2 + " << count << " WHERE id = " << userId;
	}
	else
	{
		ss << "UPDATE user SET NumsCard2 = NumsCard2 - " << count << " WHERE id = " << userId;
	}
	LLOG_ERROR("decreaseCardCount sqlStr=%s", ss.str().c_str());

	if (mysql_real_query(dbSession2.GetMysql(), ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("GonghuiManager::decreaseCardCount sql error %s", mysql_error(dbSession2.GetMysql()));
		return false;
	}
	return true;
}

bool GonghuiManager::SaveCardInfo(Lint userId, Lint cardType, Lint count, Lint operType, const Lstring& admin, Lint userCards, const Lstring& remark)
{
	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	LTime cur;
	std::stringstream ss;
	ss << "INSERT INTO charge (Time,CardType,CardNum,Type,UserId,Admin,TotalCardNum,Remark) VALUES (";
	ss << "'" << cur.Secs() << "',";
	ss << "'" << cardType << "',";
	ss << "'" << count << "',";
	ss << "'" << operType << "',";
	ss << "'" << userId << "',";
	ss << "'" << admin << "',";
	ss << "'" << userCards << "',";
	ss << "'" << remark << "')";

	LLOG_ERROR("SaveCardInfo sqlStr=%s", ss.str().c_str());

	if (mysql_real_query(dbSession2.GetMysql(), ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("GonghuiManager::decreaseCardCount sql error %s", mysql_error(dbSession2.GetMysql()));
		return false;
	}
	return true;
}

std::vector<RoomRecord> GonghuiManager::getGonghuiDeskRecords(Lint gonghuiId, Lint beginPos, Lint endPos)
{
	_loadAllUserFromDB();

	std::vector<RoomRecord> records;

	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return records;
	}

	bool m_bLoadGonhuiSuccess = true;
	std::stringstream sql2;
	sql2 << "SELECT roomId,gonghuiId,baseScore,cardType,playType,playNums,deskState,user1,user2,user3,user4,score1,score2,score3,score4,lastModify FROM room where gonghuiId = ";
	sql2 << gonghuiId;
	sql2 << " order by lastModify desc limit " << beginPos << " ," << endPos;
	LLOG_ERROR("sqlstr=%s.", sql2.str().c_str());

	if (mysql_real_query(dbSession2.GetMysql(), sql2.str().c_str(), sql2.str().size()))
	{
		LLOG_ERROR("GonghuiManager::getGonghuiDeskRecords sql error %s", mysql_error(dbSession2.GetMysql()));
		return records;
	}

	MYSQL_RES* res2 = mysql_store_result(dbSession2.GetMysql());
	if (res2 == NULL)
	{
		LLOG_ERROR("Fail to getGonghuiDeskRecords result. Error = %s", mysql_error(dbSession2.GetMysql()));
		return records;
	}

	MYSQL_ROW row2 = mysql_fetch_row(res2);
	if (!row2)
	{
		mysql_free_result(res2);
		return records;
	}

	while (row2)
	{
		RoomRecord roomRecord;
		roomRecord.m_roomId = atoi(*row2++);
		roomRecord.m_gonghuiId = atoi(*row2++);
		roomRecord.m_baseScore = atoi(*row2++);
		roomRecord.m_cardType = atoi(*row2++);
		roomRecord.m_playType = atoi(*row2++);
		roomRecord.m_playNums = atoi(*row2++);
		roomRecord.m_roomState = *row2++;
		Lint userId = atoi(*row2++);
		roomRecord.m_userId[0] = userId;
		roomRecord.m_userName[0] = getUserNameById(userId);
		userId = atoi(*row2++);
		roomRecord.m_userId[1] = userId;
		roomRecord.m_userName[1] = getUserNameById(userId);
		userId = atoi(*row2++);
		roomRecord.m_userId[2] = userId;
		roomRecord.m_userName[2] = getUserNameById(userId);
		userId = atoi(*row2++);
		roomRecord.m_userId[3] = userId;
		roomRecord.m_userName[3] = getUserNameById(userId);
		roomRecord.m_score[0] = atoi(*row2++);
		roomRecord.m_score[1] = atoi(*row2++);
		roomRecord.m_score[2] = atoi(*row2++);
		roomRecord.m_score[3] = atoi(*row2++);
		roomRecord.m_playTime = atoi(*row2++);
		records.push_back(roomRecord);
		row2 = mysql_fetch_row(res2);
	}

	mysql_free_result(res2);

	return records;
}

bool GonghuiManager::addGonghuiDeskInfo(Lint deskId, Lint gonghuiId, Lint baseScoreType, Lint roomType, Lint playType, Lint playNums, Lstring deskState, Lint user[4], Lint score[4])
{
	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	LTime cur;
	std::stringstream ss;
	ss << "INSERT INTO room (roomId,gonghuiId,baseScore,cardType,playType,playNums,deskState,user1,user2,user3,user4,score1,score2,score3,score4,lastModify) VALUES (";
	ss << "'" << deskId << "',";
	ss << "'" << gonghuiId << "',";
	ss << "'" << baseScoreType << "',";
	ss << "'" << roomType << "',";
	ss << "'" << playType << "',";
	ss << "'" << playNums << "',";
	ss << "'" << deskState << "',";
	ss << "'" << user[0] << "',";
	ss << "'" << user[1] << "',";
	ss << "'" << user[2] << "',";
	ss << "'" << user[3] << "',";
	ss << "'" << score[0] << "',";
	ss << "'" << score[1] << "',";
	ss << "'" << score[2] << "',";
	ss << "'" << score[3] << "',";
	ss << "'" << cur.Secs() << "')";

	LLOG_ERROR("addGonghuiDeskInfo sqlStr=%s", ss.str().c_str());

	if (mysql_real_query(dbSession2.GetMysql(), ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("GonghuiManager::addGonghuiDeskInfo sql error %s", mysql_error(dbSession2.GetMysql()));
		return false;
	}
	return true;
}

GonghuiUser GonghuiManager::getGonghuiUserById(Lint userId)
{
	GonghuiUser tmpUser;
	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return tmpUser;
	}

	bool m_bLoadGonhuiSuccess = true;
	std::stringstream sql2;
	sql2 << "select a.userId, b.Nike from r_gonghui_user a, user b where a.userId = b.Id and a.userId = ";
	sql2 << userId;
	LLOG_ERROR("sqlstr=%s.", sql2.str().c_str());

	if (mysql_real_query(dbSession2.GetMysql(), sql2.str().c_str(), sql2.str().size()))
	{
		LLOG_ERROR("GonghuiManager::getGonghuiUserById sql error %s", mysql_error(dbSession2.GetMysql()));
		return tmpUser;
	}

	MYSQL_RES* res2 = mysql_store_result(dbSession2.GetMysql());
	if (res2 == NULL)
	{
		LLOG_ERROR("Fail to getGonghuiUserById result. Error = %s", mysql_error(dbSession2.GetMysql()));
		return tmpUser;
	}

	MYSQL_ROW row2 = mysql_fetch_row(res2);
	if (!row2)
	{
		mysql_free_result(res2);
		return tmpUser;
	}

	while (row2)
	{
		tmpUser.id = atoi(*row2++);
		tmpUser.name = *row2++;
		row2 = mysql_fetch_row(res2);
	}

	mysql_free_result(res2);

	return tmpUser;
}

bool GonghuiManager::deleteGonghui(Lint gonghuiId)
{
	m_gonghuiInfo.erase(gonghuiId);
	m_gonghuiApplyInfo.erase(gonghuiId);

	LDBSession dbSession2;
	if (!dbSession2.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("Fail to init db session.");
		return false;
	}

	bool m_bLoadGonhuiSuccess = true;
	std::stringstream sql2;
	sql2 << "UPDATE gonghui SET status = 'disable' where gonghuiId = " << gonghuiId;
	LLOG_ERROR("deleteGonghui sqlStr=%s", sql2.str().c_str());

	if (mysql_real_query(dbSession2.GetMysql(), sql2.str().c_str(), sql2.str().size()))
	{
		LLOG_ERROR("GonghuiManager::deleteGonghui sql error %s", mysql_error(dbSession2.GetMysql()));
		return false;
	}
	return true;
}