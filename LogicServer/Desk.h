#ifndef _DESK_H_
#define _DESK_H_

#include "User.h"
#include "Card.h"
#include "AI.h"
#include "LTime.h"
#include "LVideo.h"
#include "GameHandler.h"
#include "GameDefine.h"

class Room;
struct VipLogItem;

//#define DEBUG_MODEL

struct ThinkTool
{
	ThinkVec	m_thinkData;
	TingVec		m_tingData;

	void Reset()
	{
		m_thinkData.clear();
		m_tingData.clear();
	}

	ThinkTool()
	{
		Reset();
	}

	bool HasHu()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_BOMB)
				return true;
		}
		return false;
	}

	bool HasChi()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_CHI)
				return true;
		}
		return false;
	}

	bool HasPeng()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_PENG)
				return true;
		}
		return false;
	}

	bool HasMBu()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_MBU)
				return true;
		}
		return false;
	}

	bool HasMGang()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_MGANG)
				return true;
		}
		return false;
	}

	bool HasAnGang()
	{
		for (Lsize i = 0; i < m_thinkData.size(); ++i)
		{
			if (m_thinkData[i].m_type == THINK_OPERATOR_AGANG)
				return true;
		}
		return false;
	}
	

	bool NeedThink()
	{
		return !m_thinkData.empty();
	}
};

enum DeskType
{
	DeskType_Common,   //普通场
	DeskType_Coins,    //金币场
};

class Desk
{
public:
	Desk();
	~Desk();

	bool			initDesk(int iDeskId, GameType gameType, Lint cardType);

	Lint			GetDeskId();
	Lint			GetPlayerCapacity();
	Lint            SetPlayerCapacity(Lint playerCapacity);

	void			SetVip(VipLogItem* vip);
	VipLogItem*		GetVip();

	void			Tick(LTime& curr);

	void			HanderUserReady(User* pUser,LMsgC2SUserReady* msg);
	void			HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg);
	void			HanderUserStartHu(User* pUser,LMsgC2SUserStartHuSelect* msg);

	void            HanderUserTangReq(User* pUser,LMsgC2STangCard* msg);

	void			HanderUserEndSelect(User* pUser,LMsgC2SUserEndCardSelect* msg);		//玩家处理海底牌
	void			SyncMsgUserEndSelect(Lint nPos, Lint state, Lint color, Lint number);

	void			HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg);
	void			HanderUserSpeak(User* pUser, LMsgC2SUserSpeak* msg);
	void			HanderUserLocation(User* pUser, LMsgC2SLocation* msg);

	void			HanderUserOutDesk(User* pUser);

	void			HanderResutDesk(User* pUser,LMsgC2SResetDesk* msg);

	void			HanderSelectResutDesk(User* pUser,LMsgC2SSelectResetDesk* msg);

	// 玩家定缺
	void			HanderUserDingQue(User* pUser, LMsgC2SUserDingQue* msg);
	
	// 玩家下跑
	void			HanderUserChange(User* pUser, LMsgC2SBottomPour* msg);

	void			HanderGameOver(Lint result);

	void			HanderAddCardCount(Lint pos, Lint CardNum, CARDS_OPER_TYPE AddType, Lstring admin);

	//////////////////////////////////////////////////////////////////////////
	Lint			getGameType() const;
	const std::vector<Lint>&	getPlayType() const;
	Lint			getDeskState() const;
	void			setDeskState(Lint nState);
	Lint			getDeskPlayState();
	void			setDeskPlayState(Lint nDeskPlayState);
	//////////////////////////////////////////////////////////////////////////


	void			ResetEnd();
	bool			OnUserReconnect(User* pUser);

	void			OnUserInRoom(User* user);
	void			OnUserOutRoom(User* user);

	void			OnUserInRoom(User* user[]);

	Lint			GetFreePos();
	Lint			GetUserPos(User* user);
	User*			GetPosUser(Lint pos);

	Lint			GetNextPos(Lint prev);
	Lint			GetPrevPos(Lint next);

	Lint			GetUserCount();


	void			CheckGameStart();

	void			SetDeskWait();
	void			OnDeskThingIng();//等待玩家思考中
	void			OnDeskWait();
	void			SetAllReady();


	void			SetDeskFree();

	void			CheckReset();
	void			ClearUser();
	void			BoadCast(LMsg& msg);
	void			BoadCastWithOutUser(LMsg& msg,User* user);
	void			BroadcastWithoutUser(LMsg& msg, Lint pos);

	void			ResetClear();

	void		    SpecialCardClear();   //初始化 特定牌标志

	// 设置倒计时
	void			SetAutoPlay( Lint pos, bool bauto, Lint outtime );
	void            CheckAutoPlayCard();

	void            SetDeskType(DeskType type);
	DeskType        GetDeskType();

	// Add HuNanZhuanZhuan by hanlu 02242017 Begin
	void UpdateLastOptTimeAndPos(Lint optpos);
	void UpdateLastOptTimeAndPos(User* user);
	// Add HuNanZhuanZhuan by hanlu 02242017 End

protected:
	void			_clearData();
	bool			_createRegister(GameType gameType);
private:
	Lint			m_deskState;					//桌子状态
	Lint			m_deskbeforeState;				//解散房间前桌子状态
	Lint			m_deskPlayState;				//打牌状态
	Lint			m_state;							//房间规则
	std::vector<Lint>	m_playtype;					//玩法规则
	Lstring			m_resetUser;					//申请的玩家
	Lint			m_resetTime;					//申请解算的时间
	Lint			m_timeDeskCreate;				//桌子创建的时间
public:
	User*			m_user[DESK_USER_COUNT];		//玩家列表
	Lint			m_readyState[DESK_USER_COUNT];	//玩家准备状态 
	GameHandler*	mGameHandler;					//具体游戏处理器
	VipLogItem*		m_vip;							//录相
	Card            m_specialCard[CARD_COUNT];		//存储玩家定义牌序
	Lint			m_reset[DESK_USER_COUNT];		//玩家装态重置

	// 倒计时运行时数据
	bool			m_autoPlay[DESK_USER_COUNT];	//是否检查自动出牌
	Lint			m_autoPlayTime[DESK_USER_COUNT];//自动出牌的倒计时
	Lint			m_autoOutTime[DESK_USER_COUNT]; //倒计时的超时时间

	// 配置
	Lint			m_autoChangeOutTime;			//自动换牌的时间
	Lint			m_autoPlayOutTime;				//自动出牌超时时间 配置

	Lint			m_baseScore;					//金币场基底分 可以用于表示其他卡牌场的基础分 目前为1
	Lint            m_coinsResult[DESK_USER_COUNT];	//玩家结算时的金币 增量
	
	// Add HuNanZhuanZhuan by hanlu 02242017 Begin
	Lint            m_lastOptTime;					//最后等待操作的时间
	Lint            m_curOptPos;					//当前操作的玩家
	// Add HuNanZhuanZhuan by hanlu 02242017 End
private:
	Lint			m_id;							//桌子ID
	Lint			m_iPlayerCapacity;				//桌子玩家容量不能超过DESK_USER_COUNT

	// 桌子类型
	DeskType		m_deskType;
	Lint			m_gameType;						// 商丘麻将 周口麻将 等等
	Lint			m_cardType;						// 4局 8局 等等
};

typedef std::map<DWORD,Desk*> DeskMap;
typedef std::vector<Desk*> DeskVec;

// struct PengInfo
// {
// 	CardValue card;
// 	Lint	pos;
// 	PengInfo():pos(0){}
//
// 	MSGPACK_DEFINE(m_type, m_card);
// };
// typedef std::vector<PengInfo> PengInfoVector;

#endif