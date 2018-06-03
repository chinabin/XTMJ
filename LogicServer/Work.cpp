#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "LMsgS2S.h"
#include "UserManager.h"
#include "User.h"
#include "Desk.h"
#include "RoomVip.h"
#include "RobotManager.h"
#include <openssl/md5.h>
#include "ActiveManager.h"
#include "CounterManager.h"
#include "CH_HN_ZhuanZhuan.h"
//#include "CH_HN_ZhengZhou.h"
//#include "CH_HN_HuaShui.h"
//#include "CH_HN_TuiDaoHu.h"
//#include "CH_HN_XinXiang.h"
//#include "CH_HN_KaiFeng.h"
//#include "CH_HN_LuoHe.h"
//#include "CH_HN_LuoYang.h"
//#include "CH_HN_XinYang.h"
#include "RLogHttp.h"

using namespace boost::asio;  
using boost::asio::ip::tcp; 

//初始化
bool Work::Init()
{
	//初始化随机因子
	LTime timeNow;
	srand(timeNow.Secs());

	//设置log文件
	LLog::Instance().SetFileName("LogicServer");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	LMsgFactory::Instance().Init();

	if (!gInsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}

	/*if (!CardManager::Instance()->Init())
	{
	LLOG_ERROR("CardManager::Instance().Init() error");
	return false;
	}*/
	
	
	if(!gCM_HeNan_ZhuanZhuan->Init())
	{
		LLOG_ERROR("gCM_HeNan_ZhuanZhuan::Instance().Init() error");
		return false;
	}
	
	//以下的牌都是继承的CardBase 和zhuanzhuan不同
	CardBase::Init();
	
// 	if(!gCB_HeNan_ZhengZhou->Init())
// 	{
// 		LLOG_ERROR("gCB_HeNan_ZhengZhou::Instance().Init() error");
// 		return false;
// 	}
// 
// 	if(!gCB_HeNan_HuaShui->Init())
// 	{
// 		LLOG_ERROR("gCB_HeNan_HuaShui::Instance().Init() error");
// 		return false;
// 	}
// 
// 	if(!gCB_HeNan_TuiDaoHu->Init())
// 	{
// 		LLOG_ERROR("gCB_HeNan_TuiDaoHu::Instance().Init() error");
// 		return false;
// 	}
// 
// 	if(!gCB_HeNan_XinXiang->Init())
// 	{
// 		LLOG_ERROR("gCB_HeNan_XinXiang::Instance().Init() error");
// 		return false;
// 	}
// 
// 	if(!gCB_HeNan_KaiFeng->Init())
// 	{
// 		LLOG_ERROR("gCB_HeNan_KaiFeng::Instance().Init() error");
// 		return false;
// 	}
// 
// 	if(!gCardLuoHe->Init())
// 	{
// 		LLOG_ERROR("gCardLuoHe::Instance().Init() error");
// 		return false;
// 	}
// 
// 	if (!gCardLuoYang->Init())
// 	{
// 		LLOG_ERROR("gCardLuoYang::Instance().Init() error");
// 		return false;
// 	}
// 	if (!gCB_HeNan_XinYang->Init())
// 	{
// 		LLOG_ERROR("gCB_HeNan_XinYang::Instance().Init() error");
// 		return false;
// 	}
	
	if( !gRLT.Init( false, "", 0, 0 ) )
	{
		LLOG_ERROR("gRLT.Init error");
		return false;
	}

	if(!gVipLogMgr.Init())
	{
		LLOG_ERROR("gVipLogMgr.Init() error");
		return false;
	}

	if(!gRoomVip.Init())
	{
		LLOG_ERROR("gRoomVip.Init() error");
		return false;
	}
	//初始化机器人
	if(gConfig.GetIfAddRobot() && !gRobotManager.Init("settings/RobotConfig.txt"))
	{
		LLOG_ERROR("gRobotManager.Init() error");
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
	ConnectToLogicManager();

	ConnectToDb();

	m_memeoryRecycle.Start();

	gCounterManager.Start();

	gRLT.Start();

	gInsideNet.Start();

	LRunnable::Start();
}

//等待
void Work::Join()
{
	m_memeoryRecycle.Join();

	gCounterManager.Join();

	gRLT.Join();

	gInsideNet.Join();

	//等待逻辑线程终止
	LRunnable::Join();
}

//停止
void Work::Stop()
{
	m_logicManager.reset();

	m_gateInfo.clear();

	gCounterManager.Stop();

	gRLT.Stop();

	gInsideNet.Stop();

	LRunnable::Stop();

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
		}

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

void Work::Tick(LTime& cur)
{
	//1000毫秒循环一次
	if (cur.MSecs() - m_1SceTick > 1000)
	{
		gRobotManager.Tick();
		gRoomVip.Tick(cur);
		m_1SceTick = cur.MSecs();
	}

	//15秒循环一次
	if (cur.MSecs() - m_15SecTick > 15000)
	{
		m_15SecTick = cur.MSecs();
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();
		gVipLogMgr.Tick();

		_checkLogicManagerHeartBeat();
		_checkLogicDBHeartBeat();
		_checkCoinsServerHeartBeat();
	}

	if (cur.MSecs() - m_60SecTick > 60*1000)
	{
		m_60SecTick = cur.MSecs();

		gConfig.Reload();
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
	case MSG_G_2_L_LOGIN:
		HanderGateLogin((LMsgG2LLogin*)msg);
		break;
		//////////////////////////////////////////////////////////////////////////
		//gate 跟 logic之间的交互
	case MSG_LMG_2_L_USER_LOGIN:		
		HanderUserLogin((LMsgLMG2LLogin*)msg);
		break;
	case MSG_LMG_2_L_CREATE_DESK:
		handerLMGCreateDesk((LMsgLMG2LCreateDesk*)msg);
		break;
	case MSG_LMG_2_L_ADDTO_DESK:
		handerLMGAddToDesk((LMsgLMG2LAddToDesk*)msg);
		break;
	case MSG_LMG_2_L_PAIXING_ACTIVE:
		handerPXActiveInfo((LMsgLMG2LPXActive*)msg);
		break;
	case MSG_CN_2_L_CREATE_COIN_DESK:
		HanderLMGCreateCoinDesk((LMsgCN2LCreateCoinDesk*)msg);
		break;
	case MSG_G_2_L_USER_MSG:
		{
			GateInfo* gate = GetGateInfoBySp(msg->m_sp);
			if (gate)
			{
				HanderGateUserMsg((LMsgG2LUserMsg*)msg,gate);
			}
			else
			{
				LLOG_ERROR("Work::HanderMsg gate not exist");
			}
		}
		break;
	case MSG_G_2_L_USER_OUT_MSG:
		{
			LMsgG2LUserOutMsg* gmsg = (LMsgG2LUserOutMsg*)msg;
			GateInfo* gate = GetGateInfoBySp(msg->m_sp);
			if(gate && gmsg)
			{
				HanderUserLogout(gate->m_id, gmsg->m_strUUID);
			}
			else
			{
				LLOG_ERROR("Work::HanderMsg gate not exist");
			}
		}
		break;
	case MSG_HEARDBEAT_REQUEST:
		HanderHeartBeatRequest((LMsgHeartBeatRequestMsg*)msg);
		break;
	case MSG_HEARDBEAT_REPLY:
		HanderHearBeatReply((LMsgHeartBeatReplyMsg*)msg);
		break;
	case MSG_LMG_2_GL_COINS_SERVER_INFO:
		HanderCoinsServerInfo((LMsgLMG2GLCoinsServerInfo*) msg);
		break;
	case MSG_LMG_2_L_RLOG_INFO:
		HanderRLogInfo((LMsgLMG2LRLogInfo*) msg);
		break;
	default:
		break;
	}
}

void Work::HanderUserLogin(LMsgLMG2LLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}
	User* pUser = gUserManager.GetUserbyDataId(msg->m_userid);
	if (pUser && pUser->GetDesk())
	{
		//同步卡数
		pUser->m_userData.m_numOfCard2s = msg->m_card_num;

		HanderDeskUserLogin(pUser, msg->m_gateId,  msg->m_ip, true, msg->m_buyInfo, msg->m_hide);
	}
	else
	{
		if ( pUser )
		{
			UserManager::Instance().DelUser(pUser);
			delete pUser;
			pUser = NULL;
		}
		LMsgL2LMGModifyUserState send;
		send.m_logicID = 0;
		send.m_strUUID = msg->m_strUUID;
		send.m_userid  = msg->m_userid;
		send.m_userstate = LGU_STATE_CENTER;
		SendToLogicManager(send);
		SendToCoinsServer( send );
		LLOG_ERROR("Work::HanderUserLogin user login ERROR!");
	}
}

void Work::HanderDeskUserLogin(User* pUser, Lint gateId, Lstring& ip, bool firstLogin , Lstring buyInfo , Lint hide )
{
	if (!pUser)
	{
		return;
	}
	UserManager::Instance().DelUser(pUser);
	pUser->SetUserGateId(gateId);
	pUser->SetIp(ip);
	UserManager::Instance().AddUser(pUser);
	if (firstLogin)
	{
		pUser->SendItemInfo();
		pUser->SendLoginInfo(buyInfo, hide);
	}
	pUser->Login();
	if ( pUser->m_desk && pUser->m_desk->GetDeskType() == DeskType_Coins )
	{
		pUser->setUserState(LGU_STATE_COINDESK);
	}
	else
	{
		pUser->setUserState(LGU_STATE_DESK);
	}
	pUser->ModifyUserState();
}

void Work::HanderUserLogout(Lint gateId, const Lstring& strUUID)
{
	User* user = UserManager::Instance().GetUserByGateIdAndUUID(gateId, strUUID);
	if (user && user->GetDesk())
	{
		LLOG_DEBUG("Work::HanderUserLogout %d", user->GetUserDataId());

		user->Logout();
		//在桌子中
		user->GetDesk()->HanderUserOutDesk(user);
		if ( user->GetDesk()->GetDeskType() == DeskType_Coins )
		{
			user->setUserState(LGU_STATE_COIN);
		}
		else
		{
			user->setUserState(LGU_STATE_CENTER);
		}
		user->ModifyUserState();
	}
	else
	{
		if ( user )
		{
			UserManager::Instance().DelUser(user);
			delete user;
			user = NULL;
		}
		LLOG_DEBUG("Work::HanderUserLogout user not find %d:%s", gateId, strUUID.c_str());
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if (msg->m_sp == m_logicManager)
	{
		_reconnectLogicManager();
	}
	else if(msg->m_sp == m_dbClient)
	{
		_reconnectLogicDB();
	}
	else if(m_coinsServer.m_sp && msg->m_sp == m_coinsServer.m_sp)
	{
		_reconnectCoinsServer();
	}
	else
	{
		HanderGateLogout(msg);
	}
}

void Work::HanderClientIn(LMsgIn* msg)
{
	if(msg == NULL)
	{
		return;
	}

	if(msg->m_sp == m_logicManager)
	{
		LMsgL2LMGLogin login;
		login.m_id = gConfig.GetServerID();
		login.m_ip = gConfig.GetInsideIp();
		login.m_key = gConfig.GetCenterKey();
		login.m_port = gConfig.GetInsidePort();

		SendToLogicManager(login);
	}
	else if(msg->m_sp == m_dbClient)
	{
		LMsgL2LdbLogin msg;
		msg.m_key = gConfig.GetDBKey();
		msg.m_serverID = gConfig.GetServerID();
		SendMsgToDb(msg);
	}
	else if(m_coinsServer.m_sp && msg->m_sp == m_coinsServer.m_sp)
	{
		LMsgL2CNLogin login;
		login.m_id = gConfig.GetServerID();
		login.m_ip = gConfig.GetInsideIp();
		login.m_key = gConfig.GetCenterKey();
		login.m_port = gConfig.GetInsidePort();
		m_coinsServer.m_sp->Send(login.GetSendBuff());
	}
	m_mapReconnect.erase(msg->m_sp);
}

void Work::ConnectToLogicManager()
{
	m_logicManager = gInsideNet.GetNewSocket();
	LLOG_ERROR("Work::ConnectToLogicManager begin %s:%d", gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort());
	if(m_logicManager->Connect(gConfig.GetLogicManagerIp(), gConfig.GetLogicManagerPort()))
	{
		LLOG_ERROR("Work::ConnectToLogicManager end %s:%d", gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort());
	}
	else
	{
		LLOG_ERROR("Fail to connect logic manager");
		Stop();
	}
}

void Work::SendToLogicManager(LMsg& msg)
{
	if(m_logicManager && m_logicManager->getSocketConnectStatus() == SOCKET_CONNECT_STATUS_CONNECTED)
	{
		m_logicManager->Send(msg.GetSendBuff());
	}
}

void Work::HanderGateLogin(LMsgG2LLogin* msg)
{
	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}

	auto itGateInfo = m_gateInfo.find(msg->m_id);
	if(itGateInfo != m_gateInfo.end())
	{
		LLOG_ERROR("Gate reconnect logicserver. %d", msg->m_id);
		itGateInfo->second.m_sp->Stop();
		m_gateInfo.erase(itGateInfo);
	}

	GateInfo info;
	info.m_id = msg->m_id;
	info.m_ip = msg->m_ip;
	info.m_port = msg->m_port;
	info.m_userCount = 0;
	info.m_sp = msg->m_sp;
	m_gateInfo[info.m_id] = info;

	//SendGateInfoToCenter();
}

void Work::HanderGateLogout(LMsgKick* msg)
{
	GateInfo* info = GetGateInfoBySp(msg->m_sp);
	if (info)
	{
		LLOG_ERROR(" Work::HanderGateLogout: %d", info->m_id);
		DelGateInfo(info->m_id);
	}
}

GateInfo* Work::GetGateInfoBySp(LSocketPtr sp)
{
	std::map<Lint, GateInfo>::iterator it = m_gateInfo.begin();
	for (; it != m_gateInfo.end(); ++it)
	{
		if (sp == it->second.m_sp)
			return &it->second;
	}
	return NULL;
}

GateInfo* Work::GetGateInfoById(Lint id)
{
	if (m_gateInfo.count(id))
		return &m_gateInfo[id];

	return NULL;
}

void Work::DelGateInfo(Lint id)
{
	if (m_gateInfo.count(id))
	{
		m_gateInfo.erase(m_gateInfo.find(id));
	}
}

void Work::HanderGateUserMsg(LMsgG2LUserMsg* msg,GateInfo* gate)
{
	if(msg == NULL || gate == NULL || msg->m_userMsg == NULL)
	{
		return;
	}

	Lint gateId = gate->m_id;
	User* user = gUserManager.GetUserByGateIdAndUUID(gateId, msg->m_strUUID);
	if(user)
	{
		user->HanderMsg(msg->m_userMsg);
	}
	else
	{
		LLOG_DEBUG("Work::HanderGateUserMsg user not exist %d:%s:%d", gateId, msg->m_strUUID.c_str(), msg->m_userMsgId);
	}
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

Lstring Work::Md5Hex(const void* src ,Lsize len)
{
	unsigned char sign[17];
	memset(sign,0,sizeof(sign));

	MD5((const unsigned char*)src,len,sign);

	//转换成16进制
	char signHex[33];
	memset(signHex,0,sizeof(signHex));
	for(int i = 0 ; i < 16; ++i)
	{
		sprintf(signHex + i*2,"%02x", sign[i]);
	}

	return std::string(signHex);
}

//////////////////////////////////////////////////////////////////////////
//DB链接相关
void Work::ConnectToDb()
{
	m_dbClient = gInsideNet.GetNewSocket();
	LLOG_ERROR("Work::ConnectToDb begin %s:%d", gConfig.GetDBIp().c_str(), gConfig.GetDBPort());
	if(m_dbClient->Connect(gConfig.GetDBIp(), gConfig.GetDBPort()))
	{
		LLOG_ERROR("Work::ConnectToDb end");
	}
	else
	{
		LLOG_ERROR("Fail to connect DB");
		Stop();
	}
}

void Work::SendMsgToDb(LMsg& msg)
{
	if(m_dbClient)
	{
		m_dbClient->Send(msg.GetSendBuff());
	}
}

void Work::handerLMGCreateDesk(LMsgLMG2LCreateDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}
	User* pUser = gUserManager.GetUserbyDataId(msg->m_userid);
	if (pUser && pUser->GetDesk())
	{
		//找到玩家了， 判断是否有桌子，有就加入桌子
		HanderDeskUserLogin(pUser, msg->m_gateId,  msg->m_ip);
		RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
	}
	else
	{
		if ( pUser )
		{
			// 把之前可能存在的玩家删掉
			UserManager::Instance().DelUser(pUser);
			delete pUser;
			pUser = NULL;
		}
		//没有的话，覆盖数据，创建桌子
		pUser = new User(msg->m_usert, msg->m_gateId);
		if (pUser)
		{
			pUser->SetIp(msg->m_ip);
			UserManager::Instance().AddUser(pUser);
			pUser->Login();	

			//创建桌子
			Lint nErrorCode = pUser->HanderUserCreateDesk(msg);
			if (nErrorCode == 0)
			{
				//创建房间成功
				pUser->setUserState(LGU_STATE_DESK);
				pUser->ModifyUserState();
			}
			else
			{
				//创建房间失败
				LLOG_ERROR("Work::handerLMGCreateDesk CreateTale Failed! ErrorCode: %d", nErrorCode);
				HanderUserLogout(msg->m_gateId, msg->m_strUUID);
				RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
			}
		}
		else
		{
			//创建房间失败
			LLOG_ERROR("Work::handerLMGCreateDesk CreateTale Failed No User!");
			HanderUserLogout(msg->m_gateId, msg->m_strUUID);
			RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
		}
	}
}

void Work::handerLMGAddToDesk(LMsgLMG2LAddToDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}
	User* pUser = gUserManager.GetUserbyDataId(msg->m_userid);
	if (pUser && pUser->GetDesk())
	{
		//找到玩家了， 判断是否有桌子，有就加入桌子
		HanderDeskUserLogin(pUser, msg->m_gateId,  msg->m_ip);
	}
	else
	{
		if ( pUser )
		{
			// 把之前可能存在的玩家删掉
			UserManager::Instance().DelUser(pUser);
			delete pUser;
			pUser = NULL;
		}
		pUser = new User(msg->m_usert, msg->m_gateId);
		if (pUser)
		{
			pUser->SetIp(msg->m_ip);
			UserManager::Instance().AddUser(pUser);
			pUser->Login();
			Lint nError = pUser->HanderUserAddDesk(msg->m_deskID);
			if (nError == 0)
			{
				//加入房间成功
				pUser->setUserState(LGU_STATE_DESK);
				pUser->ModifyUserState();
			}
			else
			{
				//加入房间失败
				LLOG_ERROR("Work::handerLMGCreateDesk AddTable Failed! ErrorCode: %d", nError);
				HanderUserLogout(msg->m_gateId, msg->m_strUUID);
			}
		}
		else
		{
			//加入房间失败
			LLOG_ERROR("Work::handerLMGCreateDesk AddTable Failed No User!");
			HanderUserLogout(msg->m_gateId, msg->m_strUUID);
		}
	}
}

//处理LM发送的牌型活动配置信息
void Work::handerPXActiveInfo(LMsgLMG2LPXActive* msg)
{
	if (msg == NULL)
	{
		LLOG_ERROR("Work::handerPXActiveInfo msg is null!");
		return;
	}
	gLogicActiveManager.PXReloadConfig(msg);
}

void Work::HanderLMGCreateCoinDesk( LMsgCN2LCreateCoinDesk* msg )
{
	LMsgL2CNCreateCoinDeskRet ret;
	ret.m_errorCode = LMsgL2CNCreateCoinDeskRet::CoinDesk_NoError;
	ret.m_deskId = msg->m_deskId;   //用于coinserver回收桌子号

	std::vector<LMsgCN2LCreateCoinDesk::User> &userList = msg->m_users;

	if(userList.size() != DESK_USER_COUNT)
	{
		LLOG_ERROR("Work::HanderLMGCreateCoinDesk usersize error, size=%d", userList.size());
		ret.m_errorCode = LMsgL2CNCreateCoinDeskRet::CoinDesk_SizeError;   //人数错误
		SendToCoinsServer(ret);
		return;
	}

	for(Lsize i = 0; i < userList.size(); ++i)
	{
		User* pUser = gUserManager.GetUserbyDataId(userList[i].m_usert.m_id);
		if (pUser && pUser->GetDesk())
		{
			LLOG_ERROR("Work::HanderLMGCreateCoinDesk user already in desk error, userid=%d", pUser->GetUserDataId());
			ret.m_errorCode = LMsgL2CNCreateCoinDeskRet::CoinDesk_UserError;   //用户已经在桌子上
			SendToCoinsServer(ret);
			return;
		}
	}

	User* userArray[DESK_USER_COUNT] = {0};	// 按照位置排序
	for(Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		Lint pos = userList[i].m_pos;
		if ( pos >= 0 && pos < DESK_USER_COUNT && !userArray[pos] )
		{
			userArray[pos] = new User(userList[i].m_usert, userList[i].m_gateId);
			if (userArray[pos])
			{
				userArray[pos]->SetIp(userList[i].m_ip);
			}
		}
		else
		{
			// 删除玩家
			for(Lint k = 0; k < DESK_USER_COUNT; ++k)
			{
				if(userArray[k])
				{
					delete userArray[k];
				}
			}
			LLOG_ERROR("Work::HanderLMGCreateCoinDesk pos error, pos=%d", i);
			ret.m_errorCode = LMsgL2CNCreateCoinDeskRet::CoinDesk_PosError;
			SendToCoinsServer(ret);
			return;
		}
	}

	//创建桌子
	Lint nErrorCode = gRoomVip.CreateVipCoinDesk(msg,userArray);
	if (nErrorCode == 0)  //创建房间成功
	{
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			UserManager::Instance().AddUser(userArray[i]);
			userArray[i]->setUserState(LGU_STATE_COINDESK);
			userArray[i]->ModifyUserState();
		}
	}
	else
	{
		//创建房间失败
		LLOG_ERROR("Work::HanderLMGCreateCoinDesk CreateTale Failed! ErrorCode: %d", nErrorCode);
		ret.m_errorCode = LMsgL2CNCreateCoinDeskRet::CoinDesk_Unknown;
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			if(userArray[i])
			{
				delete userArray[i];
				userArray[i] = NULL;
			}
		}
	}
	SendToCoinsServer(ret);
}

void Work::RecycleDeskId(Lint deskID)
{
	LMsgL2LMGRecyleDeskID msg;
	msg.m_serverID = gConfig.GetServerID();
	msg.m_deskID = deskID;
	gWork.SendToLogicManager(msg);
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

	LMsgHeartBeatReplyMsg replyMsg;
	replyMsg.m_fromWho = HEARBEAT_WHO_LOGICSERVER;
	replyMsg.m_iServerId = gConfig.GetServerID();

	msg->m_sp->Send(replyMsg.GetSendBuff());
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

void Work::HanderRLogInfo(LMsgLMG2LRLogInfo* msg)
{
	LLOG_ERROR( "Work::HanderRLogInfo rlog config, open=%d url=%s size=%d num=%d", msg->bopenrlog, msg->m_url.c_str(), msg->m_max_cached_size, msg->m_max_cached_num  );
	m_managerID = msg->m_managerID;
	gRLT.SetConfig( msg->bopenrlog, msg->m_url, msg->m_max_cached_size, msg->m_max_cached_num );
}

void Work::_initTickTime()
{
	LTime cur;
	m_1SceTick = cur.MSecs();
	m_15SecTick = cur.MSecs();
	m_30SceTick = cur.MSecs();
	m_60SecTick = cur.MSecs();
}

void Work::_reconnectLogicManager()
{
	if(!m_logicManager)
	{
		LLOG_ERROR("Logic manager socket is null");
		return;
	}

	LLOG_ERROR("Reconnect logic manager...");
	LTime curTime;
	Llong lOldTime = 0;
	auto itReconnect = m_mapReconnect.find(m_logicManager);
	if(itReconnect != m_mapReconnect.end()) //说明已经存在重连了
	{
		if((curTime.MSecs() - itReconnect->second) > 3 * 60 * 1000) //超过3分钟重连不再重连了
		{
			LLOG_ERROR("Reconnect time is more than 3 minutes. Stop to reconnect logic manager");
			return;
		}

		lOldTime = itReconnect->second;
	}

	m_mapReconnect.erase(m_logicManager);
	m_mapHeartBeat.erase(m_logicManager);

	m_logicManager->Stop();
	m_logicManager = gInsideNet.GetNewSocket();
	m_logicManager->AsyncConnect(gConfig.GetLogicManagerIp(), gConfig.GetLogicManagerPort());

	if(lOldTime != 0)
	{
		m_mapReconnect[m_logicManager] = lOldTime;
	}
	else
	{
		m_mapReconnect[m_logicManager] = curTime.MSecs();
	}
}

void Work::_reconnectLogicDB()
{
	if(!m_dbClient)
	{
		LLOG_ERROR("Logic DB socket is null");
		return;
	}

	LLOG_ERROR("Reconnect logic DB...");
	m_mapReconnect.erase(m_dbClient);
	m_mapHeartBeat.erase(m_dbClient);

	m_dbClient->Stop();
	m_dbClient = gInsideNet.GetNewSocket();
	m_dbClient->AsyncConnect(gConfig.GetDBIp(), gConfig.GetDBPort());

	m_mapReconnect[m_dbClient] = true;
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
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_LOGICSERVER;
	msgHeatbeat.m_iServerId = gConfig.GetServerID();

	SendToLogicManager(msgHeatbeat);

	m_mapHeartBeat[m_logicManager] = true;
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
		_reconnectLogicDB();
		return;
	}

	LMsgHeartBeatRequestMsg msgHeatbeat;
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_LOGICSERVER;
	msgHeatbeat.m_iServerId = gConfig.GetServerID();

	SendMsgToDb(msgHeatbeat);

	m_mapHeartBeat[m_dbClient] = true;
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
	msgHeatbeat.m_fromWho = HEARBEAT_WHO_LOGICSERVER;
	msgHeatbeat.m_iServerId = gConfig.GetServerID();

	m_coinsServer.m_sp->Send(msgHeatbeat.GetSendBuff());

	m_mapHeartBeat[m_coinsServer.m_sp] = true;
}