#ifndef _ROOM_VIP_H
#define _ROOM_VIP_H

#include "LMsg.h"
#include "Desk.h"
#include "LVipLog.h"

struct VipLogItem :public LVipLogItem
{
public:
	Desk*			m_desk;
	int				m_round;		// 输赢一把为一局 周口玩法中一圈可能有多局 其他玩法一圈一局
public:
	VipLogItem();
	~VipLogItem();
	Lint		GetOwerId();

	void		AddTuiDaoLog(User** user, Lint* gold,Lint* winInfo,Lint zhuangPos,Lint* agang,Lint* mgang, Lint *bomb,Lstring& videoId, Lint *ghostZimo, Lint* noghostZimo, bool addCircle = true);

	void		AddLog(User** user, Lint* gold, HuInfo* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting, bool addCircle = true);

	void		AddLog(User** user, Lint* gold, std::vector<HuInfo>* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting, bool addCircle = true);
	
	void		AddLog(User** user, Lint* gold,Lint* winInfo,Lint zhuangPos,Lint* agang,Lint* mgang, Lint *bomb, Lstring& videoId,Lint video_time, bool addCircle = true);
	
	bool		ExiestUser(Lint id);

	void		InsertToDb();

	void		UpdateToDb();

	bool		AddUser(User* user);

	Lint		GetUserPos(User* user);

	Lint		GetUserScore(User* user);

	bool		isEnd();

	bool        isNormalEnd();

	bool		IsFull(User* user);

	void		SendEnd();

	Lstring		PlayTypeToString4RLog();

	void		SendInfo();

	bool		IsBegin();

	void		RemoveUser(Lint id);

};

class VipLogMgr :public LSingleton<VipLogMgr>
{
public:	
	virtual	bool	Init();
	virtual	bool	Final();

	void			SetVipId(Lint id);

	Lstring			GetVipId();

	void			SetVideoId(Lint id);

	Lstring			GetVideoId();

	VipLogItem*		GetNewLogItem(Lint maxCircle,Lint usrId);

	VipLogItem*		GetLogItem(Lstring& logId);

	VipLogItem*	FindUserPlayingItem(Lint id);//查询玩家正在进行中的桌子

	void		AddPlayingItem(VipLogItem* item);//添加未完成的桌子

	void		Tick();
private:
	std::map<Lstring,VipLogItem*> m_item;
	Lint	m_id;

	Lint	m_videoId;
};

#define gVipLogMgr VipLogMgr::Instance()


//vip房间
class RoomVip:public LSingleton<RoomVip>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

	void			Tick(LTime& cur);

	Desk*			GetFreeDesk(Lint nDeskID, GameType gameType, Lint cardType);

	Desk*			GetDeskById(Lint id);

	Lint			CreateVipDesk(LMsgLMG2LCreateDesk* pMsg,User* pUser);

	Lint			CreateGonghuiDesk(LMsgLMG2LCreateGonghuiDesk* pMsg, User* pUser);

	// pUsers的位置是根据位置排好序的
	Lint			CreateVipCoinDesk(LMsgCN2LCreateCoinDesk*pMsg,User* pUsers[]);

	Lint			AddToVipDesk(User* pUser, Lint nDeskID);

	bool			LeaveToVipDesk(LMsgC2SLeaveDesk* pMsg, User* pUser);
	
private:
	DeskMap		m_deskMap;

	std::queue<Lint>  m_deskId;
};

#define gRoomVip RoomVip::Instance()



#endif
