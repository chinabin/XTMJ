#include "UserMessage.h"
#include "LTime.h"
#include "LMemoryRecycle.h"
#include "Config.h"
#include "DbServerManager.h"
#include "LMsgS2S.h"
#include "Work.h"
#include "ActiveManager.h"

#define CREDIT_KEY_TTL_TIME    (3*24*60*60)   //2016-12-26 modify three days
#define CREDIT_HISTORY_LIMIT   (20)             //限制条数

CUserMessage::CUserMessage():m_dbsession(NULL)
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
	if(!this->Init())
	{
		LLOG_ERROR("DbServer init Failed!!!");
		return;
	}

	if(!this->_initRedisConnect())
	{
		LLOG_ERROR("_initRedisConnect Failed!!!");
		return;
	}

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
				LLOG_ERROR("User Logic: 10000 package needs %ld time", now.MSecs() - msgTime.MSecs() - iElapseTime);

				iMsgCount = 0;
				iElapseTime = 0;

				msgTime = now;
			}
		}
	}

	this->Final();
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
	LLOG_DEBUG("user msgid:%d", msg->m_msgId);

	switch(msg->m_msgId)
	{
	case MSG_L_2_LDB_VIDEO:
		HanderLogicReqVideo((LMsgL2LDBReqVideo*) msg);
		break;
	case MSG_L_2_LDB_VIP_LOG:
		HanderLogicReqVipLog((LMsgL2LBDReqVipLog*) msg);
		break;
	case MSG_L_2_LDB_CRE_LOG:
		HanderLogicReqCRELog((LMsgL2LBDReqCRELog*) msg);
		break;
	case MSG_L_2_LDB_ROOM_SAVE:
		HanderLogicReqSaveLog((LMsgL2LDBSaveLogItem*)msg);
		break;
	case MSG_L_2_LDB_VIDEO_SAVE:
		HanderLogicReqSaveVideo((LMsgL2LDBSaveVideo*)msg);
		break;
	case MSG_C_2_S_ROOM_LOG:
		HanderLogicReqRoomLog((LMsgC2SRoomLog*)msg);
		break;
	case MSG_L_2_LDB_ENDCRE_LOG:
		HanderLogicSaveCRELog((LMsgL2LDBSaveCRELog*)msg);
		break;
	case MSG_L_2_LDB_REQ_DELGTU:
		HanderLogicDelCRELog((LMsgL2LDBDEL_GTU*) msg);
		break;
	case MSG_LMG_2_LDB_USER_LOGIN:
		HanderUserLogin((LMsgLMG2LdbUserLogin*) msg);
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
		HanderActivityRequestShare((LMsgC2SActivityRequestShare *)msg);
		break;
	case MSG_C_2_S_SHARE:
		HanderUserShare((LMsgC2SShare *)msg);
		break;
	case MSG_L_2_LDB_TASK_INFO:
		HanderTaskInfo((LMsgL2LDBTaskInfo *)msg);
		break;
	case MSG_C_2_S_TASK_INFO:
		HanderUserTaskInfo((LMsgC2STaskInfo *)msg);
		break;
	case MSG_C_2_S_TASK_REWARD:
		HanderUserTaskReward((LMsgC2STaskReward *)msg);
		break;
	default:
		LLOG_DEBUG("CUserMessage::HanderMsg not handle, msgid=%d", msg->m_msgId);
		break;
	}
}

//logic请求玩家录像
void CUserMessage::HanderLogicReqVideo(LMsgL2LDBReqVideo* msg)
{
	LMsgLDB2LReqVideo log;
	log.m_strUUID = msg->m_strUUID;
	log.m_userid = msg->m_userId;
	log.m_count = 0;

	MYSQL* pMySQL = NULL;
	if(m_dbsession)
	{
		pMySQL = m_dbsession->GetMysql();
	}

	if (pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	LLOG_ERROR("Request video. %d:%s",msg->m_userId, msg->m_videoId.c_str());
	//log id
	std::stringstream ss;	
	ss << "SELECT Id,Time,UserId1,UserId2,UserId3,UserId4,Zhuang,DeskId,MaxCircle,CurCircle,Score1,Score2,Score3,Score4,Flag,Data, PlayType FROM video WHERE Id='";
	ss << msg->m_videoId<<"'";

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicReqVideo sql error %s", mysql_error(pMySQL));
		msg->m_sp->Send(log.GetSendBuff());
		return ;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row)
	{
		VideoLog& info = log.m_video;
		info.m_Id = *row++;
		info.m_time = atoi(*row++);

		info.m_userId[0] = atoi(*row++);
		info.m_userId[1] = atoi(*row++);
		info.m_userId[2] = atoi(*row++);
		info.m_userId[3] = atoi(*row++);

		info.m_zhuang = atoi(*row++);
		info.m_deskId = atoi(*row++);
		info.m_maxCircle = atoi(*row++);
		info.m_curCircle = atoi(*row++);

		info.m_score[0] = atoi(*row++);
		info.m_score[1] = atoi(*row++);
		info.m_score[2] = atoi(*row++);
		info.m_score[3] = atoi(*row++);

		info.m_flag = atoi(*row++);

		info.m_str = *row++;

		info.m_str_playtype = *row++;
		log.m_count = 1;
	}

	mysql_free_result(res);

	msg->m_sp->Send(log.GetSendBuff());
}

//logic请求玩家战绩
void CUserMessage::HanderLogicReqVipLog(LMsgL2LBDReqVipLog* msg)
{
	LMsgLBD2LReqVipLog log;
	log.m_strUUID= msg->m_strUUID;
	log.m_userid = msg->m_userId;
	log.m_count = 0;

	MYSQL* pMySQL = NULL;
	if(m_dbsession != NULL)
	{
		pMySQL = m_dbsession->GetMysql();
	}
	if(pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	LLOG_DEBUG("Request log. %d:%d",msg->m_userId,msg->m_reqUserId);

	LTime cur;
	//log id
	std::stringstream ss;	
	ss << "SELECT Id,Time,Pos1,Pos2,Pos3,Pos4,Flag,DeskId,MaxCircle,CurCircle,Pass,Score1,Score2,Score3,Score4,Reset,Data,PlayType FROM log WHERE (Pos1=";
	ss << msg->m_reqUserId << " OR Pos2=";
	ss << msg->m_reqUserId << " OR Pos3=";
	ss << msg->m_reqUserId << " OR Pos4=";
	ss << msg->m_reqUserId << " ) AND ";
	ss << "Time>" << gWork.GetCurTime().Secs()-24*60*60*3;

	ss << " ORDER BY Time DESC LIMIT 20";		//只返回10条 这样不用后面再去判断

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("Query sql error %s", mysql_error(pMySQL));
		msg->m_sp->Send(log.GetSendBuff());
		return ;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	while (row)
	{
		LogInfo& info = log.m_info[log.m_count++];
		info.m_id = *row++;
		info.m_time = atoi(*row++);
		info.m_posUserId[0] = atoi(*row++);
		info.m_posUserId[1] = atoi(*row++);
		info.m_posUserId[2] = atoi(*row++);
		info.m_posUserId[3] = atoi(*row++);
		info.m_flag = atoi(*row++);
		info.m_deskId = atoi(*row++);
		info.m_maxCircle = atoi(*row++);
		info.m_curCircle = atoi(*row++);
		info.m_secret = *row++;
		info.m_score[0] = atoi(*row++);
		info.m_score[1] = atoi(*row++);
		info.m_score[2] = atoi(*row++);
		info.m_score[3] = atoi(*row++);
		info.m_reset = atoi(*row++);
		info.m_data = *row++;
		info.m_playtype = *row++;

		row = mysql_fetch_row(res);
		if(log.m_count >= 20)   //每次返回10条
			break;
	}

	mysql_free_result(res);

	msg->m_sp->Send(log.GetSendBuff());
}

void CUserMessage::HanderLogicReqCRELog( LMsgL2LBDReqCRELog* msg )
{
	LLOG_DEBUG("CUserMessage::Request CRE log. %d ------",msg->m_userId);

	LMsgLDB2LM_RecordCRELog msgRet;
	msgRet.m_strUUID = msg->m_strUUID;
	msgRet.m_userId = msg->m_userId;

	char name[64] = {0};  
	sprintf(name, "CREDIT_USER_%d",msg->m_userId);

	Json::Reader reader(Json::Features::strictMode());
	Json::Value value;

	std::set<Lint> userList;

	std::list<Lstring> temp;
	if(m_RedisClient.readDataByList(std::string(name),0,30,temp))
	{
		m_RedisClient.setKeyTTLTime(std::string(name),CREDIT_KEY_TTL_TIME);    //Update TTL time of the key
		for(auto it = temp.begin(); it != temp.end(); ++it)
		{
			Lstring & strValue = *it;
			msgRet.m_count++;
			msgRet.m_record.push_back(strValue);

			try
			{
				if (reader.parse(strValue, value))
				{
					Lint len = value["User"].size();
					for(Lint i = 0; i < len; ++i)
					{
						Lint id = value["User"][i].asInt();
						userList.insert(id);
					}
				}
			}
			catch (...)
			{
				LLOG_ERROR("exception -- json parse error!!!");
			}
		}

		Lint index = 0;
		for(auto it = userList.begin(); it != userList.end(); ++it)
		{
			msgRet.m_user[index++] = *it;
			if(index >= LMsgLDB2LM_RecordCRELog::Length)
			{
				break;
			}
		}
	}
	else
	{
		LLOG_ERROR("readDataByList error!!!");
	}

	msg->m_sp->Send(msgRet.GetSendBuff());
}

void CUserMessage::HanderLogicDelCRELog( LMsgL2LDBDEL_GTU* msg )
{
	char name[64] = {0};  
	sprintf(name, "CREDIT_USER_%d",msg->m_userId);

	bool ret = m_RedisClient.rmListDataByValue(std::string(name),msg->m_strLog);
	if(ret)
	{
		LLOG_DEBUG("Request Del CRE log. ------succeed..");
	}
	else
	{
		LLOG_DEBUG("Request Del CRE log.  ------failed..");
	}
}

void CUserMessage::HanderLogicReqSaveLog( LMsgL2LDBSaveLogItem* msg )
{
	if(msg)
	{
		LLOG_DEBUG("Save log. %d:%s",msg->m_type,msg->m_sql.c_str());
		Excute(msg->m_sql);
	}
}

void CUserMessage::HanderLogicReqSaveVideo( LMsgL2LDBSaveVideo* msg )
{
	if(msg)
	{
		LLOG_DEBUG("Work::HanderLogincSaveVideo save %d:%s", msg->m_type, msg->m_sql.c_str());
		Excute(msg->m_sql);
	}

}

bool CUserMessage::Excute( const std::string& str )
{
	if( m_dbsession )
	{
		MYSQL* mysql = m_dbsession->GetMysql();
		if(mysql == NULL)
		{
			LLOG_ERROR("DbServer::Excute MYSQL NULL");
			return false;
		}
		if(mysql_real_query(mysql,str.c_str(),str.size()))
		{
			LLOG_ERROR("DbServer::Excute error = %s,sql = %s ", mysql_error(mysql),str.c_str());
			return false;
		}
		return true;
	}
	else
	{
		LLOG_ERROR("DbServer::Excute dbsession is NULL, sql = %s ",str.c_str());
	}
	return false;
}

bool CUserMessage::Init()
{
	if ( m_dbsession )
	{
		delete m_dbsession;
	}
	m_dbsession = new LDBSession;
	return m_dbsession->Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort());
}

bool CUserMessage::Final()
{
	if ( m_dbsession )
	{
		delete m_dbsession;
		m_dbsession = NULL;
	}
	return true;
}

void CUserMessage::HanderLogicReqRoomLog(LMsgC2SRoomLog* msg)
{
	if(NULL == msg)
	{
		return;
	}

	LLOG_DEBUG("Request room Log. userid = %d : deskid = %d : time = %d : pos = %d",msg->m_userId,msg->m_deskId,msg->m_time,msg->m_pos);
	
	LMsgLBD2LReqRoomLog log;
	log.m_strUUID= msg->m_strUUID;
	log.m_userid = msg->m_userId;

	MYSQL* pMySQL = NULL;
	if(m_dbsession)
	{
		pMySQL = m_dbsession->GetMysql();
	}
	if(pMySQL == NULL)
	{
		LLOG_ERROR("Don't get SQL session");
		msg->m_sp->Send(log.GetSendBuff());
		return;
	}

	std::stringstream ss;	
	ss << "SELECT Flag,Data FROM log WHERE (Time=";
	ss << msg->m_time << " AND Pos1=";
	ss << msg->m_pos << " AND DeskId=";
	ss << msg->m_deskId << " )";

	if (mysql_real_query(pMySQL, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("Query sql error %s", mysql_error(pMySQL));
		msg->m_sp->Send(log.GetSendBuff());
		return ;
	}

	MYSQL_RES* res = mysql_store_result(pMySQL);
	MYSQL_ROW row = mysql_fetch_row(res);
	if(row)
	{
		log.m_flag = atoi(*row++);
		log.m_data = *row++;
	}
	mysql_free_result(res);
	
	msg->m_sp->Send(log.GetSendBuff());
}

void CUserMessage::HanderLogicSaveCRELog( LMsgL2LDBSaveCRELog* msg )
{
	Json::Value root;
	root["DeskId"] = msg->m_deskID;
	root["LogId"] = msg->m_strLogId;
	root["Time"] = msg->m_time;
	for(Lint i = 0; i < 4; ++i)
	{
		root["User"].append(msg->m_id[i]);
	}

	std::string strValue = root.toStyledString();

	for(Lint j = 0; j < 4; ++j)
	{
		if(msg->m_id[j] <= 0)
			continue;

		char name[64] = {0};
		sprintf(name, "CREDIT_USER_%d",msg->m_id[j]);
		bool ret = m_RedisClient.writeDataByList(std::string(name),strValue,CREDIT_HISTORY_LIMIT);  ///如果超过20条，删除过时的记录
		if(ret)
		{
			m_RedisClient.setKeyTTLTime(std::string(name),CREDIT_KEY_TTL_TIME);
		}
		else
		{
			LLOG_DEBUG("HanderLogicSaveCRELog Failed !!!");
		}
	}
}

void CUserMessage::HanderUserLogin(LMsgLMG2LdbUserLogin* msg)
{
	// 活动信息
	LMsgS2CActivityInfo active;

	// 填充转盘信息
	if ( gActiveManager.CheckDrawActiveValid() )
	{
		LMsgS2CActivityInfo::Info draw;
		draw.m_id = gActiveManager.GetDrawActiveId();
		char time[16];
		sprintf( time, "%d", gActiveManager.GetDrawBeginTime() );
		draw.m_attri.push_back( LMsgS2CActivityInfo::Value() );
		draw.m_attri.back().m_key = "BeginTime";
		draw.m_attri.back().m_value = time;
		sprintf( time, "%d", gActiveManager.GetDrawEndTime() );
		draw.m_attri.push_back( LMsgS2CActivityInfo::Value() );
		draw.m_attri.back().m_key = "EndTime";
		draw.m_attri.back().m_value = time;
		active.m_activities.push_back(draw);
	}

	// 填充分享信息
	if(gActiveManager.CheckShareActiveStart())  //分享活动开启
	{
		LMsgS2CActivityInfo::Info share;
		share.m_id = gActiveManager.GetShareActiveId();
		share.m_info = gActiveManager.GetShareActiveSet();

		char name[64] = {0};
		sprintf(name, "ActivityShare_userid_%d",msg->m_userId);

		bool suc = false;
		int time = 0;
		bool ret = m_RedisClient.readDataByString(std::string(name),time);
		if(ret)
		{
			if(time != 0)
			{
				LTime lastTime;
				int curDay = lastTime.GetDate();
				lastTime.SetSecs(time);
				int lastDay = lastTime.GetDate();
				if(curDay != lastDay)
					suc = true;
			}
			else
			{
				suc = true;
			}
		}

		LMsgS2CActivityInfo::ATTRI attri;
		attri.m_key = "canshare";
		attri.m_value = "0";
		if(suc)
		{
			attri.m_value = "1";
		}

		share.m_attri.push_back(attri);
		active.m_activities.push_back(share);
	}
	
	gWork.SendToUser( msg->m_strUUID, active );
}

void CUserMessage::HanderActivityPhone(LMsgC2SActivityPhone *msg)
{
	// 保存手机号
	std::stringstream ss;	
	ss << "UPDATE activitylog SET Phone = '" << msg->m_PhoneNum << "' WHERE";
	ss << " AutoId = " << msg->m_AutoId;
	ss << " AND UserId = " << msg->m_userId;

	LMsgS2CActivityPhone send;
	if ( Excute( ss.str() ) )
	{
		send.m_errorCode = 0;
	}
	else
	{
		send.m_errorCode = 1;
	}
	gWork.SendToUser( msg->m_strUUID, send );
}

void CUserMessage::HanderActivityRequestLog(LMsgC2SActivityRequestLog *msg)
{
	LMsgS2CActivityReplyLog send;
	send.m_errorCode = -1;
	if ( msg->m_activeId == ActivityID_Draw )
	{
		// 从redis里查询
		std::list<std::string> logs;
		std::ostringstream str;
		str << ActivityDraw_Win_ << msg->m_userId;
		m_RedisClient.readDataByList( str.str(), 0, ActivityDraw_WinCount, logs );

		for ( auto it = logs.begin(); it != logs.end(); ++it )
		{
			Json::Reader reader(Json::Features::strictMode());
			Json::Value  value;
			if (!reader.parse(*it, value))
			{
				LLOG_ERROR("CUserMessage::HanderActivityRequestLog draw parsr json error %s", reader.getFormatedErrorMessages().c_str());
				continue;
			}
			send.m_logs.push_back(  ActivityLog() );
			ActivityLog& log = send.m_logs.back();
			log.AutoId = value["AutoId"].isInt() ? value["AutoId"].asInt() : 0;
			log.Id = value["Id"].isInt() ? value["Id"].asInt() : 0;
			log.SpendType = value["SpendType"].isInt() ? value["SpendType"].asInt() : 0;
			log.SpendCount = value["SpendCount"].isInt() ? value["SpendCount"].asInt() : 0;
			log.GiftName = value["GiftName"].isString() ? value["GiftName"].asString() : "";
			log.GiftType = value["GiftType"].isInt() ? value["GiftType"].asInt() : 0;
			log.GiftCount = value["GiftCount"].isInt() ? value["GiftCount"].asInt() : 0;
			log.Date = value["Date"].isInt() ? value["Date"].asInt() : 0;
			log.Phone = value["Phone"].isString() ? value["Phone"].asString() : "";
		}
	}
	else
	{
		// 从数据库里查询
		std::stringstream ss;
		ss << "SELECT AutoId, Id, SpendType, SpendCount, GiftName, GiftType, GiftCount, Date, Phone FROM activitylog WHERE";
		if ( msg->m_activeId != 0 )
		{
			ss << " Id = " << msg->m_activeId << " AND";
		}
		ss << " UserId = " << msg->m_userId << " LIMIT " << ActivityDraw_WinCount;

		MYSQL* m = m_dbsession->GetMysql();
		if ( m )
		{
			if (!mysql_real_query(m, ss.str().c_str(), ss.str().size()))
			{
				send.m_errorCode = 0;
				MYSQL_RES* res = mysql_store_result(m);
				if (res)
				{
					MYSQL_ROW row = mysql_fetch_row(res);
					int iCount = 0;
					while (row)
					{
						send.m_logs.push_back(  ActivityLog() );
						ActivityLog& log = send.m_logs.back();

						log.AutoId = atoi(*row++);
						log.Id = atoi(*row++);
						log.SpendType = atoi(*row++);
						log.SpendCount = atoi(*row++);
						log.GiftName = *row++;
						log.GiftType = atoi(*row++);
						log.GiftCount = atoi(*row++);
						log.Date = atoi(*row++);
						log.Phone = *row++;

						row = mysql_fetch_row(res);

						++iCount;
						if(iCount >= ActivityDraw_WinCount)
						{
							break;
						}
					}
					mysql_free_result(res);
				}
			}
		}
	}
	gWork.SendToUser( msg->m_strUUID, send );
}

void CUserMessage::HanderActivityRequestDrawOpen(LMsgC2SActivityRequestDrawOpen *msg)
{
	LMsgS2CActivityReplyDrawOpen active;
	// 读取中奖的玩家
	if ( gActiveManager.CheckDrawActiveValid() )
	{
		active.m_errorCode = 0;
		active.m_SpendType = gActiveManager.GetDrawSpendType();
		active.m_SpendCount = gActiveManager.GetDrawSpendCount();

		std::list<std::string> winusers;
		m_RedisClient.readDataByList( ActivityDraw_WinUsers, 0, ActivityDraw_WinUsersCount, winusers );

		for ( auto it = winusers.begin(); it != winusers.end(); ++it )
		{
			active.m_winUsers.push_back( ActivityDrawWinUser() );

			ActivityDrawWinUser& user = active.m_winUsers.back();
			size_t pos = it->find( "|" );
			if ( pos != std::string::npos )
			{
				user.m_userId = it->substr(0,pos);
				user.m_giftName = it->substr(pos+1);
			}
		}
	}
	else
	{
		active.m_errorCode = 1;
	}
	gWork.SendToUser( msg->m_strUUID, active );
}

void CUserMessage::HanderActivityRequestDraw(LMsgC2SActivityRequestDraw *msg)
{
	LMsgS2CActivityReplyDraw active;
	if ( !gActiveManager.CheckDrawActiveValid() )
	{
		active.m_errorCode = 2;
		gWork.SendToUser( msg->m_strUUID, active );
		return;
	}
	
	ActivityDrawGift fGift;
	ActivityDrawGift gift = gActiveManager.GetDrawGift( fGift );

	// 消耗
	Lint spendtype = gActiveManager.GetDrawSpendType();
	Lint spendcount = gActiveManager.GetDrawSpendCount();

	if ( spendtype == 1 && spendcount > msg->m_cardNum )
	{
		active.m_errorCode = 1;
	}
	else if ( gift.m_GiftIndex != -1 )
	{
		// 抽奖成功
		active.m_errorCode = 0;

		active.m_GiftIndex = gift.m_GiftIndex;
		active.m_GiftName = gift.m_GiftName;
		active.m_GiftType = gift.m_GiftType;
		active.m_GiftCount = gift.m_GiftCount;
		if ( spendtype == 1 )	// 房卡
		{
			LMsgL2LMGModifyCard modify;
			modify.admin = "system";
			modify.cardType = CARD_TYPE_Other;
			modify.cardNum = spendcount;
			modify.isAddCard = 0;
			modify.operType = CARDS_OPER_TYPE_ACTIVE_DRAW;
			modify.m_userid = msg->m_userId;
			modify.m_strUUID = msg->m_strUUID;
			gWork.SendToLogicManager( modify );
		}

		// 奖励
		if ( gift.m_GiftType == 1 )
		{
			LMsgL2LMGModifyCard modify;
			modify.admin = "system";
			modify.cardType = CARD_TYPE_Other;
			modify.cardNum = gift.m_GiftCount;
			modify.isAddCard = 1;
			modify.operType = CARDS_OPER_TYPE_ACTIVE;
			modify.m_userid = msg->m_userId;
			modify.m_strUUID = msg->m_strUUID;
			gWork.SendToLogicManager( modify );
		}

		// mysql记录中间信息
		LTime cur;
		MYSQL* m = m_dbsession->GetMysql();
		if ( m )
		{
			std::stringstream ss;	
			ss << "insert into activitylog(Id, UserId, SpendType, SpendCount, GiftName, GiftType, GiftCount, Date) values(";
			ss << gActiveManager.GetDrawActiveId() << ",";
			ss << msg->m_userId << ",";
			ss << spendtype << ",";
			ss << spendcount << ",";
			ss << "'" << gift.m_GiftName << "',";
			ss << gift.m_GiftType << ",";
			ss << gift.m_GiftCount << ",";
			ss << cur.Secs() << ")";

			if (mysql_real_query(m, ss.str().c_str(), ss.str().size()) == 0)
			{
				// 如果奖励的类型不是房卡 让玩家填写手机号
				if ( gift.m_GiftType != 1 )
				{
					active.m_NeedPhoneNum = true;
				}
				active.m_AutoId = (Lint)mysql_insert_id(m);
			}
		}

		// redis记录中奖信息
		{
			Json::Value root;   // 构建对象 
			root["AutoId"] = active.m_AutoId;
			root["Id"] = msg->m_userId;
			root["SpendType"] = spendtype;
			root["SpendCount"] = spendcount;
			root["GiftName"] = gift.m_GiftName;
			root["GiftType"] = gift.m_GiftType;
			root["GiftCount"] = gift.m_GiftCount;
			root["Date"] = cur.Secs();
			root["Phone"] = "";

			std::ostringstream str;
			str << ActivityDraw_Win_ << msg->m_userId;
			m_RedisClient.writeDataByList( str.str(), root.toStyledString(), ActivityDraw_WinCount );
			m_RedisClient.setKeyTTLTime( str.str(), gActiveManager.GetDrawEndTime()-cur.Secs() );
		}

		// 前WinUsersLogCount的中奖纪录
		{
			std::ostringstream str;
			str << "***";
			str << msg->m_userId%1000;
			str << "|";
			str << gift.m_GiftName;
			m_RedisClient.writeDataByList( ActivityDraw_WinUsers, str.str(), ActivityDraw_WinUsersCount );
			m_RedisClient.setKeyTTLTime( ActivityDraw_WinUsers, gActiveManager.GetDrawEndTime()-cur.Secs() );
		}

		// 假数据
		if ( fGift.m_GiftIndex != -1 )
		{
			std::ostringstream str;
			str << "***";
			str << L_Rand( 100, 999 );
			str << "|";
			str << fGift.m_GiftName;
			m_RedisClient.writeDataByList( ActivityDraw_WinUsers, str.str(), ActivityDraw_WinUsersCount );
		}
	}
	else
	{
		active.m_errorCode = -1;
	}

	gWork.SendToUser( msg->m_strUUID, active );
}

void CUserMessage::HanderActivityRequestShare( LMsgC2SActivityRequestShare *msg )
{
	LMsgS2CActivityFinishShare share;
	share.m_ErrorCode = -1;
	share.m_GiftCount = -1;
	share.m_GiftType = -1;

	if(!gActiveManager.CheckShareActiveStart())
	{
		//活动未开启，不在活动时间内
		gWork.SendToUser(msg->m_strUUID,share);
		return;
	}

	char name[64] = {0};
	sprintf(name, "ActivityShare_userid_%d",msg->m_userId);

	bool suc = false;
	int time = 0;
	bool ret = m_RedisClient.readDataByString(std::string(name),time);
	if(ret)
	{
		if(time != 0)
		{
			LTime lastTime;
			int curDay = lastTime.GetDate();
			lastTime.SetSecs(time);
			int lastDay = lastTime.GetDate();
			if(curDay != lastDay)
			{
				suc = true;
			}
			//else LLOG_DEBUG("HanderLogicShareActive user = %d last share time = %d",userId,time);
		}
		else
		{
			suc = true;
		}
	}

	share.m_ErrorCode = 0;
	share.m_GiftCount = 0;
	share.m_GiftType = 1;

	if(suc)
	{
		const ActivityShare set = gActiveManager.GetShareSet();

		if ( set.m_GiftType == 1 )	// 只有房卡
		{
			LMsgL2LMGModifyCard modify;
			modify.admin = "system";
			modify.cardType = CARD_TYPE_Other;
			modify.cardNum = set.m_GiftCount;
			modify.isAddCard = 1;
			modify.operType = CARDS_OPER_TYPE_ACTIVE;
			modify.m_userid = msg->m_userId;
			modify.m_strUUID = msg->m_strUUID;
			gWork.SendToLogicManager( modify );
		}

		share.m_ErrorCode = 0;  //成功
		share.m_GiftCount = set.m_GiftCount;  //数量
		share.m_GiftType = set.m_GiftType;   //房卡

		LTime cur;
		ret = m_RedisClient.writeDataByString(std::string(name),cur.Secs(),(24*60*60));
// 		if(ret)
// 		{
// 			m_RedisClient.setKeyTTLTime(std::string(name),(24*60*60));
// 		}
	}

	gWork.SendToUser(msg->m_strUUID,share);

	LLOG_DEBUG("HanderLogicShareActive user = %d last share card = %d",msg->m_userId,share.m_GiftCount);
}

bool CUserMessage::_initRedisConnect()
{
	return m_RedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort());
}

void CUserMessage::HanderUserShare(LMsgC2SShare* msg)
{
	char key[64] = {0};
	sprintf(key, "SHARE:%d",msg->m_userId);
	int value = 0;
	bool ret = m_RedisClient.readDataByString(std::string(key), value);

	LMsgS2CShare send;

	do{
		if (value == 1)
			break;

		//LTime cur;
		//Lint time = 86400 - (cur.Secs()%86400);

		m_RedisClient.writeDataByString(std::string(key), 1);//, time);  // 改为一次性的奖励

		LMsgL2LMGModifyCard modify;
		modify.admin = "system";
		modify.cardType = CARD_TYPE_DEFAULT;
		modify.cardNum = 3;
		modify.isAddCard = 1;
		modify.operType = CARDS_OPER_TYPE_SHARE;
		modify.m_userid = msg->m_userId;
		modify.m_strUUID = msg->m_strUUID;
		gWork.SendToLogicManager( modify );

		send.m_ErrorCode = 1;
	}while(false);

	gWork.SendToUser(msg->m_strUUID, send);
}

void CUserMessage::HanderTaskInfo(LMsgL2LDBTaskInfo* msg)
{
	LTime cur;
	Lint dailyTime = 86400 - (cur.Secs()%86400);

	//每日任务 创建房间
	if(msg->m_create == 1)
	{
		char key[64] = {0};
		sprintf(key, "task_daily:%d:%d", msg->m_userId[0], TaskType_CreateDesk);
		int add = 1;
		m_RedisClient.incrby(key, add);
		m_RedisClient.setKeyTimeOut(key, dailyTime);
	}
}

void CUserMessage::HanderUserTaskInfo(LMsgC2STaskInfo* msg)
{
	LLOG_DEBUG("HanderUserTaskInfo uid:%d", msg->m_userId);

	LMsgS2CTaskInfo send;

	gWork.GetAllGameTaskToMsgTask(send);

	// 任务所需各种数据
	std::set<int> allGet;		// 所有已经领取过奖励的任务 不管是每日任务还是其他的
	int counts[TaskType_Max][TaskTime_Max] = {0};

	// ---------------------获取各种数据--------------------------
	std::vector<int> taskIds;
	// 已经领取的每日任务
	{
		taskIds.clear();
		char key[64] = {0};
		sprintf(key, "task_daily_reward:%d", msg->m_userId);
		m_RedisClient.smembers(key, taskIds);

		allGet.insert(taskIds.begin(), taskIds.end());
	}

	// 各种任务次数
	{
		char key[64] = {0};
		sprintf(key, "task_daily:%d:%d", msg->m_userId, TaskType_CreateDesk);
		bool ret = m_RedisClient.readDataByString(std::string(key), counts[TaskType_CreateDesk][TaskTime_Day]);
	}
	//----------------------获取各种数据结束-----------------------------

	for(int i=0; i<send.m_tasks.size(); ++i)
	{
		MsgTask& task = send.m_tasks[i];
		task.m_curCount = counts[task.m_taskType][task.m_taskTime];

		auto it = allGet.find(task.m_taskId);
		if(it!=allGet.end())
			task.m_get = 1;
		else
			task.m_get = 0;
	}

	gWork.SendToUser(msg->m_strUUID, send);
}

void CUserMessage::HanderUserTaskReward(LMsgC2STaskReward* msg)
{
	LMsgS2CTaskReward send;
	send.m_taskId = msg->m_taskId;

	GameTask task = gWork.GetGameTask(msg->m_taskId);
	if(!task)
	{
		send.m_ErrorCode = 3;
		gWork.SendToUser(msg->m_strUUID, send);

		LLOG_ERROR("TaskReward no task, uid:%d taskid:%d", msg->m_userId, msg->m_taskId);
		return;
	}

	char rewardKey[64] = {0};
	char countKey[64] = {0};
	int expireTime = -1;			// -1 不过期

	LTime cur;

	switch(task.m_taskTime)
	{
	case TaskTime_Day:
		sprintf(rewardKey, "task_daily_reward:%d", msg->m_userId);
		sprintf(countKey, "task_daily:%d:%d", msg->m_userId, task.m_taskType);

		expireTime = 86400 - (cur.Secs()%86400);
		break;

	default:
		send.m_ErrorCode = 4;
		gWork.SendToUser(msg->m_strUUID, send);

		LLOG_ERROR("TaskReward server err1, uid:%d taskid:%d task.m_taskTime:%d", msg->m_userId, msg->m_taskId, task.m_taskTime);
		return;
	}

	int count = 0;
	bool ret = m_RedisClient.readDataByString(std::string(countKey), count);
	if(!ret)
	{
		send.m_ErrorCode = 4;
		gWork.SendToUser(msg->m_strUUID, send);

		LLOG_ERROR("TaskReward server err2, uid:%d taskid:%d", msg->m_userId, msg->m_taskId);
		return;
	}

	if (count < task.m_taskCount)
	{
		send.m_ErrorCode = 1;
		gWork.SendToUser(msg->m_strUUID, send);

		LLOG_ERROR("TaskReward count less, uid:%d taskid:%d %d/%d", msg->m_userId, msg->m_taskId, count, task.m_taskCount);
		return;
	}

	bool addResult = false;
	ret = m_RedisClient.sadd(rewardKey, task.m_taskId, addResult);
	if(expireTime >= 0)
	{
		m_RedisClient.setKeyTimeOut(rewardKey, expireTime);
	}
	if(!ret)
	{
		send.m_ErrorCode = 4;
		gWork.SendToUser(msg->m_strUUID, send);

		LLOG_ERROR("TaskReward server err3, uid:%d taskid:%d", msg->m_userId, msg->m_taskId);
		return;
	}

	if(!addResult)
	{
		send.m_ErrorCode = 2;
		gWork.SendToUser(msg->m_strUUID, send);

		LLOG_ERROR("TaskReward has been get, uid:%d taskid:%d", msg->m_userId, msg->m_taskId);
		return;
	}

	gWork.SendToUser(msg->m_strUUID, send);

	LMsgL2LMGModifyCard modify;
	modify.admin = "system";
	modify.cardType = CARD_TYPE_DEFAULT;
	modify.cardNum = task.m_cardCount;
	modify.isAddCard = 1;
	modify.operType = CARDS_OPER_TYPE_TASK;
	modify.m_userid = msg->m_userId;
	modify.m_strUUID = msg->m_strUUID;
	gWork.SendToLogicManager( modify );
}
