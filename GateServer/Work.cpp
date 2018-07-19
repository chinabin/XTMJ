#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "OutsideNet.h"
#include "LMsgS2S.h"
#include "GateUser.h"

//初始化
bool Work::Init()
{
	//设置log文件
	LLog::Instance().SetFileName("GateServer");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	LMsgFactory::Instance().Init();

	if(!gInsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}

	if (!gOutsideNet.Init())
	{
		LLOG_ERROR("gOutsideNet.Init error");
		return false;
	}

	if (!gGateUserManager.Init())
	{
		LLOG_ERROR("gGateUserManager.Init error");
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

	ConnectToLogicManager();

	gInsideNet.Start();

	gOutsideNet.Start();

	LRunnable::Start();
}

//等待
void Work::Join()
{
	gInsideNet.Join();

	gOutsideNet.Join();

	m_memeoryRecycle.Join();

	//等待逻辑线程终止
	LRunnable::Join();
}

//停止
void Work::Stop()
{
	gInsideNet.Stop();

	gOutsideNet.Stop();

	LRunnable::Stop();

	m_logicInfo.clear();

	m_memeoryRecycle.Stop();
}

void Work::Clear()
{

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
	if (cur.MSecs() - m_5SceTick > 5000)
	{
		m_5SceTick = cur.MSecs();
	}
	
	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();

		_checkGateUserState();
		_checkClientSp();

		_checkLogicManagerHeartBeat();
		_checkLogicServerHeartBeat();
		_checkCoinsServerHeartBeat();
	}
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	if(msg == NULL)
	{
		return;
	}

	//LLOG_ERROR("Work::HanderMsg received messageID %d", msg->m_msgId);
	//玩家请求登录
	switch(msg->m_msgId)
	{
	case MSG_CLIENT_KICK:
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;
	case MSG_LMG_2_G_SYNC_LOGIC:
		HanderLogicInfo((LMsgLMG2GateLogicInfo*) msg);
		break;
	case MSG_LMG_2_GL_COINS_SERVER_INFO:
		HanderCoinsServerInfo((LMsgLMG2GLCoinsServerInfo*) msg);
		break;
	case MSG_LMG_2_G_HORSE_INFO:
		HanderHorseInfo((LMsgLMG2GHorseInfo*) msg);
		break;
	case MSG_L_2_G_MODIFY_USER_STATE:
		HanderModifyUserState((LMsgL2GModifyUserState*) msg);
		break;
	case MSG_CONVERT_CLIENTMSG:
		_doWithClientMessage((LMsgConvertClientMsg*)msg);
		break;
	case MSG_L_2_G_USER_MSG:
		{
			  LMsgL2GUserMsg* lmsg = (LMsgL2GUserMsg*)msg;
			  boost::shared_ptr<GateUser> user = gGateUserManager.findUser(lmsg->m_strUUID);
			  if (user)
			  {
				  user->m_sp->Send(lmsg->m_dataBuff);
			  }
			  else
			  {
				  LLOG_DEBUG("ERROR: Work::HanderMsg MSG_L_2_G_USER_MSG user not exiest %s", lmsg->m_strUUID.c_str());
			  }
		}
		break;
	case MSG_L_2_G_USER_OUT_MSG:
		{
			LMsgL2GUserOutMsg*lmsg = (LMsgL2GUserOutMsg*)msg;
			boost::shared_ptr<GateUser> user = gGateUserManager.findUser(lmsg->m_strUUID);
			if (user)
			{
				user->m_sp->Stop();
				gGateUserManager.DelUser(user->m_strUUID);
			}
			else
			{
				LLOG_DEBUG("ERROR: Work::HanderMsg MSG_L_2_G_USER_OUT_MSG user not exiest %s", lmsg->m_strUUID.c_str());
			}
		}
		break;
	case MSG_HEARDBEAT_REPLY:
		HanderHearBeatReply((LMsgHeartBeatReplyMsg*)msg);
		break;
	default:
		LLOG_ERROR("Don't do with the message id. Id = %d", msg->m_msgId);
		msg->m_sp->Stop();
		break;
	}
}

void Work::_doWithClientMessage(LMsgConvertClientMsg* pMsg)
{
	if(pMsg == NULL)
	{
		return;
	}

	//登录gate消息不用判断消息安全
	if(pMsg->msgEntity->m_msgId == MSG_C_2_S_LOGIN_GATE)
	{
		_doWithClientLoginGate((LMsgC2SLoginGate*)pMsg->msgEntity.get());
		return;
	}
	
	if(!_isClientMessageSafe(pMsg->msgHead))
	{
		LLOG_DEBUG("The message format is wrong. MsgId = %d", pMsg->msgEntity->m_msgId);
		pMsg->m_sp->Stop();
		return;
	}

	if(pMsg->msgEntity->m_msgId == MSG_C_2_S_HEART)
	{
		_doWithClientHeartBeat((LMsgC2SHeart*)pMsg->msgEntity.get());
	}
	else
	{
		LLOG_ERROR("GateServer receive an mesage, id=%d.", pMsg->msgEntity->m_msgId);
		_doWithClientOtherMsg(pMsg->msgEntity, pMsg->msgOriginData);
	}
}

void Work::_doWithClientLoginGate(LMsgC2SLoginGate* msg)
{
	if(msg == NULL)
	{
		return;
	}

	boost::shared_ptr<GateUser> user = gGateUserManager.findUser(msg->m_strUserUUID);
	if(user)
	{
		if(user->m_login)
		{
			SendToLogicUserOut(user);
		}

		//删除以前的
		user->m_sp->Stop();
		gGateUserManager.DelUser(user->m_strUUID);
	}

	user = gGateUserManager.createUser(msg->m_strUserUUID, msg->m_sp);
	if(user.get() == NULL)
	{
		LLOG_ERROR("Fail to create user");
		return;
	}

	LMsgS2CLoginGate msgRet;
	msgRet.m_strUserUUID = user->m_strUUID;
	msgRet.m_strKey      = user->m_strKey;
	msgRet.m_uMsgOrder   = user->m_uMsgOrder;

	user->m_sp->Send(msgRet.GetSendBuff());

	//从客户端sp列表中移除
	m_mapClientSp.erase(msg->m_sp);
}

bool Work::_isClientMessageSafe(boost::shared_ptr<LMsgC2SVerifyHead> msgHead)
{
	boost::shared_ptr<GateUser> user = gGateUserManager.findUser(msgHead->m_sp);
	if(user.get() == NULL)
	{
		LLOG_ERROR("A invalid user");
		msgHead->m_sp->Kick();
		return false;
	}

	if(msgHead->m_strVerifyKey.compare(user->m_strKey) != 0)
	{
		LLOG_ERROR("Key is wrong");
		msgHead->m_sp->Kick();
		return false;
	}

	if(msgHead->m_lMsgOrder <= user->m_uMsgOrder)
	{
		LLOG_ERROR("Wrong msg order. C = %d, S = %d", msgHead->m_lMsgOrder, user->m_uMsgOrder);
		msgHead->m_sp->Kick();
		return false;
	}

	user->m_uMsgOrder = msgHead->m_lMsgOrder;

	return true;
}

void Work::_doWithClientHeartBeat(LMsgC2SHeart* pMsg)
{
	if(pMsg == NULL)
	{
		return;
	}

	boost::shared_ptr<GateUser> user = gGateUserManager.findUser(pMsg->m_sp);
	if(user.get() == NULL)
	{
		LLOG_ERROR("User is not exist %d", pMsg->m_msgId);
		pMsg->m_sp->Stop();
		return;
	}

	LMsgS2CHeart msgHeartBeat;
	msgHeartBeat.m_time = GetCurTime().Secs();
	pMsg->m_sp->Send(msgHeartBeat.GetSendBuff());

	//消息时间
	user->m_login += 1;
	user->m_timeActive = GetCurTime().MSecs();
}

void Work::_doWithClientOtherMsg(boost::shared_ptr<LMsg> msgEntity, boost::shared_ptr<LBuff> msgOriginData)
{
	if(msgEntity.get() == NULL || msgOriginData.get() == NULL)
	{
		return;
	}

	boost::shared_ptr<GateUser> user = gGateUserManager.findUser(msgEntity->m_sp);
	if(user.get() == NULL)
	{
		LLOG_ERROR("User is not exist %d", msgEntity->m_msgId);
		msgEntity->m_sp->Stop();
		return;
	}

	LMsgG2LUserMsg msgG2L;
	msgG2L.m_strUUID	= user->m_strUUID;
	msgG2L.m_ip			= msgEntity->m_sp->GetRemoteIp();
	msgG2L.m_dataBuff   = msgOriginData;

	//区分玩家状态，将消息发送至Logic 或 LogicManager
	if((user->getUserState() == LGU_STATE_DESK || user->getUserState() == LGU_STATE_COINDESK) && user->getUserLogicID() > 0)
	{	
		//发送消息到Logic
		LogicInfo* logic = GetLogicInfoById(user->getUserLogicID());
		if (logic != NULL)
		{
			logic->m_sp->Send(msgG2L.GetSendBuff());
		}
		else
		{
			if(user->m_login)
			{
				SendToLogicUserOut(user);
			}

			//当前逻辑不存在，踢下该用户，让他重连
			user->m_sp->Stop();
			gGateUserManager.DelUser(user->m_strUUID);
			return;
		}
	}
	else if(user->getUserState() == LGU_STATE_COIN)
	{
		SendToCoinsServer( msgG2L );
	}
	else
	{
		//发送消息到Center
		if (m_logicManager)
		{
			m_logicManager->Send(msgG2L.GetSendBuff());
		}
	}

	user->m_login += 1;

	//消息时间
	user->m_timeActive = GetCurTime().MSecs();
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if(msg == NULL)
	{
		return;
	}

	if(msg->m_sp == m_logicManager)
	{
		_reconnectLogicManager();
	}
	else if(m_coinsServer.m_sp && msg->m_sp == m_coinsServer.m_sp)
	{
		_reconnectCoinsServer();
	}
	else
	{
		LogicInfo* pLogicInfo = _getLogicSP(msg->m_sp);
		if(pLogicInfo)
		{
			_reconnectLogicServer(pLogicInfo->m_id);
		}
		else
		{
			//玩家离线
			boost::shared_ptr<GateUser> user = gGateUserManager.findUser(msg->m_sp);
			if (user)
			{
				if(user->m_login)
					SendToLogicUserOut(user);

				gGateUserManager.DelUser(user->m_strUUID);
			}
		}
	}

	//从客户端sp中删除sp
	m_mapClientSp.erase(msg->m_sp);
}

void Work::HanderClientIn(LMsgIn* msg)
{
	if(msg == NULL)
	{
		return;
	}

	if(msg->m_sp == m_logicManager)
	{
		LMsgG2LMGLogin login;
		login.m_id = gConfig.GetId();
		login.m_ip = gConfig.GetOutsideIp();
		login.m_key = gConfig.GetLogicKey();
		login.m_port = gConfig.GetOutsidePort();
		m_logicManager->Send(login.GetSendBuff());
	}
	else if(m_coinsServer.m_sp && msg->m_sp == m_coinsServer.m_sp)
	{
		LMsgG2CNLogin login;
		login.m_id = gConfig.GetId();
		login.m_ip = gConfig.GetOutsideIp();
		login.m_key = gConfig.GetLogicKey();
		login.m_port = gConfig.GetOutsidePort();
		m_coinsServer.m_sp->Send(login.GetSendBuff());
	}
	else
	{
		LogicInfo* pLogicInfo = _getLogicSP(msg->m_sp);
		if(pLogicInfo)
		{
			LMsgG2LLogin login;
			login.m_id = gConfig.GetId();
			login.m_ip = gConfig.GetOutsideIp();
			login.m_key = gConfig.GetLogicKey();
			login.m_port = gConfig.GetOutsidePort();
			msg->m_sp->Send(login.GetSendBuff());

			LLOG_ERROR("Login logic successfully. %s:%d", pLogicInfo->m_ip.c_str(), pLogicInfo->m_port);
		}
		else
		{
			m_mapClientSp[msg->m_sp] = GetCurTime().Secs();
		}
	}

	m_mapReconnect.erase(msg->m_sp);
}

void Work::ConnectToLogicManager()
{
	m_logicManager = gInsideNet.GetNewSocket();
	if(m_logicManager->Connect(gConfig.GetLogicManagerIp(), gConfig.GetLogicManagerPort()) == false)
	{
		LLOG_ERROR("Fail to connect logic mananger");
		Stop();
	}
	else
	{
		LLOG_ERROR("Connect logic manager successfully");
	}
}

void Work::SendToLogicUserOut(boost::shared_ptr<GateUser> user)
{
	LMsgG2LUserOutMsg send;
	send.m_strUUID = user->m_strUUID;
	if (m_logicManager)
	{
		//通各LogicManager下线
		m_logicManager->Send(send.GetSendBuff());
	}
	SendToCoinsServer( send );
	if (user->getUserLogicID() > 0)
	{
		LogicInfo* logic = GetLogicInfoById(user->getUserLogicID());
		if (logic != NULL)
		{
			//通知Logic下线
			logic->m_sp->Send(send.GetSendBuff());
		}
	}
}

void Work::HanderLogicInfo(LMsgLMG2GateLogicInfo* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_ID <= 0)
	{
		LLOG_ERROR("Work::HanderLogicInfo %d", msg->m_ID);
		return;
	}

	std::map<int, LogicInfo> mapFromLM;
	for(int i = 0; i < msg->m_count; ++i)
	{
		mapFromLM[msg->m_logic[i].m_id] = msg->m_logic[i];
	}

	//遍历哪些logic已经不在logicmanager上了
	for(auto itLogic = m_logicInfo.begin(); itLogic != m_logicInfo.end();)
	{
		if(mapFromLM.find(itLogic->first) == mapFromLM.end()) //这个logic已经不在logicmananger上了
		{
			itLogic->second.m_sp->Stop();
			itLogic = m_logicInfo.erase(itLogic);
		}
		else //如果依然存在
		{
			mapFromLM.erase(itLogic->first);
			++itLogic;
		}
	}

	//加入新进入的
	for(auto itNew = mapFromLM.begin(); itNew != mapFromLM.end(); ++itNew)
	{
		m_logicInfo[itNew->first] = itNew->second;
		m_logicInfo[itNew->first].m_sp = gInsideNet.GetNewSocket();
		m_logicInfo[itNew->first].m_sp->Connect(itNew->second.m_ip, itNew->second.m_port);
	}
}

void Work::HanderHorseInfo(LMsgLMG2GHorseInfo* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LMsgS2CHorseInfo horse;
	horse.m_str = msg->m_str;
	horse.m_type = msg->m_type;
	SendMessageToAllUse(&horse);
}

void Work::HanderModifyUserState(LMsgL2GModifyUserState* msg)
{
	if (msg == NULL)
	{
		return;
	}

	boost::shared_ptr<GateUser> user = gGateUserManager.findUser(msg->m_strUUID);
	if (user)
	{
		LLOG_DEBUG("Work::HanderModifyUserState ID:%s, State:%d, serverID: %d", user->m_strUUID.c_str(), msg->m_userstate, msg->m_logicID);
		user->setUserState(msg->m_userstate);
		user->setUserLogicID(msg->m_logicID);
	}
	else
	{
		LLOG_DEBUG("ERROR: Work::HanderModifyUserState Not Exist ID:%s, State:%d, serverID: %d", msg->m_strUUID.c_str(), msg->m_userstate, msg->m_logicID);
	}
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

void Work::SendMessageToAllUse(LMsg* msg)
{
	if (msg == NULL)
	{
		return;
	}

	const std::map<Lstring, boost::shared_ptr<GateUser> >& mapAllUsers = gGateUserManager.getAllUsers();
	for(auto it = mapAllUsers.begin(); it != mapAllUsers.end();++it)
	{
		if(it->second->m_sp)
		{
			it->second->m_sp->Send(msg->GetSendBuff());
		}
	}
}

LogicInfo* Work::GetLogicInfoBySp(LSocketPtr sp)
{
	auto it = m_logicInfo.begin();
	for (; it != m_logicInfo.end(); ++it)
	{
		if (sp == it->second.m_sp)
			return &it->second;
	}
	return NULL;
}

LogicInfo* Work::GetLogicInfoById(Lint id)
{
	if (m_logicInfo.count(id))
		return &m_logicInfo[id];

	return NULL;
}

void Work::HanderCoinsServerInfo(LMsgLMG2GLCoinsServerInfo* msg)
{
	LLOG_ERROR( "Work::HanderCoinsServerInfo ip=%s port=%d", msg->m_conis.m_ip.c_str(), msg->m_conis.m_port );
	// 每次重新连接就行
	m_coinsServer.m_ip = msg->m_conis.m_ip;
	m_coinsServer.m_port = msg->m_conis.m_port;
	if ( !m_coinsServer.m_ip.empty() )
	{
		m_coinsServer.m_sp = gInsideNet.GetNewSocket();
		m_coinsServer.m_sp->Connect(msg->m_conis.m_ip, msg->m_conis.m_port);
	}
	else
	{
		// 如果金币服务器的ip为空 表示金币服务器失去了Manager的连接的
		m_coinsServer.m_sp.reset();
		m_mapReconnect.erase(m_coinsServer.m_sp);
	}
}

void Work::SendToCoinsServer(LMsg& msg)
{
	if ( m_coinsServer.m_sp && m_coinsServer.m_sp->getSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED )
	{
		m_coinsServer.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::_initTickTime()
{
	LTime cur;
	m_1SceTick = cur.MSecs();
	m_5SceTick = cur.MSecs();
	m_30SceTick = cur.MSecs();
}

bool Work::_findLogicSPAndDel(LSocketPtr sp)
{
	for(auto itSp = m_logicInfo.begin(); itSp != m_logicInfo.end(); ++itSp)
	{
		if(itSp->second.m_sp == sp)
		{
			m_logicInfo.erase(itSp);
			return true;
		}
	}

	return false;
}

bool Work::_isLogicSP(LSocketPtr sp)
{
	for(auto itSp = m_logicInfo.begin(); itSp != m_logicInfo.end(); ++itSp)
	{
		if(sp == itSp->second.m_sp)
		{
			return true;
		}
	}

	return false;
}

LogicInfo* Work::_getLogicSP(LSocketPtr sp)
{
	for(auto itSp = m_logicInfo.begin(); itSp != m_logicInfo.end(); ++itSp)
	{
		if(sp == itSp->second.m_sp)
		{
			return &itSp->second;
		}
	}
	return NULL;
}

void Work::_reconnectLogicManager()
{
	if(!m_logicManager)
	{
		LLOG_ERROR("Logic manager socket is null");
		return;
	}

	LLOG_ERROR("Reconnect logic manager...");
	m_mapReconnect.erase(m_logicManager);
	m_mapHeartBeat.erase(m_logicManager);

	m_logicManager->Stop();
	m_logicManager = gInsideNet.GetNewSocket();
	m_logicManager->AsyncConnect(gConfig.GetLogicManagerIp(), gConfig.GetLogicManagerPort());

	m_mapReconnect[m_logicManager] = true;
}

void Work::_reconnectLogicServer(int iLogicId)
{
	auto itLogic = m_logicInfo.find(iLogicId);
	if(itLogic == m_logicInfo.end())
	{
		LLOG_ERROR("Don't find logic. LogicId = %d", iLogicId);
		return;
	}

	LLOG_ERROR("Reconnect logic %d server...", iLogicId);

	m_mapReconnect.erase(itLogic->second.m_sp);
	m_mapHeartBeat.erase(itLogic->second.m_sp);

	itLogic->second.m_sp->Stop();
	itLogic->second.m_sp = gInsideNet.GetNewSocket();
	itLogic->second.m_sp->AsyncConnect(itLogic->second.m_ip, itLogic->second.m_port);

	m_mapReconnect[itLogic->second.m_sp] = true;
}

void Work::_reconnectCoinsServer()
{
	if(!m_coinsServer.m_sp)
	{
		LLOG_ERROR("CoinsServer socket is null");
		return;
	}

	LLOG_ERROR("Reconnect logic CoinsServer...");
	m_mapReconnect.erase(m_coinsServer.m_sp);
	m_mapHeartBeat.erase(m_coinsServer.m_sp);

	m_coinsServer.m_sp->Stop();
	m_coinsServer.m_sp = gInsideNet.GetNewSocket();
	m_coinsServer.m_sp->AsyncConnect(m_coinsServer.m_ip, m_coinsServer.m_port);

	m_mapReconnect[m_coinsServer.m_sp] = true;
}


void Work::_checkLogicManagerHeartBeat()
{
#ifdef UNCHECKHEARTBEAT
	return;
#endif
	//如果正在重连LogicManager，则不进行心跳
	if(m_mapReconnect.find(m_logicManager) != m_mapReconnect.end())
	{
		return;
	}

	//如果存在上次的心跳包则重连，说明网络存在问题
	if(m_mapHeartBeat.find(m_logicManager) != m_mapHeartBeat.end())
	{
		LLOG_ERROR("LogicManager network had problem. Reconnect....");
		_reconnectLogicManager();
		return;
	}

	LMsgHeartBeatRequestMsg msgHeatbeat;
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_GATESERVER;
	msgHeatbeat.m_iServerId = gConfig.GetId();

	m_logicManager->Send(msgHeatbeat.GetSendBuff());

	m_mapHeartBeat[m_logicManager] = true;
}

void Work::_checkLogicServerHeartBeat()
{
#ifdef UNCHECKHEARTBEAT
	return;
#endif
	for(auto itLogic = m_logicInfo.begin(); itLogic != m_logicInfo.end(); ++itLogic)
	{
		if(m_mapReconnect.find(itLogic->second.m_sp) != m_mapReconnect.end())
		{
			continue;
		}

		if(m_mapHeartBeat.find(itLogic->second.m_sp) != m_mapHeartBeat.end())
		{
			LLOG_ERROR("%d LogicServer network had problem. Reconnect....", itLogic->second.m_id);
			_reconnectLogicServer(itLogic->second.m_id);
			continue;
		}

		LMsgHeartBeatRequestMsg msgHeatbeat;
		msgHeatbeat.m_fromWho = HEARBEAT_WHO_GATESERVER;
		msgHeatbeat.m_iServerId = gConfig.GetId();

		itLogic->second.m_sp->Send(msgHeatbeat.GetSendBuff());

		m_mapHeartBeat[itLogic->second.m_sp] = true;
	}
}

void Work::_checkCoinsServerHeartBeat()
{
#ifdef UNCHECKHEARTBEAT
	return;
#endif
	//如果正在重连CoinsServer，则不进行心跳
	if(m_mapReconnect.find(m_coinsServer.m_sp) != m_mapReconnect.end())
	{
		return;
	}

	if ( !m_coinsServer.m_sp )
	{
		// 金币服务器没有连接
		return;
	}
	//如果存在上次的心跳包则重连，说明网络存在问题
	if(m_mapHeartBeat.find(m_coinsServer.m_sp) != m_mapHeartBeat.end())
	{
		LLOG_ERROR("CoinsServer network had problem. Reconnect....");
		_reconnectCoinsServer();
		return;
	}

	LMsgHeartBeatRequestMsg msgHeatbeat;
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_GATESERVER;
	msgHeatbeat.m_iServerId = gConfig.GetId();

	m_coinsServer.m_sp->Send(msgHeatbeat.GetSendBuff());

	m_mapHeartBeat[m_coinsServer.m_sp] = true;
}

void Work::_checkGateUserState()
{
#ifdef UNCHECKHEARTBEAT
	return;
#endif

	std::list<LSocketPtr> kickList;
	const std::map<Lstring, boost::shared_ptr<GateUser> >& mapAllUsers = gGateUserManager.getAllUsers();
	for(auto it = mapAllUsers.begin(); it != mapAllUsers.end();++it)
	{
		int iInterval = (GetCurTime().MSecs() - it->second->m_timeActive) / 1000;
		if(iInterval >= 30)
		{
			kickList.push_back(it->second->m_sp);
		}
	}
	for(auto it = kickList.begin(); it != kickList.end();++it)
	{
		LMsgKick kick;
		kick.m_sp = *it;
		(*it)->Stop();
		HanderUserKick(&kick);
	}
}

void Work::_checkClientSp()
{
	for(auto itSp = m_mapClientSp.begin(); itSp != m_mapClientSp.end(); )
	{
		int iInterval = GetCurTime().Secs() - itSp->second;
		if(iInterval >= 30) //超过30秒则踢出
		{
			itSp->first->Stop();
			m_mapClientSp.erase(itSp++);
		}
		else
		{
			++itSp;
		}
	}
}