#include "UserMessage.h"
#include "LTime.h"
#include "LMemoryRecycle.h"
#include "Config.h"
#include "Work.h"
#include "UserManager.h"
#include "NewUserVerifyManager.h"
#include "DbServerManager.h"

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
	if(!m_RedisClient.initRedis(gConfig.GetRedisIp(), gConfig.GetRedisPort()))
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
			//Tick(cur);
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
	switch(msg->m_msgId)
	{
	case MSG_FROM_LOGINSERVER:
		HanderFromLoginServer((LMsgFromLoginServer*)msg);
		break;
	case MSG_NEWUSER_VERIFY_REPLY:
		HanderNewUserVerifyReply((LMsgNewUserVerifyReply*)msg);
		break;
	case MSG_L_2_CE_MODIFY_USER_NEW:
		HanderModifyUserNew((LMsgL2CeModifyUserNew*) msg);
		break;
	case MSG_L_2_CE_USER_LOGIN:
		HanderLogicUserLogin((LMsgL2CeUserServerLogin*) msg);
		break;
	case MSG_L_2_CE_USER_LOGOUT:
		HanderLogicUserLogout((LMsgL2CeUserServerLogout*) msg);
		break;
	case MSG_L_2_LMG_MODIFY_USER_CARD:
		HanderModifyCard((LMsgL2LMGModifyCard*) msg);
		break;
	case MSG_L_2_LMG_ADD_USER_PLAYCOUNT:
		HanderAddUserPlayCount((LMsgL2LMGAddUserPlayCount*) msg);
		break;
	case MSG_CN_2_LMG_MODIFY_USER_COINS:
		HanderModifyUserCoinsNum((LMsgCN2LMGModifyUserCoins*) msg);
		break;
	case MSG_C_2_C_ADD_CRE:
		HanderModifyUserCreditValue((LMsgC2C_ADD_CRE*) msg);
		break;
	case MSG_LMG_2_L_EXCH_CARD:
		HanderExchangeCard((LMsgL2LMGExchCard*) msg);
		break;
	case MSG_LM_2_CE_ADD_AGENT:
		HanderAddAgent((LMsgLMG2CEAddAgent*)msg);
		break;
	default:
		break;
	}
}

void CUserMessage::HanderFromLoginServer(LMsgFromLoginServer* msg)
{
	if(msg == NULL || msg->m_userMsgId != MSG_C_2_S_MSG)
	{
		return;
	}

	//HanderUserLogin((LMsgC2SMsg*)msg->m_userMsg, msg->m_sp);
	HanderUserLogin(msg, msg->m_sp);
}

//处理玩家登陆的消息 
void CUserMessage::HanderUserLogin(LMsgFromLoginServer* _msg, LSocketPtr gameloginSP)
{
	LMsgC2SMsg* msg = (LMsgC2SMsg*)_msg->m_userMsg;
	if (msg == NULL)
	{
		return;
	}
	if(msg->m_plate == "wechat")
	{
		HanderUserWechatLogin(_msg, gameloginSP);
	}
	else if(gConfig.GetLocalLogin()=="true" && msg->m_plate == "local")
	{
		HanderUserLocalLogin(_msg, gameloginSP);
	}
	else
	{
		LLOG_ERROR("CUserMessage::HanderUserLogin plate error %s",msg->m_plate.c_str());
	}
}

void CUserMessage::HanderUserWechatLogin(LMsgFromLoginServer* _msg, LSocketPtr gameloginSP)
{
	LMsgC2SMsg* msg = (LMsgC2SMsg*)_msg->m_userMsg;
	if(msg == NULL)
	{
		return;
	}
	Lint errorCode = 1;
	std::string errorMsg;
	do 
	{
		if(!gWork.isLogicManagerExist(msg->m_severID))
		{
			LLOG_ERROR("CUserMessage::HanderUserWechatLogin logicid not exist %d",msg->m_severID);
			errorCode = 6;
			errorMsg = "logicid not exist";
			break;
		}

		if(msg->m_uuid.empty() || msg->m_uuid.size()>32)
		{
			LLOG_ERROR("CUserMessage::HanderUserWechatLogin uuid error, uuid=%s", msg->m_uuid.c_str());
			errorCode = 7;
			errorMsg = "uuid error";
			break;
		}
		boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_uuid);
		boost::shared_ptr<DUser> user;
		//长沙麻将openid 转 uuid 的方案
		if(safeUser.get() == NULL)
		{
			safeUser = gUserManager.getUser(msg->m_openId);
			if(safeUser)
			{
				user = safeUser->getUser();
				user->m_usert.m_unioid = msg->m_uuid;

				gUserManager.delUser(msg->m_openId);
				gUserManager.addUser(user);

				//更新数据库
				std::stringstream ss;
				ss << "UPDATE User SET ";
				ss << "UnionId='"<<  user->m_usert.m_unioid<<"' WHERE UnionId='";
				ss << msg->m_openId << "'";

				LLOG_ERROR("CUserMessage::HanderUserWechatLogin sql =%s", ss.str().c_str());

				Lstring* sql = new Lstring(ss.str());
				gDbServerManager.Push(sql, user->m_usert.m_id);
			}
		}
		else
		{
			user = safeUser->getUser();
		}

		//if(user && m_tickTimer.Secs() - user->m_usert.m_lastLoginTime < 24*60*60)
		if (user.get() == NULL)
		{
			//处理性别值异常
			if (msg->m_sex < 1 || msg->m_sex > 2)
			{
				msg->m_sex = 2;
			}

			//昵称中的单引号处理
			for(Lsize i = 0 ; i < msg->m_nikename.size(); ++i)
			{
				if(msg->m_nikename[i] == '\'')
					msg->m_nikename[i] = ' ';
			}

			DUser* newUser = new DUser;
			newUser->m_usert.m_openid = msg->m_openId;
			newUser->m_usert.m_unioid = msg->m_uuid;
			newUser->m_usert.m_nike = msg->m_nikename;
			newUser->m_usert.m_sex = msg->m_sex;
			newUser->m_usert.m_headImageUrl = msg->m_imageUrl;
			newUser->m_usert.m_regTime = m_tickTimer.Secs();
			newUser->m_usert.m_lastLoginTime = newUser->m_usert.m_regTime;//m_tickTimer.Secs();有时候产生1秒误差
			newUser->m_loginIP = _msg->m_clientIP;
			newUser->m_usert.m_status = 1;

			// 验证新用户登陆
			LMsgNewUserVerifyRequest* req = new LMsgNewUserVerifyRequest;
			req->m_pUser = newUser;
			req->m_gameloginSP = gameloginSP;
			req->m_severID = msg->m_severID;
			req->m_bWechatLogin = true;
			req->m_openId = msg->m_openId;
			req->m_accessToken = msg->m_accessToken;
			req->m_clientIP = _msg->m_clientIP;
			gDbNewUserManager.Push( msg->m_uuid, req );
			return;	// 直接返回
		}
		else
		{
			user->m_loginIP = _msg->m_clientIP;						// 记录本次登陆IP

			//昵称中的单引号处理
			for(Lsize i = 0 ; i < msg->m_nikename.size(); ++i)
			{
				if(msg->m_nikename[i] == '\'')
					msg->m_nikename[i] = ' ';
			}

			if(user->m_usert.m_nike != msg->m_nike ||                 //昵称
				user->m_usert.m_sex != msg->m_sex ||                  //性别
				user->m_usert.m_headImageUrl != msg->m_imageUrl)      //头像
			{
				user->m_usert.m_nike = msg->m_nikename;
				user->m_usert.m_sex = msg->m_sex;
				user->m_usert.m_headImageUrl = msg->m_imageUrl;
				user->m_usert.m_unioid = msg->m_uuid;

				UpdateUserInfo(user->m_usert);
			}
		}

		errorCode = 0;
		errorMsg = "";

	} while(false);


	SendUserLogin( msg->m_uuid, gameloginSP, msg->m_severID, errorCode, errorMsg );
}

void CUserMessage::HanderUserLocalLogin(LMsgFromLoginServer* _msg, LSocketPtr gameloginSP)
{
	LMsgC2SMsg* msg = (LMsgC2SMsg*)_msg->m_userMsg;
	if(msg == NULL)
	{
		return;
	}

	int errorCode = 1;
	std::string errorMsg;
	do 
	{
		msg->m_uuid = msg->m_openId;
		if(!gWork.isLogicManagerExist(msg->m_severID))
		{
			LLOG_ERROR("CUserMessage::HanderUserLocalLogin logicid not exist %d",msg->m_severID);
			errorCode = 6;
			errorMsg = "logicid not exist";
			break;
		}

		if (msg->m_uuid.empty() || msg->m_uuid.size() > 32)
		{
			LLOG_ERROR("CUserMessage::HanderUserLocalLogin uuid error, uuid=%s", msg->m_uuid.c_str());
			errorCode = 2;
			errorMsg = "uuid error";
			break;
		}

		boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_uuid);
		boost::shared_ptr<DUser> user;

		if(safeUser.get() == NULL)
		{
			DUser* newUser = new DUser;
			newUser->m_usert.m_openid = msg->m_openId;
			newUser->m_usert.m_unioid = msg->m_uuid;
			//newUser->m_usert.m_nike = msg->m_nike;
			newUser->m_usert.m_nike = msg->m_nikename;
			newUser->m_usert.m_sex = msg->m_sex;
			newUser->m_usert.m_headImageUrl = msg->m_imageUrl;
			newUser->m_usert.m_regTime = m_tickTimer.Secs();
			newUser->m_usert.m_lastLoginTime = newUser->m_usert.m_regTime;//m_tickTimer.Secs();有时候产生1秒误差
			newUser->m_loginIP = _msg->m_clientIP;
			newUser->m_usert.m_status = 1;

			// 验证新用户登陆
			LMsgNewUserVerifyRequest* req = new LMsgNewUserVerifyRequest;
			req->m_pUser			= newUser;
			req->m_gameloginSP	= gameloginSP;
			req->m_severID		= msg->m_severID;
			req->m_openId		= msg->m_openId;
			req->m_clientIP		= _msg->m_clientIP;
			gDbNewUserManager.Push( msg->m_uuid, req );

			return;	// 直接返回
		}
		else
		{
			user = safeUser->getUser();

			if(user.get() != NULL)
			{
				user->m_loginIP = _msg->m_clientIP;						// 记录本次登陆IP
			}
		}

		errorCode = 0;
		errorMsg = "";

	}while(false);


	SendUserLogin( msg->m_uuid, gameloginSP, msg->m_severID, errorCode, errorMsg );
}

void CUserMessage::HanderNewUserVerifyReply(LMsgNewUserVerifyReply* msg)
{
	if(!msg)		return;
	DUser* user = (DUser*)msg->m_pUser;
	if (!user)		return;

	boost::shared_ptr<DUser> newUser(user);
	LMsgS2CMsg ret;
	ret.m_errorCode = msg->m_errorCode;
	if ( msg->m_errorCode == 0 )
	{
		gUserManager.addUser( newUser );
	}

	SendUserLogin( user->m_usert.m_unioid, msg->m_gameloginSP, msg->m_severID, msg->m_errorCode, msg->m_errorMsg );
}

void CUserMessage::SendUserLogin( const Lstring& uuid, LSocketPtr gameloginSP, Lint severID, Lint errorCode, const Lstring& errorMsg )
{
	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(uuid);
	boost::shared_ptr<DUser> user;
	if ( safeUser )
	{
		user = safeUser->getUser();
	}
	LMsgS2CMsg ret;
	ret.m_errorCode = errorCode;
	ret.m_errorMsg = errorMsg;
	while ( user && errorCode == 0 )
	{
		if(user->m_usert.m_status == 2)
		{
			LLOG_ERROR("CUserMessage::SendUserLogin freeze account:%d", user->m_usert.m_id);
			ret.m_errorCode = 8;
			ret.m_errorMsg = "账号被封";
			break;
		}else if(user->m_usert.m_status == 0)
		{
			LLOG_ERROR("CUserMessage::SendUserLogin delete account:%d", user->m_usert.m_id);
			ret.m_errorCode = 8;
			ret.m_errorMsg = "账号已经删除";
			break;
		}

		if (user->m_logicID != severID && user->m_logicID != 0)
		{
			//不是在本服务器登陆的玩家, 发送消息，提示退出原APP
			LLOG_ERROR("CUserMessage::SendUserLogin logicid not empty %d : %d", severID, user->m_logicID);
			ret.m_errorCode = 5;
			ret.m_errorMsg = string_To_UTF8(gWork.getServerNameByID(user->m_logicID));
			break;
		}
		user->m_lastReqTime = m_tickTimer.Secs();
		LUser tmpUser = gUserManager.getUserInfoById(user->m_usert.m_id);
		user->m_usert.m_numOfCard2s = tmpUser.m_numOfCard2s;
		LMsgCe2LUserLogin send;
		send.m_seed = L_Rand(10000000, 99999999);
		send.user = user->m_usert;
		gWork.SendMsgToLogic(send, severID);

		GateInfo gateInfo = gWork.GetGateUsefulInfo(severID);
		ret.m_gateIp = gateInfo.m_ip;
		ret.m_gatePort = gateInfo.m_port;
		if ( gateInfo.m_id == 0 && gateInfo.m_ip.empty() )
		{
			LLOG_ERROR("CUserMessage::SendUserLogin not find gate");
		}

		ret.m_id = user->m_usert.m_id;
		ret.m_seed = send.m_seed;
		ret.m_totalPlayNum = user->m_usert.m_totalplaynum;

		break; // 直接退出
	}

	if ( gameloginSP )
	{
		LMsgToLoginServer retMsg;
		retMsg.m_strUUID = uuid;
		retMsg.m_dataBuff= ret.GetSendBuff();
		gameloginSP->Send(retMsg.GetSendBuff());
	}
}

void CUserMessage::HanderModifyUserNew(LMsgL2CeModifyUserNew* msg)
{
	if (msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_strUUID);
	if(safeUser && safeUser->getUser())
	{
		safeUser->getUser()->ModifyNew( msg->m_value );
	}
	else
	{
		LLOG_ERROR("CUserMessage::HanderModifyUserNew user not exiest, userid=%s",  msg->m_strUUID.c_str());
	}
}

//logicmanage发送玩家已经登录的信息
void CUserMessage::HanderLogicUserLogin(LMsgL2CeUserServerLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}

	LLOG_DEBUG("CUserMessage::HanderLogicUserLogin, userID: %s, ServerID %d", msg->m_openID.c_str(), msg->m_serverID);

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_openID);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		//没找到玩家
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();
	if(!gWork.isLogicManagerExist(msg->m_serverID))
	{
		user->m_logicID = 0;
		return;
	}

	user->m_logicID = msg->m_serverID;
	user->m_usert.m_lastLoginTime = m_tickTimer.Secs();
	LUser tmpUser = gUserManager.getUserInfoById(user->m_usert.m_id);
	user->m_usert.m_numOfCard2s = tmpUser.m_numOfCard2s;
	gUserManager.SaveUserLastLogin(user.get());		//玩家最后登陆时间
}

void CUserMessage::HanderLogicUserLogout(LMsgL2CeUserServerLogout* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("CUserMessage::HanderLogicUserLogot, userID: %s, ServerID %d", msg->m_openID.c_str(), msg->m_serverID);
	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_openID);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		//没找到玩家
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();
	if(!gWork.isLogicManagerExist(msg->m_serverID))
	{
		user->m_logicID = 0;
		return;
	}

	user->m_logicID = 0;
	LUser tmpUser = gUserManager.getUserInfoById(user->m_usert.m_id);
	user->m_usert.m_numOfCard2s = tmpUser.m_numOfCard2s;
	gUserManager.SaveUser(user.get());		//玩家离线，保存玩家数据
}

void CUserMessage::HanderModifyCard(LMsgL2LMGModifyCard* msg)
{
	if(!msg)
	{
		return;
	}

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("CUserMessage::HanderModifyCard user not exiest, userid=%d, type=%d, count=%d, operType=%d",  msg->m_userid, msg->cardType, msg->cardNum, msg->operType);
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	if (msg->isAddCard)
	{
		user->AddCardCount(msg->cardType, msg->cardNum, msg->operType, msg->admin);
	}
	else
	{
		user->DelCardCount(msg->cardType, msg->cardNum, msg->operType, msg->admin);
	}
}

void CUserMessage::HanderAddUserPlayCount(LMsgL2LMGAddUserPlayCount* msg)
{
	if(msg == NULL)
	{
		return;
	}

	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("CUserMessage::HanderModifyPlayCount user not exiest, userid=%d",  msg->m_userid);
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();
	user->AddPlayCount();

	// 记录每周和每月的次数，排行榜用
	LTime cur;
	char key[128] = {0};
	//每月
	//sprintf_s(key, 128, "month_rank_%4d%2d", cur.GetYear(), cur.GetMonth());
	//m_RedisClient.zincrby(key, 1, user->m_usert.m_id);

	//每周
	sprintf_s(key, 128, "week_rank_%d", cur.Secs()/RANK_REWARD_INTERVAL);
	m_RedisClient.zincrby(key, 1, user->m_usert.m_id);
}

void CUserMessage::HanderModifyUserCoinsNum(LMsgCN2LMGModifyUserCoins* msg)
{
	if(msg == NULL)
	{
		return;
	}
	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		LLOG_ERROR("CUserMessage::HanderModifyUserCoinsNum user not exiest, userid=%d, count=%d, operType=%d",  msg->m_userid, msg->coinsNum, msg->operType);
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	if(msg->isAddCoins)
	{
		user->AddCoinsCount(msg->coinsNum, msg->operType);
	}
	else
	{
		user->DelCoinsCount(msg->coinsNum, msg->operType);
	}
}

void CUserMessage::HanderModifyUserCreditValue( LMsgC2C_ADD_CRE* msg )
{
	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userId);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();
	user->AddCreditValue();
}

void CUserMessage::HanderExchangeCard( LMsgL2LMGExchCard* msg )
{
	boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userid);
	if(safeUser.get() == NULL || !safeUser->isValid())
	{
		return;
	}

	boost::shared_ptr<DUser> user = safeUser->getUser();

	user->ExchangeCardCoin(msg->m_add,msg->m_del,msg->operType,msg->cardType,msg->admin);
}

void CUserMessage::HanderAddAgent(LMsgLMG2CEAddAgent* msg)
{
	LMsgCE2LMGAddAgent sendMsg;
	sendMsg.m_agentId = msg->m_agentId;
	sendMsg.m_userid = msg->m_userid;
	sendMsg.m_strUUID = msg->m_strUUID;

	do{
		boost::shared_ptr<CSafeUser> safeUser = gUserManager.getUser(msg->m_userid);
		if(safeUser.get() == NULL || !safeUser->isValid())
		{
			sendMsg.m_result = ErrorCode::UserNotExist;
			break;
		}

		if(!gWork.IsExistAgent(msg->m_agentId))
		{
			sendMsg.m_result = ErrorCode::NoAgentPower;
			break;
		}

		boost::shared_ptr<DUser> user = safeUser->getUser();
		user->m_usert.m_agentId = msg->m_agentId;

		std::stringstream ss;
		ss << "UPDATE user SET AgentId='"<< user->m_usert.m_agentId << "' WHERE Id='";
		ss << user->m_usert.m_id << "'";

		Lstring* sql = new Lstring(ss.str());

		gDbServerManager.Push(sql, user->m_usert.m_id);
	}while(0);

	msg->m_sp->Send(sendMsg.GetSendBuff());
}

void CUserMessage::UpdateUserInfo(const LUser& usert)
{
	//查询数据库
	std::stringstream ss;
	ss << "UPDATE user SET ";
	ss << "Nike='"<<  usert.m_nike <<"',";
	ss << "Sex='"<<  usert.m_sex <<"',";
	ss << "Provice='"<<  usert.m_provice <<"',";
	ss << "City='"<<  usert.m_city <<"',";
	ss << "Country='"<<  usert.m_country<<"',";
	ss << "HeadImageUrl='"<<  usert.m_headImageUrl<< "' WHERE UnionId='";
	ss << usert.m_unioid << "'";

	Lstring* sql = new Lstring(ss.str());

	gDbServerManager.Push(sql,usert.m_id);
}

