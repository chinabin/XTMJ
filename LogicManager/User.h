#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "LActive.h"

#include "SafeResourceLock.h"

class User : public CResourceLock
{
public:
	User(LUser data, Lint gateId);
	virtual ~User();

	virtual void	Tick();

	//获取玩家数据库里id
	Lint	GetUserDataId();

	void	SetUserGateId(Lint gateId);
	Lint	GetUserGateId();

	bool	GetOnline();

	void	Login();
	void	Logout();

	void	Send(LMsg& msg);
	void    SendToUser(LMsg& msg, Lint userId);
	void	Send(const LBuffPtr& buff);

	void    SendGonghuiChange(Lint gonghuiId);

	void	SendLoginInfo(Lint lastLoginTime);
	void	SendItemInfo();
	void	SendHorseInfo();

	void    SendFreeInfo();
	void    SendExchInfo();

	void	S2CeUserLogin();
	void	S2CeUserLogout();

	void	HanderMsg(LMsg* msg);

	void    HanderGetUserGonghuiInfo(LMsgC2SGonghuiInfo* msg);
	void    HanderGetGonghuiApplyInfo(LMsgC2SQueryApplyInfo* msg);
	void    HanderUserGonghuiApply(LMsgC2SGonghuiApply* msg);
	void    HanderUserGonghuiOp(LMsgC2SGonghuiUserOp* msg);
	void    HanderUserCreateGonghuiRoom(LMsgC2SGonghuiRoomOP* msg);
	void    HanderUserQueryGonghuiDesk(LMsgC2SQueryGonghuiDesk* msg);
	void    HanderGetDeskMatchInfo(LMsgC2SQueryDeskHistory* msg);

	//玩家创建房间
	void	HanderUserCreateDesk(LMsgC2SCreateDesk* msg);

	//玩家进入房间
	void	HanderUserAddDesk(LMsgC2SAddDesk* msg);

	//进步金币桌子
	void	HanderUserEnterCoinsDesk(LMsgC2SEnterCoinsDesk* msg);

	//玩家请求战绩
	void	HanderUserVipLog(LMsgC2SVipLog* msg);

	void    HanderUserCreLog(LMsgC2SCRELog* msg);

	void    HanderUserReqGTU(LMsgC2SREQ_GTU* msg);

	void    HanderUserReqEXCH(LMsgC2SREQ_Exch* msg);

	void	HanderUserRoomLog(LMsgC2SRoomLog* msg);

	//玩家请求录像
	void	HanderUserVideo(LMsgC2SVideo* msg);

	//获取指定玩家信息
	void	HanderGetUserInfo(LMsgC2SGetUserInfo* msg);

	void    HanderBindInviter(LMsgC2SBindInviter* msg);

	void    HanderGetInvitingInfo(LMsgC2SGetInvitingInfo *msg);

	// 活动相关
	void    HanderActivityPhone(LMsgC2SActivityPhone *msg);
	void    HanderActivityRequestLog(LMsgC2SActivityRequestLog *msg);
	void    HanderActivityRequestDrawOpen(LMsgC2SActivityRequestDrawOpen *msg);
	void    HanderActivityRequestDraw(LMsgC2SActivityRequestDraw *msg);
	void    HanderActivityRequestShare(LMsgC2SActivityRequestShare *msg);

	// 排行榜
	void	HanderUpdateRank(LMsgC2SUpdateRank* msg);
	void	HanderGetRankReward(LMsgC2SGetRankReward *msg);

	void	HanderUserShare(LMsgC2SShare* msg);

	void	HanderUserTaskInfo(LMsgC2STaskInfo* msg);
	void	HanderUserTaskReward(LMsgC2STaskReward* msg);

	Lstring GetIp();

	void	SetIp(Lstring& ip);
	
	//房卡相关操作
	//判断房卡数量是否足够 create 是否是创建房间
	bool	IfCardEnough(const Lint cardType, const bool create);
	//增加房卡
	void	AddCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin, bool bNeedSave = true);
	//删除房卡
	void	DelCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin, bool bNeedSave = true);
	//修改玩家打的局数，更改数据
	void	AddPlayCount();

	//static void	AddCardCount(Lint id, Lstring strUUID, Lint cardType, Lint count, Lint operType, const Lstring& admin);
	void    AddCreditForUserlist(const std::vector<Lint> &userList);

	bool    DealExchangeCard(Lint inCard);

	void	AddCoinsCount(Lint count, Lint operType, bool bNeedSave = true);
	void	DelCoinsCount(Lint count, Lint operType, bool bNeedSave = true);

	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue){m_userState = nValue;}
	Lint	getUserLogicID(){return m_logicID;}
	void	setUserLogicID(Lint nValue){m_logicID = nValue;}
	Lint	getUserGateID(){return m_gateId;}
	void	setUserGateID(Lint nValue){m_gateId = nValue;}
	Lstring&	getUserIPStr(){return m_ip;}

	Lint	getLastCreateGameType() { return m_lastCreateGameType; }
public:
	LUser	m_userData;					//玩家数据
	std::vector< LActive >	m_Active;	//活动信息
	Lint	m_gateId;					//玩家在哪个gate上面
	Lint	m_logicID;					//玩家在哪个logic上面
	Lint	m_userState;				//玩家当前状态
	bool	m_online;
	
	Lstring	m_ip;

	Lint m_lastCreateGameType;			//玩家最近一次创建的游戏类型
};

class UserBaseInfo : public CResourceLock
{
public:
	UserBaseInfo()
	{
		m_id  = 0;
		m_sex = 1;
		m_agentPower = 0;
	}
	UserBaseInfo(const UserIdInfo& info)
	{
		copy(info);
	}

	virtual ~UserBaseInfo()
	{

	}
public:
	UserBaseInfo& operator =(const UserIdInfo& info)
	{
		copy(info);
		return *this;
	}

	void copy(const UserIdInfo& info)
	{
		m_id			= info.m_id;
		m_openId		= info.m_openId;
		m_nike			= info.m_nike;
		m_headImageUrl	= info.m_headImageUrl;
		m_sex			= info.m_sex;
		m_agentPower	= info.m_agentPower;
	}
public:
	Lint		m_id;
	Lstring		m_openId;
	Lstring		m_nike;
	Lstring		m_headImageUrl;
	Lint		m_sex;
	Lint		m_agentPower;		//代理权
};

class UserLoginInfo : public CResourceLock
{
public:
	UserLoginInfo()
	{
		m_seed = 0;
		m_time = 0;
	}
	UserLoginInfo(Lint iSeed, Lint iTime, LUser& user)
	{
		m_seed = iSeed;
		m_time = iTime;
		m_user = user;
	}

	virtual ~UserLoginInfo()
	{

	}

public:
	Lint	m_seed;
	Lint	m_time;
	LUser	m_user;
};

#endif
