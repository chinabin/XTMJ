#ifndef _GONGHUI_MANAGER_H
#define _GONGHUI_MANAGER_H

#include "LBase.h"
#include "LSingleton.h"
#include "LUser.h"
#include "LMsgS2S.h"

struct UserApplyInfo
{
	Lint gonghuiId;
	Lint userId;
	Lint opType;
	Lstring opStatus;

	UserApplyInfo()
	{
		gonghuiId = 0;
		userId = 0;
		opType = 0;
		opStatus = "";
	}
};

class GonghuiManager :public LSingleton<GonghuiManager>
{
public:
	virtual	bool Init();
	virtual	bool Final();

	bool _loadGonghuiInfoFromDB();
	std::vector<PaiJuInfo> _getPaiJuByGonghuiId(Lint gonghuiId);
	std::vector<GonghuiUser>  _getUserInfoByGonghuiId(Lint gonghuiId);
	bool _clearDirtyPaijuInfoFromDB();
	bool _loadGonghuiUserApply();
	std::vector<Gonghui> getGonghuiInfo();
	std::map<Lint, std::set<Lint>> getGonghuiApplyInfo();
	bool addGonghuiUser(Lint gonghuiId, Lint userId);
	Lint delGonghuiUser(Lint gonghuiId, Lint userId);
	bool updateGonghuiPolicy(Lint gonghuiId, Lstring gonghuiPolicy);
	bool insertGonghuiApply(Lint gonghuiId, Lint userId);
	bool updateGonghuiApply(Lint gonghuiId, Lint userId, Lstring opRet);

	Gonghui getGonghuiInfoById(Lint gonghuiId);

	bool isUserInGonghui(Lint gonghuiId, Lint userId);
	bool isUserInGonghuiApply(Lint gonghuiId, Lint userId);

	Lint gonghuiApplyOp(Lint gonghuiId, Lint userId, bool opResult);

	bool insertGonghuiDeskInfo(Lint deskId, Lint gonghuiId, Lint baseScoreType, Lint roomType, Lint playType, Lint deskState, Lstring user[4], Lstring score[4]);
	
	std::set<Lint> getUserByGonghuiId(Lint gonghuiId);
	std::set<Lint> getUserApplyInfoByGonghuiId(Lint gonghuiId);

private:
	std::map<Lint, Gonghui> m_gonghuiInfo; // 工会基本信息
	std::map<Lint, std::vector<GonghuiUser>> m_gonghuiUser;  // 工会的所有用户信息
	std::map<Lint, std::vector<PaiJuInfo>> m_gonghuiDesk;  // 工会房间信息
	std::map<Lint, std::set<Lint>> m_gonghuiApplyInfo;  // 申请加入工会信息
};

#define gGonghuiManager GonghuiManager::Instance()

#endif