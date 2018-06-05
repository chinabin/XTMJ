#include "User.h"
#include "Work.h"
#include "UserManager.h"
#include "Config.h"
#include "ActiveManager.h"
#include "DeskManager.h"
#include "RuntimeInfoMsg.h"
#include "RLogHttp.h"

User::User(LUser data, Lint gateId) : m_userData(data)
{
	m_gateId = gateId;
	m_online = true;
	m_userState = LGU_STATE_CENTER;
	m_logicID = 0;
	m_Active.clear();
	m_lastCreateGameType = -1;
}
	
User::~User()
{
}

void User::Tick()
{

}

//获取玩家数据库里id
Lint User::GetUserDataId()
{
	return m_userData.m_id;
}

void User::SetUserGateId(Lint gateId)
{
	m_gateId = gateId;
}

Lint User::GetUserGateId()
{
	return m_gateId;
}

bool User::GetOnline()
{
	return m_online;
}

void User::Login()
{
	LTime now;
	Lint lastLoginTime = m_userData.m_lastLoginTime;
	m_userData.m_lastLoginTime = now.Secs();
	m_online = true;
	if(getUserLogicID() > 0)
	{
		if(gWork.isLogicServerExist(getUserLogicID()))
		{
			LMsgLMG2LLogin msg;
			msg.m_userid = m_userData.m_id;
			msg.m_gateId = GetUserGateId();
			msg.m_strUUID = m_userData.m_unioid;
			msg.m_ip = getUserIPStr();
			msg.m_buyInfo = gRuntimeInfoMsg.getBuyInfo(m_userData.m_id);
			msg.m_hide = gRuntimeInfoMsg.getHide();
			msg.m_card_num = m_userData.m_numOfCard2s;

			gWork.SendMessageToLogic(getUserLogicID(), msg);
		}
		else
		{  
		    setUserLogicID(0);
			SendItemInfo();
			SendLoginInfo(lastLoginTime);
		}
	}
	else
	{
		// 防止金币服务器有问题 如果玩家处于金币大厅 直接修改为Center状态
		if ( m_userState == LGU_STATE_COIN && !gWork.IsCoinsServerConnected() )
		{
			m_userState = LGU_STATE_CENTER;
		}
		SendItemInfo();
		SendLoginInfo(lastLoginTime);
	}

	//玩家登陆消息发送到金币服务器
	LMsgLMG2CNUserLogin loginmsg;
	loginmsg.m_strUUID = m_userData.m_unioid;
	loginmsg.m_gateId = GetUserGateId();
	loginmsg.m_ip = getUserIPStr();
	loginmsg.m_usert = m_userData;
	gWork.SendMessageToCoinsServer( loginmsg );

	//玩家登陆消息发送到DB DB给玩家发送活动信息
	LMsgLMG2LdbUserLogin dbloginmsg;
	dbloginmsg.m_userId = m_userData.m_id;
	dbloginmsg.m_strUUID = m_userData.m_unioid;
	gWork.SendMsgToDb( dbloginmsg );

	SendHorseInfo();
	SendFreeInfo();
	SendExchInfo();

	boost::shared_ptr<UserBaseInfo> baseInfo(new UserBaseInfo());
	baseInfo->m_id = GetUserDataId();
	baseInfo->m_nike = m_userData.m_nike;
	baseInfo->m_openId = m_userData.m_openid;
	baseInfo->m_sex = m_userData.m_sex;
	baseInfo->m_headImageUrl = m_userData.m_headImageUrl;
	gUserManager.addUserBaseInfo(baseInfo);

	// 远程日志
	RLOG("login", getUserIPStr()
		<< "|" << LTimeTool::GetLocalTimeInString()
		<< "|" << m_userData.m_id
		<< "|" << m_userData.m_numOfCard2s
		<< "|" << m_userData.m_nike
		<< "|" << gConfig.GetServerID()
		<< "|" << gConfig.GetInsideIp());

	//Save();
}

void User::Logout()
{
	m_online = false;

	// 远程日志
	RLOG("logout", getUserIPStr()
		<< "|" << LTimeTool::GetLocalTimeInString()
		<< "|" << m_userData.m_id
		<< "|" << m_userData.m_numOfCard2s
		<< "|" << m_userData.m_nike
		<< "|" << gConfig.GetServerID()
		<< "|" << gConfig.GetInsideIp());
}

void User::Send(LMsg& msg)
{
	LLOG_DEBUG( "User::Send userid=%d msgid=%d", m_userData.m_id, msg.m_msgId );

	LMsgL2GUserMsg send;
	send.m_strUUID = m_userData.m_unioid;
	send.m_dataBuff = msg.GetSendBuff();

	gWork.SendMessageToGate(m_gateId, send);
}

void User::Send(const LBuffPtr& buff)
{
	LMsgL2GUserMsg send;
	send.m_strUUID = m_userData.m_unioid;
	send.m_dataBuff = buff;

	gWork.SendMessageToGate(m_gateId, send);
}

void User::SendLoginInfo(Lint lastLoginTime)
{
	LMsgS2CLogin send;
	send.m_id = m_userData.m_id;
	send.m_errorCode = 0;
	send.m_nike = m_userData.m_nike;
	send.m_exp = 0;
	send.m_sex = m_userData.m_sex;
	send.m_new = m_userData.m_new;
	send.m_state = getUserLogicID()> 0 ? 1 : 0;
	send.m_ip = m_ip;
	send.m_face = m_userData.m_headImageUrl;
	send.m_buyInfo = gRuntimeInfoMsg.getBuyInfo(m_userData.m_id);
	send.m_hide = gRuntimeInfoMsg.getHide();
	send.m_gm = m_userData.m_gm;
	send.m_inviterId = m_userData.m_agentId;
	if(m_userData.m_new == 0)
	{
		const int INIT_CARD_NUM_NEW = 8;			// 新的送6张
		// 新手送卡
		send.m_card1 = 0;
		send.m_card2 = INIT_CARD_NUM_NEW;
		send.m_card3 = 0;
		AddCardCount(CARD_TYPE_8, INIT_CARD_NUM_NEW, CARDS_OPER_TYPE_INIT,"system");

		// 把新用户状态变成老用户
		m_userData.m_new = 1;
		LMsgL2CeModifyUserNew save;
		save.m_strUUID = m_userData.m_unioid;
		save.m_value = 1;
		gWork.SendToCenter(save);

	}
	else
	{
		send.m_card1 = 0;
		send.m_card2 = 0;
		send.m_card3 = 0;
		send.m_loginInterval = 0;
		if(gActiveManager.OUGCChekGiveCard(lastLoginTime, send.m_card1, send.m_loginInterval))
		{
			if(send.m_card1 > 0)
			{
				AddCardCount(CARD_TYPE_8, send.m_card1, CARDS_OPER_TYPE_ACTIVE, "system_ougc");
			}
		}
	}

	Lint startTime = gConfig.GetTime();
	if(startTime > 0 && m_userData.m_regTime >= startTime)
	{
		send.m_oldUser = 0;
	}

	Send(send);
}

void User::SendItemInfo()
{
	LMsgS2CItemInfo send;
	send.m_card1 = m_userData.m_numOfCard1s;
	send.m_card2 = m_userData.m_numOfCard2s;
	send.m_card3 = m_userData.m_numOfCard3s;
	send.m_coins = m_userData.m_coins;
	send.m_credit = m_userData.m_creditValue;
	Send(send);
}

void User::SendHorseInfo()
{
	LMsgS2CHorseInfo send;
	send.m_str = gRuntimeInfoMsg.getHorseInfo();
	send.m_type = 0;
	Send(send);
}


void User::S2CeUserLogin()
{
	LMsgL2CeUserServerLogin msg;
	msg.m_serverID = gConfig.GetServerID();
	msg.m_openID = m_userData.m_unioid;
	LLOG_DEBUG("Work::HanderLogicUserLogin, userID: %s, ServerID %d", msg.m_openID.c_str(), msg.m_serverID);
	gWork.SendToCenter(msg);
}

void User::S2CeUserLogout()
{
	LMsgL2CeUserServerLogout msg;
	msg.m_serverID = gConfig.GetServerID();
	msg.m_openID = m_userData.m_unioid;
	LLOG_DEBUG("Work::HanderLogicUserLogot, userID: %s, ServerID %d", msg.m_openID.c_str(), msg.m_serverID);
	gWork.SendToCenter(msg);
}

void User::HanderMsg(LMsg* msg)
{
	switch (msg->m_msgId)
	{
	case MSG_C_2_S_CREATE_ROOM:
		HanderUserCreateDesk((LMsgC2SCreateDesk*)msg);
		break;
	case MSG_C_2_S_ADD_ROOM:
		HanderUserAddDesk((LMsgC2SAddDesk*)msg);
		break;
	case MSG_C_2_S_ENTER_COINS_ROOM:
		HanderUserEnterCoinsDesk((LMsgC2SEnterCoinsDesk*)msg);
		break;
	case MSG_C_2_S_VIP_LOG:
		HanderUserVipLog((LMsgC2SVipLog*) msg);
		break;
	case MSG_C_2_S_REQ_CREHIS:
		HanderUserCreLog((LMsgC2SCRELog*) msg);
		break;
	case MSG_C_2_S_REQ_EXCH:
		HanderUserReqEXCH((LMsgC2SREQ_Exch*) msg);
		break;
	case MSG_C_2_S_REQ_GTU:
		HanderUserReqGTU((LMsgC2SREQ_GTU*) msg);
		break;
	case MSG_C_2_S_ROOM_LOG:
		HanderUserRoomLog((LMsgC2SRoomLog*)msg);
		break;
	case MSG_C_2_S_VIDEO: 
		HanderUserVideo((LMsgC2SVideo*) msg);
		break;

	case MSG_C_2_S_GET_USER_INFO:     //获取 用户信息
		HanderGetUserInfo((LMsgC2SGetUserInfo*)msg);
		break;

	case MSG_C_2_S_BIND_INVITER:    //bind邀请人
		HanderBindInviter((LMsgC2SBindInviter*)msg);
		break;

	case MSG_C_2_S_GET_INVITING_INFO:
		HanderGetInvitingInfo((LMsgC2SGetInvitingInfo *)msg);
		break;

	case MSG_C_2_S_ACTIVITY_WRITE_PHONE:
		HanderActivityPhone((LMsgC2SActivityPhone *)msg);
		break;
	case MSG_C_2_S_ACTIVITY_REQUEST_LOG:
		HanderActivityRequestLog((LMsgC2SActivityRequestLog *)msg);
		break;
	case MSG_C_2_S_ACTIVITY_REQUEST_DRAW_OPEN:
		HanderActivityRequestDrawOpen((LMsgC2SActivityRequestDrawOpen *)msg);
		break;
	case MSG_C_2_S_ACTIVITY_REQUEST_DRAW:
		HanderActivityRequestDraw((LMsgC2SActivityRequestDraw *)msg);
		break;
	case MSG_C_2_S_ACTIVITY_REQUEST_SHARE:
		HanderActivityRequestShare((LMsgC2SActivityRequestShare*)msg);
		break;
	case MSG_C_2_S_UPDATE_RANK:
		HanderUpdateRank((LMsgC2SUpdateRank*)msg);
		break;
	case MSG_C_2_S_GET_RANK_REWARD:
		HanderGetRankReward((LMsgC2SGetRankReward*)msg);
		break;
	case MSG_C_2_S_SHARE:
		HanderUserShare((LMsgC2SShare*)msg);
		break;
	case MSG_C_2_S_TASK_INFO:
		HanderUserTaskInfo((LMsgC2STaskInfo*)msg);
		break;
	case MSG_C_2_S_TASK_REWARD:
		HanderUserTaskReward((LMsgC2STaskReward*)msg);
		break;

	default:
		LLOG_ERROR("LogicManager Message Error: %d", msg->m_msgId);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//注释玩家操作

void User::HanderUserCreateDesk(LMsgC2SCreateDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}

	if(getUserLogicID() > 0)
	{
		LLOG_ERROR("User::HanderLMGUserCreateDesk user already in LogicServer, userid=%d", m_userData.m_id);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 2;
		Send(ret);
		return;
	}

	//房卡不足
	if( !(gRuntimeInfoMsg.isFree(-1) || gRuntimeInfoMsg.isFree(msg->m_state)) && !IfCardEnough(msg->m_flag, true))
	{
		LLOG_ERROR("User::HanderLMGUserCreateDesk Card Not Enough, userid=%d, cardnum=%d", m_userData.m_id, m_userData.m_numOfCard2s);
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 1;
		Send(ret);
		return;
	}

	///是否 对信用做了限制
	Lint nCreditLimit = msg->m_credits;
	/*if(nCreditLimit > 0)
	{
	if(nCreditLimit > m_userData.m_creditValue)
	{
	LLOG_ERROR("User::HanderLMGUserCreateDesk Credits Not Enough, userid=%d, credits=%d", m_userData.m_id,m_userData.m_creditValue);
	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode = LMsgS2CCreateDeskRet::Err_CreditNotEnough;
	Send(ret);
	return;
	}
	}*/

	LLOG_INFO("User::HanderUserCreateDesk userid=%d flag=%d", m_userData.m_id, msg->m_flag);
	LMsgLMG2LCreateDesk send ;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_strUUID = m_userData.m_unioid;
	send.m_ip = GetIp();
	send.m_usert = m_userData;	
	send.m_flag = msg->m_flag;
	send.m_secret = msg->m_secret;
	send.m_gold = msg->m_gold;
	send.m_state = msg->m_state;
	send.m_robotNum = msg->m_robotNum;
	send.m_playTypeCount = msg->m_playType.size();
	send.m_playType = msg->m_playType;
	memcpy(send.m_cardValue, msg->m_cardValue, sizeof(send.m_cardValue) );

	Lint iLogicServerId = getUserLogicID();
	if(!gWork.isLogicServerExist(iLogicServerId))
	{
		iLogicServerId = gWork.allocateLogicServer(msg->m_state);	//获取负载最小的逻辑
	}
	else
	{
		LLOG_ERROR("User::HanderUserCreateDesk Current ServerID Not Null, userid=%d, logicid=%d", m_userData.m_id, getUserLogicID());
	}
	if(iLogicServerId == INVALID_LOGICSERVERID)
	{
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 4;
		Send(ret);

		LLOG_ERROR("User::HanderLMGUserCreateDesk Logic Not Exist, userid=%d", m_userData.m_id);
		return;
	}
	Lint cardType = msg->m_flag;
	Lint gameType = msg->m_state;
	bool limitIp = false;
	for(auto it=msg->m_playType.begin(); it!=msg->m_playType.end(); ++it)
	{
		if(*it == PT_SQ_LIMITIP)
		{
			limitIp = true;
			break;
		}
	}
	Lstring createIp = limitIp? GetIp() : "";

	send.m_deskID = gDeskManager.GetFreeDeskId(m_userData.m_id, iLogicServerId, nCreditLimit, cardType, gameType, createIp);

	gWork.SendMessageToLogic(iLogicServerId, send);

	m_lastCreateGameType = msg->m_state;

	LLOG_DEBUG("User::HanderUserCreateDesk userid=%d flag=%d roomId=%d", m_userData.m_id, msg->m_flag, send.m_deskID);
}

void User::HanderUserAddDesk(LMsgC2SAddDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}

	DeskInfos info = gDeskManager.GetDeskInfo(msg->m_deskId);
	if(!info)
	{
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskId;
		ret.m_errorCode = 2;
		Send(ret);

		LLOG_ERROR("ERROR: User::HanderUserAddDesk desk Not Exist, userid=%d", m_userData.m_id);
		return;
	}

	//Lint limit = gDeskManager.GetDeskCreditLimit(msg->m_deskId);
	/*if(limit > 0)
	{
	if(m_userData.m_creditValue < limit)
	{
	LMsgS2CAddDeskRet ret;
	ret.m_deskId = msg->m_deskId;
	ret.m_errorCode = LMsgS2CAddDeskRet::Err_CreditNotEnough;
	Send(ret);

	LLOG_ERROR("ERROR: User::HanderUserAddDesk credits not enough, userid=%d", m_userData.m_id);
	return;
	}
	}*/

	//Lstring createIp = gDeskManager.GetDeskCreateIp(msg->m_deskId);
	//Lint createUserId = gDeskManager.GetDeskCreateUserId(msg->m_deskId);
	//创建房间时选择了同IP不能进入
	if(m_userData.m_id != info.m_userId && !info.m_createIp.empty() && info.m_createIp.compare(GetIp())==0)			//(m_userData.m_id != createUserId && !createIp.empty() && createIp.compare(GetIp())==0)
	{
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskId;
		ret.m_errorCode = 4;
		Send(ret);

		LLOG_ERROR("ERROR: User::HanderUserAddDesk ip error, userid=%d", m_userData.m_id);
		return;
	}

	// 10圈的房间 每人都需要1张房卡
	//Lint cardType = gDeskManager.GetDeskCardType(msg->m_deskId);
	if(m_userData.m_id != info.m_userId && !IfCardEnough(info.m_cardType, false))
	{
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskId;
		ret.m_errorCode = 3;
		Send(ret);

		LLOG_ERROR("ERROR: User::HanderUserAddDesk cardType=%d gameType=%d userid=%d", info.m_cardType, info.m_gameType, m_userData.m_id);
		return;
	}


	LLOG_INFO("User::HanderUserAddDesk userid=%d deskid=%d", m_userData.m_id, msg->m_deskId);


	Lint iLogicServerId = getUserLogicID();
	if(!gWork.isLogicServerExist(iLogicServerId))
	{
		iLogicServerId = gDeskManager.GetLogicServerIDbyDeskID(msg->m_deskId);
		if(!gWork.isLogicServerExist(iLogicServerId))
		{
			iLogicServerId = INVALID_LOGICSERVERID;
		}
	}
	else
	{
		LLOG_ERROR("User::HanderUserAddDesk Current ServerID Not Null, userid=%d, logicid=%d", m_userData.m_id, getUserLogicID());
	}

	if(iLogicServerId == INVALID_LOGICSERVERID)
	{
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskId;
		ret.m_errorCode = 2;
		Send(ret);

		LLOG_ERROR("ERROR: User::HanderUserAddDesk Logic Not Exist, userid=%d", m_userData.m_id);
		return;
	}

	LMsgLMG2LAddToDesk send;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_strUUID = m_userData.m_unioid;
	send.m_deskID = msg->m_deskId;
	send.m_ip = GetIp();
	send.m_usert = m_userData;

	gWork.SendMessageToLogic(iLogicServerId, send);
}

void User::HanderUserEnterCoinsDesk(LMsgC2SEnterCoinsDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}

	if( !gWork.IsCoinsServerConnected() )
	{
		LMsgS2CEnterCoinsDeskRet ret;
		ret.m_errorCode = 4;
		Send(ret);

		LLOG_ERROR("User::HanderUserEnterCoinsDesk CoinsServer not Connected, userid=%d", m_userData.m_id);
		return;
	}

	if(getUserLogicID() > 0)
	{
		LMsgS2CEnterCoinsDeskRet ret;
		ret.m_errorCode = 2;
		Send(ret);

		LLOG_ERROR("User::HanderUserEnterCoinsDesk user already in LogicServer, userid=%d", m_userData.m_id);
		return;
	}

	LLOG_INFO("User::HanderUserEnterCoinsDesk userid=%d state=%d", m_userData.m_id, msg->m_state);
	LMsgLMG2CNEnterCoinDesk send;
	send.m_strUUID = m_userData.m_unioid;
	send.m_gateId = GetUserGateId();
	send.m_ip = GetIp();
	send.m_usert = m_userData;	
	send.m_state = msg->m_state;
	send.m_robotNum = msg->m_robotNum;
	send.m_playType = msg->m_playType;
	send.m_coins = msg->m_coins;
	memcpy(send.m_cardValue, msg->m_cardValue, sizeof(send.m_cardValue) );
	gWork.SendMessageToCoinsServer( send );
}

void User::HanderUserVipLog(LMsgC2SVipLog* msg)
{
	LLOG_DEBUG("User::HanderUserVipLog userid=%d, gm=%d",msg->m_userId,m_userData.m_gm);
	LMsgL2LBDReqVipLog log;
	log.m_strUUID = m_userData.m_unioid;
	log.m_userId = GetUserDataId();
	log.m_time = msg->m_time;
	if (m_userData.m_gm && msg->m_userId != 0)
	{
		log.m_reqUserId = msg->m_userId;
		LLOG_ERROR("User::HanderUserVipLog requserid=%d, userid=%d", log.m_reqUserId, log.m_userId);
	}
	else
	{
		log.m_reqUserId = GetUserDataId();
	}
	gWork.SendMsgToDb(log);
}

void User::HanderUserCreLog( LMsgC2SCRELog* msg )
{
	LLOG_DEBUG("User::HanderUserCreLog userid=%d",msg->m_userId);
	LMsgL2LBDReqCRELog log;
	log.m_strUUID = m_userData.m_unioid;
	log.m_userId = GetUserDataId();
	log.m_time = msg->m_time;
	
	gWork.SendMsgToDb(log);
}

void User::HanderUserReqGTU( LMsgC2SREQ_GTU* msg )
{
	LLOG_DEBUG("User::HanderUserReqGTU userid=%d req dian zan zan zan....",msg->m_userId);

	User::AddCreditForUserlist(msg->m_userList);

	LMsgL2LDBDEL_GTU log;
	log.m_strUUID = m_userData.m_unioid;
	log.m_userId = msg->m_userId;
	log.m_strLog = msg->m_onelog;
	gWork.SendMsgToDb(log);

	//返回客户端
	LMsgS2CREQ_GTU_RET ret;
	ret.m_index = msg->m_index;
	this->Send(ret);
}

void User::HanderUserReqEXCH( LMsgC2SREQ_Exch* msg )
{
	LLOG_DEBUG("User::HanderUserReqEXCH card=%d req exc exc....",msg->m_card);

	LMsgS2CREQ_EXCH_RET ret;
	ret.m_card = -1;  //活动未开启

	if(gActiveManager.CheckTimeValid_Exch())
	{
		bool suc = DealExchangeCard(msg->m_card);
		if(suc)
		{
			ret.m_card = msg->m_card;
		}
		else
		{
			ret.m_card = -2;   //金币不够
		}
	}
	this->Send(ret);
}

void User::HanderUserRoomLog(LMsgC2SRoomLog* msg)
{
 	LLOG_DEBUG("User::HanderUserVipLog userid=%d, deskid=%d, time=%d", m_userData.m_id, msg->m_deskId, msg->m_time);
	LMsgC2SRoomLog log;
	log.m_strUUID = m_userData.m_unioid;
	log.m_userId  = GetUserDataId();
	log.m_time	  = msg->m_time;
	log.m_deskId  = msg->m_deskId;
	log.m_pos	  = msg->m_pos;

 	gWork.SendMsgToDb(log);
}

void User::HanderUserVideo(LMsgC2SVideo* msg)
{
	LMsgL2LDBReqVideo video;
	video.m_strUUID = m_userData.m_unioid;
	video.m_userId = GetUserDataId();
	video.m_videoId = msg->m_videoId;

	gWork.SendMsgToDb(video);
}

static bool SortFun(LActive c1,LActive c2)
{
	if(c1.m_LasJoinDate > c2.m_LasJoinDate)
	{
		return true;
	}

	return false;
}

Lstring User::GetIp()
{
	return m_ip;
}

void User::SetIp(Lstring& ip)
{
	m_ip = ip;
}

bool User::IfCardEnough(const Lint cardType, const bool create)
{
	if(cardType <= CARD_TYPE_NONE || cardType >= CARD_TYPE_MAX)
		return false;

	int count = 0;
	if(NeedCardCount(cardType, count) || create)
		return m_userData.m_numOfCard2s >= count;

	return true;
}

void User::AddCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin, bool bNeedSave)
{
	LLOG_INFO("User::AddCardCount userid=%d, curcount=%d, type=%d, count=%d, operType=%d", m_userData.m_id, m_userData.m_numOfCard2s, cardType, count, operType);

	m_userData.m_numOfCard2s += count;
	m_userData.m_totalbuynum += count;

	if (bNeedSave)
	{
		// 透传给Center
		LMsgL2LMGModifyCard msg;
		msg.admin = admin;
		msg.cardType = cardType;
		msg.cardNum = count;
		msg.isAddCard = 1;
		msg.operType = operType;
		msg.m_userid = m_userData.m_id;
		msg.m_strUUID= m_userData.m_unioid;
		gWork.SendToCenter(msg);
	}
	SendItemInfo();
}

// void User::AddCardCount( Lint id, Lstring strUUID, Lint cardType, Lint count, Lint operType, const Lstring& admin )
// {
// 	// 透传给Center
// 	LMsgL2LMGModifyCard msg;
// 	msg.admin = admin;
// 	msg.cardType = cardType;
// 	msg.cardNum = count;
// 	msg.isAddCard = 1;
// 	msg.operType = operType;
// 	msg.m_userid = id;
// 	msg.m_strUUID = strUUID;
// 	gWork.SendToCenter(msg);
// }

void User::AddCreditForUserlist(const std::vector<Lint> &userList)
{
	LMsgLM2CEN_ADD_CRE msg;
	memset(msg.m_user,0,sizeof(msg.m_user));

	for(Lint i = 0; i < userList.size() && i < 4; ++i)
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userList[i]);
		if(safeUser.get() == NULL || !safeUser->isValid())
		{
			continue;
		}

		boost::shared_ptr<User> user = safeUser->getResource();
		if(user->m_userData.m_id == m_userData.m_id)
		{
			continue;   //不能给自己点赞
		}

		msg.m_user[i] = userList[i];

		if (user->GetOnline() )
		{
			user->m_userData.m_creditValue++;
		}
	}

	gWork.SendToCenter(msg);
}

bool User::DealExchangeCard(Lint inCard)
{
	std::map<Lint,Lint>& rule = gActiveManager.GetExchRule();
	auto it = rule.find(inCard);
	if(it != rule.end() && rule[inCard] <= m_userData.m_coins)
	{
		Lint delCoin = rule[inCard];

		m_userData.m_coins -= delCoin;
		m_userData.m_numOfCard2s += inCard;

		LMsgL2LMGExchCard exch;
		exch.m_add = inCard;
		exch.m_del = delCoin;
		exch.m_strUUID = m_userData.m_unioid;
		exch.m_userid = m_userData.m_id;
		exch.admin = "system";
		exch.cardType = CARD_TYPE_Other;
		exch.operType = CARDS_OPER_TYPE_EXCHANGE;

		gWork.SendToCenter(exch);

		SendItemInfo();

		return true;
	}
	return false;
}

void User::DelCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin, bool bNeedSave)
{
	LLOG_INFO("User::DelCardCount userid=%d, curcount=%d, type=%d, count=%d, operType=%d", m_userData.m_id, m_userData.m_numOfCard2s, cardType, count, operType);

	if (m_userData.m_numOfCard2s >= count)
		m_userData.m_numOfCard2s -= count;
	else
		m_userData.m_numOfCard2s = 0;

	if ( bNeedSave )
	{
		// 透传给Center
		LMsgL2LMGModifyCard msg;
		msg.admin = admin;
		msg.cardType = cardType;
		msg.cardNum = count;
		msg.isAddCard = 0;
		msg.operType = operType;
		msg.m_userid = m_userData.m_id;
		msg.m_strUUID= m_userData.m_unioid;
		gWork.SendToCenter(msg);
	}

	SendItemInfo();
}

void User::AddCoinsCount(Lint count, Lint operType, bool bNeedSave)
{
	m_userData.m_coins += count;
	if ( bNeedSave )
	{
		// 透传给Center
		LMsgCN2LMGModifyUserCoins msg;
		msg.coinsNum = count;
		msg.isAddCoins = 1;
		msg.operType = operType;
		msg.m_userid = m_userData.m_id;
		msg.m_strUUID= m_userData.m_unioid;
		gWork.SendToCenter(msg);
	}
	SendItemInfo();
}

void User::DelCoinsCount(Lint count, Lint operType, bool bNeedSave)
{
	if ( m_userData.m_coins < count )
	{
		m_userData.m_coins = 0;
	}
	else
	{
		m_userData.m_coins -= count;
	}
	if ( bNeedSave )
	{
		// 透传给Center
		LMsgCN2LMGModifyUserCoins msg;
		msg.coinsNum = count;
		msg.isAddCoins = 0;
		msg.operType = operType;
		msg.m_userid = m_userData.m_id;
		msg.m_strUUID= m_userData.m_unioid;
		gWork.SendToCenter(msg);
	}
	SendItemInfo();
}

void User::AddPlayCount()
{
	LLOG_INFO("User::AddPlayCount userid=%d", m_userData.m_id);
	m_userData.m_totalplaynum++;

	// 透传给Center
	LMsgL2LMGAddUserPlayCount msg;
	msg.m_userid = m_userData.m_id;
	msg.m_strUUID= m_userData.m_unioid;
	gWork.SendToCenter(msg);
}

void User::HanderGetUserInfo( LMsgC2SGetUserInfo* msg )
{
	LMsgS2CUserInfo ret;
	ret.m_error = ErrorCode::UserNotExist;

	boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_userId);
	if(safeBaseInfo && safeBaseInfo->isValid())
	{
		ret.m_user.m_id			= safeBaseInfo->getResource()->m_id;
		ret.m_user.m_unionId	= safeBaseInfo->getResource()->m_openId;
		ret.m_user.m_nickName	= safeBaseInfo->getResource()->m_nike;
		ret.m_user.m_headImgUrl = safeBaseInfo->getResource()->m_headImageUrl;
		ret.m_error = ErrorCode::ErrorNone;
	}
	Send(ret);
}

void User::HanderGetInvitingInfo( LMsgC2SGetInvitingInfo *msg )
{
	LMsgC2SGetInvitingInfo log;
	log.m_userId = m_userData.m_id;
	log.m_strUUID= m_userData.m_unioid;

	gWork.SendMsgToDb(log);
}

void User::HanderActivityPhone(LMsgC2SActivityPhone *msg)
{
	LMsgC2SActivityPhone send = *msg;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;
	gWork.SendMsgToDb( send );
}

void User::HanderActivityRequestLog(LMsgC2SActivityRequestLog *msg)
{
	LMsgC2SActivityRequestLog send = *msg;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;
	gWork.SendMsgToDb( send );
}

void User::HanderActivityRequestDrawOpen(LMsgC2SActivityRequestDrawOpen *msg)
{
	LMsgC2SActivityRequestDrawOpen send = *msg;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;
	gWork.SendMsgToDb( send );
}

void User::HanderActivityRequestDraw(LMsgC2SActivityRequestDraw *msg)
{
	LMsgC2SActivityRequestDraw send = *msg;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;
	send.m_cardNum = m_userData.m_numOfCard2s;
	gWork.SendMsgToDb( send );
}

void User::HanderActivityRequestShare(LMsgC2SActivityRequestShare *msg)
{
	LMsgC2SActivityRequestShare send = *msg;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;
	gWork.SendMsgToDb( send );
}

void User::HanderBindInviter( LMsgC2SBindInviter* msg )
{
	ErrorCode code = ErrorCode::ErrorNone;

	if(msg->m_inviterId != m_userData.m_id)
	{
		if(m_userData.m_agentId == 0)		//没有绑定过代理
		{
// 			boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_inviterId);
// 			if(safeBaseInfo && safeBaseInfo->isValid())
// 			{
// 				LLOG_INFO("User: HanderBindInviter bind inviterid = %d  msg!!!",msg->m_inviterId);
//
// 				LMsgLM_2_LDBBindInviter msg2db;
// 				msg2db.m_inviterId = msg->m_inviterId;
// 				msg2db.m_userId = m_userData.m_id;
// 				gWork.SendMsgToDb(msg2db);
// 			}
// 			else
// 			{
// 				LLOG_ERROR("User: bind error %d not exist!!!",msg->m_inviterId);
// 				code = ErrorCode::UserNotExist;
// 			}

// 			boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_inviterId);
// 			if(safeBaseInfo && safeBaseInfo->isValid())
// 			{
// 				if(safeBaseInfo->getResource()->m_agentPower != 0)
// 				{
// 					//m_userData.m_agentId = msg->m_inviterId;			此处不再赋值 由CenterServer返回后再处理
//
// 					LMsgLMG2CEAddAgent msg2ce;
// 					msg2ce.m_userid = m_userData.m_id;
// 					msg2ce.m_agentId = msg->m_inviterId;
// 					msg2ce.m_strUUID = m_userData.m_unioid;
//
// 					gWork.SendToCenter(msg2ce);
// 				}
// 				else
// 				{
// 					code = ErrorCode::NoAgentPower;
// 				}
// 			}
// 			else
// 			{
// 				LLOG_ERROR("User: bind error %d not exist!!!",msg->m_inviterId);
// 				code = ErrorCode::UserNotExist;
// 			}

			LMsgLMG2CEAddAgent msg2ce;
			msg2ce.m_userid = m_userData.m_id;
			msg2ce.m_agentId = msg->m_inviterId;
			msg2ce.m_strUUID = m_userData.m_unioid;
			gWork.SendToCenter(msg2ce);
		}
		else
		{
			code = ErrorCode::AlreadyBinded;
		}
	}
	else
	{   //绑定 
		code = ErrorCode::BindingSelf;
	}

	LMsgS2CBindInviter ret;
	ret.m_error = code;
// 	if(ret.m_error == ErrorCode::ErrorNone)
// 	{
// 		boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_inviterId);
// 		if(safeBaseInfo && safeBaseInfo->isValid())
// 		{
// 			ret.m_inviter.m_headImgUrl	= safeBaseInfo->getResource()->m_headImageUrl;
// 			ret.m_inviter.m_nickName	= safeBaseInfo->getResource()->m_nike;
// 			ret.m_inviter.m_unionId		= safeBaseInfo->getResource()->m_openId;
// 			ret.m_inviter.m_id			= msg->m_inviterId;
// 		}
// 	}
//
// 	Send(ret);

	//有错误直接发给客户端 没错误会发给CenterServer判断是否绑定成功
	if(ret.m_error !=ErrorCode::ErrorNone)
	{
		Send(ret);
	}
}

void User::HanderUpdateRank(LMsgC2SUpdateRank* msg)
{
	LTime cur;
	LMsgS2CUpdateRank send;
	send.m_rankType = msg->m_rankType;
	send.m_rankTime = RANK_REWARD_INTERVAL - cur.Secs()%RANK_REWARD_INTERVAL;

	gWork.GetRank(m_userData.m_id, send);
	Send(send);
}

void User::HanderGetRankReward(LMsgC2SGetRankReward* msg)
{
	int result = gWork.GetRankReward(m_userData.m_id);

	if(result > 0)
	{
		AddCardCount(CARD_TYPE_8, result, CARDS_OPER_TYPE_RANK, "rank");

		LMsgLMG2CEGetRankReward send;
		send.m_rankType = msg->m_rankType;
		send.m_uid = m_userData.m_id;

		gWork.SendToCenter(send);
	}

	LMsgS2CGetRankReward sc;
	sc.m_rankType = msg->m_rankType;
	sc.m_result = result;
	Send(sc);
}

void User::SendFreeInfo()
{
	Lstring str = gRuntimeInfoMsg.getFreeTime();
	
	LMsgS2CHorseInfo send;
	send.m_str = str;
	send.m_type = 1;
	Send(send);
}

void User::SendExchInfo()
{
	if(gActiveManager.CheckHasExch())   //有活动推送活动
	{
		Lstring str = gActiveManager.GetExchRuleInfo();

		LMsgS2CHorseInfo send;
		send.m_str = str;
		send.m_type = LMsgS2CHorseInfo::TInfo_Exch;
		Send(send);
	}
}

void User::HanderUserShare(LMsgC2SShare* msg)
{
	LMsgC2SShare send;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;

	gWork.SendMsgToDb( send );
}

void User::HanderUserTaskInfo(LMsgC2STaskInfo* msg)
{
	LMsgC2STaskInfo send;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;

	gWork.SendMsgToDb( send );
}

void User::HanderUserTaskReward(LMsgC2STaskReward* msg)
{
	LMsgC2STaskReward send;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;
	send.m_taskId = msg->m_taskId;

	gWork.SendMsgToDb( send );
}