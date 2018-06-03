#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "LActive.h"

class Desk;

class  User
{
public:
	User(LUser data, Lint gateId);
	virtual ~User();

	//获取玩家数据库里 id
	Lint	GetUserDataId();

	const LUser& GetUserData() const { return m_userData; };

	void	SetUserGateId(Lint gateId);
	Lint	GetUserGateId();

	bool	GetOnline();
	void	SendLoginInfo(Lstring& buyInfo, Lint hide);
	void	SendItemInfo();

	bool	Login();
	void	Logout();

	void	Send(LMsg& msg);

	Desk*	GetDesk();
	void	SetDesk(Desk* desk);

	void	HanderMsg(LMsg* msg);
	//玩家创建房间
	Lint	HanderUserCreateDesk(LMsgLMG2LCreateDesk* msg);

	//玩家进入房间
	Lint	HanderUserAddDesk(Lint nDeskID);

	//玩家退出房间
	void	HanderUserLeaveDesk(LMsgC2SLeaveDesk* msg);

	//玩家请求出牌
	void	HanderUserPlayCard(LMsgC2SUserPlay* msg);

	//玩家请求思考
	void	HanderUserThink(LMsgC2SUserOper* msg);

	//玩家选择起手胡
	void	HanderStartHuSelect(LMsgC2SUserStartHuSelect* msg);

	//玩家选择海底牌
	void	HanderEndCardSelect(LMsgC2SUserEndCardSelect* msg);

	//玩家请求通知内容
	void	HanderUserReady(LMsgC2SUserReady* msg);

	//申请解算房间
	void	HanderResutDesk(LMsgC2SResetDesk* msg);

	//玩家选择解算操作
	void	HanderSelectResutDesk(LMsgC2SSelectResetDesk* msg);

	//玩家请求聊天
	void	HanderUserSpeak(LMsgC2SUserSpeak* msg);
	void	HanderUserLocation(LMsgC2SLocation* msg);

	// 玩家定缺
	void HanderUserDingQue(LMsgC2SUserDingQue* msg);

	// 玩家换牌
	void HanderUserChange(LMsgC2SBottomPour* msg);

	void HanderUserTangReq(LMsgC2STangCard* msg);

	void HanderUserAIOper(LMsgC2SUserAIOper* msg);

	void HanderUserShare(LMsgC2SShare* msg);

	Lstring GetIp();

	void	SetIp(Lstring& ip);

	//增加房卡
	void	AddCardCount(Lint cardType, Lint count,Lint operType,Lstring admin, bool bNeedSave = true);

	static void    AddCardCount(Lint id, Lstring strUUID, Lint cardType, Lint count, Lint operType,Lstring admin);
	//删除房卡
	void	DelCardCount(Lint cardType, Lint count, Lint operType, Lstring admin);

	//改变玩家状态
	void	ModifyUserState(bool bLeaveServer = false);
	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue){m_userState = nValue;}
	Lint	getUserGateID(){return m_gateId;}
	void	setUserGateID(Lint nValue){m_gateId = nValue;}
public:
	LUser	m_userData;
	Lint	m_gateId;		//玩家在哪个gate上面
	Lint	m_userState;	//玩家当前状态
	bool	m_online;
	std::vector< LActive >	m_Active;
	Desk*	m_desk;

	Lstring	m_ip;
	Lstring	m_location;
};

#endif
