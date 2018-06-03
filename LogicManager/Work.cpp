#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "LMsgS2S.h"
#include "UserManager.h"
#include "User.h"
#include "ActiveManager.h"
#include <openssl/md5.h>
#include "DeskManager.h"
#include "LVipLog.h"
#include <regex>
#include "UserMessageMsg.h"
#include "RuntimeInfoMsg.h"
#include "RLogHttp.h"

using namespace boost::asio;  
using boost::asio::ip::tcp; 

#define WEEKMSECONDS	(RANK_REWARD_INTERVAL*1000)

//初始化
bool Work::Init()
{
	//设置log文件
	LLog::Instance().SetFileName("LogicManager");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}
	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	if(!gRuntimeInfoMsg.Init())
	{
		LLOG_ERROR("Fail to init runtime info");
		return false;
	}

	if (!gDeskManager.Init())
	{
		LLOG_ERROR("DeskManager.Init error");
		return false;
	}

	if (!gActiveManager.Init())
	{
		LLOG_ERROR("gActiveManager.Init error");
		return false;
	}

	if( !gRLT.Init( gConfig.GetRemoteLogStart() !=0, gConfig.GetRemoteLogUrl(), (size_t)gConfig.GetMaxCachedLogSize(), (size_t)gConfig.GetMaxCachedLogNum() ) )
	{
		LLOG_ERROR("gRLT.Init error");
		return false;
	}

	LMsgFactory::Instance().Init();

	if (!gInsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}

	_initTickTime();
	_checkLogicServerOnlySupportGameType();

	_checkRankRewardConfig();

	m_bHadLoadedData = false;
	return true;
}

bool Work::Final()
{
	return true;
}

//启动
void Work::Start()
{
	ConnectToCenter();

	ConnectToDb();

	//玩家消息线程开启
	gUserMessageMsg.Start();

	gRLT.Start();

	gInsideNet.Start();

	m_memeoryRecycle.Start();

	LRunnable::Start();

// 	_checkRank();
// 	_checkLastWeekRank();
}

//等待
void Work::Join()
{
	//等待玩家消息线程终止
	gUserMessageMsg.Join();

	gRLT.Join();

	gInsideNet.Join();

	m_memeoryRecycle.Join();

	//等待逻辑线程终止
	LRunnable::Join();
}

//停止
void Work::Stop()
{
	m_centerClient.reset();

	m_gateInfo.clear();

	gUserMessageMsg.Stop();

	gRLT.Stop();

	gInsideNet.Stop();

	m_memeoryRecycle.Stop();

	LRunnable::Stop();
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

	//LLOG_DEBUG("Work recv msg:%d", pMessage->m_msgId);

	switch(pMessage->m_msgId)
	{
	/*
	Center to LogicManager
	*/
	case MSG_CE_2_L_USER_LOGIN:
		{
			LMsgCe2LUserLogin* pLoginMsg = (LMsgCe2LUserLogin*)pMessage;
			if(pLoginMsg)
			{
				gUserMessageMsg.handlerMessage(pLoginMsg->user.m_unioid, pLoginMsg);
			}
			break;
		}
	case MSG_CE_2_L_GM_CHARGE:
		{
			LMsgCe2LGMCharge* pChargeMsg = (LMsgCe2LGMCharge*)pMessage;
			if(pChargeMsg)
			{
				gUserMessageMsg.handlerMessage(pChargeMsg->m_strUUID, pChargeMsg);
			}
			break;
		}
	case MSG_CE_2_L_GM_COINS:
		{
			LMsgCe2LGMCoins* pCoinsMsg = (LMsgCe2LGMCoins*)pMessage;
			if(pCoinsMsg)
			{
				gUserMessageMsg.handlerMessage(pCoinsMsg->m_strUUID, pCoinsMsg);
			}
			break;
		}
	case MSG_CE_2_LM_ADD_AGENT:
		{
			LMsgCE2LMGAddAgent* pCoinsMsg = (LMsgCE2LMGAddAgent*)pMessage;
			if(pCoinsMsg)
			{
				gUserMessageMsg.handlerMessage(pCoinsMsg->m_strUUID, pCoinsMsg);
			}
			break;
		}

	/*
	Gate to LogicManager
	*/
	case MSG_G_2_L_USER_MSG:
		{
			LMsgG2LUserMsg* pUserMsg = (LMsgG2LUserMsg*)pMessage;
			if(pUserMsg)
			{
				gUserMessageMsg.handlerMessage(pUserMsg->m_strUUID, pUserMsg);
			}
			break;
		}
	case MSG_G_2_L_USER_OUT_MSG:
		{
			LMsgG2LUserOutMsg* pUserOutMsg = (LMsgG2LUserOutMsg*)msg;
			if(pUserOutMsg)
			{
				gUserMessageMsg.handlerMessage(pUserOutMsg->m_strUUID, pUserOutMsg);
			}
			break;
		}


	/*
	LogicServer or CoinsServer to LogicManager
	*/
	case MSG_L_2_LMG_MODIFY_USER_STATE:
		{
			LMsgL2LMGModifyUserState* pStateMsg = (LMsgL2LMGModifyUserState*)pMessage;
			if(pStateMsg)
			{
				gUserMessageMsg.handlerMessage(pStateMsg->m_strUUID, pStateMsg);
			}
			break;
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
			LMsgL2LMGAddUserPlayCount* pPlayCountMsg = (LMsgL2LMGAddUserPlayCount*)pMessage;
			if(pPlayCountMsg)
			{
				gUserMessageMsg.handlerMessage(pPlayCountMsg->m_strUUID, pPlayCountMsg);
			}
			break;
		}

	/*
	DB to LogicManager
	*/
	case MSG_LDB_2_L_VIP_LOG:
		{
			LMsgLBD2LReqVipLog* pVipLogMsg = (LMsgLBD2LReqVipLog*)pMessage;
			if(pVipLogMsg)
			{
				gUserMessageMsg.handlerMessage(pVipLogMsg->m_strUUID, pVipLogMsg);
			}
			break;
		}
	case MSG_LDB_2_LM_RET_CRELOGHIS:
		{
			LMsgLDB2LM_RecordCRELog* pCreLogHisMsg = (LMsgLDB2LM_RecordCRELog*)pMessage;
			if(pCreLogHisMsg)
			{
				gUserMessageMsg.handlerMessage(pCreLogHisMsg->m_strUUID,pCreLogHisMsg);
			}
			break;
		}
	case MSG_LDB_2_L_ROOM_LOG:
		{
			LMsgLBD2LReqRoomLog* pRoomLogMsg = (LMsgLBD2LReqRoomLog*)pMessage;
			if(pRoomLogMsg)
			{
				gUserMessageMsg.handlerMessage(pRoomLogMsg->m_strUUID, pRoomLogMsg);
			}
			break;
		}
	case MSG_LDB_2_L_VIDEO:
		{
			LMsgLDB2LReqVideo* pReqVideoMsg = (LMsgLDB2LReqVideo*)pMessage;
			if(pReqVideoMsg)
			{
				gUserMessageMsg.handlerMessage(pReqVideoMsg->m_strUUID, pReqVideoMsg);
			}
			break;
		}
	case MSG_LDB_2_LMG_REQ_BINDING_RELATIONSHIPS:
		{
			LMsgLDB2LMGBindingRelationships* pRelationshipsMsg = (LMsgLDB2LMGBindingRelationships*)pMessage;
			if(pRelationshipsMsg)
			{
				gUserMessageMsg.handlerMessage(pRelationshipsMsg->m_strUUID, pRelationshipsMsg);
			}
			break;
		}
	case MSG_LDB_2_LM_RET_INFO:
		{
			LMsgLDB_2_LM_RetInfo* pRetInfoMsg = (LMsgLDB_2_LM_RetInfo*)pMessage;
			if(pRetInfoMsg)
			{
				gUserMessageMsg.handlerMessage(pRetInfoMsg->m_strUUID, pRetInfoMsg);
			}
			break;
		}
	case MSG_LDB_2_LMG_USER_MSG:
		{
			LMsgLDB2LMGUserMsg* pUserMsg = (LMsgLDB2LMGUserMsg*)pMessage;
			if(pUserMsg)
			{
				gUserMessageMsg.handlerMessage(pUserMsg->m_strUUID, pUserMsg);
			}
			break;
		}
	/*
	CoinsServer to LogicManager
	*/
	case MSG_CN_2_LMG_MODIFY_USER_COINS:
		{
			LMsgCN2LMGModifyUserCoins* pActiveInfoMsg = (LMsgCN2LMGModifyUserCoins*)pMessage;
			if(pActiveInfoMsg)
			{
				gUserMessageMsg.handlerMessage(pActiveInfoMsg->m_strUUID, pActiveInfoMsg);
			}
			break;
		}
	/*
	Other message
	*/
	default:
		LRunnable::Push(msg);
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
				LLOG_ERROR("10000 package needs %ld time", now.MSecs() - msgTime.MSecs() - iElapseTime);

				iMsgCount = 0;
				iElapseTime = 0;

				msgTime = now;
			}
		}
	}
}

void Work::Tick(LTime& cur)
{
	//1000毫秒循环一次
	if (cur.MSecs() - m_1SceTick > 1000)
	{
		m_1SceTick = cur.MSecs();
	}

	//5秒循环一次
	if(cur.MSecs() - m_5SecTick > 5000)
	{
		m_5SecTick = cur.MSecs();
		_checkLogicServerState();
		_checkCoinsServerState();
	}

	//15秒循环一次
	if (cur.MSecs() - m_15SceTick > 15000)
	{
		m_15SceTick = cur.MSecs();
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();
		SendGateInfoToCenter();

		if(m_bHadLoadedData)
		{
			_checkCenterHeartBeat();
			_checkLogicDBHeartBeat();
		}
		_checkFreeModel();
		_checkExchActive();
	}

	//三分钟循环一次
	if(cur.MSecs() - m_180SecTick > 3 * 60 * 1000)
	{
		m_180SecTick = cur.MSecs();
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
	{
		m_600SceTick = cur.MSecs();
		_checkAndTouchLogicServer();
		_checkLogicServerOnlySupportGameType();
		_checkRLogStart();

		_checkRankRewardConfig();
	}

	//每小时整点
	if(cur.MSecs() - m_1HourTick > 3600*1000)
	{
		m_1HourTick += 3600*1000;
		_checkRank();
	}

	//每周开始时间
	if(cur.MSecs() - m_1WeekTick > WEEKMSECONDS)
	{
		m_1WeekTick += WEEKMSECONDS;
		_checkLastWeekRank();
	}

	//每月
	if(cur.GetMonth() != m_1MonthTick.GetMonth())
	{
		m_1MonthTick = cur;
		//_checkLastMonthRank();
	}
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	//玩家请求登录
	switch(msg->m_msgId)
	{
	case MSG_CLIENT_KICK:
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;

	//////////////////////////////////////////////////////////////////////////
	//center 跟 logicmanager之间的交互
	case MSG_CE_2_L_USER_ID_INFO:
		HanderCenterUserInInfo((LMsgCe2LUserIdInfo*)msg);
		break;
	case MSG_CE_2_L_GM_HORSE://center发送gm跑马灯
		HanderCenterGMHorse((LMsgCe2LGMHorse*) msg);
		break;
	case MSG_CE_2_L_GM_BUYINFO:
		HanderCenterGMBuyInfo((LMsgCe2LGMBuyInfo*) msg);
		break;
	case MSG_CE_2_L_GM_HIDE:
		HanderCenterGMHide((LMsgCe2LGMHide*) msg);
		break;
	case MSG_CE_2_L_SET_GAME_FREE:
		HanderLogicSetGameFree((LMsgCE2LSetGameFree*) msg);
		break;
	case MSG_CE_2_L_SET_PXACTIVE:
		HanderLogicSetPXActive((LMsgCE2LSetPXActive*)msg);
		break;
	case MSG_CE_2_L_SET_OUGCACTIVE:
		HanderLogicSetOUGCActive((LMsgCE2LSetOUGCActive*)msg);
		break;
	case MSG_CE_2_L_SET_EXCHACTIVE:
		HanderLogicSetExchActive((LMsgCE2LSetExchActive*) msg);
		break;
	case MSG_CE_2_L_SET_ACTIVITY:
		HanderLogicSetActivity((LMsgCE2LSetActivity*) msg);
		break;
	case MSG_CE_2_LM_RANK:
		HanderRank((LMsgCE2LMGRank*)msg);
		break;


		//////////////////////////////////////////////////////////////////////////
		//logic 跟 logicmanager之间的交互
	case MSG_L_2_LMG_LOGIN:
		HanderLogicLogin((LMsgL2LMGLogin*)msg);
		break;
	case MSG_L_2_LMG_RECYLE_DESKID:
		{
			LMsgL2LMGRecyleDeskID* pRecyleMsg = (LMsgL2LMGRecyleDeskID*)msg;
			if(pRecyleMsg)
			{
				Lint iLogicServerId = gDeskManager.RecycleDeskId(pRecyleMsg->m_deskID);
				if(iLogicServerId != INVALID_LOGICSERVERID)
				{
					delDeskCountOnLogicServer(iLogicServerId);
				}
			}

			break;
		}
		
		//////////////////////////////////////////////////////////////////////////
		//gate 跟 logicmanager之间的交互
	case MSG_G_2_LMG_LOGIN:
		HanderGateLogin((LMsgG2LMGLogin*)msg);
		break;
	case MSG_HEARDBEAT_REQUEST:
		HanderHeartBeatRequest((LMsgHeartBeatRequestMsg*)msg);
		break;
	case MSG_HEARDBEAT_REPLY:
		HanderHearBeatReply((LMsgHeartBeatReplyMsg*)msg);
		break;
	case MSG_CN_2_LMG_LOGIN:
		HanderCoinsLogin((LMsgCN2LMGLogin*)msg);
		break;
	case MSG_CN_2_LMG_FREE_DESK_REQ:
		HanderCoinFreeDeskReq((LMsgCN2LMGFreeDeskReq*)msg);
		break;
	case MSG_CN_2_LMG_RECYCLE_DESK:
		HanderCoinRecycleDesk((LMsgCN2LMGRecycleDesk*)msg);
		break;
	default:
		break;
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if (msg->m_sp == m_centerClient)
	{
		_reconncetCenter();
	}
	else if(msg->m_sp == m_dbClient)
	{
		_reconnectDB();
	}
	else if( m_coinsServer.m_sp && msg->m_sp == m_coinsServer.m_sp)
	{
		// 金币服务器断开
		HanderCoinsLogout(msg);
	}
	else
	{
		HanderLogicLogout(msg);
		HanderGateLogout(msg);
	}
}

void Work::HanderClientIn(LMsgIn* msg)
{
	if(msg == NULL)
	{
		return;
	}

	if(m_dbClient == msg->m_sp)
	{
		LMsgLMG2LdbLogin msg;
		msg.m_key = gConfig.GetDBKey();
		msg.m_serverID = gConfig.GetServerID();
		SendMsgToDb(msg);
	}
	else if(m_centerClient == msg->m_sp)
	{
		LMsgL2CeLogin login;
		login.m_ID = gConfig.GetServerID();
		login.m_key = gConfig.GetCenterKey();
		login.m_ServerName = gConfig.GetServerName();
		login.m_needLoadUserInfo = m_bHadLoadedData ? 0 : 1;
		SendToCenter(login);

		if(m_bHadLoadedData)
		{
			SendGateInfoToCenter();
		}
	}

	m_mapReconnect.erase(msg->m_sp);
}

void Work::ConnectToCenter()
{
	m_centerClient = gInsideNet.GetNewSocket();
	LLOG_ERROR("Work::ConnectCenter begin %s:%d", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
	if(m_centerClient->Connect(gConfig.GetCenterIp(), gConfig.GetCenterPort()))
	{
		LLOG_ERROR("Work::ConnectCenter end %s:%d", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
	}
	else
	{
		LLOG_ERROR("Fail to connect center");
		Stop();
	}
}

void Work::SendToCenter(LMsg& msg)
{
	boost::mutex::scoped_lock l(m_mutexCenterClient);

	if(m_centerClient && m_centerClient->getSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED)
	{
		m_centerClient->Send(msg.GetSendBuff());
	}
}

void Work::HanderCenterUserInInfo(LMsgCe2LUserIdInfo*msg)
{
	if(msg == NULL)
	{
		return;
	}

	for(Lint i = 0; i < msg->m_count; ++i)
	{
		boost::shared_ptr<UserBaseInfo> baseInfo(new UserBaseInfo(msg->m_info[i]));
		gUserManager.addUserBaseInfo(baseInfo);
	}

	if(msg->m_hasSentAll == 1)
	{
		m_bHadLoadedData = true;

		// 当玩家都加载完成后再获取排行榜
		_checkRank();
		_checkLastWeekRank();
	}
}

//center GM设置跑马灯
void Work::HanderCenterGMHorse(LMsgCe2LGMHorse* msg)
{
	if(msg == NULL)
	{
		return;
	}

	LLOG_DEBUG("Work::HanderCenterGMHorse %s",msg->m_str.c_str());
	
	gRuntimeInfoMsg.setHorseInfoAndNotify(msg->m_str);
}
void Work::HanderCenterGMBuyInfo(LMsgCe2LGMBuyInfo* msg)
{
	LLOG_DEBUG("Work::HanderCenterGMBuyInfo %s", msg->m_str.c_str());
	gRuntimeInfoMsg.setBuyInfo(msg->m_str);
}

void Work::HanderCenterGMHide(LMsgCe2LGMHide* msg)
{
	gRuntimeInfoMsg.setHide(msg->m_hide);
}

void Work::HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg)
{
	if(msg == NULL)
	{
		return;
	}

	int fromwho = msg->m_fromWho;
	if ( fromwho < HEARBEAT_WHO_UNKNOWN || fromwho > HEARBEAT_WHO_NUM )
	{
		fromwho = HEARBEAT_WHO_UNKNOWN;
	}
	LLOG_DEBUG("Request heartbeat from %s -- %d", HearBeat_Name[fromwho], msg->m_iServerId);

	LMsgHeartBeatReplyMsg msgReply;
	msgReply.m_fromWho = HEARBEAT_WHO_LOGICMANAGER;
	msgReply.m_iServerId = 0;

	msg->m_sp->Send(msgReply.GetSendBuff());
}

void Work::HanderHearBeatReply(LMsgHeartBeatReplyMsg* msg)
{
	if(msg == NULL)
	{
		return;
	}

	int fromwho = msg->m_fromWho;
	if ( fromwho < HEARBEAT_WHO_UNKNOWN || fromwho > HEARBEAT_WHO_NUM )
	{
		fromwho = HEARBEAT_WHO_UNKNOWN;
	}
	LLOG_DEBUG("Reply heartbeat from %s -- %d", HearBeat_Name[fromwho], msg->m_iServerId);

	m_mapHeartBeat.erase(msg->m_sp);
}

void Work::HanderGateLogin(LMsgG2LMGLogin* msg)
{
	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}

	{
		boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

		auto itGateInfo = m_gateInfo.find(msg->m_id);
		if(itGateInfo != m_gateInfo.end()) //重连
		{
			LLOG_ERROR("Gate reconnect logic manager. Id = %d", itGateInfo->second.m_id);
			itGateInfo->second.m_sp->Stop();
			m_gateInfo.erase(itGateInfo);
		}


		LLOG_ERROR("Work::HanderGateLogin Succeed! %d", msg->m_id);

		//广播GATE消息
		GateInfo info;
		info.m_id = msg->m_id;
		info.m_ip = msg->m_ip;
		info.m_port = msg->m_port;
		info.m_userCount = 0;
		info.m_sp = msg->m_sp;
		m_gateInfo[info.m_id] = info;
	}

	//同步Logic消息
	SendLogicInfoToGates(msg->m_id);

	SendGateInfoToCenter();

	SendCoinsInfoToGates(msg->m_id);
}

void Work::HanderGateLogout(LMsgKick* msg)
{
	int iDelId = -1;
	
	{
		boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);
		for(auto itGateInfo = m_gateInfo.begin(); itGateInfo != m_gateInfo.end(); ++itGateInfo)
		{
			if(itGateInfo->second.m_sp == msg->m_sp)
			{
				iDelId = itGateInfo->first;
				m_gateInfo.erase(itGateInfo);
				break;
			}
		}
	}

	if(iDelId != -1)
	{
		LLOG_ERROR("Work::HanderGateLogout Succeed! %d", iDelId);
		SendGateInfoToCenter();
	}
}

void Work::SendGateInfoToCenter()
{
	LMsgL2CeGateInfo info;
	info.m_ID = gConfig.GetServerID();
	info.m_count = 0;

	{
		boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

		for(auto itGate = m_gateInfo.begin(); itGate != m_gateInfo.end(); ++itGate)
		{
			itGate->second.m_userCount = gRuntimeInfoMsg.getOnlineNum(itGate->first);
			info.m_gate[info.m_count++] = (itGate->second);
		}
	}
	
	SendToCenter(info);
}

Lint Work::getGateIdBySp(LSocketPtr sp)
{
	boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

	for(auto itGateInfo = m_gateInfo.begin(); itGateInfo != m_gateInfo.end(); ++itGateInfo)
	{
		if(itGateInfo->second.m_sp == sp)
		{
			return itGateInfo->first;
		}
	}

	return INVALID_GATEID;
}

void Work::SendMessageToAllGate(LMsg& msg)
{
	boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

	auto gate = m_gateInfo.begin();
	for (; gate != m_gateInfo.end(); ++gate)
	{
		gate->second.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::SendMessageToGate(Lint gateID, LMsg& msg)
{
	boost::recursive_mutex::scoped_lock l(m_mutexGateInfo);

	auto gate = m_gateInfo.find(gateID);
	if ( gate != m_gateInfo.end())
	{
		gate->second.m_sp->Send(msg.GetSendBuff());
	}
}

//////////////////////////////////////////////////////////////////////////
//DB链接相关
void Work::ConnectToDb()
{
	m_dbClient = gInsideNet.GetNewSocket();
	LLOG_ERROR("Work::ConnectToDb begin %s:%d", gConfig.GetDBIp().c_str(), gConfig.GetDBPort());
	if(m_dbClient->Connect(gConfig.GetDBIp(), gConfig.GetDBPort()))
	{
		LLOG_ERROR("Connect DB successfully");
	}
	else
	{
		LLOG_ERROR("Fail to connect db");
		Stop();
	}
}

void Work::SendMsgToDb(LMsg& msg)
{
	boost::mutex::scoped_lock l(m_mutexDBClient);

	if(m_dbClient && m_dbClient->getSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED)
	{
		m_dbClient->Send(msg.GetSendBuff());
	}
}

////////////////
void Work::HanderLogicLogin(LMsgL2LMGLogin* msg)
{
	if(msg == NULL)
	{
		return;
	}

	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}

	{
		boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);

		auto itLogic = m_logicServerInfo.find(msg->m_id);
		if(itLogic != m_logicServerInfo.end())
		{
			itLogic->second.m_logicInfo.m_sp->Stop();
			m_logicServerInfo.erase(itLogic);
		}

		//广播Logic消息
		LLOG_ERROR("Work::HanderLogicLogin Succeed! %d", msg->m_id);
		LOGIC_SERVER_INFO logicInfo;

		logicInfo.m_logicInfo.m_id = msg->m_id;
		logicInfo.m_logicInfo.m_ip = msg->m_ip;
		logicInfo.m_logicInfo.m_port = msg->m_port;
		logicInfo.m_logicInfo.m_deskCount = 0;
		logicInfo.m_logicInfo.m_sp = msg->m_sp;
		logicInfo.m_logicInfo.m_flag = 0;
		logicInfo.m_tickTime = 0;
		m_logicServerInfo[logicInfo.m_logicInfo.m_id] = logicInfo;
		gActiveManager.PXSendConfig(logicInfo.m_logicInfo.m_id);		// logic登录，发送牌型活动信息
	}
	
	SendLogicInfoToGates();

	SendRLogInfoToLogic( msg->m_id );

	SendCoinsInfoToLogic( msg->m_id );
}

void Work::HanderLogicLogout(LMsgKick* msg)
{
	if (msg == NULL)
	{
		return;
	}

	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);
	for(auto itLogic = m_logicServerInfo.begin(); itLogic != m_logicServerInfo.end(); ++itLogic)
	{
		if(itLogic->second.m_logicInfo.m_sp == msg->m_sp)
		{
			LLOG_ERROR("%d Logic server is kicked", itLogic->second.m_logicInfo.m_id);

			//仅仅只是设置踢出时间，然后3分钟都连接不上才进行删除
			LTime curTime;
			itLogic->second.m_tickTime = curTime.MSecs();
		}
	}
}

void Work::_checkLogicServerState()
{
	bool bDelLogic = false;

	{
		boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);

		LTime cur;
		for(auto itLogic = m_logicServerInfo.begin(); itLogic != m_logicServerInfo.end();)
		{
			if(itLogic->second.m_tickTime == 0)
			{
				++itLogic;
				continue;
			}

			Llong lDiff = cur.MSecs() - itLogic->second.m_tickTime;
			if(lDiff >= 3 * 60 * 1000)
			{
				gDeskManager.RecycleAllDeskIdOnLogicServer(itLogic->second.m_logicInfo.m_id);
				m_logicServerInfo.erase(itLogic++);
				bDelLogic = true;
			}
			else
			{
				++itLogic;
			}
		}
	}
	
	if(bDelLogic)
	{
		SendLogicInfoToGates();
	}
}

void Work::_checkAndTouchLogicServer()
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);

	if(m_logicServerInfo.empty())
		return;

	LIniConfig	m_ini;
	std::list<std::string> fields;
	m_ini.LoadFile("logic_manager_config.ini");
	Lstring line=m_ini.GetString("FailoverLogics","");

	std::vector<Lstring> des;
	L_ParseString(line, des, ",");

	//去除某些逻辑
	auto iBegin = m_logicServerInfo.begin();
	for (; iBegin != m_logicServerInfo.end(); ++iBegin)
	{
		iBegin->second.m_logicInfo.m_flag = 0;
		for (Lsize i = 0; i < des.size(); ++i)
		{
			if (atoi(des[i].c_str()) == iBegin->first)
			{
				iBegin->second.m_logicInfo.m_flag = -1;
				break;
			}
		}
	}
	
	//打印桌子数
	auto iLogic = m_logicServerInfo.begin();
	for (; iLogic != m_logicServerInfo.end(); ++iLogic)
	{
		LLOG_ERROR("LogicIndex: %d Current Desk: %d", iLogic->first, iLogic->second.m_logicInfo.m_deskCount);
	}
}

void Work::SendLogicInfoToGates(Lint nGateID)
{
	LMsgLMG2GateLogicInfo info;
	info.m_ID = gConfig.GetServerID();
	info.m_count = 0;

	{
		boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);

		for(auto itLogic = m_logicServerInfo.begin(); itLogic != m_logicServerInfo.end(); ++itLogic)
		{
			if(itLogic->second.m_tickTime != 0)
			{
				continue;
			}
			info.m_logic[info.m_count++] = itLogic->second.m_logicInfo;
		}
	}

	if (nGateID > 0)
	{
		SendMessageToGate(nGateID, info);
	}
	else
	{
		SendMessageToAllGate(info);
	}
}

void Work::SendRLogInfoToLogic(Lint nLogicID)
{
	RLogConfig cfg = gRLT.GetCoinfig();
	LMsgLMG2LRLogInfo info;
	info.m_managerID = gConfig.GetServerID();
	info.bopenrlog = cfg.bopenrlog;
	info.m_url = cfg.m_url;
	info.m_max_cached_size = cfg.m_max_cached_size;
	info.m_max_cached_num = cfg.m_max_cached_num;

	if (nLogicID > 0)
	{
		SendMessageToLogic(nLogicID, info);
	}
	else
	{
		SendMessageToAllLogic(info);
	}
}

void Work::HanderCoinsLogin(LMsgCN2LMGLogin* msg)
{
	LMsgLMG2CNLogin login;
	if (msg->m_key.empty())
	{
		login.m_result = 1;
		msg->m_sp->Send( login.GetSendBuff() );

		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderCoinsLogin key error %s", msg->m_key.c_str());
		return;
	}

	if ( m_coinsServer.m_sp )
	{
		if ( m_coinsServer.m_closeTickTime == 0 )
		{
			login.m_result = 2;
			msg->m_sp->Send( login.GetSendBuff() );

			msg->m_sp->Stop();
			// 金币服务器有连接 并且没有关闭倒计时，说明金币服务器重复连接了
			LLOG_ERROR("Work::HanderCoinsLogin CoinsServer already Exist");
			return;
		}
	}

	LLOG_ERROR("Work::HanderCoinsLogin Succeed! %s", msg->m_ip.c_str());

	// 桌子数不一致
	if ( gDeskManager.GetCoinsDeskSize() != msg->m_deskcount )
	{
		LLOG_ERROR("Work::HanderCoinsLogin desk count error, coinsserver count=%d, this count=%d", msg->m_deskcount, gDeskManager.GetCoinsDeskSize() );

		// 金币桌内的玩家状态 待完善

		// 回收金币桌子ID
		gDeskManager.RecycleCoinsDeskId();
	}

	m_coinsServer.m_ip = msg->m_ip;
	m_coinsServer.m_port = msg->m_port;
	m_coinsServer.m_sp = msg->m_sp;
	m_coinsServer.m_closeTickTime = 0;

	login.m_result = 0;
	login.m_deskcount = gDeskManager.GetCoinsDeskSize();
	SendMessageToCoinsServer( login );

	SendCoinsInfoToGates();
	SendCoinsInfoToLogic();
}

void Work::HanderCoinsLogout(LMsgKick* msg)
{
	if (msg == NULL || !m_coinsServer.m_sp || msg->m_sp != m_coinsServer.m_sp)
	{
		return;
	}

	//仅仅只是设置踢出时间，然后3分钟都连接不上才进行删除
	LTime curTime;
	m_coinsServer.m_closeTickTime = curTime.MSecs();
}

void Work::HanderCoinFreeDeskReq(LMsgCN2LMGFreeDeskReq* msg)
{
	LLOG_ERROR( "Work::HanderCoinFreeDeskReq req coindesk, reqcount=%d, count=%d", msg->m_count, gDeskManager.GetCoinsDeskSize() );

	std::vector<Lint> deskid;
	gDeskManager.GetFreeCoinDeskID( msg->m_count, deskid );

	LMsgLMG2CNFreeDeskReply reply;
	reply.m_gameType = msg->m_gameType;
	for ( auto it = deskid.begin(); it != deskid.end(); ++it )
	{
		Lint iLogicServerId = gWork.allocateLogicServer( msg->m_gameType );
		if(iLogicServerId == INVALID_LOGICSERVERID)
		{
			continue;
		}
		CoinsDesk desk;
		desk.m_id = *it;
		desk.m_logicID = iLogicServerId;
		reply.m_desk.push_back( desk );
	}
	SendMessageToCoinsServer( reply );
}

void Work::HanderCoinRecycleDesk(LMsgCN2LMGRecycleDesk* msg)
{
	gDeskManager.RecycleCoinsDeskId( msg->m_deskid );
}

void Work::_checkCoinsServerState()
{
	if ( !m_coinsServer.m_sp )
	{
		return;
	}

	if( m_coinsServer.m_closeTickTime == 0 )
	{
		return;
	}

	LTime cur;
	Llong lDiff = cur.MSecs() - m_coinsServer.m_closeTickTime;
	if(lDiff >= 3 * 60 * 1000)
	{
		m_coinsServer.m_sp.reset();
		m_coinsServer.m_ip = "";
		m_coinsServer.m_port = 0;
		m_coinsServer.m_closeTickTime = 0;

		// 金币桌内的玩家状态 待完善

		// 回收金币桌子ID
		gDeskManager.RecycleCoinsDeskId();

		SendCoinsInfoToGates();
		SendCoinsInfoToLogic();
	}
}

void Work::SendCoinsInfoToGates(Lint nGateID)
{
	LMsgLMG2GLCoinsServerInfo info;
	info.m_conis.m_ip = m_coinsServer.m_ip;
	info.m_conis.m_port = m_coinsServer.m_port;

	if (nGateID > 0)
	{
		SendMessageToGate(nGateID, info);
	}
	else
	{
		SendMessageToAllGate(info);
	}
}

void Work::SendCoinsInfoToLogic(Lint nLogicID)
{
	LMsgLMG2GLCoinsServerInfo info;
	info.m_conis.m_ip = m_coinsServer.m_ip;
	info.m_conis.m_port = m_coinsServer.m_port;

	if (nLogicID > 0)
	{
		SendMessageToLogic(nLogicID, info);
	}
	else
	{
		SendMessageToAllLogic(info);
	}
}

bool Work::IsCoinsServerConnected()
{
	if ( m_coinsServer.m_sp && m_coinsServer.m_sp->getSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED )
	{
		return true;
	}
	return false;
}

void Work::SendMessageToCoinsServer(LMsg& msg)
{
	if ( m_coinsServer.m_sp && m_coinsServer.m_sp->getSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED )
	{
		m_coinsServer.m_sp->Send(msg.GetSendBuff());
	}
}

bool Work::isLogicServerExist(int iLogicServerId)
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);
	auto itLogicServer = m_logicServerInfo.find(iLogicServerId);
	if(itLogicServer == m_logicServerInfo.end())
	{
		return false;
	}

	return true;
}

void Work::SendMessageToAllLogic(LMsg& msg)
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);
	auto itLogic = m_logicServerInfo.begin();
	for (; itLogic != m_logicServerInfo.end(); ++itLogic)
	{
		itLogic->second.m_logicInfo.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::SendMessageToLogic(Lint iLogicServerId, LMsg& msg)
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);
	auto itLogic = m_logicServerInfo.find(iLogicServerId);
	if(itLogic != m_logicServerInfo.end() && itLogic->second.m_tickTime == 0)
	{
		itLogic->second.m_logicInfo.m_sp->Send(msg.GetSendBuff());
	}
}

Lint Work::allocateLogicServer(int iGameType)
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);

	Lint nMiniCouunt = 9999999;
	bool bGameTypeIsLimited = false;

	auto itAllocLogic = m_logicServerInfo.end();
	for(auto itOnlySurpport = m_mapLogicServerOnlySupportType.begin(); itOnlySurpport != m_mapLogicServerOnlySupportType.end(); ++itOnlySurpport)
	{
		if(itOnlySurpport->second.find(iGameType) != itOnlySurpport->second.end())
		{
			auto itAllocLogicTmp = m_logicServerInfo.find(itOnlySurpport->first);
			if( itAllocLogicTmp != m_logicServerInfo.end()						&&
				itAllocLogicTmp->second.m_logicInfo.m_deskCount < nMiniCouunt	&&
				itAllocLogicTmp->second.m_logicInfo.m_flag != -1				&&
				itAllocLogicTmp->second.m_tickTime == 0)
			{
				nMiniCouunt = itAllocLogicTmp->second.m_logicInfo.m_deskCount;
				itAllocLogic = itAllocLogicTmp;
			}

			bGameTypeIsLimited = true;
		}
	}

	if(itAllocLogic != m_logicServerInfo.end())
	{
		itAllocLogic->second.m_logicInfo.m_deskCount++;
		return itAllocLogic->first;
	}

	if(bGameTypeIsLimited)	//如果已经被限制分配，则只能分配到规定的服务器中，如果没有此服务器则失败
	{
		return INVALID_LOGICSERVERID;
	}

	nMiniCouunt = 9999999;
	for(auto itLogic = m_logicServerInfo.begin(); itLogic != m_logicServerInfo.end(); ++itLogic)
	{
		if( itLogic->second.m_logicInfo.m_deskCount < nMiniCouunt	&& 
			itLogic->second.m_logicInfo.m_flag != -1					&&
			itLogic->second.m_tickTime == 0							&&
			m_mapLogicServerOnlySupportType.find(itLogic->first) == m_mapLogicServerOnlySupportType.end())	//此玩法不能出现在限制服务器中
		{
			nMiniCouunt = itLogic->second.m_logicInfo.m_deskCount;
			itAllocLogic = itLogic;
		}
	}

	if(itAllocLogic != m_logicServerInfo.end())
	{
		itAllocLogic->second.m_logicInfo.m_deskCount++;		//分配桌子后增加一
		return itAllocLogic->first;
	}

	return INVALID_LOGICSERVERID;
}

void Work::delDeskCountOnLogicServer(Lint iLogicServerId, int iDelCount)
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);

	auto itLogicServer = m_logicServerInfo.find(iLogicServerId);
	if(itLogicServer != m_logicServerInfo.end())
	{
		itLogicServer->second.m_logicInfo.m_deskCount -= iDelCount;
		if(itLogicServer->second.m_logicInfo.m_deskCount < 0)
		{
			itLogicServer->second.m_logicInfo.m_deskCount = 0;
		}
	}
}

/////////////////////////////////////

void Work::HanderLogicSetGameFree(LMsgCE2LSetGameFree* msg)
{
	if (!msg || msg->m_ServerID != gConfig.GetServerID())
	{
		return;
	}

	gRuntimeInfoMsg.setFreeTimeAndNotify(msg->m_strFreeSet);
}

void Work::HanderLogicSetPXActive(LMsgCE2LSetPXActive* msg)
{
	if (!msg || msg->m_ServerID != gConfig.GetServerID())
	{
		return;
	}

	gActiveManager.PXReloadConfig(msg->m_strActiveSet);
}

void Work::HanderLogicSetOUGCActive(LMsgCE2LSetOUGCActive* msg)
{
	if (!msg || msg->m_ServerID != gConfig.GetServerID())
	{
		return;
	}

	gActiveManager.OUGCReloadConfig(msg->m_strActiveSet);
}

void Work::HanderLogicSetExchActive( LMsgCE2LSetExchActive* msg )
{
	if (!msg || msg->m_ServerID != gConfig.GetServerID())
	{
		return;
	}

	gActiveManager.ExchReloadConfig(msg->m_strActiveSet);
}

void Work::HanderLogicSetActivity(LMsgCE2LSetActivity* msg)
{
	if (!msg || msg->m_ServerID != gConfig.GetServerID())
	{
		return;
	}
	// 直接转发给DB
	LMsgCE2LSetActivity active;
	active.m_ServerID = msg->m_ServerID;
	active.m_activityId = msg->m_activityId;
	active.m_strActivity = msg->m_strActivity;
	gWork.SendMsgToDb( *msg );
}

void Work::fillRankInfo(RankInfo& info)
{
// 	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(info.m_id);
// 	if(safeUser.get() == NULL || !safeUser->isValid())
// 	{
// 		info.m_nickName = "玩家";
// 	}
// 	else
// 	{
// 		boost::shared_ptr<User> user = safeUser->getResource();
// 		info.m_nickName = user->m_userData.m_nike;
// 		info.m_imageUrl = user->m_userData.m_headImageUrl;
// 	}

	boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(info.m_id);
	if(safeBaseInfo && safeBaseInfo->isValid())
	{
		auto user = safeBaseInfo->getResource();
		info.m_nickName = user->m_nike;
		info.m_imageUrl = user->m_headImageUrl;
	}
}

void Work::HanderRank(LMsgCE2LMGRank* msg)
{
	std::vector<RankReward> reward;
	{
		ReadLock lock(m_weekRankRewardConfigLock);
		reward = m_weekRankRewardConfig;
	}

	if(msg->m_rankType == 1)
	{
		WriteLock lock(m_weekRankLock);

		m_weekRankInfo.clear();
		m_weekRankInfo.reserve(msg->m_rankData.size());

		int index = 0;
		for(auto it=msg->m_rankData.begin(); it!=msg->m_rankData.end(); ++it, ++index)
		{
			RankInfo info;
			info.m_id = it->id;
			info.m_count = it->score;

			for(int i=0; i<reward.size(); ++i)
			{
				if(index < reward[i].index)
				{
					info.m_diamond = reward[i].reward;
					break;
				}
			}

			fillRankInfo(info);

			m_weekRankInfo.push_back(info);
		}
	}
// 	else if(msg->m_rankType == 2)
// 	{
// 		WriteLock lock(m_monthRankLock);
//
// 		m_monthRankInfo.clear();
// 		m_monthRankInfo.reserve(msg->m_rankData.size());
//
// 		for(auto it=msg->m_rankData.begin(); it!=msg->m_rankData.end(); ++it)
// 		{
// 			RankInfo info;
// 			info.m_id = it->id;
// 			info.m_count = it->score;
//
// 			fillRankInfo(info);
//
// 			m_monthRankInfo.push_back(info);
// 		}
// 	}
	else if(msg->m_rankType == 3)
	{
		WriteLock lock(m_lastWeekRankLock);

		m_lastWeekRankInfo.clear();
		m_lastWeekRankInfo.reserve(msg->m_rankData.size());

		int index = 0;
		for(auto it=msg->m_rankData.begin(); it!=msg->m_rankData.end(); ++it, ++index)
		{
			RankInfo info;
			info.m_id = it->id;
			info.m_count = it->score;

			for(int i=0; i<reward.size(); ++i)
			{
				if(index < reward[i].index)
				{
					info.m_diamond = reward[i].reward;
					break;
				}
			}

			fillRankInfo(info);

			m_lastWeekRankInfo.push_back(info);
		}

		m_lastWeekRewardId = msg->m_rankRewardId;
	}
// 	else if(msg->m_rankType == 4)
// 	{
// 		WriteLock lock(m_lastMonthRankLock);
//
// 		m_lastMonthRankInfo.clear();
// 		m_lastMonthRankInfo.reserve(msg->m_rankData.size());
//
// 		for(auto it=msg->m_rankData.begin(); it!=msg->m_rankData.end(); ++it)
// 		{
// 			RankInfo info;
// 			info.m_id = it->id;
// 			info.m_count = it->score;
//
// 			fillRankInfo(info);
//
// 			m_lastMonthRankInfo.push_back(info);
// 		}
//
// 		m_lastMonthRewardId = msg->m_rankRewardId;
// 	}
}

void Work::GetRank(const Lint uid, LMsgS2CUpdateRank& msg)
{
	if(msg.m_rankType == 1)
	{
		ReadLock lock(m_weekRankLock);

		msg.m_randData = m_weekRankInfo;
	}
// 	else if(msg.m_rankType == 2)
// 	{
// 		ReadLock lock(m_monthRankLock);
//
// 		msg.m_randData = m_monthRankInfo;
// 	}
	else if(msg.m_rankType == 3)
	{
		ReadLock lock(m_lastWeekRankLock);

		msg.m_randData = m_lastWeekRankInfo;
		bool find = false;
		for(int i=0; i<m_lastWeekRewardId.size(); ++i)
		{
			if(m_lastWeekRewardId[i] == uid)
			{
				find = true;
				break;
			}
		}
		msg.m_rankReward = find?1:0;
	}
// 	else if(msg.m_rankType == 4)
// 	{
// 		ReadLock lock(m_lastMonthRankLock);
//
// 		msg.m_randData =  m_lastMonthRankInfo;
// 		msg.m_rankReward = 0;
// 	}
}

//>0领取到的数量 -1不在排名中 -2已经领取过 -3没有对应的排名奖励，也就是排行榜数量大于配置的奖励数量
int Work::GetRankReward(const Lint uid)
{
	std::vector<RankReward> reward;
	{
		ReadLock lock(m_weekRankRewardConfigLock);
		reward = m_weekRankRewardConfig;
	}

	WriteLock lock(m_lastWeekRankLock);

	for(int i=0; i<m_lastWeekRewardId.size(); ++i)
	{
		if(m_lastWeekRewardId[i] == uid)
		{
			LLOG_WARN("repeat get rank reward uid:%d", uid);
			return -2;
		}
	}

	int index = -1;
	for(int i=0; i<m_lastWeekRankInfo.size(); ++i)
	{
		if(m_lastWeekRankInfo[i].m_id == uid)
		{
			index = i;
			break;
		}
	}

	if(index == -1)
	{
		LLOG_WARN("not in rank uid:%d", uid);
		return -1;
	}


	bool get = false;
	for(int i=0; i<reward.size(); ++i)
	{
		if(index < reward[i].index)
		{
			m_lastWeekRewardId.push_back(uid);
			return reward[i].reward;
		}
	}

	LLOG_ERROR("config err, rank:%d uid:%d", index, uid);
	return -3;
}

void Work::_initTickTime()
{
	LTime cur;
	m_1SceTick   = cur.MSecs();
	m_5SecTick   = cur.MSecs();
	m_15SceTick  = cur.MSecs();
	m_30SceTick  = cur.MSecs();
	m_180SecTick = cur.MSecs();
	m_600SceTick = cur.MSecs();
	m_1HourTick  = cur.MSecs();
	m_1HourTick -= m_1HourTick%(3600*1000);
	m_1WeekTick  = cur.MSecs();
	m_1WeekTick -= m_1WeekTick%WEEKMSECONDS - 2*1000;	// 多给2秒 保证在从CenterServer获取的时候确实过一周时间了
	m_1MonthTick = cur;
}

void Work::_reconnectDB()
{
	if(!m_dbClient)
	{
		LLOG_ERROR("DB socket is null");
		return;
	}

	LLOG_ERROR("Reconnect DB...");
	m_mapReconnect.erase(m_dbClient);
	m_mapHeartBeat.erase(m_dbClient);

	{
		boost::mutex::scoped_lock l(m_mutexDBClient);
		m_dbClient->Stop();
		m_dbClient = gInsideNet.GetNewSocket();
		m_dbClient->AsyncConnect(gConfig.GetDBIp(), gConfig.GetDBPort());
	}
	
	m_mapReconnect[m_dbClient] = true;
}

void Work::_reconncetCenter()
{
	if(!m_centerClient)
	{
		LLOG_ERROR("Center socket is null");
		return;
	}

	LLOG_ERROR("Reconnect center...");
	m_mapReconnect.erase(m_centerClient);
	m_mapHeartBeat.erase(m_centerClient);

	{
		boost::mutex::scoped_lock l(m_mutexCenterClient);
		m_centerClient->Stop();
		m_centerClient = gInsideNet.GetNewSocket();
		m_centerClient->AsyncConnect(gConfig.GetCenterIp(), gConfig.GetCenterPort());
	}

	m_mapReconnect[m_centerClient] = true;
}

void Work::_checkCenterHeartBeat()
{
#ifdef UNCHECKHEARTBEAT
	return;
#endif
	//如果正在重连center，则不进行心跳
	if(m_mapReconnect.find(m_centerClient) != m_mapReconnect.end())
	{
		return;
	}

	//如果存在上次的心跳包则重连，说明网络存在问题
	if(m_mapHeartBeat.find(m_centerClient) != m_mapHeartBeat.end())
	{
		LLOG_ERROR("Center network had problem. Reconnect....");
		_reconncetCenter();
		return;
	}

	LMsgHeartBeatRequestMsg msgHeatbeat;
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_LOGICMANAGER;
	msgHeatbeat.m_iServerId = 0;

	SendToCenter(msgHeatbeat);

	m_mapHeartBeat[m_centerClient] = true;
}

void Work::_checkLogicDBHeartBeat()
{
#ifdef UNCHECKHEARTBEAT
	return;
#endif
	//如果正在重连LogicDB，则不进行心跳
	if(m_mapReconnect.find(m_dbClient) != m_mapReconnect.end())
	{
		return;
	}

	//如果存在上次的心跳包则重连，说明网络存在问题
	if(m_mapHeartBeat.find(m_dbClient) != m_mapHeartBeat.end())
	{
		LLOG_ERROR("LogicDB network had problem. Reconnect....");
		_reconnectDB();
		return;
	}

	LMsgHeartBeatRequestMsg msgHeatbeat;
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_LOGICMANAGER;
	msgHeatbeat.m_iServerId = 0;

	SendMsgToDb(msgHeatbeat);

	m_mapHeartBeat[m_dbClient] = true;
}

// 检查免费时间是否从有效变无效,通知客户端,30秒检查一次
void Work::_checkFreeModel()
{
	if(gRuntimeInfoMsg.updateFree())	
	{
		LMsgLMG2GHorseInfo msgFreeTime;
		msgFreeTime.m_type = 1;
		msgFreeTime.m_str = gRuntimeInfoMsg.getFreeTime();
		gWork.SendMessageToAllGate(msgFreeTime);
	}
}

void Work::_checkExchActive()
{
	static bool bExchActiveRun = false;
	bool nowRun = gActiveManager.CheckTimeValid_Exch();
	if(bExchActiveRun && !nowRun)     //活动结束 推送
	{
		LMsgLMG2GHorseInfo msgExch;
		msgExch.m_type = 2;
		msgExch.m_str = "";
		gWork.SendMessageToAllGate(msgExch);
	}
	bExchActiveRun = nowRun;
}

//Config format
//OnlySupportGameType = LogicId_GameType|LogicId_GameType ......
void Work::_checkLogicServerOnlySupportGameType()
{
	boost::recursive_mutex::scoped_lock l(m_mutexLogicServerInfo);
	
	m_mapLogicServerOnlySupportType.clear();

	LIniConfig	m_ini;
	m_ini.LoadFile("logic_manager_config.ini");

	Lstring strOnlySupportGameType = m_ini.GetString("OnlySupportGameType");
	if(strOnlySupportGameType.empty())	//don't config
	{
		return;
	}

	std::vector<std::string> vecIdAndType;
	L_ParseString(strOnlySupportGameType, vecIdAndType, "|");
	for(auto itIdType = vecIdAndType.begin(); itIdType != vecIdAndType.end(); ++itIdType)
	{
		std::vector<std::string> vecData;
		L_ParseString(*itIdType, vecData, "_");
		if(vecData.size() != 2)
		{
			LLOG_ERROR("OnlySupportGameType is wrong. %s", itIdType->c_str());
			continue;
		}

		Lint iLogicId = atoi(vecData[0].c_str());
		Lint iGameType= atoi(vecData[1].c_str());

		auto itOnlySupport = m_mapLogicServerOnlySupportType.find(iLogicId);
		if(itOnlySupport == m_mapLogicServerOnlySupportType.end())
		{
			std::map<Lint, Lint> mapGameType;
			mapGameType[iGameType] = iGameType;
			m_mapLogicServerOnlySupportType[iLogicId] = mapGameType;
		}
		else
		{
			itOnlySupport->second[iGameType] = iGameType;
		}
	}
}

void Work::_checkRLogStart()
{
	LIniConfig	m_ini;
	m_ini.LoadFile("logic_manager_config.ini");

	if ( gRLT.SetConfig( m_ini.GetInt("RemoteLogStart", 0) !=0, m_ini.GetString("RemoteLogUrl", ""), (size_t)m_ini.GetInt("MaxCachedLogSize", 20480), (size_t)m_ini.GetShort("MaxCachedLogNum", 100) ) )
	{
		LLOG_ERROR( "Work::_checkRLogStart rlog config charge, open=%d url=%s size=%d num=%d", m_ini.GetInt("RemoteLogStart", 0), m_ini.GetString("RemoteLogUrl", "").c_str(), m_ini.GetInt("MaxCachedLogSize", 20480), m_ini.GetShort("MaxCachedLogNum", 100) );
		SendRLogInfoToLogic();
	}
}

void Work::_checkRank()
{
	LMsgLMG2CERank sendweek;
	sendweek.m_rankType = 1;
	sendweek.m_rankCount = 20;
	SendToCenter(sendweek);

// 	LMsgLMG2CERank sendmonth;
// 	sendmonth.m_rankType = 2;
// 	sendmonth.m_rankCount = 20;
// 	SendToCenter(sendmonth);
}

void Work::_checkLastWeekRank()
{
	LMsgLMG2CERank sendweek;
	sendweek.m_rankType = 3;
	sendweek.m_rankCount = 20;
	SendToCenter(sendweek);
}

void Work::_checkLastMonthRank()
{
	LMsgLMG2CERank sendweek;
	sendweek.m_rankType = 4;
	sendweek.m_rankCount = 20;
	SendToCenter(sendweek);
}

void Work::_checkRankRewardConfig()
{
	WriteLock lock(m_weekRankRewardConfigLock);

	m_weekRankRewardConfig.clear();

	LIniConfig	m_ini;
	m_ini.LoadFile("logic_manager_config.ini");

	Lstring strOnlySupportGameType = m_ini.GetString("RankReward");
	if(strOnlySupportGameType.empty())	//don't config
	{
		LLOG_ERROR("logic_manager_config.ini RankReward is empty");
		return;
	}

	std::vector<std::string> vecIdAndType;
	L_ParseString(strOnlySupportGameType, vecIdAndType, "|");
	for(auto itIdType = vecIdAndType.begin(); itIdType != vecIdAndType.end(); ++itIdType)
	{
		std::vector<std::string> vecData;
		L_ParseString(*itIdType, vecData, ",");
		if(vecData.size() != 2)
		{
			LLOG_ERROR("RankReward is wrong. %s", itIdType->c_str());
			continue;
		}

		RankReward rr;
		rr.index = atoi(vecData[0].c_str());
		rr.reward = atoi(vecData[1].c_str());

		m_weekRankRewardConfig.push_back(rr);
	}
}
