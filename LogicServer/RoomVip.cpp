#include "RoomVip.h"
#include "Work.h"
#include "LLog.h"
#include "UserManager.h"
#include "Config.h"
#include "RobotManager.h"
#include "RLogHttp.h"

VipLogItem::VipLogItem()
{
	m_desk = NULL;
	m_round = 0;
}

VipLogItem::~VipLogItem()
{

}

Lint VipLogItem::GetOwerId()
{
	return m_posUserId[0];
}

void VipLogItem::AddTuiDaoLog(User** user, Lint* gold,Lint* winInfo, Lint zhuangPos,Lint* agang,Lint* mgang,Lint *bomb, Lstring& videId, Lint* ghostZimo, Lint *noghostZimo, bool addCircle)
{
	if (NULL == user || NULL == gold || NULL == winInfo || NULL == agang || NULL == mgang || NULL == bomb || NULL == ghostZimo || NULL == noghostZimo)
	{
		LLOG_ERROR("VipLogItem find Null point!");
		return;
	}
	VipDeskLog* log = new VipDeskLog();
	log->m_zhuangPos = zhuangPos;
	log->m_time = gWork.GetCurTime().Secs();
	log->m_videoId = videId;
	for (Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		log->m_gold[i] = gold[i];
		log->m_win[i] = winInfo[i];

		m_score[i] += gold[i];
		log->m_angang[i] += agang[i];
		log->m_mgang[i] += mgang[i];
		log->m_bomb[i] += bomb[i];

		log->m_zimo[i] = ghostZimo[i];
		log->m_dzimo[i] = noghostZimo[i];
	}
	m_log.push_back(log);

// 	m_curCircle += 1;
// 	m_curZhuangPos = zhuangPos;
//
// 	if(m_curCircle == 1)
// 	{
// 		InsertToDb();
// 	}
// 	else
// 	{
// 		UpdateToDb();
// 	}

	++m_round;
	if(addCircle)
		m_curCircle += 1;
	m_curZhuangPos = zhuangPos;

	if(m_round == 1)
	{
		InsertToDb();
	}
	else
	{
		UpdateToDb();
	}

	//第八局存上圈数
	if (m_curCircle == m_maxCircle)
	{
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			if (user[i] != NULL)
			{
				user[i]->m_userData.m_totalplaynum ++;
				LMsgL2LMGModifyPlayCount msg;
				msg.m_userid = user[i]->m_userData.m_id;
				msg.m_curPlayCount = user[i]->m_userData.m_totalplaynum;
				gWork.SendToLogicManager(msg);
			}
		}
	}
	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);
}
void VipLogItem::AddLog(User** user, Lint* gold, HuInfo* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting, bool addCircle)
{
	VipDeskLog* log = new VipDeskLog();
	log->m_videoId = videoId;
	log->m_zhuangPos = zhuangPos;
	log->m_time = gWork.GetCurTime().Secs();
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_gold[i] = gold[i];
		m_score[i] += gold[i];
		log->m_angang[i] += agang[i];
		log->m_mgang[i] += mgang[i];
		log->m_checkTing[i] += ting[i];
		if(huinfo[i].type == WIN_SUB_ZIMO)
		{
			log->m_zimo[i] += 1;
		}
		else if(huinfo[i].type == WIN_SUB_BOMB)
		{
			log->m_win[i] += 1;
			if ( huinfo[i].abombpos != INVAILD_POS )
			{
				log->m_bomb[huinfo[i].abombpos] += 1;	// 放炮者次数累加
			}
		}
		else if(huinfo[i].type == WIN_SUB_DZIMO)
		{
			log->m_dzimo[i] += 1;
		}
		else if(huinfo[i].type == WIN_SUB_DBOMB)
		{
			log->m_dwin[i] += 1;
			if ( huinfo[i].abombpos != INVAILD_POS )
			{
				log->m_dbomb[huinfo[i].abombpos] += 1;	// 放炮者次数累加
			}
		}
	}
	m_log.push_back(log);


// 	m_curCircle += 1;
// 	m_curZhuangPos = zhuangPos;
//
// 	if(m_curCircle == 1)
// 	{
// 		InsertToDb();
// 	}
// 	else
// 	{
// 		UpdateToDb();
// 	}

	++m_round;
	if(addCircle)
		m_curCircle += 1;
	m_curZhuangPos = zhuangPos;

	if(m_round == 1)
	{
		InsertToDb();
	}
	else
	{
		UpdateToDb();
	}

	//第八局存上圈数
	if (m_curCircle == m_maxCircle)
	{
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (user[i] != NULL)
			{
				user[i]->m_userData.m_totalplaynum ++;
				LMsgL2LMGAddUserPlayCount msg;
				msg.m_userid = user[i]->m_userData.m_id;
				msg.m_strUUID = user[i]->m_userData.m_unioid;
				gWork.SendToLogicManager(msg);
			}
		}
	}
	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);
}

void VipLogItem::AddLog(User** user, Lint* gold, std::vector<HuInfo>* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting, bool addCircle)
{
	VipDeskLog* log = new VipDeskLog();
	log->m_videoId = videoId;
	log->m_zhuangPos = zhuangPos;
	log->m_time = gWork.GetCurTime().Secs();
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_gold[i] = gold[i];
		m_score[i] += gold[i];
		log->m_angang[i] += agang[i];
		log->m_mgang[i] += mgang[i];
		log->m_checkTing[i] += ting[i];

		for (size_t k =0; k < huinfo[i].size(); ++k)
		{
			if(huinfo[i][k].type == WIN_SUB_ZIMO)
			{
				log->m_zimo[i] += 1;
			}
			else if(huinfo[i][k].type == WIN_SUB_BOMB)
			{
				log->m_win[i] += 1;
				if ( huinfo[i][k].abombpos != INVAILD_POS )
				{
					log->m_bomb[huinfo[i][k].abombpos] += 1;	// 放炮者次数累加
				}
			}
			else if(huinfo[i][k].type == WIN_SUB_DZIMO)
			{
				log->m_dzimo[i] += 1;
			}
			else if(huinfo[i][k].type == WIN_SUB_DBOMB)
			{
				log->m_dwin[i] += 1;
				if ( huinfo[i][k].abombpos != INVAILD_POS )
				{
					log->m_dbomb[huinfo[i][k].abombpos] += 1;	// 放炮者次数累加
				}
			}
		}
	}
	m_log.push_back(log);

// 	m_curCircle += 1;
// 	m_curZhuangPos = zhuangPos;
//
// 	if(m_curCircle == 1)
// 	{
// 		InsertToDb();
// 	}
// 	else
// 	{
// 		UpdateToDb();
// 	}

	++m_round;
	if(addCircle)
		m_curCircle += 1;
	m_curZhuangPos = zhuangPos;

	if(m_round == 1)
	{
		InsertToDb();
	}
	else
	{
		UpdateToDb();
	}

	//第八局存上圈数
	if (m_curCircle == m_maxCircle)
	{
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (user[i] != NULL)
			{
				user[i]->m_userData.m_totalplaynum ++;
				LMsgL2LMGAddUserPlayCount msg;
				msg.m_userid = user[i]->m_userData.m_id;
				msg.m_strUUID = user[i]->m_userData.m_unioid;
				gWork.SendToLogicManager(msg);
			}
		}
	}
	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);
}

bool VipLogItem::ExiestUser(Lint id)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (id == m_posUserId[i])
			return true;
	}
	return false;
}

void VipLogItem::InsertToDb()
{
	std::stringstream ss;
	ss << "INSERT INTO";
	if ( m_state > 1000 )	// > 1000 存放到金币表中
	{
		ss << " logcoins ";
	}
	else
	{
		ss << " log ";
	}
	ss << "(Id,Time,Pos1,Pos2,Pos3,Pos4,Flag,DeskId,MaxCircle,CurCircle,Pass,Score1,Score2,Score3,Score4,Reset,checkTing1,checkTing2,checkTing3,checkTing4,Data,PlayType) VALUES (";
	ss << "'" << m_id << "',";
	ss << "'" << m_time << "',";
	ss << "'" << m_posUserId[0]<< "',";
	ss << "'" << m_posUserId[1] << "',";
	ss << "'" << m_posUserId[2] << "',";
	ss << "'" << m_posUserId[3] << "',";
	ss << "'" << m_state << "',";
	ss << "'" << m_deskId << "',";
	ss << "'" << m_maxCircle << "',";
	ss << "'" << m_curCircle << "',";
	ss << "'" << m_secret << "',";
	ss << "'" << m_score[0] << "',";
	ss << "'" << m_score[1] << "',";
	ss << "'" << m_score[2] << "',";
	ss << "'" << m_score[3] << "',";
	ss << "'" << m_reset << "',";	
	ss << "'" << m_checkTing[0] << "',";
	ss << "'" << m_checkTing[1] << "',";
	ss << "'" << m_checkTing[2] << "',";
	ss << "'" << m_checkTing[3] << "',";
	ss << "'" << ToString() << "',";
	ss << "'" << PlayTypeToStrint() << "')";

	LMsgL2LDBSaveLogItem send;

	send.m_userId = m_posUserId[0];
	send.m_type = 0;
	send.m_sql = ss.str();
	//gWork.SendToCenter(send);
	gWork.SendMsgToDb(send);
}

bool VipLogItem::IsBegin()
{
	return m_curCircle != 0;
}

void VipLogItem::RemoveUser(Lint id)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (id == m_posUserId[i])
			m_posUserId[i] = 0;
	}
}

void VipLogItem::UpdateToDb()
{
	std::stringstream ss;
	ss << "UPDATE";
	if ( m_state > 1000 )	// > 1000 存放到金币表中
	{
		ss << " logcoins ";
	}
	else
	{
		ss << " log ";
	}
	ss << "SET CurCircle=";
	ss << "'" << m_curCircle << "',Pos1=";
	ss << "'" << m_posUserId[0] << "',Pos2=";
	ss << "'" << m_posUserId[1] << "',Pos3=";
	ss << "'" << m_posUserId[2] << "',Pos4=";
	ss << "'" << m_posUserId[3] << "',Score1=";
	ss << "'" << m_score[0] << "',Score2=";
	ss << "'" << m_score[1] << "',Score3=";
	ss << "'" << m_score[2] << "',Score4=";
	ss << "'" << m_score[3] << "',Reset=";
	ss << "'" << m_reset << "',Pass=";
	ss << "'" << m_secret << "',Data=";
	ss << "'" << ToString()  << "',checkTing1=";
	ss << "'" << m_checkTing[0] << "',checkTing2=";
	ss << "'" << m_checkTing[1] << "',checkTing3=";
	ss << "'" << m_checkTing[2] << "',checkTing4=";
	ss << "'" << m_checkTing[3] << "' WHERE Id=";
	ss << "'" << m_id << "'";

	LMsgL2LDBSaveLogItem send;

	send.m_userId = m_posUserId[0];
	send.m_type = 1;
	send.m_sql = ss.str();
	//gWork.SendToCenter(send);
	gWork.SendMsgToDb(send);
}

bool VipLogItem::IsFull(User* user)
{
	Lint count = 0;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_posUserId[i] == 0 || user->GetUserDataId() == m_posUserId[i])
			return false;
	}

	return true;
}

void VipLogItem::SendInfo()
{
	LMsgS2CVipInfo info;
	info.m_curCircle = m_curCircle ;
	info.m_curMaxCircle = m_maxCircle ;
	info.m_playtype = m_playtype;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		User* user = gUserManager.GetUserbyDataId(m_posUserId[i]);
		if (user)
			user->Send(info);
	}
}

void VipLogItem::SendEnd()
{
	LMsgS2CVipEnd msg;
	User* user[4] = { NULL };

	msg.m_deskid = m_desk->GetDeskId();
	msg.m_playMode = m_desk->GetVip()->m_state;
	msg.m_playPara = m_desk->getPlayType();
	msg.m_maxCircle = m_desk->GetVip()->m_maxCircle;
	//msg.m_roomMasterNike = m_desk->m_MasterNike;

	for (Lint i = 0; i < 4; ++i)
	{
		user[i] = gUserManager.GetUserbyDataId(m_posUserId[i]);
		if (user[i])
		{
			msg.m_nike[i] = user[i]->m_userData.m_nike;
		}
		msg.m_id[i] = m_posUserId[i];
	}

	memset(msg.m_gold,0,sizeof(msg.m_gold));
	memset(msg.m_zimo,0,sizeof(msg.m_zimo));
	memset(msg.m_win,0,sizeof(msg.m_win));
	memset(msg.m_bomb,0,sizeof(msg.m_bomb));
	memset(msg.m_dzimo,0,sizeof(msg.m_dzimo));
	memset(msg.m_dwin,0,sizeof(msg.m_dwin));
	memset(msg.m_dbomb,0,sizeof(msg.m_dbomb));
	memset(msg.m_agang,0,sizeof(msg.m_agang));
	memset(msg.m_mgang,0,sizeof(msg.m_mgang));

	for (Lsize i = 0; i < m_log.size(); ++i)
	{
		for(Lint  j = 0; j < 4; ++j)
		{
			msg.m_gold[j] += m_log[i]->m_gold[j];

			//[bing] 每局的分也发送给客户端
			msg.m_oncegold[j].push_back(m_log[i]->m_gold[j]);

			if(m_log[i]->m_win[j] == WIN_SUB_ZIMO)
				msg.m_zimo[j] += 1;
			else if(m_log[i]->m_win[j] == WIN_SUB_BOMB)
				msg.m_win[j] += 1;
			else if(m_log[i]->m_win[j] == WIN_SUB_ABOMB)
				//msg.m_bomb[j] += 1;		m_win只记点炮位置， 炮信息由bomb记录
			{
				for(int k = 0; k < DESK_USER_COUNT; ++k)
				{
					if (m_log[i]->m_bomb[k] == WIN_SUB_ABOMB)
						msg.m_bomb[j] += 1;
					else if (m_log[i]->m_bomb[k] == WIN_SUB_DABOMB)
						msg.m_dbomb[j] += 1;
				}
			}
			else if(m_log[i]->m_win[j] == WIN_SUB_DZIMO)
				msg.m_dzimo[j] += 1;
			else if(m_log[i]->m_win[j] == WIN_SUB_DBOMB)
				msg.m_dwin[j] += 1;

			msg.m_mgang[j] += m_log[i]->m_mgang[j];
			msg.m_agang[j] += m_log[i]->m_angang[j];
		}
	}

	for (Lint i = 0; i < 4; ++i)
	{
		if (user[i])
			user[i]->Send(msg);
	}

	// 远程日志
	RLOG("bswf", LTimeTool::GetLocalTimeInString()
		<< "|" << gWork.GetManagerID()
		<< "|" << m_state
		<< "|" << PlayTypeToString4RLog()
		<< "|" << m_maxCircle
		<< "|" << m_curCircle
		<< "|" << m_reset
		<< "|" << m_posUserId[0]
		<< ";" << m_posUserId[1]
		<< ";" << m_posUserId[2]
		<< ";" << m_posUserId[3]
		<< "|" << gConfig.GetInsideIp()
		<< "|" << gConfig.GetInsidePort()
		<< "|" << gConfig.GetServerID()
		);
}

Lstring VipLogItem::PlayTypeToString4RLog()
{
	std::stringstream ss;
	if (!m_playtype.empty())
	{
		ss << m_playtype.front();
	}

	for (Lsize i = 1; i < m_playtype.size(); ++i)
	{
		ss << ";" << m_playtype[i];
	}
	return ss.str();
}

bool VipLogItem::isEnd()
{
	return m_maxCircle <= m_curCircle || m_reset == 1;
}

bool VipLogItem::isNormalEnd()
{
	return m_maxCircle == m_curCircle;
}

Lint VipLogItem::GetUserPos(User* user)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (user->GetUserDataId() == m_posUserId[i])
			return i;
	}

	return INVAILD_POS;
}

Lint VipLogItem::GetUserScore(User* user)
{
	Lint pos = GetUserPos(user);
	if (pos != INVAILD_POS)
	{
		return m_score[pos];
	}
	return 0;
}

bool VipLogItem::AddUser(User* user)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (user->GetUserDataId() == m_posUserId[i])
			return true;
		
		if (m_posUserId[i] == 0)
		{
			m_posUserId[i] = user->GetUserDataId();
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
bool VipLogMgr::Init()
{
	m_id = 1;
	m_videoId = 1;
	return true;
}

bool VipLogMgr::Final()
{
	return true;
}

void VipLogMgr::SetVipId(Lint id)
{
	m_id = id;
}

Lstring VipLogMgr::GetVipId()
{
	++m_id;
	m_id = m_id % 5000;
	char mVipLogID[64] = { 0 };
	sprintf_s(mVipLogID, sizeof(mVipLogID)-1, "%d%d%d", gConfig.GetServerID(), gWork.GetCurTime().Secs(), m_id);
	return mVipLogID;
}

void VipLogMgr::SetVideoId(Lint id)
{
	m_videoId = id;
}

Lstring VipLogMgr::GetVideoId()
{
	++m_videoId;
	m_videoId = m_videoId % 5000;
	char mVipVideoID[64] = { 0 };
	sprintf_s(mVipVideoID, sizeof(mVipVideoID)-1, "%d%d%d", gConfig.GetServerID(), gWork.GetCurTime().Secs(), m_videoId);
	return mVipVideoID;
}

VipLogItem*	 VipLogMgr::GetNewLogItem(Lint card,Lint usrId)
{
	VipLogItem* item = new VipLogItem();
	item->m_id = GetVipId();
	item->m_maxCircle = card;
	item->m_time = gWork.GetCurTime().Secs();
	m_item[item->m_id]= item;
	return item;
}

VipLogItem*	VipLogMgr::GetLogItem(Lstring& logId)
{
	if (m_item.count(logId))
	{
		return m_item[logId];
	}

	return NULL;
}

VipLogItem*	VipLogMgr::FindUserPlayingItem(Lint id)
{
	auto it = m_item.begin();
	for(; it != m_item.end(); ++it)
	{
		VipLogItem* item = it->second;
		if(!item->isEnd() && item->ExiestUser(id))
		{
			return item;
		}
	}
	return NULL;
}

void VipLogMgr::AddPlayingItem(VipLogItem* item)
{
	m_item[item->m_id] = item;
}

void VipLogMgr::Tick()
{
	auto it = m_item.begin();
	for (; it != m_item.end();)
	{
		if (it->second->isEnd())
		{
			LLOG_INFO("VipLogMgr::Tick vip end %s",it->first.c_str());
			delete it->second;
			m_item.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool RoomVip::Init()
{

	return true;
}

bool RoomVip::Final()
{
	return true;
}

void RoomVip::Tick(LTime& cur)
{
	DeskMap::iterator it = m_deskMap.begin();
	for (; it != m_deskMap.end(); ++it)
	{
		it->second->Tick(cur);
	}
}

Desk* RoomVip::GetFreeDesk(Lint nDeskID, GameType gameType, Lint cardType)
{
	Desk* ret = NULL;
	
	DeskMap::iterator it = m_deskMap.begin();
	for (; it != m_deskMap.end(); ++it)
	{
		if (it->second->getDeskState() == DESK_FREE)
		{
			ret = it->second;
			m_deskMap.erase(it);
			break;
		}
	}
	
	if(ret == NULL)
	{
		ret = new Desk();
	}

	if(!ret->initDesk(nDeskID, gameType, cardType))
	{
		delete ret;
		ret = NULL;
		LLOG_ERROR("RoomVip::GetFreeDesk -- Fail to init desk");
	}
	else
	{
		m_deskMap[nDeskID] = ret;
		ret->SetDeskWait();
		LLOG_ERROR("RoomVip::GetFreeDesk size=%d", m_deskMap.size());
	}
	
	return ret;
}

Desk* RoomVip::GetDeskById(Lint id)
{
	if(m_deskMap.count(id))
	{
		return m_deskMap[id];
	}

	return NULL;
}

Lint RoomVip::CreateVipDesk(LMsgLMG2LCreateDesk* pMsg, User* pUser)
{
	if (!pUser || !pMsg)
	{
		return -1;
	}
	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode = 0;

	if(pMsg->m_playType.empty())
	{
		LLOG_ERROR("RoomVip::CreateVipDesk play type is empty, userid=%d deskid=%d", pUser->GetUserDataId(), pMsg->m_deskID);
		/*ret.m_errorCode = 5;
		pUser->Send(ret);
		return ret.m_errorCode;*/
	}

 	Lint circle = 0;
	if(pMsg->m_flag <= CARD_TYPE_NONE || pMsg->m_flag >= CARD_TYPE_MAX)
		pMsg->m_flag = CARD_TYPE_DEFAULT;

	circle = GetCircle(pMsg->m_state ,pMsg->m_flag);//CARD_TYPE_CIRCLE[pMsg->m_flag];
	
	if (pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::CreateVipDesk in desk, userid=%d deskid=%d", pUser->GetUserDataId(), pMsg->m_deskID);
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	if (GetDeskById(pMsg->m_deskID) != NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk deskID is in Use, userid=%d deskid=%d", pUser->GetUserDataId(), pMsg->m_deskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	//创建桌子失败
	Desk* desk = GetFreeDesk(pMsg->m_deskID, (GameType)pMsg->m_state, pMsg->m_flag);
	if (desk == NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk create Desk Failed! userid=%d deskid=%d gametype=%d", pUser->GetUserDataId(), pMsg->m_deskID, pMsg->m_state);
		ret.m_errorCode = 4;
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	LLOG_INFO("RoomVip::CreateVipDesk userid=%d deskid=%d gametype=%d", pUser->GetUserDataId(), pMsg->m_deskID, pMsg->m_state);
	//////////////////////////////////////////////////////////////////////////
	VipLogItem* log = gVipLogMgr.GetNewLogItem(circle, pUser->GetUserDataId());
	log->m_desk = desk;
	log->m_curCircle = 0;
	log->m_maxCircle = circle;
	log->m_state = pMsg->m_state;
	log->m_deskId = log->m_desk->GetDeskId();
	log->m_playtype = pMsg->m_playType;
	std::vector<int>::iterator tmpIt;
	for (tmpIt = pMsg->m_playType.begin(); tmpIt != pMsg->m_playType.end(); tmpIt++)
	{
		LLOG_ERROR("play type: %d.", *tmpIt);
	}
	log->m_iPlayerCapacity = desk->GetPlayerCapacity();
	ret.m_deskId = log->m_desk->GetDeskId();
	log->m_desk->SetVip(log);
	pUser->Send(ret);

	log->AddUser(pUser);
	log->m_desk->OnUserInRoom(pUser);

	//判断是否需要加电脑， 增加电脑
	if (gConfig.GetDebugModel())
	{
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			log->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].m_color;
			log->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].m_number;
		}

		if(gConfig.GetIfAddRobot())
		{
			if (pMsg->m_robotNum > 0 && pMsg->m_robotNum < 4)
			{
				gRobotManager.AddRobotToDesk(log->m_deskId, pMsg->m_robotNum);
			}
		}
	}
	return ret.m_errorCode;
}

Lint RoomVip::CreateVipCoinDesk(LMsgCN2LCreateCoinDesk*pMsg,User* pUsers[])
{
	if(!pMsg) 
		return -1;

	Lint circle = 1;

	//创建桌子失败
	Desk* desk = GetFreeDesk(pMsg->m_deskId, (GameType)pMsg->m_state, 0);				// 金币房间不管 cardType是后来为了结算加入的
	if (desk == NULL)
	{
		return -1;
	}

	desk->SetDeskType( DeskType_Coins );   //金币场桌子
	desk->m_baseScore = pMsg->m_baseScore;
	desk->m_autoChangeOutTime = pMsg->m_changeOutTime;
	desk->m_autoPlayOutTime = pMsg->m_opOutTime;
	
	VipLogItem* log = gVipLogMgr.GetNewLogItem(circle, pUsers[0]->GetUserDataId());
	log->m_desk = desk;
	log->m_curCircle = 0;
	log->m_maxCircle = circle;
	log->m_state = pMsg->m_state;
	log->m_deskId = log->m_desk->GetDeskId();
	log->m_playtype = pMsg->m_playType;
	log->m_iPlayerCapacity = desk->GetPlayerCapacity();
	log->m_desk->SetVip(log);

	for(Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		log->AddUser(pUsers[i]);
	}

	// 是否配牌
	if (gConfig.GetDebugModel())
	{
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			log->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].m_color;
			log->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].m_number;
		}
	}

	log->m_desk->OnUserInRoom(pUsers);

	return 0;
}

Lint RoomVip::AddToVipDesk(User* pUser, Lint nDeskID)
{
	if (!pUser)
	{
		return -1;
	}
	LMsgS2CAddDeskRet ret;
	ret.m_deskId = nDeskID;
	ret.m_errorCode = 0;

	if(pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk has desk, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 5;		// 理论上应该没有
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	Desk* desk = GetDeskById(nDeskID);

	if (!desk )
	{
		LLOG_ERROR("RoomVip::AddToVipDesk not find desk, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (!desk->GetVip() || desk->GetVip()->IsBegin())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk already begin, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 1;			// 提示满员 以后有需要在改
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (desk->GetVip()->IsFull(pUser) || desk->GetUserCount() == desk->GetPlayerCapacity() || desk->getDeskState() !=  DESK_WAIT)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk full, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if ( !desk->GetVip()->AddUser(pUser) )
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk adduser fail, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	LLOG_INFO("RoomVip::AddToVipDesk userid=%d deskid=%d deskusercount=%d", pUser->GetUserDataId(), nDeskID, desk->GetUserCount());
	desk->OnUserInRoom(pUser);

	pUser->Send(ret);
	return ret.m_errorCode;
}


bool RoomVip::LeaveToVipDesk(LMsgC2SLeaveDesk* pMsg, User* pUser)
{
	if (!pMsg || !pUser)
	{
		return false;
	}
	LMsgS2CLeaveDesk send;
	send.m_errorCode = 0;

	Desk* desk = pUser->GetDesk();
	if(desk == NULL || desk->getDeskState() != DESK_WAIT)
	{
		LLOG_ERROR("RoomVip::LeaveToVipDesk fail, userid=%d deskid=%d deskstate=%d", pUser->GetUserDataId(), desk ? desk->GetDeskId() : 0, desk ? desk->getDeskState() : -1);
		send.m_errorCode = 1;
		pUser->Send(send);
		return false;
	}

	if(!desk->GetVip() || desk->GetVip()->IsBegin())
	{
		LLOG_ERROR("RoomVip::LeaveToVipDesk desk already begin, userid=%d deskid=%d", pUser->GetUserDataId(), desk->GetDeskId());
		send.m_errorCode = 1;
		pUser->Send(send);
		return false;
	}

	LLOG_INFO("RoomVip::LeaveToVipDesk userid=%d deskid=%d", pUser->GetUserDataId(), desk->GetDeskId());
	pUser->Send(send);

	//房主
	if(desk->GetVip()->GetOwerId() ==  pUser->GetUserDataId())
	{
		//房间保留，不做任何操作
		gWork.HanderUserLogout(pUser->getUserGateID() , pUser->m_userData.m_unioid);
	}
	else
	{
		desk->GetVip()->RemoveUser(pUser->GetUserDataId());
		desk->OnUserOutRoom(pUser);
	}
	return true;
}

void VipLogItem::AddLog(User** user, Lint* gold,Lint* winInfo, Lint zhuangPos,Lint* agang,Lint* mgang,Lint *bomb, Lstring& videId,Lint video_time, bool addCircle)
{
	if(!m_desk)
	{
		LLOG_ERROR("VipLogItem::AddLog have no desk");
		return;
	}
	VipDeskLog* log = new VipDeskLog();
	log->m_zhuangPos = zhuangPos;
	log->m_time = video_time;
	log->m_videoId = videId;
	for (Lint i = 0; i < m_desk->GetUserCount(); ++i)
	{
		log->m_gold[i] = gold[i];
		log->m_win[i] = winInfo[i];
		m_score[i] += gold[i];
		log->m_angang[i] += agang[i];
		log->m_mgang[i] += mgang[i];
		log->m_bomb[i] += bomb[i];
	}
	m_log.push_back(log);

// 	m_curCircle += 1;
// 	m_curZhuangPos = zhuangPos;
//
// 	if(m_curCircle == 1)
// 	{
// 		InsertToDb();
// 	}
// 	else
// 	{
// 		UpdateToDb();
// 	}

	++m_round;
	if(addCircle)
		m_curCircle += 1;
	m_curZhuangPos = zhuangPos;

	if(m_round == 1)
	{
		InsertToDb();
	}
	else
	{
		UpdateToDb();
	}

	if(!m_reset)
	{
		for (Lint i = 0; i < m_desk->GetUserCount(); ++i)
		{
			if (user[i] != NULL)
			{
				LMsgL2LMGUserCircleIncreased msg;
				msg.m_userid = user[i]->m_userData.m_id;
				gWork.SendToLogicManager(msg);
			}
		}
	}

	if (m_curCircle == m_maxCircle)
	{
		for (Lint i = 0; i <  m_desk->GetUserCount(); ++i)
		{
			if (user[i] != NULL)
			{
				user[i]->m_userData.m_totalplaynum ++;
				LMsgL2LMGModifyPlayCount msg;
				msg.m_userid = user[i]->m_userData.m_id;
				msg.m_curPlayCount = user[i]->m_userData.m_totalplaynum;
				gWork.SendToLogicManager(msg);
			}
		}

		for (Lint i = 0; i < m_desk->GetUserCount(); ++i)
		{
			if (user[i] != NULL)
			{
				LMsgL2LMGUserCompleteGame msg;
				msg.m_userId = user[i]->m_userData.m_id;
				msg.m_startTime = m_time;
				msg.m_totalCircleNum = m_maxCircle;
				msg.m_playerNum = m_desk->GetUserCount();
				for (int j = 0; j < m_desk->GetUserCount(); ++j)
				{
					if (user[j] != NULL)
						msg.m_players[j] = user[j]->m_userData.m_id;
					else
						msg.m_players[j] = -1;
				}
				gWork.SendToLogicManager(msg);
			}
		}
	}
	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);
}
