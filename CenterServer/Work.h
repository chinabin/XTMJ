#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "DbServer.h"
#include "DbServerManager.h"
#include "LMemoryRecycle.h"
#include "LRedisClient.h"

class Work:public LRunnable,public LSingleton<Work>
{
public:
	virtual bool	Init();
	virtual bool	Final();

	virtual void	Push(void* msg);
	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();

	void		Tick(LTime& cur);
	void		SaveCurrentOnline();
	LTime&		GetCurTime();
protected:
	void		HanderMsg(LMsg* msg);
protected:	
	void		HanderClientIn(LMsgIn* msg);
	void		HanderUserKick(LMsgKick* msg);
	void		HanderLogicLogin(LMsgL2CeLogin* msg);
	void		HanderLogicGateInfo(LMsgL2CeGateInfo* msg);
	void		HanderLogicLogout(LMsgKick*msg);
	void		HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg);
	void        HanderModifyCredit(LMsgLM2CEN_ADD_CRE* msg);
	void		HanderGetRank(LMsgLMG2CERank* msg);
	void		HanderGetRankReward(LMsgLMG2CEGetRankReward* msg);
protected:
	void		HanderHttp(LMsgHttp* msg);	
	void		HanderGMCharge(std::map<Lstring,Lstring>& param,LSocketPtr sp);
	void		HanderGMSetAgentPower(std::map<Lstring,Lstring>& param,LSocketPtr sp);
	void		HanderGMCoins(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMHorse(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMBuyInfo(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMOnlineCount(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMCloseServer(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	bool		HanderCheckMd5(std::map<Lstring, Lstring>& param);
	void		HanderSetFreeSet(std::map<Lstring,Lstring>& param,LSocketPtr sp);
	void		HanderSetPXActive(std::map<Lstring,Lstring>& param,LSocketPtr sp);
	void		HanderSetOUGCActive(std::map<Lstring,Lstring>& param,LSocketPtr sp);

	void		HanderSetExchange(std::map<Lstring,Lstring>& param,LSocketPtr sp);

	void        HanderActivityDrawSet(std::map<Lstring,Lstring>& param,LSocketPtr sp);
	void        HanderActivityShareSet(std::map<Lstring,Lstring>& param,LSocketPtr sp);

	void		HanderFreezeUser(std::map<Lstring,Lstring>& param,LSocketPtr sp);
	void		HanderGHMsgs(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	
public:
	bool		isLogicManagerExist(Lint logicManagerID);
	bool		isLogicManagerExist(LSocketPtr sp);

	GateInfo	GetGateUsefulInfo(Lint logicID);
	Lstring		getServerNameByID(Lint serverID);

	void		SendMsgToLogic(LMsg& msg, Lint logicID);
	void		SendMsgToLogic(LMsg& msg, Lint logicID, int iLimitCountInSendPool);
	void		SendMsgToAllLogic(LMsg& msg);

	bool		IsExistAgent(const Lint id);	// 查找agent是否存在
protected:
	void		_initTickTime();
	bool		_initRedisAndActive();
	void		SendActiveSetToLM(Lint LMid);

	// agent相关
	bool		LoadAgentList();				// 启动时从数据库加载一次agent表，更新由后台进行添加或删除，不直接处理agent表

	boost::mutex m_mutexAgentList;
	std::set<Lint> m_agentList;
private:
	LTime		m_tickTimer;
	Llong		m_30SceTick;//30秒循环一次
private:
	boost::mutex m_mutexLogicManager;
	std::map<Lint, LSocketPtr> m_mapLogicManagerSp;
	std::map<Lint, Lstring> m_mapLogicManagerName;
private:
	boost::mutex m_mutexGateInfo;
	std::map<Lint, std::map<Lint, GateInfo> > m_mapGateInfo;
private:
	DbServer        m_dbServer;   //用于charge 、onlinelog表
private:
	CMemeoryRecycle m_memeoryRecycle;

private:
	CRedisClient	m_RedisClient;

// 活动配置相关
private:
	Lstring			m_strActiveFreeSet;
	Lstring			m_strActivePXSet;
	Lstring			m_strActiveOUGCSet;
	Lstring         m_strActiveExchangeSet;
};


#define gWork Work::Instance()

#endif
