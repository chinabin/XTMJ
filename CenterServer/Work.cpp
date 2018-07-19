#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "DbServer.h"
#include "InsideNet.h"
#include "UserManager.h"
#include "HttpRequest.h"
#include "DbServerManager.h"
#include "UserMessageMsg.h"
#include "NewUserVerifyManager.h"


#define ActiveFreeSetKey	"ActiveFreeSet"
#define ActivePXSetKey		"ActivePXSet"
#define ActiveOUGCSetKey	"ActiveOUGCSet"
#define ActiveExchangeSetKey "ActiveExchangeSet"



//初始化
bool Work::Init()
{
	//首先设置log文件，不然log没有开启，log文件没办法查看
	LLog::Instance().SetFileName("CenterServer");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	//设置log级别
	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	//网络消息初始化
	LMsgFactory::Instance().Init();

	//测试数据库
	LDBSession dbSession;
	if(!dbSession.Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort()))
	{
		LLOG_ERROR("DB err");
		return false;
	}

	//废弃 不再使用
// 	if(!LoadAgentList())
// 	{
// 		LLOG_ERROR("Load agent table err");
// 		return false;
// 	}

	if(!gUserManager.Init())
	{
		LLOG_ERROR("gUserManager.Init error");
		return false;
	}

	//内部端口初始化
	if(!gInsideNet.Init())
	{
		LLOG_ERROR("Fail to init inside ip and port");
		return false;
	}

	//gm访问端口初始化
	if (!gGmNet.Init())
	{
		LLOG_ERROR("Fail to init gm ip and port");
		return false;
	}

	if(!_initRedisAndActive())
	{
		LLOG_ERROR("Fail to init Redis ip and port");
		return false;
	}

	_initTickTime();

	return true;
}

bool Work::Final()
{
	return true;
}

//启动
void Work::Start()
{
	m_memeoryRecycle.Start();

	//玩家消息线程开启
	gUserMessageMsg.Start();

	//主线程开启
	LRunnable::Start();

	//数据库线程开启
	m_dbServer.Start();

	gDbServerManager.Start();

	gDbNewUserManager.Start();

	//内部网络开启
	gInsideNet.Start();

	//gm网络开启
	gGmNet.Start();
}

//等待
void Work::Join()
{
	//等待玩家消息线程终止
	gUserMessageMsg.Join();

	gGmNet.Join();

	//等待网络线程终止
	gInsideNet.Join();

	//等待数据库线程终止
	m_dbServer.Join();

	gDbServerManager.Join();

	gDbNewUserManager.Join();

	m_memeoryRecycle.Join();

	//等待逻辑线程终止
	LRunnable::Join();
}

//停止
void Work::Stop()
{
	gUserMessageMsg.Stop();

	gGmNet.Stop();

	m_dbServer.Stop();

	gDbServerManager.Stop();

	gDbNewUserManager.Stop();

	LRunnable::Stop();

	m_memeoryRecycle.Stop();
}

void Work::Clear()
{

}

void Work::Push(void* msg)
{
	LMsg* pMessage = (LMsg*)msg;
	if(pMessage == NULL)
	{
		return;
	}

	switch(pMessage->m_msgId)
	{
	case MSG_FROM_LOGINSERVER:
		{
			LMsgFromLoginServer* pFromLoginServer = (LMsgFromLoginServer*)pMessage;
			if(pFromLoginServer)
			{
				gUserMessageMsg.handlerMessage(pFromLoginServer->m_strUUID, pFromLoginServer);
			}
			break;
		}
	case MSG_L_2_CE_MODIFY_USER_NEW:
		{
			LMsgL2CeModifyUserNew* pUserNewMsg = (LMsgL2CeModifyUserNew*)pMessage;
			if(pUserNewMsg)
			{
				gUserMessageMsg.handlerMessage(pUserNewMsg->m_strUUID, pUserNewMsg);
			}
			break;
		}
	case MSG_L_2_CE_USER_LOGIN:
		{
			LMsgL2CeUserServerLogin* pUserLoginMsg = (LMsgL2CeUserServerLogin*)pMessage;
			if(pUserLoginMsg)
			{
				gUserMessageMsg.handlerMessage(pUserLoginMsg->m_openID, pUserLoginMsg);
			}
			break;
		}
	case MSG_L_2_CE_USER_LOGOUT:
		{
			LMsgL2CeUserServerLogout* pLogoutMsg = (LMsgL2CeUserServerLogout*)pMessage;
			if(pLogoutMsg)
			{
				gUserMessageMsg.handlerMessage(pLogoutMsg->m_openID, pLogoutMsg);
			}
			break;;
		}
	case MSG_L_2_LMG_MODIFY_USER_CARD:
		{
			LMsgL2LMGModifyCard* pModifyCardMsg = (LMsgL2LMGModifyCard*)pMessage;
			if(pModifyCardMsg)
			{
				gUserMessageMsg.handlerMessage(pModifyCardMsg->m_strUUID, pModifyCardMsg);
			}
			break;
		}
	case MSG_L_2_LMG_ADD_USER_PLAYCOUNT:
		{
			LMsgL2LMGAddUserPlayCount* pPlayerCount = (LMsgL2LMGAddUserPlayCount*)pMessage;
			if(pPlayerCount)
			{
				gUserMessageMsg.handlerMessage(pPlayerCount->m_strUUID, pPlayerCount);
			}
			break;
		}
	case MSG_CN_2_LMG_MODIFY_USER_COINS:
		{
			LMsgCN2LMGModifyUserCoins* pActiveInfoMsg = (LMsgCN2LMGModifyUserCoins*)pMessage;
			if(pActiveInfoMsg)
			{
				gUserMessageMsg.handlerMessage(pActiveInfoMsg->m_strUUID, pActiveInfoMsg);
			}
			break;
		}
	case MSG_C_2_C_ADD_CRE:
		{
			LMsgC2C_ADD_CRE* pActiveInfoMsg = (LMsgC2C_ADD_CRE*)pMessage;
			if(pActiveInfoMsg)
			{
				gUserMessageMsg.handlerMessage(pActiveInfoMsg->m_strUUID, pActiveInfoMsg);
			}
			break;
		}
	case MSG_LMG_2_L_EXCH_CARD:
		{
			LMsgL2LMGExchCard* pCoinsMsg = (LMsgL2LMGExchCard*)pMessage;
			if(pCoinsMsg)
			{
				gUserMessageMsg.handlerMessage(pCoinsMsg->m_strUUID, pCoinsMsg);
			}
			break;
		}
	case MSG_LM_2_CE_ADD_AGENT:
		{
			LMsgLMG2CEAddAgent* pCoinsMsg = (LMsgLMG2CEAddAgent*)pMessage;
			if(pCoinsMsg)
			{
				gUserMessageMsg.handlerMessage(pCoinsMsg->m_strUUID, pCoinsMsg);
			}
			break;
		}
	default:
		LRunnable::Push(pMessage);
		break;
	}
}

void Work::Run()
{
	LTime msgTime;
	int iMsgCount = 0;
	int iElapseTime = 0;
	while(!GetStop())
	{
		//这里处理游戏主循环
		LTime cur;
		if(cur.MSecs() - m_tickTimer.MSecs() > 1)
		{
			m_tickTimer = cur;
			Tick(cur);	
		}

		LMsg* msg = (LMsg*)Pop();
		if(msg == NULL)
		{
			Sleep(1);
			++iElapseTime;
			continue;
		}
		else
		{
			HanderMsg(msg);
			m_memeoryRecycle.recycleMsg(msg);

			//每一万个包输出所用时间
			++iMsgCount;
			if(iMsgCount == 10000)
			{
				LTime now;
				LLOG_ERROR("Work Logic: 10000 package needs %ld time", now.MSecs() - msgTime.MSecs() - iElapseTime);

				iMsgCount = 0;
				iElapseTime = 0;
				
				msgTime = now;
			}
		}
	}
}

void Work::Tick(LTime& cur)
{
	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();
		SaveCurrentOnline();
	}
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	switch(msg->m_msgId)
	{
	case MSG_CLIENT_KICK:
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;
	case MSG_HTTP:
		HanderHttp((LMsgHttp*)msg);
		break;
	case MSG_L_2_CE_LOGIN:
		HanderLogicLogin((LMsgL2CeLogin*)msg);
		break;
	case MSG_L_2_CE_GATE_INFO:
		HanderLogicGateInfo((LMsgL2CeGateInfo*)msg);
		break;
	case MSG_HEARDBEAT_REQUEST:
		HanderHeartBeatRequest((LMsgHeartBeatRequestMsg*)msg);
		break;
	case MSG_LM_2_CEN_ADD_CRE:
		HanderModifyCredit((LMsgLM2CEN_ADD_CRE*) msg);
		break;
	case MSG_LM_2_CE_RANK:
		HanderGetRank((LMsgLMG2CERank*)msg);
		break;
	case MSG_LM_2_CE_GET_RANK_REWARD:
		HanderGetRankReward((LMsgLMG2CEGetRankReward*)msg);
		break;
	default:
		break;
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if(isLogicManagerExist(msg->m_sp))
	{
		HanderLogicLogout(msg);
	}
}

void Work::HanderClientIn(LMsgIn* msg)
{

}

void Work::HanderLogicLogin(LMsgL2CeLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}
	//判断key是否正确
	if (msg->m_key != gConfig.GetCenterKey())
	{
		msg->m_sp->Stop();
		return;
	}

	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);

		auto itLogicM = m_mapLogicManagerSp.find(msg->m_ID);
		if(itLogicM != m_mapLogicManagerSp.end())
		{
			if(itLogicM->second)
			{
				itLogicM->second->Stop();
			}

			m_mapLogicManagerSp.erase(itLogicM);
		}

		m_mapLogicManagerSp[msg->m_ID] = msg->m_sp;
		m_mapLogicManagerName[msg->m_ID] = msg->m_ServerName;
	}
	
	// 发送活动配置信息
	SendActiveSetToLM(msg->m_ID);

	if(msg->m_needLoadUserInfo == 1)
	{
		gUserManager.LoadUserIdInfo(msg->m_ID);
		gUserManager.loadGonghuiInfo(msg->m_ID);
	}
}

void Work::HanderLogicLogout(LMsgKick*msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_ERROR("Work::HanderLogicLogout");
	
	int iServerId = 0;

	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); ++itLogicM)
		{
			if(itLogicM->second == msg->m_sp)
			{
				iServerId = itLogicM->first;
				m_mapLogicManagerSp.erase(itLogicM);
				break;
			}
		}
	}

	if(iServerId > 0)
	{
		boost::mutex::scoped_lock ll(m_mutexGateInfo);
		m_mapGateInfo.erase(iServerId);
	}
}

void Work::HanderLogicGateInfo(LMsgL2CeGateInfo* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_ID <= 0)
	{
		LLOG_ERROR("Work::HanderLogicGateInfo %d", msg->m_ID);
		return;
	}

	if(!isLogicManagerExist(msg->m_ID))
	{
		LLOG_ERROR("LogicManger's id isn't exist. %d", msg->m_ID);
		msg->m_sp->Stop();
		return;
	}

	boost::mutex::scoped_lock l(m_mutexGateInfo);
	m_mapGateInfo[msg->m_ID].clear();
	for (Lint i = 0; i < msg->m_count; ++i)
	{
		m_mapGateInfo[msg->m_ID][msg->m_gate[i].m_id] = msg->m_gate[i];
	}
}

void Work::HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg)
{
	int fromwho = msg->m_fromWho;
	if ( fromwho < HEARBEAT_WHO_UNKNOWN || fromwho > HEARBEAT_WHO_NUM )
	{
		fromwho = HEARBEAT_WHO_UNKNOWN;
	}

	LLOG_DEBUG("Recieve heartbeat from %s %d", HearBeat_Name[fromwho], msg->m_iServerId);

	LMsgHeartBeatReplyMsg msgReply;
	msgReply.m_fromWho = HEARBEAT_WHO_CENTER;
	msgReply.m_iServerId = 0;

	msg->m_sp->Send(msgReply.GetSendBuff());
}

void Work::HanderModifyCredit( LMsgLM2CEN_ADD_CRE* msg )
{
	for(Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		if(msg->m_user[i] > 0)
		{
			boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_user[i]);
			if(safeUser.get() == NULL || !safeUser->isValid())
			{
				LLOG_DEBUG("#ERROR SAVE CREDIT USER NOT EXIST = %d",msg->m_user[i]);
				continue;
			}

			boost::shared_ptr<DUser> user = safeUser->getUser();
			LMsgC2C_ADD_CRE* cmsg = new LMsgC2C_ADD_CRE();
			cmsg->m_strUUID = user->m_usert.m_unioid;
			cmsg->m_userId = user->m_usert.m_id;
			Push(cmsg);
		}
	}
}

void Work::HanderGetRank(LMsgLMG2CERank* msg)
{
	if(msg->m_rankCount <= 0)
		msg->m_rankCount = 20;

	LTime cur;
	char key[128] = {0};
	char rewardKey[128] = {0};

	if(msg->m_rankType == 1)		// 本周排行
	{
		sprintf_s(key, 128, "week_rank_%d", cur.Secs()/RANK_REWARD_INTERVAL);
	}
// 	else if(msg->m_rankType == 2)	// 本月排行
// 	{
// 		sprintf_s(key, 128, "month_rank_%4d%2d", cur.GetYear(), cur.GetMonth());
// 	}
	else if(msg->m_rankType == 3)	// 上周排行
	{
		int m = cur.Secs()/RANK_REWARD_INTERVAL - 1;

		sprintf_s(key, 128, "week_rank_%d", m);

		sprintf_s(rewardKey, 128, "week_rank_id_%d", m);
	}
// 	else if(msg->m_rankType == 4)	// 上月排行
// 	{
// 		int y = cur.GetYear();
// 		int m = cur.GetMonth()-1;
// 		if(m == -1)
// 		{
// 			m = 11;
// 			--y;
// 		}
//
// 		sprintf_s(key, 128, "month_rank_%4d%2d", y, m);
//
// 		sprintf_s(rewardKey, 128, "month_rank_id_%4d%2d", y, m);
// 	}

	std::vector<CRedisClient::msdata> members;
	m_RedisClient.zrevrangeWithScores(key, 0, msg->m_rankCount-1, members);

	LMsgCE2LMGRank rank;

	rank.m_rankType = msg->m_rankType;
	//rank.m_rankCount = members.size();
	//rank.m_rankData.reserve(rank.m_rankCount);
	for(auto it=members.begin(); it!=members.end(); ++it)
	{
		RankData rd;
		rd.id = it->member;
		rd.score = it->score;
		rank.m_rankData.push_back(rd);
	}
	rank.m_rankCount = rank.m_rankData.size();

	if(msg->m_rankType == 3 || msg->m_rankType == 4)
	{
		m_RedisClient.smembers(rewardKey, rank.m_rankRewardId);
	}

	msg->m_sp->Send(rank.GetSendBuff());
}

void Work::HanderGetRankReward(LMsgLMG2CEGetRankReward* msg)
{
	LTime cur;
	char rewardKey[128] = {0};

	if(msg->m_rankType == 3)	// 上周排行
	{
		int m = cur.Secs()/RANK_REWARD_INTERVAL - 1;

		sprintf_s(rewardKey, 128, "week_rank_id_%d", m);

		bool result;
		m_RedisClient.sadd(rewardKey, msg->m_uid, result);
	}
}

GateInfo Work::GetGateUsefulInfo(Lint logicID)
{
	static Lint sindex = 0;
	GateInfo gateinfo;

	boost::mutex::scoped_lock l(m_mutexGateInfo);
	auto itGateInfo = m_mapGateInfo.find(logicID);
	if(itGateInfo != m_mapGateInfo.end())
	{
		Lint index = 0;
		auto it = itGateInfo->second.begin();
		for (; it != itGateInfo->second.end(); ++it)
		{
			if (index++ % itGateInfo->second.size() == sindex % itGateInfo->second.size())
			{
				gateinfo = it->second;
				break;
			}
		}

		++sindex;
	}

	return gateinfo;
}

Lstring Work::getServerNameByID(Lint serverID)
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);

	auto itor = m_mapLogicManagerName.find(serverID);
	if (itor != m_mapLogicManagerName.end())
	{
		return itor->second;
	}

	return "未知麻将";
}

void Work::SendMsgToLogic(LMsg& msg, Lint logicID)
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);

	auto itLogicM = m_mapLogicManagerSp.find(logicID);
	if(itLogicM != m_mapLogicManagerSp.end())
	{
		itLogicM->second->Send(msg.GetSendBuff());
	}
	else
	{
		LLOG_ERROR("void Work::SendMsgToLogic ERROR: %d is invilid", logicID);
	}
}

void Work::SendMsgToLogic(LMsg& msg, Lint logicID, int iLimitCountInSendPool)
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);

	auto itLogicM = m_mapLogicManagerSp.find(logicID);
	if (itLogicM != m_mapLogicManagerSp.end())
	{
		while(itLogicM->second->getSendPoolSize() > iLimitCountInSendPool)
		{
			boost::this_thread::sleep(boost::posix_time::millisec(30));
		}

		itLogicM->second->Send(msg.GetSendBuff());
	}
	else
	{
		LLOG_ERROR("void Work::SendMsgToLogic ERROR: %d is invilid", logicID);
	}
}

void Work::SendMsgToAllLogic(LMsg& msg)
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);

	for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); ++itLogicM)
	{
		itLogicM->second->Send(msg.GetSendBuff());
	}
}

static const char* ADMIN_PATH = "/cgi-bin/admin";

static void SendRet(Lstring errorInfo, LSocketPtr send)
{
	char strOK[512] = { 0 };

	std::string strSendBuf;
	strSendBuf.reserve(1024);
	strSendBuf += "HTTP/1.1 200 OK\r\n";

	sprintf_s(strOK, sizeof(strOK)-1, "Content-Length:%d\r\n", errorInfo.size());
	strSendBuf += strOK;
	strSendBuf += "Content-Type: text/html; charset=utf-8\r\n\r\n";

	memset(strOK,0,sizeof(strOK));
	sprintf_s(strOK, sizeof(strOK)-1, "%s", errorInfo.c_str());
	strSendBuf += strOK;

	LBuffPtr buff(new LBuff());
	buff->Write(strSendBuf.c_str(), strSendBuf.size());
	send->Send(buff);
}

/*
errorCode 类型
0	-	请求成功
1	-	请求url错误
2	-	消息类型不存
3	-	sign不正确
4	-	请求超时
5	-	请求方法错误
6	-	请求参数错误
*/

void Work::HanderHttp(LMsgHttp* msg)
{	
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_head.size() > 4 && msg->m_head.substr(0, 4) == "GET ")
	{
		std::string::size_type pos = msg->m_head.find("\r\n");
		if (pos == std::string::npos || pos < 8)
		{
			LLOG_ERROR("Work::HanderHttp GET error");
			SendRet("{\"errorCode\":5,\"errorMsg\":\"req function error\"}",msg->m_sp);
			return;
		}

		// GET /cgi-bin/admin?msg=getUserInfo&openId=122&time=1466238689575&sign=bf4c9859c07c9687a29796ba723eff62 HTTP/1.1
		std::string totalUrl = msg->m_head.substr(4, pos-13);	// 去掉前面的Get+空格 去掉后面的空格+HTTP/1.1\r\n
		std::string::size_type pos2 = totalUrl.find('?', 0);
		if (pos2 == std::string::npos)
		{
			LLOG_ERROR("Work::HanderHttp GET param error");
			SendRet("{\"errorCode\":6,\"errorMsg\":\"req param error\"}", msg->m_sp);
			return;
		}

		std::string reqType = totalUrl.substr(0, pos2);
		std::string reqParam = totalUrl.substr(pos2 + 1, totalUrl.size() - pos2);
		if (reqType == ADMIN_PATH )
		{
			std::map<Lstring, Lstring> paraMaps;
			HttpRequest::Instance().DeodeUrlParams(reqParam, paraMaps, true);
			if (!HanderCheckMd5(paraMaps))
			{
				SendRet("{\"errorCode\":3,\"errorMsg\":\"sign error\"}", msg->m_sp);//签名错误
				return;
			}
			//lmiao:暂时屏蔽掉，不判断时间，由于服务器被攻击，有可能长时间收不到
			// 			Lint time = atoi(paraMaps["time"].c_str());
			// 			if (gWork.GetCurTime().Secs() - time > 5)
			// 			{
			// 				SendRet("{\"errorCode\":4,\"errorMsg\":\"time out\"}", msg->m_sp);//超时
			// 				return;
			// 			}
			if (paraMaps["msg"] == "charge")
				HanderGMCharge(paraMaps,msg->m_sp);
			else if(paraMaps["msg"] == "agent")		// 设置代理
				HanderGMSetAgentPower(paraMaps,msg->m_sp);
			else if(paraMaps["msg"] == "exchange")    //换房卡了
				HanderSetExchange(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "coins")
				HanderGMCoins(paraMaps,msg->m_sp);
			else if (paraMaps["msg"] == "horse")
				HanderGMHorse(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "buy")
				HanderGMBuyInfo(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "online")
				HanderGMOnlineCount(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "close")
				HanderGMCloseServer(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "Free")
				HanderSetFreeSet(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "PXActive")
				HanderSetPXActive(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "OldUGiveCardActive")
				HanderSetOUGCActive(paraMaps, msg->m_sp);	// 老玩家送房卡活动
			else if(paraMaps["msg"] == "ActivityDraw")
				HanderActivityDrawSet(paraMaps,msg->m_sp);
			else if(paraMaps["msg"] == "ActivityShare")
				HanderActivityShareSet(paraMaps,msg->m_sp);
			else if(paraMaps["msg"] == "freeze")
				HanderFreezeUser(paraMaps,msg->m_sp);
			else
				SendRet("{\"errorCode\":2,\"errorMsg\":\"msg type error\"}", msg->m_sp);//消息类型错误
		}
		else
		{
			SendRet("{\"errorCode\":1,\"errorMsg\":\"get url error\"}", msg->m_sp);//请求链接错误
		}
	}
}

bool Work::HanderCheckMd5(std::map<Lstring, Lstring>& param)
{
	std::vector<std::string> keys;
	for (std::map<std::string, std::string>::const_iterator it = param.begin(); it != param.end(); ++it)
	{
		const std::string& strKey = it->first;
		if (strKey.compare("sign") != 0)
			keys.push_back(strKey);
	}
	std::sort(keys.begin(), keys.end());
	std::string reqsort;
	for (size_t i = 0; i < keys.size(); ++i)
	{
		reqsort += keys[i] + param[keys[i]];
	}
	reqsort += "1234567890987654321";


	std::string sign = HttpRequest::Instance().Md5Hex(reqsort.c_str(), reqsort.length());
	LLOG_DEBUG("Work::HanderCheckMd5,source:%s,sign:%s", reqsort.c_str(), sign.c_str());

	//加密参数不对
	if (sign != param["sign"])
	{
		LLOG_ERROR("Work::HanderCheckMd5 sign error %s:%s", param["sign"].c_str(), sign.c_str());
		return false;
	}

	return true;
}

void Work::HanderGMCharge(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin openId=openId cardType=1 cardNum=1 operType=1 sign=sign time=time

	LLOG_ERROR("Work::HanderGMCharge %s:%s", param["openId"].c_str(), param["admin"].c_str());

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(atoi(param["openId"].c_str()));			// 此处openId改为使用玩家的userId 也就是user表的Id字段
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"user not exiest\"}", sp);//玩家不存在
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	Lint	errorCode = 0;
	Lint	cardType = atoi(param["cardType"].c_str());
	Lint	cardNum = atoi(param["cardNum"].c_str());
	Lint	operType = atoi(param["operType"].c_str());

	if (user->m_logicID)//这里在logic里面有备份
	{
		LMsgCe2LGMCharge gmcharge;
		gmcharge.m_cardType = cardType;
		gmcharge.m_cardCount = cardNum;
		gmcharge.m_oper = operType;
		gmcharge.m_strUUID= user->m_usert.m_unioid;
		gmcharge.m_userid = user->m_usert.m_id;
		gmcharge.m_admin = param["admin"];
		SendMsgToLogic(gmcharge, user->m_logicID);
	}

	user->AddCardCount( cardType, cardNum, operType, param["admin"], param["remark"]);

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

//添加或删除代理权
void Work::HanderGMSetAgentPower(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin openId=openId power=1 sign=sign time=time

// 	LLOG_ERROR("Work::HanderGMSetAgentPower %s:%s %s", param["openId"].c_str(), param["admin"].c_str(), param["power"].c_str());
//
// 	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(param["openId"]);
// 	if(safeUser.get() == NULL || !safeUser->isValid())
// 	{
// 		SendRet("{\"errorCode\":7,\"errorMsg\":\"user not exiest\"}", sp);//玩家不存在
// 		return;
// 	}
//
// 	boost::shared_ptr<DUser> user = safeUser->getUser();
//
// 	Lint	errorCode = 0;
// 	Lint	power = atoi(param["power"].c_str());
//
// 	user->m_usert.m_agentPower = power;
//
// 	std::stringstream ss;
// 	ss << "UPDATE user SET AgentPower='"<< user->m_usert.m_agentPower << "' WHERE Id='";
// 	ss << user->m_usert.m_id << "'";
//
// 	Lstring* sql = new Lstring(ss.str());
//
// 	gDbServerManager.Push(sql, user->m_usert.m_id);
//
// 	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功


	// agentId=1
	LLOG_ERROR("Work::HanderGMSetAgentPower agentId[%s]", param["agentId"].c_str());

	Lint	agentId = atoi(param["agentId"].c_str());

	{
		boost::mutex::scoped_lock l(m_mutexAgentList);
		m_agentList.insert(agentId);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderGMCoins(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_ERROR("Work::HanderGMCoins %s:%s", param["openId"].c_str());

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(param["openId"]);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"user not exiest\"}", sp);//玩家不存在
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	Lint	errorCode = 0;
	Lint	coins = atoi(param["coins"].c_str());

	if ( coins > 0 )
	{
		user->AddCoinsCount( coins, 0 );
	}
	else
	{
		user->DelCoinsCount( coins, 0 );
	}
	
	if (user->m_logicID)//这里在logic里面有备份
	{
		LMsgCe2LGMCoins gmcharge;
		gmcharge.m_strUUID= user->m_usert.m_unioid;
		gmcharge.m_userid = user->m_usert.m_id;
		gmcharge.m_coins = coins;
		gmcharge.m_totalcoins = user->m_usert.m_coins;
		SendMsgToLogic(gmcharge, user->m_logicID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderGMHorse(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin horse=horse sign=sign time=time serverid=10
	LMsgCe2LGMHorse horse;
	horse.m_str = param["horse"];
	if (param["serverid"].empty())
	{
		SendMsgToAllLogic(horse);
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		SendMsgToLogic(horse, nServerID);
	}
	LLOG_ERROR("Work::HanderGMHorse sets horse %s", param["horse"].c_str());
	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderGMBuyInfo(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin buy=buy sign=sign time=time serverid=10
	LMsgCe2LGMBuyInfo buy;
	buy.m_str = param["buy"];
	if (param["serverid"].empty())
	{
		SendMsgToAllLogic(buy);
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		SendMsgToLogic(buy, nServerID);
	}
	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderGMOnlineCount(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin sign=sign time=time

	boost::mutex::scoped_lock l(m_mutexGateInfo);

	std::stringstream ss;
	ss << "{\"errorCode\":0,\"errorMsg\":\"success\",\"data\":[";

	Lint nCount = 0;
	auto it = m_mapGateInfo.begin();
	for (; it != m_mapGateInfo.end(); ++it)
	{
		auto itor = it->second.begin();
		for (; itor != it->second.end(); ++itor)
		{
			nCount += itor->second.m_userCount;
		}
	}

	ss << "{\"count\":" << nCount << "},";

	Lstring tmp = ss.str().substr(0, ss.str().size() - 1);
	tmp += "]}";

	SendRet(tmp, sp);
}

void Work::HanderGMCloseServer(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_DEBUG("Work::HanderGMCloseServer %s", param["cmd"].c_str());
	if (param["cmd"] == "close_server_ty_ll")
	{
		SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
		Stop();

	}
	else
	{
		SendRet("{\"errorCode\":10,\"errorMsg\":\"close cmd error\"}", sp);//成功
	}
}

void Work::HanderSetFreeSet(std::map<Lstring,Lstring>& param,LSocketPtr sp)
{
	if(param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderSetGameFree is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"GameFree centent fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderSetFreeSet %s", param["content"].c_str());

	m_strActiveFreeSet = param["content"];
	if(!m_RedisClient.writeDataByString(ActiveFreeSetKey, m_strActiveFreeSet))
	{
		LLOG_ERROR("Work::HanderSetFreeSet sets storage Redis failed, please check Redis-server");
	}

	if (param["serverid"].empty())
	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); ++itLogicM)
		{
			LMsgCE2LSetGameFree free;
			free.m_ServerID = itLogicM->first;
			free.m_strFreeSet = m_strActiveFreeSet;
			itLogicM->second->Send(free.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetGameFree free;
		free.m_ServerID = nServerID;
		free.m_strFreeSet = m_strActiveFreeSet;
		SendMsgToLogic(free, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderSetPXActive(std::map<Lstring,Lstring>& param,LSocketPtr sp)
{
	if(param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderSetPXActive is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"PXActive content fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderSetPXActive %s", param["content"].c_str());

	m_strActivePXSet = param["content"];
	if(!m_RedisClient.writeDataByString(ActivePXSetKey, m_strActivePXSet))
	{
		LLOG_ERROR("Work::HanderSetPXActive sets storage Redis failed, please check Redis-server");
	}

	if (param["serverid"].empty())
	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); ++itLogicM)
		{
			LMsgCE2LSetPXActive active;
			active.m_ServerID = itLogicM->first;
			active.m_strActiveSet = m_strActivePXSet;
			itLogicM->second->Send(active.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetPXActive active;
		active.m_ServerID = nServerID;
		active.m_strActiveSet = m_strActivePXSet;
		SendMsgToLogic(active, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderSetOUGCActive(std::map<Lstring,Lstring>& param,LSocketPtr sp)
{
	if(param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderSetOUGCActive is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"HanderSetOUGCActive content fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderSetOUGCActive %s", param["content"].c_str());

	m_strActiveOUGCSet = param["content"];
	if(!m_RedisClient.writeDataByString(ActiveOUGCSetKey, m_strActiveOUGCSet))
	{
		LLOG_ERROR("Work::HanderSetOUGCActive sets storage Redis failed, please check Redis-server");
	}

	if (param["serverid"].empty())
	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); ++itLogicM)
		{
			LMsgCE2LSetOUGCActive active;
			active.m_ServerID = itLogicM->first;
			active.m_strActiveSet = m_strActiveOUGCSet;
			itLogicM->second->Send(active.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetOUGCActive active;
		active.m_ServerID = nServerID;
		active.m_strActiveSet = m_strActiveOUGCSet;
		SendMsgToLogic(active, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderSetExchange( std::map<Lstring,Lstring>& param,LSocketPtr sp )
{
	if(param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderSetExchange is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"DrawActive content fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderSetExchange %s", param["content"].c_str());

	m_strActiveExchangeSet = param["content"];
	if(!m_RedisClient.writeDataByString(ActiveExchangeSetKey, m_strActiveExchangeSet))
	{
		LLOG_ERROR("Work::HanderSetExchange sets storage Redis failed, please check Redis-server");
	}

	if (param["serverid"].empty())
	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); ++itLogicM)
		{
			LMsgCE2LSetExchActive active;
			active.m_ServerID = itLogicM->first;
			active.m_strActiveSet = m_strActiveExchangeSet;
			itLogicM->second->Send(active.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetExchActive active;
		active.m_ServerID = nServerID;
		active.m_strActiveSet = m_strActiveExchangeSet;
		SendMsgToLogic(active, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderActivityDrawSet(std::map<Lstring,Lstring>& param,LSocketPtr sp)
{
	if(param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderActivityShareSet is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"DrawActive content fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderActivityShareSet %s", param["content"].c_str());

	Lstring strSet = param["content"];

	if (param["serverid"].empty())
	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); ++itLogicM)
		{
			LMsgCE2LSetActivity active;
			active.m_ServerID = itLogicM->first;
			active.m_strActivity = strSet;
			active.m_activityId = ActivityID_Draw;
			itLogicM->second->Send(active.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetActivity active;
		active.m_ServerID = nServerID;
		active.m_strActivity = strSet;
		active.m_activityId = ActivityID_Draw;
		SendMsgToLogic(active, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderActivityShareSet( std::map<Lstring,Lstring>& param,LSocketPtr sp )   //分享送房卡
{
	if(param.find("content") == param.end())
	{
		LLOG_ERROR("Work::HanderActivityShareSet is null");
		SendRet("{\"errorCode\":6,\"errorMsg\":\"DrawActive content fail\"}", sp);//失败
		return;
	}
	LLOG_ERROR("Work::HanderActivityShareSet %s", param["content"].c_str());

	Lstring strShareSet = param["content"];

	if (param["serverid"].empty())
	{
		boost::mutex::scoped_lock l(m_mutexLogicManager);
		for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); ++itLogicM)
		{
			LMsgCE2LSetActivity active;
			active.m_ServerID = itLogicM->first;
			active.m_strActivity = strShareSet;
			active.m_activityId = ActivityID_Share;
			itLogicM->second->Send(active.GetSendBuff());
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		LMsgCE2LSetActivity active;
		active.m_ServerID = nServerID;
		active.m_strActivity = strShareSet;
		active.m_activityId = ActivityID_Share;
		SendMsgToLogic(active, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderFreezeUser(std::map< Lstring, Lstring >& param, LSocketPtr sp)
{
	// admin=admin uid=1 value=-1
	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(atoi(param["uid"].c_str()));			// user表的Id字段
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"user not exiest\"}", sp);//玩家不存在
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	Lint old = user->m_usert.m_status;

	Lint v = atoi(param["value"].c_str());

	user->m_usert.m_status = v;
	user->Save();

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功

	LLOG_ERROR("Work::HanderFreezeUser %s:%s %d->%d", param["uid"].c_str(), param["admin"].c_str(), old, v);
}

void Work::SaveCurrentOnline()
{
	boost::mutex::scoped_lock l(m_mutexGateInfo);

	auto iServer = m_mapGateInfo.begin();
	for (; iServer != m_mapGateInfo.end(); ++iServer)
	{
		Lint nCount = 0;
		auto iGate = iServer->second.begin();
		for (; iGate != iServer->second.end(); ++iGate)
		{
			nCount += iGate->second.m_userCount;
		}


		std::stringstream ss;	
		ss << "insert into onlinelog(DateTime, serverID, serverName, onlineCount) values('";
		ss << gWork.GetCurTime().Secs() << "','";
		ss << iServer->first << "','";
		ss << string_To_UTF8(getServerNameByID(iServer->first))  << "','";
		ss << nCount << "')";

		Lstring* sql = new Lstring(ss.str());
		m_dbServer.Push(sql);
	}
}

bool Work::isLogicManagerExist(Lint logicManagerID)
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);
	auto itLogicM = m_mapLogicManagerSp.find(logicManagerID);
	if(itLogicM == m_mapLogicManagerSp.end())
	{
		return false;
	}

	return true;
}

bool Work::isLogicManagerExist(LSocketPtr sp)
{
	boost::mutex::scoped_lock l(m_mutexLogicManager);
	for(auto itLogicM = m_mapLogicManagerSp.begin(); itLogicM != m_mapLogicManagerSp.end(); ++itLogicM)
	{
		if(itLogicM->second == sp)
		{
			return true;
		}
	}

	return false;
}

void Work::_initTickTime()
{
	LTime cur;

	m_30SceTick = cur.MSecs();
}

bool Work::_initRedisAndActive()
{
	if(m_RedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort()))
	{
		if(!m_RedisClient.readDataByString(ActiveFreeSetKey, m_strActiveFreeSet))
		{
			m_strActiveFreeSet = "";
			LLOG_ERROR("Work::_initRedisAndActive ActiveFreeSet get from redis is failed, now set is empty");
		}
		else
		{
			LLOG_ERROR("ActiveFreeSet is %s", m_strActiveFreeSet.c_str());
		}

		if(!m_RedisClient.readDataByString(ActivePXSetKey, m_strActivePXSet))
		{
			m_strActivePXSet = "";
			LLOG_ERROR("Work::_initRedisAndActive ActivePXSet get from redis is failed, now set is empty");
		}
		else
		{
			LLOG_ERROR("ActivePXSet is %s", m_strActivePXSet.c_str());
		}

		if(!m_RedisClient.readDataByString(ActiveOUGCSetKey, m_strActiveOUGCSet))
		{
			m_strActiveOUGCSet = "";
			LLOG_ERROR("Work::_initRedisAndActive ActiveOUGCSet get from redis is failed, now set is empty");
		}
		else
		{
			LLOG_ERROR("ActiveOUGCSet is %s", m_strActiveOUGCSet.c_str());
		}

		if(!m_RedisClient.readDataByString(ActiveExchangeSetKey, m_strActiveExchangeSet))
		{
			m_strActiveExchangeSet = "";
			LLOG_ERROR("Work::_initRedisAndActive ActiveOUGCSet get from redis is failed, now set is empty");
		}
		else
		{
			LLOG_ERROR("ActiveExchangeSetKey is %s", m_strActiveExchangeSet.c_str());
		}

		return true;
	}
	else
	{
		m_strActiveFreeSet = "";
		m_strActivePXSet = "";
		m_strActiveOUGCSet = "";
		m_strActiveExchangeSet = "";
		return false;
	}
}

void Work::SendActiveSetToLM(Lint LMid)
{
	// 限时免费活动
	LMsgCE2LSetGameFree FreeActive;
	FreeActive.m_ServerID = LMid;
	FreeActive.m_strFreeSet = m_strActiveFreeSet;
	SendMsgToLogic(FreeActive, LMid);

	// 牌型活动
	LMsgCE2LSetPXActive PXActive;
	PXActive.m_ServerID = LMid;
	PXActive.m_strActiveSet = m_strActivePXSet;
	SendMsgToLogic(PXActive, LMid);

	// 老用户登录送房卡活动
	LMsgCE2LSetOUGCActive OUGCActive;
	OUGCActive.m_ServerID = LMid;
	OUGCActive.m_strActiveSet = m_strActiveOUGCSet;
	SendMsgToLogic(OUGCActive, LMid);

	LMsgCE2LSetExchActive ExchActive;
	ExchActive.m_ServerID = LMid;
	ExchActive.m_strActiveSet = m_strActiveExchangeSet;
	SendMsgToLogic(ExchActive,LMid);
}

bool Work::LoadAgentList()
{
	LDBSession dbSession;
	if(!dbSession.Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort()))
	{
		return false;
	}

	std::stringstream sql;
	sql << "select Id from agent";

	if(mysql_real_query(dbSession.GetMysql(), sql.str().c_str(), sql.str().size()))
	{
		LLOG_ERROR("load agent sql error %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(dbSession.GetMysql());
	if(res == nullptr)
	{
		LLOG_ERROR("Fail to store result. Error = %s", mysql_error(dbSession.GetMysql()));
		return false;
	}

	MYSQL_ROW row;
	while(row = mysql_fetch_row(res))
	{
		Lint agentId = atoi(*row++);
		m_agentList.insert(agentId);
	}

	mysql_free_result(res);

	LLOG_ERROR("load agent count:%d", m_agentList.size());

	return true;
}

bool Work::IsExistAgent(const Lint id)
{
	boost::mutex::scoped_lock l(m_mutexAgentList);
	auto it = m_agentList.find(id);
	return it != m_agentList.end();
}
