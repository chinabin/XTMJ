#include "LLog.h"
#include "NewUserVerify.h"
#include "Config.h"
#include "DUser.h"
#include "HttpRequest.h"
#include "UserMessageMsg.h"
#include "Config.h"

//////////////////////////////////////////////////////////////////////////

NewUserVerify::NewUserVerify()
	: m_dbsession(NULL),m_needWechatVerify(1)
{
	LTime cur;
	m_600SceTick = cur.MSecs();
	m_60SceTick = cur.MSecs();
}

bool NewUserVerify::Init()
{
	if ( m_dbsession )
	{
		delete m_dbsession;
	}
	m_dbsession = new LDBSession;
	return m_dbsession->Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort());
}

bool NewUserVerify::Final()
{
	if ( m_dbsession )
	{
		delete m_dbsession;
		m_dbsession = NULL;
	}
	return true;
}

void NewUserVerify::Clear()
{

}

void NewUserVerify::Run()
{
	if(!this->Init())
	{
		LLOG_ERROR("DbServer init Failed!!!");
		return;
	}
	
	while(!GetStop())
	{
		LTime cur;

		if(cur.MSecs() - m_tickTimer.MSecs() > 1)
		{
			m_tickTimer = cur;
			//60 second
			if (cur.MSecs() - m_60SceTick > 60*1000)
			{
				m_60SceTick = cur.MSecs();
				ClearOpenids( cur );
			}

			//5分钟循环一次
			if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
			{
				ReadConfig();
				m_600SceTick = cur.MSecs();
			}
		}

		LMsg* msg = (LMsg*)Pop();
		if(msg == NULL)
		{
			Sleep(1);
			continue;
		}
		else
		{
			HanderMsg(msg);
			delete msg;
		}
	}
	
	this->Final();
}

void NewUserVerify::HanderMsg(LMsg* msg)
{
	if (msg == NULL)
	{
		return;
	}
	//玩家请求登录
	switch(msg->m_msgId)
	{
	case MSG_NEWUSER_VERIFY_REQUEST:
		HanderNewUserVerifyRequest((LMsgNewUserVerifyRequest*)msg);
		break;
	}
}

void NewUserVerify::ClearOpenids( const LTime& cur )
{
	std::map<std::string,LTime>::iterator it = m_openids.begin();
	std::map<std::string,LTime>::iterator end = m_openids.end();
	while ( it != end )
	{
		if ( cur.MSecs() - it->second.MSecs() >= 15*60*1000 ) //15分钟
		{
			m_openids.erase( it++ );
		}
		else
		{
			++it;
		}
	}
}

void NewUserVerify::HanderNewUserVerifyRequest(LMsgNewUserVerifyRequest* msg)
{
	DUser* user = (DUser*)msg->m_pUser;
	if ( user == NULL )
	{
		return;
	}

	LMsgNewUserVerifyReply* reply = new LMsgNewUserVerifyReply;
	reply->m_pUser = msg->m_pUser;
	reply->m_gameloginSP = msg->m_gameloginSP;
	reply->m_severID = msg->m_severID;
	do
	{
		std::map<std::string,LTime>::iterator it = m_openids.find( msg->m_openId );
		if ( it != m_openids.end() )
		{
			// 存在加入的这个玩家直接退出 一般是客户端快速大量的发送这个消息导致
			reply->m_errorCode = 4;
			reply->m_errorMsg = "login too frequently";
			LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest login too frequently");
			break;
		}

		LTime cur;
		m_openids[msg->m_openId] = cur;

		if ( msg->m_bWechatLogin && m_needWechatVerify)
		{
			std::stringstream ss;
			ss << "https://api.weixin.qq.com/sns/userinfo?access_token=";
			ss << msg->m_accessToken << "&openid=";
			ss << msg->m_openId;

			Lstring userinfoRet;
			if(!HttpRequest::Instance().HttpGet(ss.str(),userinfoRet,true))
			{
				LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest get userinfo error %s",ss.str().c_str());
				reply->m_errorCode = 2;
				reply->m_errorMsg = "wechat get userinfo error";
				break;
			}

			// 读取结果值
			Json::Reader reader;
			Json::Value value;
			if (!reader.parse(userinfoRet, value))
			{
				reply->m_errorCode = 2;
				LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest parsr json  error %s", userinfoRet.c_str());
				reply->m_errorMsg = "wechat get userinfo error";
				break;
			}

			// 含有errcode 表示错误了
			if(!value["errcode"].isNull())
			{
				//微信登录返回失败
				reply->m_errorCode = value["errcode"].isInt() ? value["errcode"].asInt() : 3;
				reply->m_errorMsg = "wechat verify fail";
				LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest wechat error=%d", reply->m_errorCode);
				return;
			}

			// 验证下uuid
			Lstring uuid = value["unionid"].isString() ? value["unionid"].asString() : "";
			if ( uuid != user->m_usert.m_unioid )
			{
				reply->m_errorCode = 3;
				reply->m_errorMsg = "wechat verify fail";
				LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest uuid error, %s:%s", uuid.c_str(), user->m_usert.m_unioid.c_str());
				break;
			}
		}

		// 数据库创建玩家
		reply->m_errorCode = 4;
		reply->m_errorMsg = "fail";
		if ( !m_dbsession )
		{
			LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest DBSession null");
			break;
		}

		MYSQL* m = m_dbsession->GetMysql();
		if ( !m )
		{
			LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest mysql null");
			break;
		}

		//插入数据库
		std::stringstream ss;
		ss << "INSERT INTO user (OpenId,Nike,Sex,Provice,City,Country,HeadImageUrl,UnionId,NumsCard1,NumsCard2,NumsCard3,LastLoginTime,RegTime,New,Gm,TotalCardNum,TotalPlayNum,status,RegIP) VALUES (";
		ss << "'" << user->m_usert.m_openid << "',";
		ss << "'" << user->m_usert.m_nike << "',";
		ss << "'" << user->m_usert.m_sex << "',";
		ss << "'" << user->m_usert.m_provice << "',";
		ss << "'" << user->m_usert.m_city << "',";
		ss << "'" << user->m_usert.m_country << "',";
		ss << "'" << user->m_usert.m_headImageUrl << "',";
		ss << "'" << user->m_usert.m_unioid << "',";
		ss << "'" << user->m_usert.m_numOfCard1s << "',";
		ss << "'" << user->m_usert.m_numOfCard2s << "',";
		ss << "'" << user->m_usert.m_numOfCard3s << "',";
		ss << "'" << user->m_usert.m_lastLoginTime<< "',";
		ss << "'" << user->m_usert.m_regTime << "',";
		ss << "'" << user->m_usert.m_new << "',";
		ss << "'" << user->m_usert.m_gm << "',";
		ss << "'" << user->m_usert.m_totalbuynum << "',";
		ss << "'" << user->m_usert.m_totalplaynum << "',";
		ss << "'" << user->m_usert.m_status << "',";
		ss << "'" << msg->m_clientIP << "'";
		ss << ")";

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()) != 0)
		{
			LLOG_ERROR("NewUserVerify::HanderNewUserVerifyRequest sql error %s:%s", ss.str().c_str(),mysql_error(m));
			break;
		}
		user->m_usert.m_id = (Lint)mysql_insert_id(m);
		reply->m_errorCode = 0;
		reply->m_errorMsg = "";
	}
	while ( false );

	if ( reply->m_errorCode != 0 )
	{
		// 如果没有处理正确 先去掉这个记录
		m_openids.erase( msg->m_openId );
	}
	gUserMessageMsg.handlerMessage(user->m_usert.m_unioid, reply);
}

void NewUserVerify::ReadConfig()
{
	LIniConfig	m_ini;
	m_ini.LoadFile("center_config.ini");
	m_needWechatVerify = m_ini.GetShort("NeedWechat",1);
}
