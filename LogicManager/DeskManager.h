#ifndef _DESK_MANAGER_H
#define _DESK_MANAGER_H

#include "LBase.h"
#include "LSingleton.h"

struct DeskInfos
{
	Lint m_userId;			//开房者ID
	Lint m_logicServerId;   //logicserver id
	Lint m_creditsLimit;    //房间 信用限制值，>0 有限制，否则没有信用限制
	Lint m_cardType;		//打多少圈，打10圈的时候，每人需要1张房卡，所以加入房间需要判断自己是否房卡足够
	Lint m_gameType;		//玩的是什么游戏 商丘麻将 周口麻将 等等
	Lstring m_createIp;		//如果玩家指定了同IP不能进入那么就是创建者的ip
	//...
	//其他扩展
	DeskInfos(Lint uid=0, Lint serverid=0, Lint climit=0, Lint cardType=0, Lint gameType=0, Lstring ip=""):m_userId(uid),m_logicServerId(serverid),m_creditsLimit(climit),m_cardType(cardType),m_gameType(gameType),m_createIp(ip)
	{
	}

	bool operator!()const
	{
		return m_userId == 0 && m_logicServerId == 0 && m_creditsLimit == 0 && m_cardType == 0 && m_gameType == 0;		// ip就省略了
	}
};

class DeskManager:public LSingleton<DeskManager>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();
public:
	//返回LogicServer ID
	Lint	RecycleDeskId(Lint iDeskId);

	Lint	GetFreeDeskId(Lint uid, Lint nLogicServerID,Lint nInCredits, Lint cardType, Lint gameType, Lstring ip);

	Lint    GetDeskCreditLimit(Lint iDeskId);

	Lint	GetLogicServerIDbyDeskID(Lint nDeskID);

	Lint	GetDeskCardType(Lint iDeskId);

	Lstring GetDeskCreateIp(Lint iDeskId);

	Lint	GetDeskCreateUserId(Lint iDeskId);

	DeskInfos GetDeskInfo(const Lint iDeskId);							// 不返回引用 避免访问期间数据修改

	void	RecycleAllDeskIdOnLogicServer(Lint nLogicServerID);

	Lint	GetCoinsDeskSize();
	// 分配金币桌子count表示分配的数量 deskid表示返回值
	void	GetFreeCoinDeskID( int count, std::vector<Lint>& deskid );
	void	RecycleCoinsDeskId( const std::vector<int>& deskid );
	void	RecycleCoinsDeskId();

private:
	boost::mutex  m_mutexDeskId;

	std::queue<Lint>		m_FreeDeskList;
	std::map<Lint, DeskInfos>	m_mapDeskId2LogicServerId;
	std::list<Lint>			m_coinsDeskID;		// 金币桌子列表
};
#define gDeskManager DeskManager::Instance()

#endif