#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"

class User;

struct UserLoginInfo
{
	Lint	m_seed;
	Lint	m_time;
	LUser	m_user;
};

class Work:public LRunnable,public LSingleton<Work>
{
public:
	virtual bool	Init();
	virtual bool	Final();

	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();

	void			Tick(LTime& cur);
	void			HanderMsg(LMsg* msg);

	LTime&			GetCurTime();
	//md5加密算法
	Lstring			Md5Hex(const void* data,Lsize size);
public:
	//处理客户端连接上来消息
	void			HanderClientIn(LMsgIn* msg);
	//处理客户端掉线的消息 
	void			HanderUserKick(LMsgKick* msg);

	//处理玩家登陆的消息 
	void			HanderUserLogin(LMsgLMG2LLogin* msg);
	
	//处理玩家登陆的消息
	void			HanderDeskUserLogin(User* pUser, Lint gateId, Lstring& ip, bool firstLogin = false, Lstring buyInfo = "", Lint hide = 0);

	//处理玩家登出的消息 
	void			HanderUserLogout(Lint gateId, const Lstring& strUUID);

	//处理玩家创建桌子
	void			handerLMGCreateDesk(LMsgLMG2LCreateDesk* msg);

	//处理玩家加入桌子
	void			handerLMGAddToDesk(LMsgLMG2LAddToDesk* msg);

	//处理LM发送的牌型活动配置信息
	void			handerPXActiveInfo(LMsgLMG2LPXActive* msg);
	
	void            HanderLMGCreateCoinDesk(LMsgCN2LCreateCoinDesk* msg);

	void			HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg);

	void			HanderHearBeatReply(LMsgHeartBeatReplyMsg* msg);

	void			HanderRLogInfo(LMsgLMG2LRLogInfo* msg);

	//回收桌子ID（创建失败，房间结束）
	void			RecycleDeskId(Lint deskID);
public:
	void			ConnectToLogicManager();

	void			SendToLogicManager(LMsg& msg);
public:
	void			HanderGateLogin(LMsgG2LLogin* msg);
	void			HanderGateLogout(LMsgKick* msg);
	GateInfo*		GetGateInfoBySp(LSocketPtr sp);
	GateInfo*		GetGateInfoById(Lint id);
	void			DelGateInfo(Lint id);
	Lint			GetManagerID() { return m_managerID; }

	void			HanderGateUserMsg(LMsgG2LUserMsg* msg,GateInfo* gate);

protected:
	//处理LogicManager同步的Coins信息
	void			HanderCoinsServerInfo(LMsgLMG2GLCoinsServerInfo* msg);
public:
	void			SendToCoinsServer(LMsg& msg);

public:
	//DB链接相关
	void			ConnectToDb();
	void			SendMsgToDb(LMsg& msg);
protected:
	void			_initTickTime();
protected:
	void			_reconnectLogicManager();
	void			_reconnectLogicDB();
	void			_reconnectCoinsServer();

	void			_checkLogicManagerHeartBeat();
	void			_checkLogicDBHeartBeat();
	void			_checkCoinsServerHeartBeat();

private:
	LTime		m_tickTimer;

	Llong		m_1SceTick;//1秒循环一次
	Llong		m_15SecTick;//15秒循环一次
	Llong		m_30SceTick;//30秒循环一次
	Llong		m_60SecTick;//1分钟循环一次
private:
	LSocketPtr	m_logicManager;//链接center
	LSocketPtr	m_dbClient;//链接db
	CoinsInfo	m_coinsServer;

	std::map<Lint, GateInfo> m_gateInfo;
	Lint m_managerID;		// 表示游戏ID
private:
	std::map<LSocketPtr, Llong> m_mapReconnect;
	std::map<LSocketPtr, bool> m_mapHeartBeat;
private:
	CMemeoryRecycle m_memeoryRecycle;
};

#define gWork Work::Instance()

#endif
