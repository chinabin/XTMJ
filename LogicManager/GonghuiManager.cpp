#include "GonghuiManager.h"
#include "LDBSession.h"
#include "Config.h"
#include "DbServer.h"
#include "DbServerManager.h"

bool GonghuiManager::Init()
{
	 // ȥ���������ѱ���ģ����õ�����
	_clearDirtyPaijuInfoFromDB();
	// ���ع�����Ϣ�������Ӧ���û���Ϣ
	_loadGonghuiInfoFromDB();
	// �����û�������빤����б�
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

// ������빤�ᣬ�ڵ��ô˷���֮ǰ��Ҫ�ж��û��Ƿ��Ѿ��ڹ���
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

	// ������ͬ�⻹�ǲ�ͬ�⣬��ɾ���������¼�е�����
	applyUserList.erase(setIter);
	iter->second = applyUserList;

	// TODO ���Ӷ�Ӧ�����ݿ����
	if (opResult)
	{
		// �û����빤��
		return addGonghuiUser(gonghuiId, userId);
	}
	else
	{
		// ��ͬ���û����빤�ᣬ��Ҫ����ɾ�����ݿ��е������¼
	}
	return 0;
}