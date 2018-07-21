#include "User.h"
#include "Work.h"
#include "RoomVip.h"
#include "UserManager.h"
#include "Config.h"

User::User(LUser data, Lint gateId) : m_userData(data)
{
	m_gateId = gateId;
	m_online = true;
	m_desk = NULL;
	m_Active.clear();
	m_ip = "";
	m_userState = 0;
}
	
User::~User()
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

bool User::Login()
{
	//这个方法要改，这样调用太LOW了
	bool b_login = false;
	m_userData.m_lastLoginTime = gWork.GetCurTime().Secs();
	m_online = true;

	if(GetDesk())
	{
		b_login = GetDesk()->OnUserReconnect(this);
	}
	return b_login;
}

void User::Logout()
{
	m_online = false;
}

void User::Send(LMsg& msg)
{
	LLOG_DEBUG( "User::Send userid=%d msgid=%d", m_userData.m_id, msg.m_msgId );
	GateInfo* info = gWork.GetGateInfoById(m_gateId);
	if (info)
	{
		LMsgL2GUserMsg send;
		send.m_strUUID = m_userData.m_unioid;
		send.m_dataBuff = msg.GetSendBuff();
		info->m_sp->Send(send.GetSendBuff());
	}
}

void User::SendLoginInfo(Lstring& buyInfo, Lint hide)
{
	LMsgS2CLogin send;
	send.m_id = m_userData.m_id;
	send.m_errorCode = 0;
	send.m_nike = m_userData.m_nike;
	send.m_exp = 0;
	send.m_sex = m_userData.m_sex;
	send.m_new = m_userData.m_new;
	send.m_state = GetDesk() ? 1 : 0;
	send.m_ip = m_ip;
	send.m_face = m_userData.m_headImageUrl;
	send.m_buyInfo = buyInfo;
	send.m_hide = hide;
	send.m_gm = m_userData.m_gm;

	send.m_card1 = 0;
	send.m_card2 = 0;
	send.m_card3 = 0;

	send.m_inviterId = m_userData.m_agentId;

	Send(send);
}

void User::SendItemInfo()
{
	LMsgS2CItemInfo send;
	send.m_card1 = m_userData.m_numOfCard1s;
	send.m_card2 = m_userData.m_numOfCard2s;
	send.m_card3 = m_userData.m_numOfCard3s;
	send.m_coins = m_userData.m_coins;
	Send(send);
}

void User::SetDesk(Desk* desk)
{
	m_desk = desk;
}

Desk* User::GetDesk()
{
	return m_desk;
}

void User::HanderMsg(LMsg* msg)
{
	LLOG_DEBUG( "User::HanderMsg userid=%d msgid=%d", m_userData.m_id, msg->m_msgId );
	switch (msg->m_msgId)
	{
	case MSG_C_2_S_READY:
		HanderUserReady((LMsgC2SUserReady*) msg);
		break;
	case MSG_C_2_S_RESET_ROOM:
		HanderResutDesk((LMsgC2SResetDesk*) msg);
		break;
	case MSG_C_2_S_RESET_ROOM_SELECT:
		HanderSelectResutDesk((LMsgC2SSelectResetDesk*) msg);
		break;
	case MSG_C_2_S_LEAVE_ROOM:
		HanderUserLeaveDesk((LMsgC2SLeaveDesk*) msg);
		break;
	case MSG_C_2_S_PLAY_CARD:
		HanderUserPlayCard((LMsgC2SUserPlay*) msg);
		break;
	case MSG_C_2_S_USER_OPER:
		HanderUserThink((LMsgC2SUserOper*) msg);
		break;
	case MSG_C_2_S_START_HU_SELECT:
		HanderStartHuSelect((LMsgC2SUserStartHuSelect*) msg);
		break;
	case MSG_C_2_S_END_SELECT:
		HanderEndCardSelect((LMsgC2SUserEndCardSelect*) msg);
		break;
	case MSG_C_2_S_USER_SPEAK:
		HanderUserSpeak((LMsgC2SUserSpeak*) msg);
		break;
	case MSG_C_2_S_LOCATION:
		HanderUserLocation((LMsgC2SLocation*) msg);
		break;
	case MSG_C_2_S_USER_DINGQUE:
		HanderUserDingQue((LMsgC2SUserDingQue*) msg);
		break;
	case MSG_C_2_S_BOTTOM_POUR:
		HanderUserChange((LMsgC2SBottomPour*) msg);
		break;/**/
	case MSG_C_2_S_TANGCARD:
		HanderUserTangReq((LMsgC2STangCard*) msg);
		break;
	case MSG_C_2_S_USER_AIOPER:
		HanderUserAIOper((LMsgC2SUserAIOper*) msg);
		break;
	case MSG_C_2_S_SHARE:
		HanderUserShare((LMsgC2SShare*)msg);
	default:
		LLOG_ERROR("LogicServer Message Error: %d", msg->m_msgId);
		break;
	}
}

Lint User::HanderCreateGonghuiDesk(LMsgLMG2LCreateGonghuiDesk* msg)
{
	return gRoomVip.CreateGonghuiDesk(msg, this);
}

Lint User::HanderUserCreateDesk(LMsgLMG2LCreateDesk* msg)
{
	return gRoomVip.CreateVipDesk(msg, this);
}

Lint User::HanderUserAddDesk(Lint nDeskID)
{
	return gRoomVip.AddToVipDesk(this, nDeskID);
}

void User::HanderUserReady(LMsgC2SUserReady*msg)
{
	if (m_desk)
	{
		m_desk->HanderUserReady(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserReady no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderResutDesk(LMsgC2SResetDesk* msg)
{
	if(m_desk)
	{
		m_desk->HanderResutDesk(this,msg);
	}
	else
	{
		LLOG_ERROR("User::HanderResutDesk no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderSelectResutDesk(LMsgC2SSelectResetDesk* msg)
{
	if(m_desk)
	{
		m_desk->HanderSelectResutDesk(this,msg);
	}
	else
	{
		LLOG_ERROR("User::HanderSelectResutDesk no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderUserLeaveDesk(LMsgC2SLeaveDesk* msg)
{
	gRoomVip.LeaveToVipDesk(msg,this);
}

void User::HanderUserPlayCard(LMsgC2SUserPlay* msg)
{
	if(m_desk)
	{
		m_desk->HanderUserPlayCard(this,msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserPlayCard no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderUserThink(LMsgC2SUserOper* msg)
{
	if(m_desk)
	{
		m_desk->HanderUserOperCard(this,msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserThink no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderStartHuSelect(LMsgC2SUserStartHuSelect* msg)
{
	if(m_desk)
	{
		m_desk->HanderUserStartHu(this,msg);
	}
	else
	{
		LLOG_ERROR("User::HanderStartHuSelect no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderEndCardSelect(LMsgC2SUserEndCardSelect* msg)
{
	if(m_desk)
	{
		m_desk->HanderUserEndSelect(this,msg);
	}
	else
	{
		LLOG_ERROR("User::HanderEndCardSelect no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderUserSpeak(LMsgC2SUserSpeak* msg)
{
	if (m_desk)
	{
		m_desk->HanderUserSpeak(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserSpeak no desk, userid=%d", m_userData.m_id);
	}
}

void User::HanderUserLocation(LMsgC2SLocation* msg)
{
	if (m_desk)
	{
		m_desk->HanderUserLocation(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserLocation no desk, userid=%d", m_userData.m_id);
	}
}

// 玩家定缺
void User::HanderUserDingQue(LMsgC2SUserDingQue* msg)
{
	if (m_desk)
	{
		m_desk->HanderUserDingQue(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserDingQue no desk, userid=%d", m_userData.m_id);
	}
}

//// 玩家换牌
void User::HanderUserChange(LMsgC2SBottomPour* msg)
{
	if (m_desk)
	{
		m_desk->HanderUserChange(this, msg);
	}
	else
	{
		LLOG_ERROR("User::HanderUserChange no desk, userid=%d", m_userData.m_id);
	}
}

Lstring User::GetIp()
{
	return m_ip;
}

void User::SetIp(Lstring& ip)
{
	m_ip = ip;
}

void User::AddCardCount(Lint cardType, Lint count, Lint operType,Lstring admin, bool bNeedSave)
{
	LLOG_INFO("User::AddCardCount type=%d,count=%d,operType=%d", cardType, count, operType);

	m_userData.m_numOfCard2s += count;
	m_userData.m_totalbuynum += count;

	LMsgL2LMGModifyCard msg;
	msg.admin = admin;
	msg.cardType = cardType;
	msg.cardNum = count;
	msg.isAddCard = 1;
	msg.operType = operType;
	msg.m_userid = m_userData.m_id;
	msg.m_strUUID = m_userData.m_unioid;
	gWork.SendToLogicManager(msg);
}

void User::DelCardCount(Lint cardType, Lint count, Lint operType,Lstring admin)
{
	LLOG_INFO("User::DelCardCount type=%d,count=%d,operType=%d", cardType, count, operType);

	if (m_userData.m_numOfCard2s >= count)
		m_userData.m_numOfCard2s -= count;
	else
		m_userData.m_numOfCard2s = 0;

	LMsgL2LMGModifyCard msg;
	msg.admin = admin;
	msg.cardType = cardType;
	msg.cardNum = count;
	msg.isAddCard = 0;
	msg.operType = operType;
	msg.m_userid = m_userData.m_id;
	msg.m_strUUID = m_userData.m_unioid;
	gWork.SendToLogicManager(msg);
}

void User::ModifyUserState(bool bLeaveServer)
{
	LMsgL2GModifyUserState msg_gate;
	msg_gate.m_strUUID = m_userData.m_unioid;
	msg_gate.m_logicID =  bLeaveServer? 0 : gConfig.GetServerID();
	msg_gate.m_userstate = getUserState();
	GateInfo* gateInfo = gWork.GetGateInfoById(GetUserGateId());

	if (gateInfo)
	{
		gateInfo->m_sp->Send(msg_gate.GetSendBuff());
	}

	LMsgL2LMGModifyUserState msg_logic;
	msg_logic.m_strUUID = m_userData.m_unioid; 
	msg_logic.m_userid  = m_userData.m_id;
	msg_logic.m_logicID = bLeaveServer? 0 : gConfig.GetServerID();
	msg_logic.m_userstate = getUserState();

	gWork.SendToLogicManager(msg_logic);
	gWork.SendToCoinsServer(msg_logic);
}

void User::AddCardCount(Lint id, Lstring strUUID, Lint cardType, Lint count, Lint operType,Lstring admin)
{
	LLOG_INFO("User::AddCardCount type=%d,count=%d,operType=%d", cardType, count, operType);
	
	LMsgL2LMGModifyCard msg;
	msg.admin = admin;
	msg.cardType = cardType;
	msg.cardNum = count;
	msg.isAddCard = 1;
	msg.operType = operType;
	msg.m_userid = id;
	msg.m_strUUID = strUUID;
	gWork.SendToLogicManager(msg);
}

void User::HanderUserTangReq( LMsgC2STangCard* msg )
{
	if(m_desk)
		m_desk->HanderUserTangReq(this,msg);
}

void User::HanderUserAIOper(LMsgC2SUserAIOper* msg)
{
	if(m_desk)
	{
		Lint pos = m_desk->GetUserPos( this );
		if(pos != INVAILD_POS)
		{
			if ( !m_desk->m_autoPlay[pos] )
			{
				LLOG_ERROR("HanderUserAIOper user not have action %s", m_userData.m_nike.c_str());
			}
			else
			{
				m_desk->m_autoPlayTime[pos] = 0;	// 改写记录的倒计时为0
				m_desk->CheckAutoPlayCard();
			}
		}
		else
		{
			LLOG_INFO("HanderUserAIOper pos error %s", m_userData.m_nike.c_str());
		}
	}
}

void User::HanderUserShare(LMsgC2SShare*msg)
{
	LMsgC2SShare send = *msg;
	send.m_strUUID = m_userData.m_unioid;
	send.m_userId = m_userData.m_id;
	//gWork.SendToLogicManager(send);
	gWork.SendMsgToDb(send);
}