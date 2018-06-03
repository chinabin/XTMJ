#include "Desk.h"
#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "UserManager.h"
Desk::Desk()
{
	_clearData();
}

Desk::~Desk()
{

}

bool Desk::initDesk(int iDeskId, GameType gameType, Lint cardType)
{
	_clearData();
	
// 	if(gameType == MJHeNanHuaShui
// 		|| gameType == MJHeNanZhengZhou
// 		|| gameType == MJHeNanTuiDaoHu
// 		|| gameType == MJHeNanXinXiang
// 		|| gameType == MJHeNanKaiFeng
// 		|| MJHeNanZhuanZhuanFour == gameType
// 		|| MJHeNanLuoYang == gameType||MJHeNanXinYang == gameType)
// 	{
// 		m_iPlayerCapacity = 4;
// 	}
// 	else if (MJHeNanZhuanZhuanThree == gameType)
// 	{
// 		m_iPlayerCapacity = 3;
// 	}
// 	else if (MJHeNanZhuanZhuanTwo == gameType)
// 	{
// 		m_iPlayerCapacity = 2;
// 	}
// 	else
// 	{
// 		LLOG_ERROR("Desk::initDesk -- Game type is wrong. Type = %d", gameType);
// 		return false;
// 	}

	switch (gameType)
	{
	case MJHeNanHuaShui:
	case MJHeNanZhengZhou:
	case MJHeNanTuiDaoHu:
	case MJHeNanXinXiang:
	case MJHeNanKaiFeng:
	case MJHeNanZhuanZhuanFour:
	case MJHeNanLuoYang:
	case MJHeNanXinYang:
	case MJHeNanShangQiu4:
	case MJHeNanZhouKou:
	case MJWuHanXianTao:
		m_iPlayerCapacity = 4;
		break;
	case MJHeNanZhuanZhuanThree:
	case MJHeNanShangQiu3:
		m_iPlayerCapacity = 3;
		break;
	case MJHeNanZhuanZhuanTwo:
	case MJHeNanShangQiu2:
		m_iPlayerCapacity = 2;
		break;
	default:
		LLOG_ERROR("Desk::initDesk -- Game type is wrong. Type = %d", gameType);
		return false;
	}

	if(!_createRegister(gameType))
	{
		return false;
	}

	m_id = iDeskId;
	m_gameType = gameType;
	m_cardType = cardType;

	LTime cur;
	m_timeDeskCreate = cur.Secs();

	return true;
}

Lint Desk::GetDeskId()
{
	return m_id;
}

Lint Desk::GetPlayerCapacity()
{
	return m_iPlayerCapacity;
}

void Desk::Tick(LTime& curr)
{
	CheckReset();

	CheckAutoPlayCard();

	if(m_timeDeskCreate && m_deskState == DESK_WAIT)	//如果超过8小时还没有开始过就进行解散
	{
		if(m_vip == NULL || !m_vip->IsBegin())	//如果没有赋值或者没有开始过
		{
			LTime cur;
			Lint iInterval = cur.Secs() - m_timeDeskCreate;
			if(iInterval >= (8 * 60 * 60))
			{
				LLOG_ERROR("The desk(%d) doesn't begin beyond 8 hours", m_id);
				ResetEnd();
			}
		}
	}
}

void Desk::SetAutoPlay( Lint pos, bool bauto, Lint outtime )
{
	m_autoPlay[pos] = bauto;
	if ( bauto )
	{
		LTime cur;
		m_autoPlayTime[pos] = cur.Secs();
		m_autoOutTime[pos] = outtime;
	}
}

void Desk::CheckAutoPlayCard()
{
	if(m_deskType != DeskType_Coins)
		return;

	if(m_deskState == DESK_PLAY )
	{
		LTime cur;
		for(int i = 0; i < m_iPlayerCapacity; ++i)
		{
			if ( !m_user[i] )
			{
				continue;
			}
			Lint outtime = m_autoOutTime[i];
			if ( outtime > 0 )	// <=0 不给玩家自动操作
			{
				if ( !m_user[i]->GetOnline() )	// 玩家不在线倒计时短一些
				{
					outtime = 1;
				}
				else
				{
					outtime += DIFFOPOUTTIME;	// 服务器的时间要长一些
				}
				if ( gConfig.GetDebugModel() && m_user[i]->GetUserDataId() >= 10000001 )	// 机器人倒计时
				{
					outtime = 2;
				}
				if ( m_autoPlay[i] && cur.Secs() - m_autoPlayTime[i] > outtime )
				{
					LLOG_DEBUG("Desk::CheckAutoPlayCard [deskid = %d][pos = %d] auto play card",m_id, i);
					m_autoPlay[i] = false;
					mGameHandler->ProcessAutoPlay(i,m_user[i]);
				}
			}
		}
	}
}

void Desk::SetDeskType( DeskType type )
{
	m_deskType = type;
}

DeskType Desk::GetDeskType()
{
	return m_deskType;
}

//申请解算房间
void Desk::HanderResutDesk(User* pUser,LMsgC2SResetDesk* msg)
{
	
	//重复申请
	if (m_resetTime != 0)
	{
		return;
	}

	Lint pos = GetUserPos(pUser);
	if(pos == INVAILD_POS)
	{
		return;
	}

	if(!m_vip  || m_deskState == DESK_FREE)
	{
		LLOG_ERROR("Desk::HanderResutDesk error desk free");
		return;
	}

	if (m_deskState == DESK_PLAY)
	{
		//将桌子状态设置为解散房间状态
		m_deskbeforeState = m_deskState;
		//m_deskState = DESK_RESET_ROOM;

		memset(m_reset, RESET_ROOM_TYPE_WAIT, sizeof(m_reset));
		m_reset[pos] = RESET_ROOM_TYPE_AGREE;
		m_resetTime = gWork.GetCurTime().Secs() + 5 * 60;
		m_resetUser = pUser->m_userData.m_nike;

		for(Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (m_user[i] == NULL)
				continue;

			LMsgS2CResetDesk send;
			send.m_errorCode = 0;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if (m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);
		}
	}
	else if (m_deskState == DESK_WAIT)
	{
		if (!m_vip->IsBegin())
		{
			if(pos == 0)
			{
				LMsgS2CResetDesk send;
				send.m_errorCode = 1;
				send.m_applay = pUser->m_userData.m_nike;
				send.m_flag = 1;
				BoadCast(send);
				ResetEnd();
			}
		}
		else
		{
			memset(m_reset, RESET_ROOM_TYPE_WAIT, sizeof(m_reset));
			//将桌子状态设置为解散房间状态
			m_deskbeforeState = m_deskState;
			//m_deskState = DESK_RESET_ROOM;

			m_reset[pos] = RESET_ROOM_TYPE_AGREE;
			m_resetTime = gWork.GetCurTime().Secs() + 5 * 60;
			m_resetUser = pUser->m_userData.m_nike;

			for (Lint i = 0; i < m_iPlayerCapacity; ++i)
			{
				if (m_user[i] == NULL)
					continue;

				LMsgS2CResetDesk send;
				send.m_errorCode = 0;
				send.m_applay = m_resetUser;
				send.m_time = m_resetTime - gWork.GetCurTime().Secs();
				send.m_flag = m_reset[i] ? 1 : 0;
				for (Lint j = 0; j < m_iPlayerCapacity; ++j)
				{
					if (m_user[j] == NULL)
						continue;
					if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
					{
						send.m_agree.push_back(m_user[j]->m_userData.m_nike);
					}
					else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
					{
						send.m_wait.push_back(m_user[j]->m_userData.m_nike);
					}
					else
					{
						send.m_refluse = m_user[j]->m_userData.m_nike;
					}
				}
				m_user[i]->Send(send);
			}
		}
	}
}

//玩家选择解算操作
void Desk::HanderSelectResutDesk(User* pUser,LMsgC2SSelectResetDesk* msg)
{
	if(m_resetTime == 0)
		return;
	
	Lint pos = GetUserPos(pUser);
	if (pos == INVAILD_POS)
	{
		return;
	}

	if (msg->m_flag < 1 || msg->m_flag >2)
	{
		msg->m_flag = 1;
	}

	m_reset[pos] = msg->m_flag;

	Lint agree = 0, refluse = 0;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_reset[i] == RESET_ROOM_TYPE_AGREE)
			++agree;
		else if (m_reset[i] == RESET_ROOM_TYPE_REFLUSE)
			++refluse;
	}

	if (refluse >= 1)
	{
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if(m_user[i] == NULL)
				continue;

			LMsgS2CResetDesk send;
			send.m_errorCode = 4;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if(m_user[j] == NULL)
					continue;

				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);
		}
		m_deskState = m_deskbeforeState;
		ResetClear();

		//由于网络原因可能某些玩家已经准备但延迟收到解散命令,当有人拒绝的时候应该再次check一下
		CheckGameStart();
	}
	else if (agree >= (m_iPlayerCapacity - 1))
	{
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if(m_user[i] == NULL)
				continue;
			LMsgS2CResetDesk send;
			send.m_errorCode = 2;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if(m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);
		}
		ResetEnd();
	}
	else
	{
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if(m_user[i] == NULL)
				continue;
			LMsgS2CResetDesk send;
			send.m_errorCode = 0;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if(m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);
		}
	}
}

void Desk::CheckReset()
{
	if (m_resetTime && gWork.GetCurTime().Secs() > m_resetTime)
	{
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if(m_user[i] == NULL)
				continue; 
			LMsgS2CResetDesk send;
			send.m_errorCode = 3;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_iPlayerCapacity; ++j)
			{
				if(m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);
		}
		ResetEnd();
	}
}

void Desk::HanderUserReady(User* pUser,LMsgC2SUserReady* msg)
{
	LMsgS2CUserReady ready;
	Lint pos = GetUserPos(pUser);

	if(m_deskState != DESK_WAIT/*&&m_deskState !=DESK_COUNT_RESULT*/)
	{
		LLOG_ERROR("Desk::HanderUserReady state error, userid=%d deskstate=%d", pUser->GetUserDataId(), m_deskState);
		return;
	}

	if(pos == INVAILD_POS)
	{
		LLOG_ERROR("Desk::HanderUserReady pos error, userid=%d pos=%d", pUser->GetUserDataId(), pos);
		return;
	}

	//日志太多暂时屏蔽
	//LLOG_INFO("Desk::HanderUserReady userid=%d pos=%d", pUser->GetUserDataId(), pos);

	ready.m_pos = pos;
	BoadCast(ready);
	m_readyState[pos] = 1;

	CheckGameStart();
}

void Desk::HanderUserOutDesk(User* pUser)
{
	Lint pos = GetUserPos(pUser);
	if(pos == INVAILD_POS)
	{
		return;
	}

	//谁回来了
	LMsgS2CUserOnlineInfo info;
	info.m_flag = 0;
	info.m_pos = pos;
	BoadCastWithOutUser(info, pUser);
}

void Desk::HanderUserEndSelect(User* pUser,LMsgC2SUserEndCardSelect* msg)
{
	if(m_deskPlayState != DESK_PLAY_END_CARD)
	{
		LLOG_DEBUG("Desk::HanderUserEndSelect state error %d:%d:%d",m_deskState,m_deskPlayState,msg->m_flag);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderUserEndSelect(pUser, msg);
	}
}

void Desk::HanderUserStartHu(User* pUser,LMsgC2SUserStartHuSelect* msg)
{
	LLOG_DEBUG("Desk::HanderUserStartHu %d:%d",msg->m_type,msg->m_card.size());

	if(m_deskState != DESK_PLAY || m_deskPlayState != DESK_PLAY_START_HU)
	{
		LLOG_DEBUG("Desk::HanderUserStartHu state error %d:%d:%d",m_deskState,m_deskPlayState,msg->m_type);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderUserStartHu(pUser, msg);
	}
}

void Desk::HanderUserTangReq( User* pUser,LMsgC2STangCard* msg )
{
	if(mGameHandler)
		mGameHandler->HanderUserTangReq(pUser,msg);
}

void Desk::HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg)
{
	if(m_deskState != DESK_PLAY || m_deskPlayState != DESK_PLAY_GET_CARD)
	{
		//pUser->Send(sendMsg);
		LLOG_ERROR("Desk::HanderUserPlayCard state error %s, m_deskState:%d m_deskPlayState:%d",pUser->m_userData.m_nike.c_str(), m_deskState, m_deskPlayState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderUserPlayCard(pUser, msg);
	}
}

void Desk::HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg)
{
	if (m_deskState != DESK_PLAY || m_deskPlayState != DESK_PLAY_THINK_CARD)
	{
		LLOG_DEBUG("Desk::HanderUserOperCard state error %s, m_deskState:%d m_deskPlayState:%d", pUser->m_userData.m_nike.c_str(), m_deskState, m_deskPlayState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderUserOperCard(pUser, msg);
	}
}

void Desk::HanderUserSpeak(User* pUser, LMsgC2SUserSpeak* msg)
{
	if(mGameHandler)
	{
		mGameHandler->HanderUserSpeak(pUser,msg);
	}

	Lint pos = GetUserPos(pUser);
	if (pos == INVAILD_POS)
	{
		return;
	}

	LMsgS2CUserSpeak speak;
	speak.m_userId = m_user[pos]->GetUserDataId();
	speak.m_pos = pos;
	speak.m_id = msg->m_id;
	speak.m_type = msg->m_type;
	speak.m_musicUrl = msg->m_musicUrl;
	speak.m_msg = msg->m_msg;
	BoadCast(speak);
}

void Desk::HanderUserLocation(User* pUser, LMsgC2SLocation* msg)
{
	Lint pos = GetUserPos(pUser);
	if (pos == INVAILD_POS)
	{
		return;
	}

	pUser->m_location = msg->m_location;

	LMsgS2CLocation location;
	location.m_pos = pos;
	location.m_location = msg->m_location;
	BoadCast(location);
}

bool Desk::OnUserReconnect(User* pUser)
{
	Lint pos = GetUserPos(pUser);
	if (pos == INVAILD_POS)
	{
		LLOG_ERROR("Desk::OnUserReconnect pos error %d", pUser->GetUserDataId());
		return false;
	}

	m_readyState[pos] = 1;
	if(m_deskState == DESK_WAIT)	//如果是等待状态回来时需要把准备
	{
		LMsgS2CUserReady ready;
		ready.m_pos = pos;
		BoadCast(ready);
	}

	//把自己加进来
	LMsgS2CIntoDesk send1;
	send1.m_deskId = m_id;
	send1.m_pos = pos;
	send1.m_ready = m_readyState[pos];
	send1.m_score = m_vip ? m_vip->GetUserScore(pUser) : 0;
	send1.m_coins = pUser->GetUserData().m_coins;
	send1.m_state = m_state;
	send1.m_maxCircle = m_vip ? m_vip->m_maxCircle : 0;
	send1.m_playtype = m_playtype;
	send1.m_changeOutTime = m_autoChangeOutTime;
	send1.m_opOutTime = m_autoPlayOutTime;
	send1.m_baseScore = m_baseScore;
	send1.m_credits = pUser->m_userData.m_creditValue;
	pUser->Send(send1);

 	//把其他人加进来
 	for(Lint i = 0 ;i < m_iPlayerCapacity; ++i)
 	{
 		if(m_user[i] != NULL && m_user[i] != pUser)
 		{
 			LMsgS2CDeskAddUser addmsg2;
 			addmsg2.m_userId = m_user[i]->GetUserDataId();
 			addmsg2.m_pos = i;
 			addmsg2.m_nike = m_user[i]->m_userData.m_nike;
			addmsg2.m_ready = m_readyState[i];
 			addmsg2.m_sex = m_user[i]->m_userData.m_sex;
 			addmsg2.m_face = m_user[i]->m_userData.m_headImageUrl;
 			addmsg2.m_ip = m_user[i]->GetIp();
			addmsg2.m_location = m_user[i]->m_location;
			addmsg2.m_score = m_vip ? m_vip->GetUserScore(m_user[i]) : 0;
			addmsg2.m_online = m_user[i]->GetOnline(); 
			addmsg2.m_coins = m_user[i]->GetUserData().m_coins;
			addmsg2.m_credits = m_user[i]->m_userData.m_creditValue;
 			pUser->Send(addmsg2);
 		}
 	}

	//谁回来了
	LMsgS2CUserOnlineInfo info;
	info.m_flag = 1;
	info.m_pos = pos;
	BoadCastWithOutUser(info, pUser);

	//不在游戏中，不需要后面的数据
	if(m_deskState == DESK_WAIT)
	{
		CheckGameStart();
	}
	else
	{
		//发送当前圈数信息
		if(m_vip)
		{
			m_vip->SendInfo();
		}

		if(mGameHandler)
		{
			mGameHandler->OnUserReconnect(pUser);
		}
	}

	//如果有人申请解散则通知解散信息
	if(m_resetTime)
	{
		LMsgS2CResetDesk send;
		send.m_errorCode = 0;
		send.m_applay = m_resetUser;
		send.m_time = m_resetTime - gWork.GetCurTime().Secs();
		send.m_flag = m_reset[pos] ? 1 : 0;
		for (Lint j = 0; j < m_iPlayerCapacity; ++j)
		{
			if(m_user[j] == NULL)
				continue; 
			if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
			{
				send.m_agree.push_back(m_user[j]->m_userData.m_nike);
			}
			else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
			{
				send.m_wait.push_back(m_user[j]->m_userData.m_nike);
			}
			else
			{
				send.m_refluse = m_user[j]->m_userData.m_nike;
			}
		}
		pUser->Send(send);
	}

	return true;
}

// 玩家定缺
void Desk::HanderUserDingQue(User* pUser, LMsgC2SUserDingQue* msg)
{
	if (mGameHandler)
	{
		mGameHandler->HanderUserDingQue(pUser, msg);
	}
}

// 玩家换牌
void Desk::HanderUserChange(User* pUser, LMsgC2SBottomPour* msg)
{
	if (mGameHandler)
	{
		mGameHandler->HanderUserChange(pUser, msg);
	}
}

void Desk::SetVip(VipLogItem* vip)
{
	m_vip = vip;
	if(m_vip)
	{
		m_state = vip->m_state;
		m_playtype = vip->m_playtype;
	}
}

VipLogItem* Desk::GetVip()
{
	return m_vip;
}

Lint Desk::GetFreePos()
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i] == NULL)
			return i;
	}

	return INVAILD_POS;

}

void Desk::OnUserInRoom(User* user)
{
	if (GetUserPos(user) != INVAILD_POS)
	{
		LLOG_ERROR("Desk::OnUserInRoom is in desk %d", user->GetUserDataId());
		return;
	}

	Lint pos = GetFreePos();
	if(pos == INVAILD_POS)
	{
		LLOG_ERROR("Desk::OnUserInRoom INVAILD_POS %d",user->GetUserDataId());
		return;
	}

	m_user[pos] = user;
	user->SetDesk(this);
	m_readyState[pos] = 1;

	LMsgS2CIntoDesk send1;
	send1.m_deskId = m_id;
	send1.m_pos = pos;
	send1.m_ready = m_readyState[pos];
	send1.m_score = m_vip ? m_vip->GetUserScore(user) : 0;
	send1.m_coins = user->GetUserData().m_coins;
	send1.m_state = m_state;
	send1.m_maxCircle = m_vip ? m_vip->m_maxCircle : 0;
	send1.m_playtype = m_playtype;
	send1.m_changeOutTime = m_autoChangeOutTime;
	send1.m_opOutTime = m_autoPlayOutTime;
	send1.m_baseScore = m_baseScore;
	send1.m_credits = user->m_userData.m_creditValue;
	user->Send(send1);

 	LMsgS2CDeskAddUser addmsg1;
 	addmsg1.m_userId = user->GetUserDataId();
 	addmsg1.m_score = m_vip ? m_vip->GetUserScore(user) : 0;
	addmsg1.m_pos = pos;
 	addmsg1.m_nike = user->m_userData.m_nike;
 	addmsg1.m_ready = m_readyState[pos];
 	addmsg1.m_sex = user->m_userData.m_sex;
 	addmsg1.m_ip = user->GetIp();
	addmsg1.m_location = user->m_location;
 	addmsg1.m_face = user->m_userData.m_headImageUrl;
	addmsg1.m_online = user->GetOnline();
	addmsg1.m_coins = user->GetUserData().m_coins;
	addmsg1.m_credits = user->m_userData.m_creditValue;
 	
 	for(Lint i = 0 ;i < m_iPlayerCapacity; ++i)
 	{
 		if(m_user[i] != NULL && m_user[i] != user)
 		{
 			LMsgS2CDeskAddUser addmsg2;
 			addmsg2.m_userId = m_user[i]->GetUserDataId();
 			addmsg2.m_pos = i;
 			addmsg2.m_nike = m_user[i]->m_userData.m_nike;
			addmsg2.m_ready = m_readyState[i];
 			addmsg2.m_sex = m_user[i]->m_userData.m_sex;
 			addmsg2.m_face = m_user[i]->m_userData.m_headImageUrl;
 			addmsg2.m_ip = m_user[i]->GetIp();
			addmsg2.m_location = m_user[i]->m_location;
			addmsg2.m_score = m_vip ? m_vip->GetUserScore(m_user[i]) : 0;
			addmsg2.m_online = m_user[i]->GetOnline();
			addmsg2.m_coins = m_user[i]->GetUserData().m_coins;
			addmsg2.m_credits = m_user[i]->m_userData.m_creditValue;
 			user->Send(addmsg2);
 			m_user[i]->Send(addmsg1);
 		}
 	}

	CheckGameStart();
}

void Desk::OnUserInRoom(User* user[])
{
	for(Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		Lint pos = GetFreePos();
		if(pos == INVAILD_POS)
		{
			LLOG_ERROR("Desk::OnUserInRoom INVAILD_POS %d",user[0]->GetUserDataId());
			return;
		}

		m_user[pos] = user[i];
		user[i]->SetDesk(this);
		m_readyState[pos] = 1;
	}

	CheckGameStart();
}

void Desk::OnUserOutRoom(User* user)
{
 	Lint pos = GetUserPos(user);
 	if(pos == INVAILD_POS)
 	{
 		LLOG_ERROR("Desk::OnUserOutRoom INVAILD_POS %d",user->GetUserDataId());
 		return;
 	}
 
 	LMsgS2CDeskDelUser del;
 	del.m_pos = pos;	
 	for(Lint i = 0 ;i < m_iPlayerCapacity; ++i)
 	{
 		if(m_user[i] != NULL)
 		{
 			m_user[i]->Send(del);
 		}
	}
	//修改状态
	user->setUserState(LGU_STATE_CENTER);
	user->ModifyUserState(true);

	//清除状态
	user->SetDesk(NULL);
 	m_user[pos] = NULL;
	m_readyState[pos] = 0;

	//删除玩家 不删除机器人
	if (user->getUserGateID() != 65535)		//不是机器人删除掉
	{
		UserManager::Instance().DelUser(user);
		delete user;
	}

}

Lint Desk::GetNextPos(Lint prev)
{
	if(prev < 0 || prev >= m_iPlayerCapacity)
	{
		return INVAILD_POS;
	}

	return prev == (m_iPlayerCapacity - 1) ? 0 : (prev + 1);
}

Lint Desk::GetPrevPos(Lint next)
{
	if(next < 0 || next >= m_iPlayerCapacity)
	{
		return INVAILD_POS;
	}

	return next == 0 ? (m_iPlayerCapacity - 1) : (next - 1);
}

Lint Desk::GetUserPos(User* user)
{
	if(user == NULL)
	{
		return INVAILD_POS;
	}

	for(Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if(m_user[i] == user)
			return i;
	}

	return INVAILD_POS;
}

User* Desk::GetPosUser(Lint pos)
{
	if(pos<0 || pos >= INVAILD_POS)
	{
		return NULL;
	}

	return m_user[pos];
}

Lint Desk::GetUserCount()
{
	Lint cnt = 0;
	for(Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if(m_user[i])
		{
			cnt += 1;
		}
	}

	return cnt;
}

void Desk::SetDeskWait()
{
	memset(m_readyState, 0, sizeof(m_readyState));
	m_deskState = DESK_WAIT;
}

void Desk::OnDeskWait()
{
	
}

void Desk::CheckGameStart()
{
	if(m_deskState != DESK_WAIT)	//只有桌子为等待状态才继续
	{
		LLOG_DEBUG("Desk's status is wrong. Desk state = %d.", m_deskState);
		return;
	}

	if(m_resetTime != 0)
	{
		LLOG_DEBUG("Desk ising reseted");
		return;
	}

	//判断所有的人是否都已经准备
	if(GetUserCount() != m_iPlayerCapacity)
	{
		LLOG_DEBUG("The player num isn't enought");
		return;
	}

	for(int i = 0; i < m_iPlayerCapacity; ++i)
	{
		if(m_user[i] == NULL || !m_readyState[i])
		{
			LLOG_DEBUG("The player hasn't  been ready");
			return;
		}
	}

	if(mGameHandler)
	{
		mGameHandler->SetDeskPlay();
	}
	else
	{
		LLOG_ERROR("Game handler han't beed created");
	}
}

void Desk::SetDeskFree()
{
	m_deskState = DESK_FREE;
	m_timeDeskCreate = 0;

	if(mGameHandler)
	{
		GameHandlerFactory::getInstance()->destroy((GameType)m_state,mGameHandler);
		LLOG_DEBUG("Desk::SetDeskFree    mHander");
		mGameHandler = NULL;
	}
	if( m_deskType == DeskType_Coins )
	{
		// 金币桌子由金币服务器负责回收自己回收
	}
	else
	{
		gWork.RecycleDeskId(m_id);
	}
}

void Desk::BoadCast(LMsg& msg)
{
	for(Lint i = 0 ; i < m_iPlayerCapacity ;++i)
	{
		if(m_user[i])
			m_user[i]->Send(msg);
	}
}

void Desk::BoadCastWithOutUser(LMsg& msg, User* user)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_user[i] && m_user[i] != user)
			m_user[i]->Send(msg);
	}
}

void Desk::BroadcastWithoutUser(LMsg& msg, Lint pos)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (i != pos && m_user[i])
			m_user[i]->Send(msg);
	}
}

void Desk::ClearUser()
{
	if( m_deskType == DeskType_Common )
	{
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (m_user[i])
				OnUserOutRoom(m_user[i]);
		}
	}
	else
	{
		for(Lint i = 0 ;i < m_iPlayerCapacity; ++i)
		{
			if(m_user[i])
			{
				m_user[i]->setUserState(LGU_STATE_COIN);
				m_user[i]->ModifyUserState(true);   //修改user状态，通知金币服务器

				m_user[i]->SetDesk(NULL);

				//删除玩家 不删除机器人
				if (m_user[i]->getUserGateID() != 65535)		//不是机器人删除掉
				{
					UserManager::Instance().DelUser(m_user[i]);
					delete m_user[i];
					m_user[i] = NULL;
				}

				m_readyState[i] = 0;
			}
		}
	}
}


void Desk::SetAllReady()
{
	memset(m_readyState, 1, sizeof(m_readyState));
}

void Desk::ResetClear()
{
	m_resetTime = 0;
	m_resetUser.clear();
	memset(m_reset, 0, sizeof(m_reset));
}

void Desk::ResetEnd()
{
	if(m_vip)
	{
		m_vip->m_reset = 1;
	}
	
	if (m_deskState == DESK_PLAY)
	{
		if (mGameHandler)
		{
			LLOG_DEBUG("@m_state:[%d]", m_state);
			if (MJHeNanZhuanZhuanFour == m_state || MJHeNanHuaShui == m_state || MJHeNanTuiDaoHu == m_state || MJHeNanXinXiang == m_state || MJHeNanKaiFeng == m_state) 
			{
				Lint tmpHu[DESK_USER_COUNT] = {0};
				mGameHandler->OnGameOver(WIN_NONE, tmpHu, INVAILD_POS, NULL);
			} 
			else
			{
				mGameHandler->OnGameOver(WIN_DISS, INVAILD_POS);
			}
		}
	}
	else
	{
		if(m_vip && m_vip->IsBegin())
			m_vip->SendEnd();
		SetVip(NULL);
		ClearUser();
		ResetClear();
		SetDeskFree();
	}
}

void Desk::SpecialCardClear()
{
	memset(m_specialCard,0,sizeof(m_specialCard));
}


//////////////////////////////////////////////////////////////////////////

Lint Desk::getGameType() const
{
	return m_state;
}

const std::vector<Lint>& Desk::getPlayType() const
{
	return m_playtype;
}

Lint Desk::getDeskState() const
{
	return m_deskState;
}

void Desk::setDeskState(Lint nState)
{
	m_deskState = nState;
}

Lint Desk::getDeskPlayState()
{
	return m_deskPlayState;
}

void Desk::setDeskPlayState(Lint nDeskPlayState)
{
	m_deskPlayState = nDeskPlayState;
}

void Desk::HanderGameOver(Lint result)
{
	if (m_vip == NULL)
	{
		return;
	}

	if( m_deskType == DeskType_Common )
	{
		//检测扣房卡
		if (/*m_vip->m_curCircle == 1*/ m_vip->m_round == 1 && !m_vip->m_reset)
		{
// 			Lint cardType = CARD_TYPE_DEFAULT;
// 			if (m_vip->m_maxCircle == 8)
// 				cardType = CARD_TYPE_8;
// 			else if (m_vip->m_maxCircle == 16)
// 				cardType = CARD_TYPE_16;
// 			m_user[0]->DelCardCount(cardType, CARD_TYPE_NEED_COUNT[cardType], CARDS_OPER_TYPE_CREATE_ROOM,"system");

// 			for(int i=CARD_TYPE_NONE+1; i<CARD_TYPE_MAX; ++i)
// 			{
// 				if(m_vip->m_maxCircle != CARD_TYPE_CIRCLE[i])
// 					continue;
//
// 				cardType = i;
// 				break;
// 			}

// 			Lint cardType = GetCardTypeByCircle(m_vip->m_maxCircle);
//
// 			//打10圈的是每个人都要扣 特殊处理
// 			if(cardType == CARD_TYPE_10)
// 			{
// 				for(int i=0; i<m_iPlayerCapacity; ++i)
// 				{
// 					m_user[i]->DelCardCount(cardType, CARD_TYPE_NEED_COUNT[cardType], CARDS_OPER_TYPE_CREATE_ROOM,"system");
// 				}
// 			}
// 			else
// 			{
// 				m_user[0]->DelCardCount(cardType, CARD_TYPE_NEED_COUNT[cardType], CARDS_OPER_TYPE_CREATE_ROOM,"system");
// 			}

			Lint count = 0;
			if(NeedCardCount(m_cardType, count))
			{
				for(int i=0; i<m_iPlayerCapacity; ++i)
				{
					m_user[i]->DelCardCount(m_cardType, count, CARDS_OPER_TYPE_CREATE_ROOM,"system");
				}
			}
			else
			{
				m_user[0]->DelCardCount(m_cardType, count, CARDS_OPER_TYPE_CREATE_ROOM,"system");
			}
		}
	}

	if(m_vip->isNormalEnd())
	{
		if(m_deskType == DeskType_Common)
		{
			LMsgL2LDBSaveCRELog log;
			log.m_strUUID = m_user[0]->m_userData.m_unioid;
			log.m_deskID = this->m_id;
			log.m_strLogId = m_vip->m_id;
			log.m_time = m_vip->m_time;
			for(Lint i = 0; i < m_iPlayerCapacity; ++i)
			{
				log.m_id[i] = m_user[i]->m_userData.m_id;
			}
			gWork.SendMsgToDb(log);
		}
	}

	if(m_vip->isEnd())
	{
		m_vip->SendEnd();
		m_vip->m_desk = NULL;
		ClearUser();
		SetDeskFree();
		SetVip(NULL);
		ResetClear();
	}
	SpecialCardClear();
}

void Desk::HanderAddCardCount(Lint pos, Lint CardNum, CARDS_OPER_TYPE AddType, Lstring admin)
{
	if(pos < 0 || pos >= INVAILD_POS)
	{
		LLOG_ERROR("Desk::HanderAddCardCount pos = %d error", pos);
		return;
	}

	if(!m_user[pos])
	{
		LLOG_ERROR("Desk::HanderAddCardCount user = %d is null", pos);
		return;
	}
	if(CardNum <= 0)		return;
	m_user[pos]->AddCardCount(CARD_TYPE_8, CardNum, AddType, admin);
}

void Desk::_clearData()
{
	m_lastOptTime	  = 0;
	m_curOptPos		  = 0;

	m_id = 0;
	
	m_deskbeforeState = DESK_FREE;
	m_deskState = DESK_FREE;
	m_deskPlayState = -1;

	memset(m_user, 0, sizeof(m_user));
	m_vip = 0;
	memset(m_readyState, 0, sizeof(m_readyState));
	m_state = 0;
	m_playtype.clear();
	ResetClear();
	SpecialCardClear();
	m_resetUser = "";					//申请的玩家
	m_resetTime = 0;					//申请解算的时间
	mGameHandler = NULL;

	m_iPlayerCapacity = 0;

	m_timeDeskCreate = 0;

	memset( m_autoPlay, 0, sizeof(m_autoPlay) );
	memset( m_autoPlayTime, 0, sizeof(m_autoPlayTime) );
	memset( m_autoOutTime, 0, sizeof(m_autoOutTime) );
	m_autoChangeOutTime = 0;
	m_autoPlayOutTime = 0;
	m_baseScore = 1;
	memset( m_coinsResult, 0, sizeof(m_coinsResult) );

	m_deskType = DeskType_Common;
	m_gameType = 0;
	m_cardType = 0;
}

bool Desk::_createRegister(GameType gameType)
{
	mGameHandler = GameHandlerFactory::getInstance()->create(gameType);
	if (!mGameHandler)
	{
		LLOG_ERROR("No game handler for game type %d found.", gameType);
		return false;
	}

	mGameHandler->startup(this);
	return true;
}

// Add HuNanZhuanZhuan by hanlu 02242017 Begin
void Desk::UpdateLastOptTimeAndPos(Lint optpos)
{
	if (optpos < 0 || optpos > 3) {
		LLOG_ERROR("ZZ: UpdateLastOptTimeAndPos is failed by optpos.");
		return;
	}
	m_lastOptTime = gWork.GetCurTime().Secs();
	m_curOptPos = optpos;
}

void Desk::UpdateLastOptTimeAndPos(User* user)
{
	if (NULL == user) {
		LLOG_ERROR("ZZ: UpdateLastOptTimeAndPos is failed by user is null.");
		return;
	}

	Lint optpos = GetUserPos(user);
	if (optpos < 0 || optpos > 3) {
		LLOG_ERROR("ZZ: UpdateLastOptTimeAndPos is failed by user.");
		return;
	}
	UpdateLastOptTimeAndPos(optpos);
}
// Add HuNanZhuanZhuan by hanlu 02242017 End

//////////////////////////////////////////////////////////////////////////
