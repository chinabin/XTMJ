#include "UserMessage.h"
#include "LTime.h"
#include "LMemoryRecycle.h"
#include "Config.h"
#include "Work.h"
#include "UserManager.h"
#include "RuntimeInfoMsg.h"
#include "LVipLog.h"
#include "GonghuiManager.h"

CUserMessage::CUserMessage()
{
}

CUserMessage::~CUserMessage()
{

}

void CUserMessage::Clear()
{

}

void CUserMessage::Run()
{
	LTime msgTime;
	int iMsgCount = 0;
	int iElapseTime = 0;
	while(!GetStop())
	{
		LTime cur;
		if(cur.MSecs() - m_tickTimer.MSecs() > 1)
		{
			m_tickTimer = cur;
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
			m_memoryRecycle.recycleMsg(msg);

			//每一万个包输出所用时间
			++iMsgCount;
			if(iMsgCount == 10000)
			{
				LTime now;
				LLOG_ERROR("User Logic: 10000 package needs %d time.", (int)(now.MSecs() - msgTime.MSecs() - iElapseTime));

				iMsgCount = 0;
				iElapseTime = 0;

				msgTime = now;
			}
		}
	}
}

void CUserMessage::Start()
{
	m_memoryRecycle.Start();
	LRunnable::Start();
}

void CUserMessage::Stop()
{
	m_memoryRecycle.Stop();
	LRunnable::Stop();
}

void CUserMessage::Join()
{
	m_memoryRecycle.Join();
	LRunnable::Join();
}

void CUserMessage::HanderMsg(LMsg* msg)
{
	if(msg == NULL)
	{
		return;
	}

	switch(msg->m_msgId)
	{
	/*
	Center to LogicManager
	*/
	case MSG_CE_2_L_USER_LOGIN:
		HanderCenterUserLogin((LMsgCe2LUserLogin*)msg);
		break;
	case MSG_CE_2_L_GM_CHARGE:
		HanderCenterGMCharge((LMsgCe2LGMCharge*)msg);
		break;
	case MSG_CE_2_L_GM_COINS:
		HanderCenterGMCoins((LMsgCe2LGMCoins*)msg);
		break;
	case MSG_CE_2_LM_ADD_AGENT:
		HanderCenterAddAgent((LMsgCE2LMGAddAgent*)msg);
		break;

	/*
	LogicServer or CoinsServer to LogicManager
	*/
	case MSG_L_2_LMG_MODIFY_USER_STATE:
		HanderModifyUserState((LMsgL2LMGModifyUserState*)msg);
		break;
	case MSG_L_2_LMG_MODIFY_USER_CARD:
		HanderModifyUserCardNum((LMsgL2LMGModifyCard*)msg);
		break;
	case MSG_L_2_LMG_ADD_USER_PLAYCOUNT:
		HanderAddUserPlayCount((LMsgL2LMGAddUserPlayCount*)msg);
		break;
	case MSG_L_2_LMG_GONGHUIDESK_CHANGE:
		HanderGonghuiDeskChange((LMsgL2LMGGonghuiDeskChange*)msg);
		break;

	/*
	Gate to LogicManager
	*/
	case MSG_G_2_L_USER_MSG:
		HanderUserMsg((LMsgG2LUserMsg*)msg);
		break;
	case MSG_G_2_L_USER_OUT_MSG:
		HanderUserOutMsg((LMsgG2LUserOutMsg*)msg);
		break;

	/*
	DB to LogicManager
	*/
	case MSG_LDB_2_L_VIP_LOG:
		HanderDBReqVipLog((LMsgLBD2LReqVipLog*) msg);
		break;
	case MSG_LDB_2_LM_RET_CRELOGHIS:
		HanderDBReqCRELogHis((LMsgLDB2LM_RecordCRELog*)msg);
		break;
	case MSG_LDB_2_L_ROOM_LOG:
		HanderDBReqRoomLog((LMsgLBD2LReqRoomLog*) msg);
		break;
	case MSG_LDB_2_L_VIDEO:
		HanderDBReqVideo((LMsgLDB2LReqVideo*) msg);
		break;
	case MSG_LDB_2_LMG_REQ_BINDING_RELATIONSHIPS:
		HanderDBRetBindingRelation((LMsgLDB2LMGBindingRelationships*)msg);
		break;
	case MSG_LDB_2_LM_RET_INFO:
		HanderDBRetInfo((LMsgLDB_2_LM_RetInfo*)msg);
		break;
	case MSG_LDB_2_LMG_USER_MSG:
		HanderDBUserMsg((LMsgLDB2LMGUserMsg*) msg);
		break;
	case MSG_CN_2_LMG_MODIFY_USER_COINS:
		HanderModifyUserCoinsNum((LMsgCN2LMGModifyUserCoins*) msg);
		break;
	default:
		break;
	}
}

void CUserMessage::HanderCenterUserLogin(LMsgCe2LUserLogin* msg)
{
	if(msg == NULL)
	{
		return;
	}

	boost::shared_ptr<UserLoginInfo> userLoginInfo(new UserLoginInfo);
	userLoginInfo->m_seed = msg->m_seed;
	userLoginInfo->m_time = GetCurTime().Secs();
	userLoginInfo->m_user = msg->user;

	gUserManager.addUserLoginInfo(userLoginInfo);
}

void CUserMessage::HanderCenterGMCharge(LMsgCe2LGMCharge* msg)
{
	LLOG_DEBUG("Work::HanderCenterGMCharge %d:%d:%d:%d", msg->m_userid, msg->m_cardType, msg->m_cardCount, msg->m_oper);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if (safeUser && safeUser->isValid())
	{
		safeUser->getResource()->AddCardCount(msg->m_cardType, msg->m_cardCount, msg->m_oper, msg->m_admin, false);
	}
	else
	{
		LLOG_ERROR("Work::HanderCenterGMCharge user not exiest %d:%d:%d:%d", msg->m_userid, msg->m_cardType, msg->m_cardCount, msg->m_oper);
	}
}

void CUserMessage::HanderCenterGMCoins(LMsgCe2LGMCoins* msg)
{
	LLOG_DEBUG("Work::HanderCenterGMCoins %d:%d:%d", msg->m_userid, msg->m_coins, msg->m_totalcoins);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if (safeUser && safeUser->isValid())
	{
		if (msg->m_coins > 0)
		{
			safeUser->getResource()->AddCoinsCount(msg->m_coins, COINS_OPER_TYPE_CHARGE, false);
		}
		else
		{
			safeUser->getResource()->DelCoinsCount(msg->m_coins, COINS_OPER_TYPE_CHARGE, false);
		}
	}
	else
	{
		LLOG_ERROR("Work::HanderCenterGMCoins user not exiest %d:%d:%d", msg->m_userid, msg->m_coins, msg->m_totalcoins);
	}
}

void CUserMessage::HanderCenterAddAgent(LMsgCE2LMGAddAgent* msg)
{
	LLOG_DEBUG("CUserMessage::HanderCenterAddAgent uid:%d agentId:%d", msg->m_userid, msg->m_agentId);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if (safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();

	if (msg->m_result == ErrorCode::ErrorNone)
		user->m_userData.m_agentId = msg->m_agentId;

	LMsgS2CBindInviter ret;
	ret.m_error = (ErrorCode)msg->m_result;						// 参照User::HanderBindInviter
	ret.m_inviter.m_id = msg->m_agentId;

	user->Send(ret);
}

void CUserMessage::HanderGonghuiDeskChange(LMsgL2LMGGonghuiDeskChange* msg)
{
	if (msg == NULL)
	{
		return;
	}

	gUserManager.updateGonghuiPaiju(msg->m_gonghuiId, msg->m_roomId, std::to_string(msg->m_roomState), msg->m_user);

	if (msg->m_roomState == DESK_PLAY || msg->m_roomState == 3 || msg->m_roomState == 4)
	{
		// TODO 创建工会房间
		// 需要先检测房卡是否够，如果不够，则不能创建房间
		int count;
		NeedCardCount(msg->m_roomType, count);
		// 这里代码有问题
		Gonghui x = gUserManager.getGonghuiInfoById(msg->m_gonghuiId);
		Lint userCards = gGonghuiManager.getUserCardsById(x.m_adminUserId);
		LLOG_ERROR("gonghuiID=%d, adminUserId=%d, userCard=%d, roomState=%d, count=%d.", x.m_gonghuiId, x.m_adminUserId, userCards, msg->m_roomState, count);
		if (msg->m_roomState == DESK_PLAY && userCards >= count)
		{
			gWork.CreateGonghuiRoom(msg->m_gonghuiId, msg->m_roomType, msg->m_playType, msg->m_baseScore);
		}

		Lstring roomState;
		if (3 == msg->m_roomState)
		{
			roomState = "End";
			if (!gGonghuiManager.addGonghuiDeskInfo(msg->m_roomId, msg->m_gonghuiId, msg->m_baseScore, msg->m_roomType, msg->m_playType, msg->m_playNum, roomState, msg->m_user, msg->m_score))
			{
				LLOG_ERROR("Error, failed to record desk info to db, deskId:%d, gonghuiid=%d.", msg->m_roomId, msg->m_gonghuiId);
			}

			gUserManager.delGonghuiPaiju(msg->m_gonghuiId, msg->m_roomId);
		}
		else if (4 == msg->m_roomState)
		{
			roomState = "Abort";
			if (!gGonghuiManager.addGonghuiDeskInfo(msg->m_roomId, msg->m_gonghuiId, msg->m_baseScore, msg->m_roomType, msg->m_playType, msg->m_playNum, roomState, msg->m_user, msg->m_score))
			{
				LLOG_ERROR("Abort desk, failed to record desk info to db, deskId:%d, gonghuiid=%d.", msg->m_roomId, msg->m_gonghuiId);
			}
			gUserManager.delGonghuiPaiju(msg->m_gonghuiId, msg->m_roomId);
		}
	}

	// TODO 这里简单处理，一单发现桌子状态变更之后，把当前工会下的所有桌子都发出去
	LMsgS2CGonghuiDeskChange cMsg;
	cMsg.m_gonghuiId = msg->m_gonghuiId;
	cMsg.m_gonghui = gUserManager.getGonghuiInfoById(msg->m_gonghuiId);

	std::vector<GonghuiUser> gonghuiUsers = gUserManager.getGonghuiUserInfoById(msg->m_gonghuiId);

	for (GonghuiUser tmpUser : gonghuiUsers)
	{
		// 这里发送消息到所有工会的已登录的用户客户端上，同时更新工会缓存信息
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(tmpUser.id);
		if (safeUser.get() == NULL || !safeUser->isValid())
		{
			continue;
		}

		// 发送的消息需要转换为其他
		boost::shared_ptr<User> user = safeUser->getResource();
		if (user->GetOnline())
		{
			LLOG_ERROR("Send gonghui room change to user %d.", user->m_userData.m_id);
			user->Send(cMsg);
		}
	}
}

void CUserMessage::HanderModifyUserState(LMsgL2LMGModifyUserState* msg)
{
	if(msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();

	LLOG_DEBUG("Work::HanderModifyUserState ID:%d, State:%d, serverID: %d", user->m_userData.m_id, msg->m_userstate, msg->m_logicID);
	user->setUserState(msg->m_userstate);
	user->setUserLogicID(msg->m_logicID);
}

void CUserMessage::HanderModifyUserCardNum(LMsgL2LMGModifyCard* msg)
{
	if(msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("Work::HanderModifyUserCardNum user not exiest, userid=%d, type=%d, count=%d, operType=%d",  msg->m_userid, msg->cardType, msg->cardNum, msg->operType);
		
		// 取不到用户，说明用户可能未登陆，直接扣除房卡即可
		bool ret1 = gGonghuiManager.decreaseCardCount(msg->m_userid, msg->isAddCard, msg->cardNum);
		Lint userCards = gGonghuiManager.getUserCardsById(msg->m_userid);
		bool ret2 = gGonghuiManager.SaveCardInfo(msg->m_userid, msg->cardType, msg->cardNum, msg->operType, msg->admin, userCards, "");
		LLOG_ERROR("decrease card count, userId=%d, count=%d, numCards=%d, ret1=%d, ret2=%d.", msg->m_userid, msg->cardNum, userCards, ret1, ret2);
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();

	if(msg->isAddCard)
	{
		user->AddCardCount(msg->cardType, msg->cardNum, msg->operType, msg->admin);
	}
	else
	{
		if ( msg->operType == CARDS_OPER_TYPE_CREATE_ROOM )
		{
			if (!gRuntimeInfoMsg.isFree(-1) && !gRuntimeInfoMsg.isFree(user->getLastCreateGameType()))
			{
				user->DelCardCount(msg->cardType, msg->cardNum, msg->operType, msg->admin);
			}
			else
			{
				LLOG_INFO("Work::HanderModifyUserCardNum Now Free, userid=%d, type=%d, count=%d, operType=%d gametype=%d",  msg->m_userid, msg->cardType, msg->cardNum, msg->operType, user->getLastCreateGameType());
			}
		}
		else
		{
			user->DelCardCount(msg->cardType, msg->cardNum, msg->operType, msg->admin);
		}
	}
}

void CUserMessage::HanderAddUserPlayCount(LMsgL2LMGAddUserPlayCount* msg)
{
	if(msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_DEBUG("Work::HanderModifyUserPlayCount user not exiest, userid=%d",  msg->m_userid);
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	user->AddPlayCount();
}

void CUserMessage::HanderUserMsg(LMsgG2LUserMsg* msg)
{
	if(msg == NULL)
	{
		return;
	}

	Lint gateId = gWork.getGateIdBySp(msg->m_sp);
	if(gateId == INVALID_GATEID)
	{
		LLOG_ERROR("%s have not gate", msg->m_strUUID.c_str());
		return;
	}

	if (msg->m_userMsg)
	{
		LLOG_DEBUG("CUserMessage recv msg:%d", msg->m_userMsgId);

		switch (msg->m_userMsgId)
		{
		case MSG_C_2_S_LOGIN:
			HanderUserLogin(((LMsgC2SLogin*)msg->m_userMsg)->m_id, ((LMsgC2SLogin*)msg->m_userMsg)->m_seed, gateId, ((LMsgC2SLogin*)msg->m_userMsg)->m_md5, msg->m_ip);
			break;
		case MSG_C_2_S_GET_COINS:
			{
				// 玩家主动领取金币 玩家没进入金币场是消息会转发到这里 这里在转发给金币服务器
				LMsgLMG2CNGetCoins send;
				send.m_strUUID	= msg->m_strUUID;
				send.m_userid	= ((LMsgC2SGetCoins*)msg->m_userMsg)->m_userid;
				send.m_gateid   = gateId;
				gWork.SendMessageToCoinsServer( send );
			}
			break;
		default:
			{
				boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserByGateIdAndUUID(gateId, msg->m_strUUID);
				if(safeUser && safeUser->isValid())
				{
					safeUser->getResource()->HanderMsg(msg->m_userMsg);
				}
				else
				{
					LLOG_DEBUG("Work::HanderUserMsg user not exist %d:%s:%d", gateId, msg->m_strUUID.c_str(), msg->m_userMsgId);
				}
			}
			break;
		}
	}
	else
	{
		LLOG_DEBUG("Work::HanderUserMsg msg error %d:%s:%d", gateId, msg->m_strUUID.c_str(), msg->m_userMsgId);
	}
}

void CUserMessage::HanderUserOutMsg(LMsgG2LUserOutMsg* msg)
{
	if(msg == NULL)
	{
		return;
	}

	Lint gateId = gWork.getGateIdBySp(msg->m_sp);
	if(gateId == INVALID_GATEID)
	{
		LLOG_ERROR("%s have not gate", msg->m_strUUID.c_str());
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = UserManager::Instance().getUserByGateIdAndUUID(gateId, msg->m_strUUID);
	if(safeUser && safeUser->isValid())
	{
		boost::shared_ptr<User> user = safeUser->getResource();

		LLOG_DEBUG("Work::HanderUserLogout %d", user->GetUserDataId());

		user->Logout();

		gRuntimeInfoMsg.changeOnlineNum(gateId, user->GetUserDataId(), false);

		if(!gWork.isLogicServerExist(user->getUserLogicID()))
		{
			user->S2CeUserLogout();
			UserManager::Instance().delUser(user->m_userData.m_id);
		}
	}
	else
	{
		LLOG_DEBUG("Work::HanderUserLogout user not find %d:%s", gateId, msg->m_strUUID.c_str());
	}
}

void CUserMessage::HanderUserLogin(Lint id, Lint seed, Lint gateId, Lstring& md5,Lstring& ip)
{
	boost::shared_ptr<CSafeResourceLock<UserLoginInfo> > safeLoginInfo = gUserManager.getUserLoginInfo(id);
	if(safeLoginInfo.get() == NULL || !safeLoginInfo->isValid())
	{
		//这里是登录超时，或者非法登录
		LLOG_ERROR("Work::HanderUserLogin erro %d:%d:%d", id, gateId,seed); 
		return;
	}

	if(safeLoginInfo->getResource()->m_seed != seed)
	{
		//这里seed不对
		LLOG_ERROR("Work::HanderUserLogin seed error %d:%d:%d:%d", id, gateId, seed, safeLoginInfo->getResource()->m_seed);
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(id);
	boost::shared_ptr<User> user;
	if(safeUser && safeUser->isValid())
	{
		user = safeUser->getResource();

		bool bBeforeOnlineState = user->GetOnline();

		if(user->GetUserGateId() != gateId) //说明在其它gate上登录过，踢出
		{
			LLOG_DEBUG("Work::HanderUserLogin user online %d:%d", user->GetUserDataId(), user->GetUserGateId());
			LMsgL2GUserOutMsg msgOut;
			msgOut.m_strUUID = user->m_userData.m_unioid;
			gWork.SendMessageToGate(user->GetUserGateId(), msgOut);

			gRuntimeInfoMsg.changeOnlineNum(user->GetUserGateId(), user->GetUserDataId(), false);
		}

		UserManager::Instance().delUser(id);
		LLOG_DEBUG("login user valid, nums=%d.", user->m_userData.m_numOfCard2s);
		user->SetUserGateId(gateId);
		user->SetIp(ip);
		UserManager::Instance().addUser(user);
		user->Login();

		if(!bBeforeOnlineState)
		{
			gUserManager.delUserLoginInfo(user->GetUserDataId());
		}
	}
	else
	{
		user.reset(new User(safeLoginInfo->getResource()->m_user, gateId));
		if (user->m_userData.m_new != 0)
		{
			user->m_userData.m_numOfCard2s = gGonghuiManager.getUserCardsById(user->m_userData.m_id);
		}
		LLOG_DEBUG("login user invalid, nums=%d.", user->m_userData.m_numOfCard2s);
		user->SetIp(ip);
		UserManager::Instance().addUser(user);
		user->S2CeUserLogin();
		user->Login();
		gUserManager.delUserLoginInfo(user->GetUserDataId());

		LLOG_DEBUG("Work::HanderUserLogin user login success %d:%d:%d:%s", user->GetUserDataId(), user->GetUserDataId(), user->GetUserGateId(),user->GetIp().c_str());
	}

	if(user)
	{
		gRuntimeInfoMsg.changeOnlineNum(gateId, user->GetUserDataId(), true);
	}
}

//db发送玩家战绩
void CUserMessage::HanderDBReqVipLog(LMsgLBD2LReqVipLog* msg)
{
	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	if(user && user->GetOnline())
	{
		LMsgS2CVipLog log;
		log.m_size = msg->m_count;
		for(Lint i = 0 ; i < msg->m_count; ++i)
		{
			/*VipLogItemInfo& info = log.m_info[i];
			info.m_id = msg->m_info[i].m_id;
			info.m_time = msg->m_info[i].m_time;
			info.m_flag = msg->m_info[i].m_flag;
			info.m_reset = msg->m_info[i].m_reset;
			info.m_deskId = msg->m_info[i].m_deskId;
			memcpy(info.m_score,msg->m_info[i].m_score,sizeof(info.m_score));
			memcpy(info.m_userid,msg->m_info[i].m_posUserId,sizeof(info.m_userid));*/

			LVipLogItem item;
			memcpy(item.m_posUserId,msg->m_info[i].m_posUserId,sizeof(item.m_posUserId));
			item.FromString(msg->m_info[i].m_data);
			item.PlayTypeFromString(msg->m_info[i].m_playtype);
			VipLogItemInfo& info = log.m_info[i];
			info.m_id = msg->m_info[i].m_id;
			info.m_time = msg->m_info[i].m_time;
			info.m_reset = msg->m_info[i].m_reset;
			info.m_deskId = msg->m_info[i].m_deskId;
			memcpy(info.m_score,msg->m_info[i].m_score,sizeof(info.m_score));
			memcpy(info.m_userid,msg->m_info[i].m_posUserId,sizeof(info.m_userid));
			info.m_size = item.m_log.size();
			//查找玩家昵称
			for (Lint j = 0; j < 4; ++j)
			{
				boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(info.m_userid[j]);
				if(safeBaseInfo && safeBaseInfo->isValid())
				{
					info.m_nike[j] = safeBaseInfo->getResource()->m_nike;
				}
			}

			for(Lint j = 0; j < item.m_log.size(); ++j)
			{
				VipLog& l = info.m_item[j];
				VipDeskLog* dl = item.m_log[j];
				memcpy(l.m_score,dl->m_gold,sizeof(l.m_score));
				l.m_time = dl->m_time;
				l.m_videoId = dl->m_videoId;
			}
		}
		user->Send(log);
	}
}

void CUserMessage::HanderDBReqRoomLog( LMsgLBD2LReqRoomLog* msg )
{
	class ItemLog
	{
	public:
		~ItemLog()
		{
			for(auto it = m_log.begin(); it != m_log.end(); ++it)
			{
				if(*it)
				{
					delete *it;
					*it = NULL;
				}
			}
			m_log.clear();
		}
		void FromString(const Lstring& str)
		{
			std::vector<Lstring> str1, str2;

			if ( str.find( "|" ) != std::string::npos )
			{
				L_ParseString(str, str2, "|");
				for(size_t j = 0 ; j < str2.size(); ++j)
				{
					str1.clear();
					L_ParseString(str2[j], str1, ";");
					if ( str1.size() >= 8 )
					{
						VipDeskLog* log = new VipDeskLog();

						sscanf( str1[0].c_str(), "%d,%d,%d,%d", &(log->m_gold[0]), &(log->m_gold[1]), &(log->m_gold[2]), &(log->m_gold[3]) );
						sscanf( str1[1].c_str(), "%d,%d,%d,%d", &(log->m_mgang[0]), &(log->m_mgang[1]), &(log->m_mgang[2]), &(log->m_mgang[3]) );
						sscanf( str1[2].c_str(), "%d,%d,%d,%d", &(log->m_angang[0]), &(log->m_angang[1]), &(log->m_angang[2]), &(log->m_angang[3]) );
						log->m_zhuangPos = atoi(str1[3].c_str());
						log->m_time = atoi(str1[4].c_str());
						log->m_videoId = str1[5].c_str();
						sscanf( str1[6].c_str(), "%d,%d,%d,%d", &(log->m_checkTing[0]), &(log->m_checkTing[1]), &(log->m_checkTing[2]), &(log->m_checkTing[3]) );
						sscanf( str1[7].c_str(), "%d,%d,%d,%d", &(log->m_zimo[0]), &(log->m_zimo[1]), &(log->m_zimo[2]), &(log->m_zimo[3]) );
						sscanf( str1[8].c_str(), "%d,%d,%d,%d", &(log->m_bomb[0]), &(log->m_bomb[1]), &(log->m_bomb[2]), &(log->m_bomb[3]) );
						sscanf( str1[9].c_str(), "%d,%d,%d,%d", &(log->m_win[0]), &(log->m_win[1]), &(log->m_win[2]), &(log->m_win[3]) );
						sscanf( str1[10].c_str(), "%d,%d,%d,%d", &(log->m_dzimo[0]), &(log->m_dzimo[1]), &(log->m_dzimo[2]), &(log->m_dzimo[3]) );
						sscanf( str1[11].c_str(), "%d,%d,%d,%d", &(log->m_dbomb[0]), &(log->m_dbomb[1]), &(log->m_dbomb[2]), &(log->m_dbomb[3]) );
						sscanf( str1[12].c_str(), "%d,%d,%d,%d", &(log->m_dwin[0]), &(log->m_dwin[1]), &(log->m_dwin[2]), &(log->m_dwin[3]) );
						m_log.push_back(log);
					}
				}
			}
		}
	public:
		std::vector<VipDeskLog*> m_log;//每一把的记录 
	};

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	if(user && user->GetOnline())
	{
		ItemLog item;
		item.FromString(msg->m_data);

		LMsgS2CRoomLog log;
		log.m_size = item.m_log.size();
		log.m_flag = msg->m_flag;

		for(size_t j = 0; j < item.m_log.size(); ++j)
		{
			if(j < 32)
			{
				VipLog& l = log.m_item[j];
				VipDeskLog* dl = item.m_log[j];
				memcpy(l.m_score,dl->m_gold,sizeof(l.m_score));
				l.m_time = dl->m_time;
				l.m_videoId = dl->m_videoId;
				//printf("=%d %d %d\n",l.m_time,l.m_score[0],l.m_videoId);
			}
		}

		user->Send(log);
	}
}

void CUserMessage::HanderDBReqVideo(LMsgLDB2LReqVideo* msg)
{
	if(msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	if(user && user->GetOnline())// && msg->m_count)
	{
		LMsgS2CVideo video;

		if(msg->m_count>0)
		{
			video.m_video = msg->m_video;
			video.m_video.FromString(video.m_video.m_str);
			video.m_video.PlayTypeFromString(video.m_video.m_str_playtype);
			video.m_deskId = msg->m_video.m_deskId;
			video.m_curCircle = msg->m_video.m_curCircle;
			video.m_maxCircle = msg->m_video.m_maxCircle;
			video.m_zhuang = msg->m_video.m_zhuang;
			video.m_flag = msg->m_video.m_flag;
			memcpy(video.m_score,msg->m_video.m_score,sizeof(video.m_score));

			for (Lint i = 0; i < 4; ++i)
			{
				if ( msg->m_video.m_userId[i] == 0 )
				{
					continue;
				}

				boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_video.m_userId[i]);
				if(safeBaseInfo && safeBaseInfo->isValid())
				{
					video.m_nike[i]		= safeBaseInfo->getResource()->m_nike;
					video.m_sex[i]		= safeBaseInfo->getResource()->m_sex;
					video.m_imageUrl[i] = safeBaseInfo->getResource()->m_headImageUrl;
				}
			}
		}

		user->Send(video);
	}
}

void CUserMessage::HanderDBRetBindingRelation( LMsgLDB2LMGBindingRelationships* msg )
{
}

void CUserMessage::HanderDBRetInfo(LMsgLDB_2_LM_RetInfo* msg)
{
}

void CUserMessage::HanderDBUserMsg(LMsgLDB2LMGUserMsg* msg)
{
	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserUUID(msg->m_strUUID);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}
	boost::shared_ptr<User> user = safeUser->getResource();
	user->Send( msg->m_dataBuff );
}

void CUserMessage::HanderDBReqCRELogHis( LMsgLDB2LM_RecordCRELog *msg )
{
	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userId);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();
	if(user && user->GetOnline())
	{
		LMsgS2CCRELogRet ret;

		//查找玩家昵称
		for (Lint j = 0; j < LMsgLDB2LM_RecordCRELog::Length; ++j)
		{
			if(msg->m_user[j] == 0)
				continue;

			boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > safeBaseInfo = gUserManager.getUserBaseInfo(msg->m_user[j]);
			if(safeBaseInfo && safeBaseInfo->isValid())
			{
				auto tmp = safeBaseInfo->getResource();
				UserBaseInfo_s info;
				info.m_strNike = tmp->m_nike;
				info.m_userId = msg->m_user[j];
				info.m_strHeadUrl = tmp->m_headImageUrl;
				info.m_sex = tmp->m_sex;
			
				ret.m_info.push_back(info);
			}
			else
			{
				LLOG_DEBUG("##Warnning ReqCRELog not find the user = %d",msg->m_user[j]);
			}
		}

		for(auto it = msg->m_record.begin(); it != msg->m_record.end(); ++it)
		{
			ret.m_record.push_back(*it);
		}

		user->Send(ret);
	}
}

void CUserMessage::HanderModifyUserCoinsNum(LMsgCN2LMGModifyUserCoins* msg)
{
	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("CUserMessage::HanderModifyUserCoinsNum user not exiest, userid=%d, count=%d, operType=%d",  msg->m_userid, msg->coinsNum, msg->operType);
		return;
	}

	boost::shared_ptr<User> user = safeUser->getResource();

	if(msg->isAddCoins)
	{
		user->AddCoinsCount(msg->coinsNum, msg->operType);
	}
	else
	{
		user->DelCoinsCount(msg->coinsNum, msg->operType);
	}
}

LTime& CUserMessage::GetCurTime()
{
	return m_tickTimer;
}