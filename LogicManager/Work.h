#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"

#define INVALID_GATEID			0xFFFFFFFF
#define INVALID_LOGICSERVERID	0xFFFFFFFF

struct LOGIC_SERVER_INFO
{
	LOGIC_SERVER_INFO()
	{
		m_tickTime = 0;
	}

	LogicInfo m_logicInfo;
	Llong	  m_tickTime;								//当这个logic被tick的时候记录时间，3分钟不恢复才进行剔除.默认值为0
};

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

protected:
	void			Tick(LTime& cur);
	LTime&			GetCurTime();
protected:
	void			HanderMsg(LMsg* msg);

	void			HanderClientIn(LMsgIn* msg);
	void			HanderUserKick(LMsgKick* msg);

	void            HanderCenterGonghuiInfo(LMsgCe2LGonghuiInfo* msg);
	void            PrintGonghuiInfo(std::vector<Gonghui> gonghuiInfo);

	//center发送玩家id信息
	void			HanderCenterUserInInfo(LMsgCe2LUserIdInfo*msg);

	void			HanderCenterGMHorse(LMsgCe2LGMHorse* msg);
	void			HanderCenterGMBuyInfo(LMsgCe2LGMBuyInfo* msg);
	void			HanderCenterGMHide(LMsgCe2LGMHide* msg);
	void			HanderLogicSetGameFree(LMsgCE2LSetGameFree* msg);
	void			HanderLogicSetPXActive(LMsgCE2LSetPXActive* msg);
	void			HanderLogicSetOUGCActive(LMsgCE2LSetOUGCActive* msg);

	void            HanderLogicSetExchActive(LMsgCE2LSetExchActive* msg);
	void            HanderLogicSetActivity(LMsgCE2LSetActivity* msg);
	void			HanderRank(LMsgCE2LMGRank* msg);

	void			HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg);
	void			HanderHearBeatReply(LMsgHeartBeatReplyMsg* msg);
protected:
	void			HanderGateLogin(LMsgG2LMGLogin* msg);
	void			HanderGateLogout(LMsgKick* msg);

	//发送网关信息到center
	void			SendGateInfoToCenter();

	//发送logic消息到其他gate
	void			SendLogicInfoToGates(Lint nGateID = 0);
	// 发送远程日志信息
	void			SendRLogInfoToLogic(Lint nLogicID = 0);
protected:
	// 金币服务器相关
	CoinsInfo		m_coinsServer;
	void			HanderCoinsLogin(LMsgCN2LMGLogin* msg);
	void			HanderCoinsLogout(LMsgKick* msg);
	void			HanderCoinFreeDeskReq(LMsgCN2LMGFreeDeskReq* msg);
	void			HanderCoinRecycleDesk(LMsgCN2LMGRecycleDesk* msg);

	void			_checkCoinsServerState();

	//发送Coins信息到Gate
	void			SendCoinsInfoToGates(Lint nGateID = 0);
	//发送Coins信息到Logic
	void			SendCoinsInfoToLogic(Lint nLogicID = 0);

public:
	bool			IsCoinsServerConnected();
	void			SendMessageToCoinsServer(LMsg& msg);

public:
	Lint			getGateIdBySp(LSocketPtr sp);
	void			SendMessageToAllGate(LMsg& msg);
	void			SendMessageToGate(Lint gateID, LMsg& msg);
private:
	boost::recursive_mutex m_mutexGateInfo;
	std::map<Lint, GateInfo> m_gateInfo;
protected:
	void			HanderLogicLogin(LMsgL2LMGLogin* msg);
	void			HanderLogicLogout(LMsgKick* msg);
	void			_checkLogicServerState();
	void			_checkAndTouchLogicServer();
	void			_checkLogicServerOnlySupportGameType();
	void			_checkRLogStart();
protected:
	void            _checkFreeModel();
	void            _checkExchActive();
public:
	void            SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);
	bool			isLogicServerExist(int iLogicServerId);
	void			SendMessageToAllLogic(LMsg& msg);
	void			SendMessageToLogic(Lint iLogicServerId, LMsg& msg);
	Lint			allocateLogicServer(int iGameType);
	
	void			delDeskCountOnLogicServer(Lint iLogicServerId, int iDelCount = 1);
private:
	boost::recursive_mutex m_mutexLogicServerInfo;
	std::map<Lint, LOGIC_SERVER_INFO> m_logicServerInfo;
	std::map<Lint, std::map<Lint, Lint> > m_mapLogicServerOnlySupportType; //这些服务器只支持这些游戏类型。格式： 主键是logicserver。后面是游戏类型
public:
	void			SendToCenter(LMsg& msg);
	void			SendMsgToDb(LMsg& msg);
protected:
	//以下在程序启动的时候连接不需要互斥
	void			ConnectToCenter();
	void			ConnectToDb();

	void			_reconnectDB();
	void			_reconncetCenter();
	void			_checkCenterHeartBeat();
	void			_checkLogicDBHeartBeat();

protected:
	void			_checkRank();
	void			_checkLastWeekRank();
	void			_checkLastMonthRank();
	void			_checkRankRewardConfig();			// 定时重新读取排行榜奖励配置

	void			fillRankInfo(RankInfo& info);

	typedef boost::shared_lock<boost::shared_mutex> ReadLock;
	typedef boost::unique_lock<boost::shared_mutex> WriteLock;

	boost::shared_mutex m_weekRankLock;
	boost::shared_mutex m_monthRankLock;

	std::vector<RankInfo> m_weekRankInfo;
	std::vector<RankInfo> m_monthRankInfo;

	boost::shared_mutex m_lastWeekRankLock;
	boost::shared_mutex m_lastMonthRankLock;

	std::vector<RankInfo> m_lastWeekRankInfo;
	std::vector<RankInfo> m_lastMonthRankInfo;

	std::vector<int> m_lastWeekRewardId;			// 领取过奖励的人
	std::vector<int> m_lastMonthRewardId;			// 领取过奖励的人

	struct RankReward
	{
		int index;	// 排名 从1开始配置的
		int reward;
	};
	boost::shared_mutex m_weekRankRewardConfigLock;
	std::vector<RankReward> m_weekRankRewardConfig;
public:
	void GetRank(const Lint uid, LMsgS2CUpdateRank& msg);
	int GetRankReward(const Lint uid);

private:
	boost::mutex m_mutexCenterClient;
	boost::mutex m_mutexDBClient;

	LSocketPtr	m_centerClient;//链接center
	LSocketPtr	m_dbClient;//链接db

	std::map<LSocketPtr, bool> m_mapReconnect;
	std::map<LSocketPtr, bool> m_mapHeartBeat;
protected:
	void		_initTickTime();
private:
	LTime		m_tickTimer;
	Llong		m_1SceTick;		//1秒循环一次
	Llong		m_5SecTick;		//5秒循环一次
	Llong		m_15SceTick;	//15秒循环一次
	Llong		m_30SceTick;	//30秒循环一次
	Llong		m_180SecTick;	//3分钟循环一次
	Llong		m_600SceTick;	//5分钟秒循环一次
	Llong		m_1HourTick;	//每小时整点
	Llong		m_1WeekTick;	//每周开始时间
	LTime		m_1MonthTick;	//每月时间，没有存开始时间，直接比较月份即可，月份不同就表示过了

	bool		m_bHadLoadedData;
private:
	CMemeoryRecycle m_memeoryRecycle;
};

#define gWork Work::Instance()

#endif
