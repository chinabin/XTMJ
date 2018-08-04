#ifndef _LMSG_S2S_H_
#define _LMSG_S2S_H_

#include "LMsg.h"
#include "LUser.h"
#include "LCharge.h"



//////////////////////////////////////////////////////////////////////////
//logic 跟 center 

//////////////////////////////////////////////////////////////////////////
struct LMsgL2CeLogin:public LMsg
{
	Lint	m_ID;//类型
	Lstring m_key;
	Lstring m_ServerName;
	Lint	m_needLoadUserInfo;  //center是否需要发送当前所有的玩家的信息 0:不发送 1：发送

	LMsgL2CeLogin() :LMsg(MSG_L_2_CE_LOGIN)
	{
		m_ID = 0;
		m_needLoadUserInfo = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_key);
		buff.Read(m_ServerName);
		buff.Read(m_needLoadUserInfo);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_key);
		buff.Write(m_ServerName);
		buff.Write(m_needLoadUserInfo);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeLogin();
	}
};


//////////////////////////////////////////////////////////////////////////

struct LogicInfo
{
	Lint		m_id;
	Lint		m_deskCount;
	Lstring		m_ip;
	Lshort		m_port;
	LSocketPtr	m_sp;
	Lint		m_flag;	//0,normal; -1,failover
	LogicInfo()
	{
		m_id = 0;
		m_deskCount = 0;
		m_port = 0;
		m_flag = 0;
	}
};

struct LMsgLMG2GateLogicInfo :public LMsg
{
	Lint		m_ID;
	Lint		m_count;
	LogicInfo	m_logic[256];

	LMsgLMG2GateLogicInfo() :LMsg(MSG_LMG_2_G_SYNC_LOGIC)
	{
		m_ID = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_count);
		for (Lint i = 0; i < m_count && i < 256; ++i)
		{
			buff.Read(m_logic[i].m_id);
			buff.Read(m_logic[i].m_deskCount);
			buff.Read(m_logic[i].m_ip);
			buff.Read(m_logic[i].m_port);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_count);
		for (Lint i = 0; i < m_count && i < 256; ++i)
		{
			buff.Write(m_logic[i].m_id);
			buff.Write(m_logic[i].m_deskCount);
			buff.Write(m_logic[i].m_ip);
			buff.Write(m_logic[i].m_port);
		} 
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2GateLogicInfo();
	}
};

struct GateInfo
{
	Lint		m_id;
	Lint		m_userCount;
	Lstring		m_ip;
	Lshort		m_port;
	LSocketPtr	m_sp;

	GateInfo()
	{
		m_id = 0;
		m_userCount = 0;
		m_port = 0;
	}
};

struct LMsgL2CeGateInfo :public LMsg
{
	Lint		m_ID;
	Lint		m_count;
	GateInfo	m_gate[256];

	LMsgL2CeGateInfo() :LMsg(MSG_L_2_CE_GATE_INFO)
	{
		m_ID = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_count);
		for (Lint i = 0; i < m_count && i < 256; ++i)
		{
			buff.Read(m_gate[i].m_id);
			buff.Read(m_gate[i].m_userCount);
			buff.Read(m_gate[i].m_ip);
			buff.Read(m_gate[i].m_port);
		}
		
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_count);
		for (Lint i = 0; i < m_count && i < 256; ++i)
		{
			buff.Write(m_gate[i].m_id);
			buff.Write(m_gate[i].m_userCount);
			buff.Write(m_gate[i].m_ip);
			buff.Write(m_gate[i].m_port);
		} 
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeGateInfo();
	}
};


//////////////////////////////////////////////////////////////////////////
struct LMsgCe2LUserLogin:public LMsg
{
	Lint		m_seed;
	LUser		user;

	LMsgCe2LUserLogin() :LMsg(MSG_CE_2_L_USER_LOGIN)
	{
		m_seed = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_seed);
		user.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_seed);
		user.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LUserLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//center发送玩家id信息到

//玩家基本信息
struct UserIdInfo
{
	Lint		m_id;
	Lstring		m_openId;
	Lstring		m_nike;
	Lstring		m_headImageUrl;
	Lint		m_sex;
	Lint		m_agentPower;			//	代理权

	UserIdInfo()
	{
		m_id = 0;
		m_sex = 1;
		m_agentPower = 0;
	}
};

struct LMsgCe2LUserIdInfo:public LMsg
{
	Lint	m_count;
	std::vector<UserIdInfo> m_info;
	Lint	m_hasSentAll; //已经发送所有了的id。 0：没有 1：已经发送所有
	
	LMsgCe2LUserIdInfo() :LMsg(MSG_CE_2_L_USER_ID_INFO)
	{
		m_count = 0;
		m_hasSentAll = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			UserIdInfo info;
			buff.Read(info.m_id);
			buff.Read(info.m_openId);
			buff.Read(info.m_nike);
			buff.Read(info.m_sex);
			buff.Read(info.m_headImageUrl);
			buff.Read(info.m_agentPower);
			m_info.push_back(info);
		}
		buff.Read(m_hasSentAll);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		m_count = m_info.size();
		buff.Write(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			buff.Write(m_info[i].m_id);
			buff.Write(m_info[i].m_openId);
			buff.Write(m_info[i].m_nike);
			buff.Write(m_info[i].m_sex);
			buff.Write(m_info[i].m_headImageUrl);
			buff.Write(m_info[i].m_agentPower);
		}
		buff.Write(m_hasSentAll);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LUserIdInfo();
	}

	virtual Lint preAllocSendMemorySize() //重新分配64K,由于消息长度是short型
	{
		return 1024 * 64;
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送保存 玩家信息
struct LMsgL2CeModifyUserNew:public LMsg
{
	Lstring			m_strUUID;
	Lint			m_value;	// 0表示新生成的用户 1表示老用户并给玩家送过卡
	LMsgL2CeModifyUserNew() :LMsg(MSG_L_2_CE_MODIFY_USER_NEW)
	{
		m_value = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_value);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_value);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeModifyUserNew();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送保存 桌子信息
struct LMsgL2CeSaveLogItem:public LMsg
{
	Lint		m_type;//类型,0-插入,1-更新
	Lstring		m_sql;

	LMsgL2CeSaveLogItem() :LMsg(MSG_L_2_CE_ROOM_SAVE)
	{
		m_type = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_type);
		buff.Read(m_sql);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_type);
		buff.Write(m_sql);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeSaveLogItem();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 通知玩家登陆服务器
struct LMsgL2CeUserServerLogin :public LMsg
{
	Lint		m_serverID;//类型,0-插入,1-更新
	Lstring		m_openID;

	LMsgL2CeUserServerLogin() :LMsg(MSG_L_2_CE_USER_LOGIN)
	{
		m_serverID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_serverID);
		buff.Read(m_openID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_serverID);
		buff.Write(m_openID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeUserServerLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 通知玩家登出服务器
struct LMsgL2CeUserServerLogout:public LMsg
{
	Lint		m_serverID;//类型,0-插入,1-更新
	Lstring		m_openID;

	LMsgL2CeUserServerLogout() :LMsg(MSG_L_2_CE_USER_LOGOUT)
	{
		m_serverID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_serverID);
		buff.Read(m_openID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_serverID);
		buff.Write(m_openID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeUserServerLogout();
	}
};

//////////////////////////////////////////////////////////////////////////
//center 发送玩家充值
struct LMsgCe2LGMCharge:public LMsg
{
	Lstring		m_strUUID;
	Lint		m_userid;
	Lint		m_cardType;
	Lint		m_cardCount;
	Lint		m_oper;
	Lstring		m_admin;

	LMsgCe2LGMCharge() :LMsg(MSG_CE_2_L_GM_CHARGE)
	{
		m_userid = 0;
		m_cardType = 0;
		m_cardCount = 0;
		m_oper = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_cardType);
		buff.Read(m_cardCount);
		buff.Read(m_oper);
		buff.Read(m_admin);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_cardType);
		buff.Write(m_cardCount);
		buff.Write(m_oper);
		buff.Write(m_admin);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMCharge();
	}
};


//////////////////////////////////////////////////////////////////////////
//center 发送gm设置跑马灯
struct LMsgCe2LGMHorse :public LMsg
{
	Lstring		m_str;
	
	LMsgCe2LGMHorse() :LMsg(MSG_CE_2_L_GM_HORSE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_str);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_str);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMHorse();
	}
};

//////////////////////////////////////////////////////////////////////////
//center 发送gm够买提示
struct LMsgCe2LGMBuyInfo:public LMsg
{
	Lstring		m_str;

	LMsgCe2LGMBuyInfo() :LMsg(MSG_CE_2_L_GM_BUYINFO)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_str);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_str);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMBuyInfo();
	}
};

struct LMsgCe2LGMCoins:public LMsg
{
	Lstring		m_strUUID;
	Lint		m_userid;
	Lint		m_coins;		// 充值的金币
	Lint		m_totalcoins;	// 充值后的总金币

	LMsgCe2LGMCoins() :LMsg(MSG_CE_2_L_GM_COINS)
	{
		m_userid = 0;
		m_coins = 0;
		m_totalcoins = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_coins);
		buff.Read(m_totalcoins);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_coins);
		buff.Write(m_totalcoins);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMCoins();
	}
};

//////////////////////////////////////////////////////////////////////////
//center 发送gm够买提示
struct LMsgCe2LGMHide:public LMsg
{
	Lint		m_hide;

	LMsgCe2LGMHide() :LMsg(MSG_CE_2_L_GM_HIDE)
	{
		m_hide = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_hide);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_hide);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMHide();
	}
};
//////////////////////////////////////////////////////////////////////////
//logic 跟 gate 之间的消息交互
//////////////////////////////////////////////////////////////////////////

struct LMsgG2LLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;

	LMsgG2LLogin() :LMsg(MSG_G_2_L_LOGIN)
	{
		m_id = 0;
		m_port = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LLogin();
	}
};

struct LMsgG2LMGLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;

	LMsgG2LMGLogin() :LMsg(MSG_G_2_LMG_LOGIN)
	{
		m_id = 0;
		m_port = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LMGLogin();
	}
};

struct LMsgL2LMGLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;

	LMsgL2LMGLogin() :LMsg(MSG_L_2_LMG_LOGIN)
	{
		m_id = 0;
		m_port = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGLogin();
	}
};

struct LMsgLMG2LRLogInfo :public LMsg
{
	Lint m_managerID;
	bool bopenrlog;
	std::string m_url;
	size_t m_max_cached_size;
	size_t m_max_cached_num;

	LMsgLMG2LRLogInfo() :LMsg(MSG_LMG_2_L_RLOG_INFO)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_managerID);
		buff.Read(bopenrlog);
		buff.Read(m_url);
		buff.Read(m_max_cached_size);
		buff.Read(m_max_cached_num);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_managerID);
		buff.Write(bopenrlog);
		buff.Write(m_url);
		buff.Write(m_max_cached_size);
		buff.Write(m_max_cached_num);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LRLogInfo();
	}
};

//////////////////////////////////////////////////////////////////////////
//gate 发送玩家消息 到 logic
struct LMsgG2LUserMsg :public LMsg
{
	/*
	以下字段由gate进行填充
	*/
	Lstring			m_strUUID;	//玩家uuid
	Lstring			m_ip;
	LBuffPtr		m_dataBuff;

	/*
	以下字段由read中解析
	*/
	Lint			m_userMsgId;
	LMsg*			m_userMsg;

	LMsgG2LUserMsg() :LMsg(MSG_G_2_L_USER_MSG)
	{
		m_userMsgId = 0;
		m_userMsg = NULL;
	}

	virtual~LMsgG2LUserMsg()
	{
		if (m_userMsg)
			
			delete m_userMsg;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_ip);

		m_userMsgId = MSG_ERROR_MSG;
		try
		{
			msgpack::unpacked  unpack;
			msgpack::unpack(&unpack, buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
			msgpack::object obj = unpack.get();

			ReadMapData(obj, "m_msgId", m_userMsgId);
			m_userMsg = LMsgFactory::Instance().CreateMsg(m_userMsgId);
			if (m_userMsg)
			{
				m_userMsg->Read(obj);
			}
			else
			{
				LLOG_ERROR("LMsgG2LUserMsg read msgId not exiest %d", m_userMsgId);
			}
		}
		catch (...)
		{
			LLOG_ERROR("MSG_G_2_L_USER_MSG::RecvMsgPack error");
		}
		
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_ip);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LUserMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发现玩家消息到 gate
struct LMsgL2GUserMsg : public LMsg
{
	Lstring			m_strUUID;
	LBuffPtr		m_dataBuff;
	
	LMsgL2GUserMsg() :LMsg(MSG_L_2_G_USER_MSG)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		m_dataBuff = LBuffPtr(new LBuff);
		m_dataBuff->Write(buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2GUserMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//gate 发送玩家掉线消息 到 logic
struct LMsgG2LUserOutMsg :public LMsg
{
	Lstring	 m_strUUID;		//玩家UUID
	
	LMsgG2LUserOutMsg():LMsg(MSG_G_2_L_USER_OUT_MSG)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LUserOutMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送玩家消息到 gate
struct LMsgL2GUserOutMsg : public LMsg
{
	Lstring		m_strUUID;	//玩家uuid

	LMsgL2GUserOutMsg():LMsg(MSG_L_2_G_USER_OUT_MSG)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2GUserOutMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//logicmanager 跟 logicdb 之间的消息交互
struct LMsgLMG2LdbUserLogin:public LMsg
{
	Lint		m_userId;
	Lstring		m_strUUID;
	LMsgLMG2LdbUserLogin() :LMsg(MSG_LMG_2_LDB_USER_LOGIN)
	{
		m_userId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_strUUID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_strUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LdbUserLogin();
	}
};

struct LMsgLDB2LMGUserMsg : public LMsg
{
	Lstring			m_strUUID;
	LBuffPtr		m_dataBuff;

	LMsgLDB2LMGUserMsg() :LMsg(MSG_LDB_2_LMG_USER_MSG)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		m_dataBuff = LBuffPtr(new LBuff);
		m_dataBuff->Write(buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LMGUserMsg();
	}
};

//logic 登录 logicdb
struct LMsgLMG2LdbLogin : public LMsg
{
	Lstring			m_key;
	Lint			m_serverID;
	LMsgLMG2LdbLogin():LMsg(MSG_LMG_2_LDB_LOGIN)
	{
		m_serverID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_key);
		buff.Read(m_serverID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_key);
		buff.Write(m_serverID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LdbLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 登录 logicdb
struct LMsgL2LdbLogin : public LMsg
{
	Lstring			m_key;
	Lint			m_serverID;
	LMsgL2LdbLogin():LMsg(MSG_L_2_LDB_LOGIN)
	{
		m_serverID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_key);
		buff.Read(m_serverID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_key);
		buff.Write(m_serverID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LdbLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送保存 桌子信息
struct LMsgL2LDBSaveLogItem:public LMsg
{
	Lint		m_type;//类型,0-插入,1-更新
	Lstring		m_sql;
	Lint		m_serverID;
	Lint        m_userId;  //add

	LMsgL2LDBSaveLogItem() :LMsg(MSG_L_2_LDB_ROOM_SAVE)
	{
		m_type = 0;
		m_serverID = 0;
		m_userId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_type);
		buff.Read(m_sql);
		buff.Read(m_serverID);
		buff.Read(m_userId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_type);
		buff.Write(m_sql);
		buff.Write(m_serverID);
		buff.Write(m_userId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBSaveLogItem();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送保存录像
struct LMsgL2LDBSaveVideo :public LMsg
{
	Lint		m_type;//类型,0-插入,1-更新
	Lstring		m_sql;
	Lint		m_serverID;
	Lint        m_userId;  //add

	LMsgL2LDBSaveVideo() :LMsg(MSG_L_2_LDB_VIDEO_SAVE)
	{
		m_type = 0;
		m_serverID = 0;
		m_userId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_type);
		buff.Read(m_sql);
		buff.Read(m_serverID);
		buff.Read(m_userId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_type);
		buff.Write(m_sql);
		buff.Write(m_serverID);
		buff.Write(m_userId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBSaveVideo();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送玩家请求战绩
struct LMsgL2LBDReqVipLog:public LMsg
{
	Lstring m_strUUID;
	Lint	m_userId;
	Lint	m_reqUserId;
	Lint    m_time;

	LMsgL2LBDReqVipLog() :LMsg(MSG_L_2_LDB_VIP_LOG)
	{
		m_userId = 0;
		m_reqUserId = 0;
		m_time = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_reqUserId);
		buff.Read(m_time);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_reqUserId);
		buff.Write(m_time);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LBDReqVipLog();
	}
};


//////////////////////////////////////////////////////////////////////////
//logicdb 返回 玩家战绩
struct LogInfo
{
	Lstring			m_id;		//id
	Lint			m_time;		//时间
	Lint			m_flag;		//房间规则
	Lint			m_deskId;	//桌子id
	Lstring			m_secret;	//密码
	Lint			m_maxCircle;//总圈数
	Lint			m_curCircle;//当前圈数
	Lint			m_posUserId[4];//0-3各个位置上的玩家id
	Lint			m_curZhuangPos;//当前庄家
	Lint			m_score[4];// 各个位置上面的积分
	Lint			m_reset;//是否解算
	Lstring			m_data;
	Lstring			m_playtype;	//玩法

	LogInfo()
	{
		m_time = 0;
		m_flag = 0;	
		m_deskId = 0;
		m_maxCircle = 0;
		m_curCircle = 0;
		memset(m_posUserId, 0, sizeof(m_posUserId));
		m_curZhuangPos = 0;
		memset(m_score, 0, sizeof(m_score)); 
		m_reset = false;
	}
};

struct LMsgLBD2LReqVipLog:public LMsg
{
	Lstring		m_strUUID;
	Lint		m_userid;
	Lint		m_count;
	LogInfo		m_info[21];

	LMsgLBD2LReqVipLog() :LMsg(MSG_LDB_2_L_VIP_LOG)
	{
		m_userid = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			LogInfo& info = m_info[i];
			buff.Read(info.m_id);
			buff.Read(info.m_time);
			buff.Read(info.m_flag);
			buff.Read(info.m_deskId);
			buff.Read(info.m_secret);
			buff.Read(info.m_maxCircle);
			buff.Read(info.m_curCircle);
			buff.Read(info.m_posUserId[0]);
			buff.Read(info.m_posUserId[1]);
			buff.Read(info.m_posUserId[2]);
			buff.Read(info.m_posUserId[3]);

			buff.Read(info.m_curZhuangPos);
			buff.Read(info.m_score[0]);
			buff.Read(info.m_score[1]);
			buff.Read(info.m_score[2]);
			buff.Read(info.m_score[3]);
			buff.Read(info.m_reset);
			buff.Read(info.m_data);
			buff.Read(info.m_playtype);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			LogInfo& info = m_info[i];
			buff.Write(info.m_id);
			buff.Write(info.m_time);
			buff.Write(info.m_flag);
			buff.Write(info.m_deskId);
			buff.Write(info.m_secret);
			buff.Write(info.m_maxCircle);
			buff.Write(info.m_curCircle);
			buff.Write(info.m_posUserId[0]);
			buff.Write(info.m_posUserId[1]);
			buff.Write(info.m_posUserId[2]);
			buff.Write(info.m_posUserId[3]);

			buff.Write(info.m_curZhuangPos);
			buff.Write(info.m_score[0]);
			buff.Write(info.m_score[1]);
			buff.Write(info.m_score[2]);
			buff.Write(info.m_score[3]);
			buff.Write(info.m_reset);
			buff.Write(info.m_data);
			buff.Write(info.m_playtype);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLBD2LReqVipLog();
	}
};

//LogicDB 2 LogicManager
struct LMsgLBD2LReqRoomLog:public LMsg
{
	Lstring		m_strUUID;
	Lint		m_userid;
	Lint		m_flag;
	Lstring     m_data;

	LMsgLBD2LReqRoomLog() :LMsg(MSG_LDB_2_L_ROOM_LOG)
	{
		m_userid = 0;
		m_flag = 0;
		m_data = "";
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_flag);
		buff.Read(m_data);
		
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_flag);
		buff.Write(m_data);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLBD2LReqRoomLog();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送玩家请求录像
struct LMsgL2LDBReqVideo:public LMsg
{
	Lstring m_strUUID;
	Lint	m_userId;
	Lstring	m_videoId;

	LMsgL2LDBReqVideo() :LMsg(MSG_L_2_LDB_VIDEO)
	{
		m_userId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_videoId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_videoId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBReqVideo();
	}
};

//////////////////////////////////////////////////////////////////////////
//center 返回 玩家战绩
struct LMsgLDB2LReqVideo:public LMsg
{
	Lstring		m_strUUID;
	Lint		m_userid;
	Lint		m_count;
	VideoLog	m_video;

	LMsgLDB2LReqVideo() :LMsg(MSG_LDB_2_L_VIDEO)
	{
		m_userid = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_count);
		m_video.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_count);
		m_video.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LReqVideo();
	}
};

struct LMsgLDB2LLogInfo:public LMsg
{
	Lint	m_lastId;
	Lint	m_videoId;
	Lint	m_count;
	std::vector<LogInfo> m_info;

	LMsgLDB2LLogInfo() :LMsg(MSG_LDB_2_L_LOG_INFO)
	{
		m_lastId = 0;
		m_videoId = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_lastId);
		buff.Read(m_videoId);
		buff.Read(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			LogInfo info;
			buff.Read(info.m_id);
			buff.Read(info.m_time);
			buff.Read(info.m_flag);
			buff.Read(info.m_deskId);
			buff.Read(info.m_secret);
			buff.Read(info.m_maxCircle);
			buff.Read(info.m_curCircle);
			buff.Read(info.m_posUserId[0]);
			buff.Read(info.m_posUserId[1]);
			buff.Read(info.m_posUserId[2]);
			buff.Read(info.m_posUserId[3]);

			buff.Read(info.m_curZhuangPos);
			buff.Read(info.m_score[0]);
			buff.Read(info.m_score[1]);
			buff.Read(info.m_score[2]);
			buff.Read(info.m_score[3]);
			buff.Read(info.m_reset);
			buff.Read(info.m_data);
			buff.Read(info.m_playtype);

			m_info.push_back(info);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_lastId);
		buff.Write(m_videoId);
		m_count = m_info.size();
		buff.Write(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			LogInfo& info = m_info[i];
			buff.Write(info.m_id);
			buff.Write(info.m_time);
			buff.Write(info.m_flag);
			buff.Write(info.m_deskId);
			buff.Write(info.m_secret);
			buff.Write(info.m_maxCircle);
			buff.Write(info.m_curCircle);
			buff.Write(info.m_posUserId[0]);
			buff.Write(info.m_posUserId[1]);
			buff.Write(info.m_posUserId[2]);
			buff.Write(info.m_posUserId[3]);

			buff.Write(info.m_curZhuangPos);
			buff.Write(info.m_score[0]);
			buff.Write(info.m_score[1]);
			buff.Write(info.m_score[2]);
			buff.Write(info.m_score[3]);
			buff.Write(info.m_reset);
			buff.Write(info.m_data);
			buff.Write(info.m_playtype);
		}


		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LLogInfo();
	}
};

struct LMsgLMG2GHorseInfo :public LMsg
{
	Lstring	m_str;
	Lint    m_type;  //1= 免费收费消息推送 2= 推送金币房卡信息

	LMsgLMG2GHorseInfo():LMsg(MSG_LMG_2_G_HORSE_INFO)
	{
		m_type = 0;   //默认跑马灯 消息模式
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_str);
		buff.Read(m_type);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_str);
		buff.Write(m_type);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2GHorseInfo();
	}
};


struct LMsgLMG2LLogin:public LMsg
{
	Lint		m_userid;
	Lint		m_gateId;
	Lstring		m_strUUID;
	Lstring		m_ip;
	Lstring		m_buyInfo;//够买信息
	Lint		m_hide;//购买隐藏0-隐藏 1- 不隐藏
	Lint		m_card_num;	//房卡数 （Card2）
	LMsgLMG2LLogin() :LMsg(MSG_LMG_2_L_USER_LOGIN)
	{
		m_userid = 0;
		m_gateId = 0;
		m_hide = 0;
		m_card_num = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_strUUID);
		buff.Read(m_ip);
		buff.Read(m_buyInfo);
		buff.Read(m_hide);
		buff.Read(m_card_num);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_strUUID);
		buff.Write(m_ip);
		buff.Write(m_buyInfo);
		buff.Write(m_hide);
		buff.Write(m_card_num);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LLogin();
	}
};

struct LMsgL2LMGModifyUserState:public LMsg
{
	Lstring m_strUUID;
	Lint	m_userid;
	Lint	m_userstate;
	Lint	m_logicID;

	LMsgL2LMGModifyUserState() :LMsg(MSG_L_2_LMG_MODIFY_USER_STATE)
	{
		m_userid = 0;
		m_userstate = 0;
		m_logicID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_userstate);
		buff.Read(m_logicID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_userstate);
		buff.Write(m_logicID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGModifyUserState();
	}
};

struct LMsgL2GModifyUserState:public LMsg
{
	Lstring m_strUUID;	 //玩家uuid
	Lint	m_userstate;
	Lint	m_logicID;

	LMsgL2GModifyUserState() :LMsg(MSG_L_2_G_MODIFY_USER_STATE)
	{
		m_userstate = 0;
		m_logicID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userstate);
		buff.Read(m_logicID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userstate);
		buff.Write(m_logicID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2GModifyUserState();
	}
};

struct LMsgLMG2LCreateGonghuiDesk : public LMsg
{
	Lint m_userId;    // 创建者，即会场id
	Lint m_deskId;    
	Lint m_gonghuiId; 
	Lint m_baseScore;    // 桌子底分
	Lint m_roomType;    // 桌子牌局数
	Lint m_playType;  // 几人麻将
	// Lint m_deskCount;    // 创建的房子数量

	LMsgLMG2LCreateGonghuiDesk() :LMsg(MSG_LMG_2_L_CREATE_GONGHUIDESK)
	{
		m_userId = 0;
		m_deskId = 0;
		m_gonghuiId = 0;
		m_baseScore = 0;
		m_roomType = 0;
		m_playType = 0;
		//m_deskCount = 0;
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_deskId);
		buff.Read(m_gonghuiId);
		buff.Read(m_baseScore);
		buff.Read(m_roomType);
		buff.Read(m_playType);
		//buff.Read(m_deskCount);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_deskId);
		buff.Write(m_gonghuiId);
		buff.Write(m_baseScore);
		buff.Write(m_roomType);
		buff.Write(m_playType);
		//buff.Write(m_deskCount);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LCreateGonghuiDesk();
	}
};

struct LMsgL2LMGGonghuiDeskChange : public LMsg
{
	Lstring m_strUUID;	 //玩家uuid
	Lint m_gonghuiId;  // 工会ID
	Lint m_playType;   // 几人麻将
	Lint m_baseScore;  // 底分类型
	Lint m_roomType;   // 房间类型，多少局
	Lint m_roomId;     // 工会房间ID
	Lint m_roomState;  // 工会房间状态，为0表示空闲，1表示准备（新用户加入房间），2表示Playing，3表示房间正常结束，4表示房间解散
	Lint m_user[4]; // 进入房间的用户，当roomState为2时使用，更新数据库数据
	Lint m_score[4];   // 当房间局数结束时，记录总分数
	Lint m_playNum;

	LMsgL2LMGGonghuiDeskChange() : LMsg(MSG_L_2_LMG_GONGHUIDESK_CHANGE)
	{
		m_gonghuiId = 0;
		m_roomId = 0;
		m_playType = 0;
		m_roomState = 0;
		m_baseScore = 0;
		m_roomType = 0;
		for (int i = 0; i < 4; i++)
		{
			m_user[i] = 0;
			m_score[i] = 0;
		}
		m_playNum = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_gonghuiId);
		buff.Read(m_playType);
		buff.Read(m_baseScore);
		buff.Read(m_roomType);
		buff.Read(m_roomId);
		buff.Read(m_roomState);
		int i = 0;
		for (i = 0; i < 4; i++)
		{
			buff.Read(m_user[i]);
		}

		for (i = 0; i < 4; i++)
		{
			buff.Read(m_score[i]);
		}
		buff.Read(m_playNum);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_gonghuiId);
		buff.Write(m_playType);
		buff.Write(m_baseScore);
		buff.Write(m_roomType);
		buff.Write(m_roomId);
		buff.Write(m_roomState);
		int i = 0;
		for (i = 0; i < 4; i++)
		{
			buff.Write(m_user[i]);
		}

		for (i = 0; i < 4; i++)
		{
			buff.Write(m_score[i]);
		}
		buff.Write(m_playNum);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGGonghuiDeskChange();
	}
};

struct LMsgLMG2LCreateDesk : public LMsg
{
	Lint				m_userid;
	Lint				m_gateId;
	Lstring				m_strUUID;
	Lint				m_deskID;
	Lstring				m_ip;
	LUser				m_usert;

	Lint				m_flag;				//房间类型，1-2圈，2-四圈，3-8圈
	Lstring				m_secret;			//房间密码，如果为空，服务器会随机一个密码
	Lint				m_gold;				//房间抵住，单位元，最低为1元，最多不能超过100
	Lint				m_state;			//玩法规则 0 转转   1 长沙
	Lint				m_robotNum;			// 0,不加机器人，1，2，3加机器人数量
	CardValue			m_cardValue[CARD_COUNT];
	Lint				m_playTypeCount;
	std::vector<Lint>	m_playType;			//玩法

	LMsgLMG2LCreateDesk() :LMsg(MSG_LMG_2_L_CREATE_DESK)
	{
		m_userid = 0;
		m_gateId = 0;
		m_deskID = 0;
		m_flag = 0; 					
		m_gold = 0;			
		m_state = 0;		
		m_robotNum = 0;		
		m_playTypeCount = 0;
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_strUUID);
		buff.Read(m_deskID);
		buff.Read(m_ip);
		m_usert.Read(buff);
		buff.Read(m_flag);
		buff.Read(m_gold);
		buff.Read(m_state);
		buff.Read(m_robotNum);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Read(m_cardValue[i].m_color);
			buff.Read(m_cardValue[i].m_number);
		}
		buff.Read(m_playTypeCount);
		for (Lint i = 0 ; i < m_playTypeCount; i ++ )
		{
			Lint playType;
			buff.Read(playType);
			m_playType.push_back(playType);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_strUUID);
		buff.Write(m_deskID);
		buff.Write(m_ip);
		m_usert.Write(buff);
		buff.Write(m_flag);
		buff.Write(m_gold);
		buff.Write(m_state);
		buff.Write(m_robotNum);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Write(m_cardValue[i].m_color);
			buff.Write(m_cardValue[i].m_number);
		}
		buff.Write(m_playTypeCount);
		for (Lint i = 0 ; i < m_playTypeCount; i ++ )
		{
			buff.Write(m_playType[i]);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LCreateDesk();
	}
};

struct LMsgLMG2LAddToDesk : public LMsg
{
	Lint			m_userid;
	Lint			m_gateId;
	Lstring			m_strUUID;
	Lint			m_deskID;
	Lstring			m_ip;
	LUser			m_usert;

	LMsgLMG2LAddToDesk() :LMsg(MSG_LMG_2_L_ADDTO_DESK)
	{
		m_userid = 0;
		m_gateId = 0;
		m_deskID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_strUUID);
		buff.Read(m_deskID);
		buff.Read(m_ip);
		m_usert.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_strUUID);
		buff.Write(m_deskID);
		buff.Write(m_ip);
		m_usert.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LAddToDesk();
	}
};


struct LMsgL2LMGModifyCard :public LMsg
{
	Lint			m_userid;
	Lstring			m_strUUID;
	Lint			isAddCard;
	Lint			cardType;
	Lint			cardNum;
	Lint			operType;	// 参考 CARDS_OPER_TYPE
	Lstring			admin;
	LMsgL2LMGModifyCard() :LMsg(MSG_L_2_LMG_MODIFY_USER_CARD)
	{
		m_userid = 0;
		isAddCard = 0;
		cardType = 0;
		cardNum = 0;
		operType = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_strUUID);
		buff.Read(isAddCard);
		buff.Read(cardType);
		buff.Read(cardNum);
		buff.Read(operType);
		buff.Read(admin);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_strUUID);
		buff.Write(isAddCard);
		buff.Write(cardType);
		buff.Write(cardNum);
		buff.Write(operType);
		buff.Write(admin);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGModifyCard();
	}
};


struct LMsgL2LMGAddUserPlayCount :public LMsg
{
	Lint			m_userid;
	Lstring			m_strUUID;
	LMsgL2LMGAddUserPlayCount() :LMsg(MSG_L_2_LMG_ADD_USER_PLAYCOUNT)
	{
		m_userid = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_strUUID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_strUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGAddUserPlayCount();
	}
};

struct PXActiveItem
{
	Lint m_iPXId;
	Lint m_iCardNum;
	PXActiveItem()
	{
		m_iPXId = -1;
		m_iCardNum = 0;
	}

	PXActiveItem(Lint id, Lint cards)
	{
		m_iPXId = id;
		m_iCardNum = cards;
	}
};

struct PXActive
{
	Lint		m_iGameId;
	Lstring		m_strBeginTime;
	Lstring		m_strEndTime;
	Lint		m_iRewardCount;
	std::vector<PXActiveItem> m_vecPXAndReward;

	PXActive()
	{
		m_iGameId = -1;
		m_strBeginTime = "";
		m_strEndTime = "";
		m_iRewardCount = 0;
	}
};

// LMtoL 发送牌型活动信息
struct LMsgLMG2LPXActive :public LMsg
{
	Lint     m_iGameSetCount;
	std::vector<PXActive> m_vecPXActive;
	
	LMsgLMG2LPXActive() :LMsg(MSG_LMG_2_L_PAIXING_ACTIVE)
	{
		m_iGameSetCount = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_iGameSetCount);
		for(Lint i = 0 ; i < m_iGameSetCount; ++i)
		{
			PXActive Active;
			buff.Read(Active.m_iGameId);
			buff.Read(Active.m_strBeginTime);
			buff.Read(Active.m_strEndTime);
			buff.Read(Active.m_iRewardCount);
			for(Lint j = 0; j < Active.m_iRewardCount; ++j)
			{
				PXActiveItem item;
				buff.Read(item.m_iPXId);
				buff.Read(item.m_iCardNum);
				Active.m_vecPXAndReward.emplace_back(item);
			}
			m_vecPXActive.emplace_back(Active);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		m_iGameSetCount = m_vecPXActive.size();
		buff.Write(m_iGameSetCount);
		for(Lint i = 0 ; i < m_iGameSetCount; ++i)
		{
			buff.Write(m_vecPXActive[i].m_iGameId);
			buff.Write(m_vecPXActive[i].m_strBeginTime);
			buff.Write(m_vecPXActive[i].m_strEndTime);
			m_vecPXActive[i].m_iRewardCount = m_vecPXActive[i].m_vecPXAndReward.size();
			buff.Write(m_vecPXActive[i].m_iRewardCount);
			std::vector<PXActiveItem>& vec = m_vecPXActive[i].m_vecPXAndReward;
			for(Lint j = 0; j < m_vecPXActive[i].m_iRewardCount; ++j)
			{
				buff.Write(vec[j].m_iPXId);
				buff.Write(vec[j].m_iCardNum);
			}
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LPXActive();
	}
};

struct LMsgL2LMGRecyleDeskID :public LMsg
{
	Lint			m_serverID;
	Lint			m_deskID;
	LMsgL2LMGRecyleDeskID() :LMsg(MSG_L_2_LMG_RECYLE_DESKID)
	{
		m_serverID = 0;
		m_deskID = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_serverID);
		buff.Read(m_deskID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_serverID);
		buff.Write(m_deskID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGRecyleDeskID();
	}
};

//logic 通知活动免费
struct LMsgCE2LSetGameFree:public LMsg
{
	Lint		m_ServerID;
	Lstring		m_strFreeSet;
	LMsgCE2LSetGameFree() :LMsg(MSG_CE_2_L_SET_GAME_FREE)
	{
		m_ServerID = 0;
		m_strFreeSet = "";
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ServerID);
		buff.Read(m_strFreeSet);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ServerID);
		buff.Write(m_strFreeSet);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LSetGameFree();
	}
};

//logic 通知设置牌型活动配置
struct LMsgCE2LSetPXActive:public LMsg
{
	Lint	m_ServerID;
	Lstring	m_strActiveSet;

	LMsgCE2LSetPXActive() :LMsg(MSG_CE_2_L_SET_PXACTIVE)
	{
		m_ServerID = 0;
		m_strActiveSet = "";
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ServerID);
		buff.Read(m_strActiveSet);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ServerID);
		buff.Write(m_strActiveSet);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LSetPXActive();
	}
};

//logic 通知设置老玩家送卡活动配置
struct LMsgCE2LSetOUGCActive:public LMsg
{
	Lint	m_ServerID;
	Lstring	m_strActiveSet;

	LMsgCE2LSetOUGCActive() :LMsg(MSG_CE_2_L_SET_OUGCACTIVE)
	{
		m_ServerID = 0;
		m_strActiveSet = "";
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ServerID);
		buff.Read(m_strActiveSet);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ServerID);
		buff.Write(m_strActiveSet);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LSetOUGCActive();
	}
};

//logic 通知设置老玩家送卡活动配置
struct LMsgCE2LSetExchActive:public LMsg
{
	Lint	m_ServerID;
	Lstring	m_strActiveSet;

	LMsgCE2LSetExchActive() :LMsg(MSG_CE_2_L_SET_EXCHACTIVE)
	{
		m_ServerID = 0;
		m_strActiveSet = "";
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ServerID);
		buff.Read(m_strActiveSet);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ServerID);
		buff.Write(m_strActiveSet);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LSetExchActive();
	}
};

// 设置活懂内容
struct LMsgCE2LSetActivity:public LMsg
{
	Lint	m_ServerID;
	ActivityID m_activityId;
	Lstring	m_strActivity;

	LMsgCE2LSetActivity() :LMsg(MSG_CE_2_L_SET_ACTIVITY)
	{
		m_ServerID = 0;
		m_strActivity = "";
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ServerID);
		buff.Read(m_activityId);
		buff.Read(m_strActivity);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ServerID);
		buff.Write(m_activityId);
		buff.Write(m_strActivity);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LSetActivity();
	}
};

//inviter
struct LMsgLM_2_LDBBindInviter : public LMsgSC
{
	Lint				m_inviterId;
	Lint                m_userId;
	LMsgLM_2_LDBBindInviter() : LMsgSC(MSG_LM_2_LDB_BIND_INVITER) 
	{
		m_inviterId = 0;
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_inviterId), m_inviterId);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_inviterId), m_inviterId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgLM_2_LDBBindInviter; }
};

struct LMsgLDB2LMGBindingRelationships : public LMsg
{
#define    NUM_MAX_INVITEE   40
	Lstring		m_strUUID;
	Lint		m_userId;
	Lint		m_inviterId;
	Lint		m_finished;
	Lint		m_inviteeCount;
	Lint		m_invitees[NUM_MAX_INVITEE];
	Lint		m_taskFinished[NUM_MAX_INVITEE];

	LMsgLDB2LMGBindingRelationships() : LMsg(MSG_LDB_2_LMG_REQ_BINDING_RELATIONSHIPS) 
	{
		m_userId = 0;
		m_inviterId = 0;
		m_finished = 0;
		m_inviteeCount = 0;
		memset(m_taskFinished,0,sizeof(m_taskFinished));
		memset(m_invitees,0,sizeof(m_invitees));
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_inviterId);
		buff.Read(m_finished);
		buff.Read(m_inviteeCount);
		for (int i = 0; i < m_inviteeCount; ++i)
		{
			buff.Read(m_invitees[i]);
			buff.Read(m_taskFinished[i]);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_inviterId);
		buff.Write(m_finished);
		buff.Write(m_inviteeCount);
		for (int i = 0; i < m_inviteeCount; ++i)
		{
			buff.Write(m_invitees[i]);
			buff.Write(m_taskFinished[i]);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LMGBindingRelationships;
	}
};

//请求 邀请人id及任务完成
struct LMsgLM_2_LDB_ReqInfo : public LMsgSC  
{
	enum ReqCode
	{
		ReqInviterInfo = 1,
		TaskFinished = 2,
		DeleteUser = 3,
	};

	Lint                m_reqCode;    //1:req 2:task finished 
	Lstring				m_strUUID;
	Lint                m_userId;
	Lint                m_param;
	LMsgLM_2_LDB_ReqInfo() : LMsgSC(MSG_LM_2_LDB_REQ_INFO) 
	{
		m_reqCode = 0;
		m_userId = 0;
		m_param = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_reqCode), m_reqCode);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_param), m_param);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_reqCode), m_reqCode);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_param), m_param);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgLM_2_LDB_ReqInfo; }
};

//返回 邀请人id及任务 完成情况
struct LMsgLDB_2_LM_RetInfo : public LMsgSC
{
	Lstring				m_strUUID;
	Lint                m_userId;
	Lint                m_inviterId;   //邀请人的id
	Lint                m_finished;
	LMsgLDB_2_LM_RetInfo() : LMsgSC(MSG_LDB_2_LM_RET_INFO) 
	{
		m_userId = 0;
		m_inviterId = 0;
		m_finished = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_inviterId), m_inviterId);
		ReadMapData(obj, NAME_TO_STR(m_finished), m_finished);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_inviterId), m_inviterId);
		WriteKeyValue(pack, NAME_TO_STR(m_finished), m_finished);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgLDB_2_LM_RetInfo; }
};

//LoginServer Msg
struct LMsgFromLoginServer:public LMsg
{
	/*
	一下字段由LoginServer填写
	*/
	Lstring	 m_strUUID;
	Lstring  m_clientIP;
	LBuffPtr m_dataBuff;

	/*
	以下字段由read中解析
	*/
	Lint			m_userMsgId;
	LMsg*			m_userMsg;

	LMsgFromLoginServer() : LMsg(MSG_FROM_LOGINSERVER)
	{
		m_userMsgId = 0;
		m_userMsg = NULL;
	}
	~LMsgFromLoginServer()
	{
		if(m_userMsg)
		{
			delete m_userMsg;
		}
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_clientIP);

		m_userMsgId = MSG_ERROR_MSG;
		try
		{
			msgpack::unpacked  unpack;
			msgpack::unpack(&unpack, buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
			msgpack::object obj = unpack.get();

			ReadMapData(obj, "m_msgId", m_userMsgId);
			m_userMsg = LMsgFactory::Instance().CreateMsg(m_userMsgId);
			if (m_userMsg)
			{
				m_userMsg->Read(obj);
			}
			else
			{
				LLOG_ERROR("LMsgG2LUserMsg read msgId not exiest %d", m_userMsgId);
			}
		}
		catch (...)
		{
			LLOG_ERROR("MSG_G_2_L_USER_MSG::RecvMsgPack error");
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_clientIP);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgFromLoginServer();
	}
};

struct LMsgToLoginServer : public LMsg
{
	Lstring	 m_strUUID;
	LBuffPtr m_dataBuff;

	LMsgToLoginServer() : LMsg(MSG_TO_LOGINSERVER)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		m_dataBuff = LBuffPtr(new LBuff);
		m_dataBuff->Write(buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgToLoginServer();
	}
};

//客户端数据包经过解析后生成的结构
struct LMsgConvertClientMsg : public LMsg
{
	boost::shared_ptr<LMsgC2SVerifyHead> msgHead;
	boost::shared_ptr<LMsg>	msgEntity;
	boost::shared_ptr<LBuff> msgOriginData;

	LMsgConvertClientMsg() : LMsg(MSG_CONVERT_CLIENTMSG)
	{

	}

	virtual LMsg* Clone()
	{
		return new LMsgConvertClientMsg();
	}
};

//公共心跳包
enum HEARBEAT_WHO
{
	HEARBEAT_WHO_UNKNOWN = 0,
	HEARBEAT_WHO_CENTER,
	HEARBEAT_WHO_LOGICMANAGER,
	HEARBEAT_WHO_LOGICSERVER,
	HEARBEAT_WHO_LOGINSERVER,
	HEARBEAT_WHO_GATESERVER,
	HEARBEAT_WHO_LOGICDB,
	HEARBEAT_WHO_COINSSERVER,
	HEARBEAT_WHO_NUM
};
const char* const HearBeat_Name[HEARBEAT_WHO_NUM] =
{
	"Unknown", "Center", "Manager", "Logic", "Login", "Gate", "DB", "Coins"
};

struct LMsgHeartBeatRequestMsg : public LMsg
{
	HEARBEAT_WHO m_fromWho;
	int m_iServerId;		//服务器标识

	LMsgHeartBeatRequestMsg() : LMsg(MSG_HEARDBEAT_REQUEST)
	{
		m_fromWho   = HEARBEAT_WHO_UNKNOWN;
		m_iServerId = -1;
	}

	virtual bool Read(LBuff& buff)
	{
		int iFromWho = 0;
		buff.Read(iFromWho);
		buff.Read(m_iServerId);

		m_fromWho = (HEARBEAT_WHO)iFromWho;
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write((int)m_fromWho);
		buff.Write(m_iServerId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgHeartBeatRequestMsg();
	}
};

struct LMsgHeartBeatReplyMsg : public LMsg
{
	HEARBEAT_WHO m_fromWho;
	int m_iServerId;		//服务器标识

	LMsgHeartBeatReplyMsg() : LMsg(MSG_HEARDBEAT_REPLY)
	{
		m_fromWho   = HEARBEAT_WHO_UNKNOWN;
		m_iServerId = -1;
	}

	virtual bool Read(LBuff& buff)
	{
		int iFromWho = 0;
		buff.Read(iFromWho);
		buff.Read(m_iServerId);

		m_fromWho = (HEARBEAT_WHO)iFromWho;
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write((int)m_fromWho);
		buff.Write(m_iServerId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgHeartBeatReplyMsg();
	}
};

// 验证用户请求
struct LMsgNewUserVerifyRequest : public LMsg
{
	void* m_pUser;
	LSocketPtr m_gameloginSP;
	Lint m_severID;
	bool m_bWechatLogin;
	Lstring m_openId;
	Lstring m_accessToken;
	Lstring m_clientIP;
	LMsgNewUserVerifyRequest() : LMsg(MSG_NEWUSER_VERIFY_REQUEST)
	{
		m_pUser = NULL;
		m_severID = 0;
		m_bWechatLogin = false;
	}
	~LMsgNewUserVerifyRequest()
	{

	}

	virtual LMsg* Clone()
	{
		return new LMsgNewUserVerifyRequest();
	}
};

struct LMsgNewUserVerifyReply : public LMsg
{
	void* m_pUser;
	LSocketPtr m_gameloginSP;
	Lint m_severID;
	Lint m_errorCode;	// 结果 和LMsgS2CMsg结果一直
	Lstring m_errorMsg;
	LMsgNewUserVerifyReply() : LMsg(MSG_NEWUSER_VERIFY_REPLY)
	{
		m_pUser = NULL;
		m_severID = 0;
		m_errorCode = -1;
	}
	~LMsgNewUserVerifyReply()
	{

	}

	virtual LMsg* Clone()
	{
		return new LMsgNewUserVerifyReply();
	}
};


// 金币服务器信息
struct CoinsInfo
{
	LSocketPtr	m_sp;			//不为空表示金币服务器连接上了
	Lstring		m_ip;			//m_sp有值时有效
	Lshort		m_port;			//m_sp有值时有效 
	Llong	  m_closeTickTime;	//m_sp有值时有效，不为0，表示金币服务器断开了连接，3分钟不恢复才进行剔除.默认值为0

	CoinsInfo()
	{
		m_port = 0;
		m_closeTickTime = 0;
	}
};

struct LMsgCN2LMGLogin :public LMsg
{
	Lstring		m_key;	// 用作验证
	Lstring		m_ip;
	Lshort		m_port;
	Lint		m_deskcount;	// 已经分配的桌子的数量 CoinsServer断线时 用来简单的判断

	LMsgCN2LMGLogin() :LMsg(MSG_CN_2_LMG_LOGIN)
	{
		m_port = 0;
		m_deskcount = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		buff.Read(m_deskcount);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		buff.Write(m_deskcount);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCN2LMGLogin();
	}
};

// 登录结果
struct LMsgLMG2CNLogin :public LMsg
{
	Lint		m_result;		// 0成功
	Lint		m_deskcount;	// 已经分配的桌子的数量 CoinsServer断线时 用来简单的判断

	LMsgLMG2CNLogin() :LMsg(MSG_LMG_2_CN_LOGIN)
	{
		m_result = -1;
		m_deskcount = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_result);
		buff.Read(m_deskcount);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_result);
		buff.Write(m_deskcount);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CNLogin();
	}
};

struct LMsgCN2LMGFreeDeskReq :public LMsg
{
	Lint		m_gameType;
	Lint		m_count;	// 请求的数量

	LMsgCN2LMGFreeDeskReq() :LMsg(MSG_CN_2_LMG_FREE_DESK_REQ)
	{
		m_gameType = 0;
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_gameType);
		buff.Read(m_count);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_gameType);
		buff.Write(m_count);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCN2LMGFreeDeskReq();
	}
};

struct CoinsDesk
{
	CoinsDesk()
	{
		m_id = 0;
		m_logicID = 0;
	}

	Lint	m_id;		// 桌子号
	Lint	m_logicID;	// 要分配到的逻辑服务器

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_logicID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_logicID);
		return true;
	}
};

struct LMsgLMG2CNFreeDeskReply :public LMsg
{
	Lint		m_gameType;
	std::vector<CoinsDesk> m_desk;

	LMsgLMG2CNFreeDeskReply() :LMsg(MSG_LMG_2_CN_FREE_DESK_REPLY)
	{
		m_gameType = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_gameType);
		int count = 0;
		buff.Read(count);
		for (Lint i = 0 ; i < count; i ++ )
		{
			CoinsDesk desk;
			desk.Read( buff );
			m_desk.push_back(desk);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_gameType);
		buff.Write((int)m_desk.size());
		for (Lsize i = 0 ; i < m_desk.size(); i ++ )
		{
			m_desk[i].Write(buff);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CNFreeDeskReply();
	}
};

struct LMsgCN2LMGRecycleDesk :public LMsg
{
	std::vector<int> m_deskid;

	LMsgCN2LMGRecycleDesk() :LMsg(MSG_CN_2_LMG_RECYCLE_DESK)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		int count = 0;
		buff.Read(count);
		for (Lint i = 0 ; i < count; i ++ )
		{
			Lint deskid;
			buff.Read(deskid);
			m_deskid.push_back(deskid);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write((int)m_deskid.size());
		for (Lsize i = 0 ; i < m_deskid.size(); i ++ )
		{
			buff.Write(m_deskid[i]);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCN2LMGRecycleDesk();
	}
};

struct LMsgLMG2CNUserLogin:public LMsg
{
	Lstring		m_strUUID;
	Lint		m_gateId;
	Lstring		m_ip;
	LUser		m_usert;	// 玩家基础数据 同步
	LMsgLMG2CNUserLogin() :LMsg(MSG_LMG_2_CN_USER_LOGIN)
	{
		m_gateId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_gateId);
		buff.Read(m_ip);
		m_usert.Read( buff );
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_gateId);
		buff.Write(m_ip);
		m_usert.Write( buff );
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CNUserLogin();
	}
};

struct LMsgLMG2CNEnterCoinDesk : public LMsg
{
	Lstring				m_strUUID;
	Lint				m_gateId;
	Lstring				m_ip;
	LUser				m_usert;

	Lint				m_state;			//玩法规则 0 转转   1 长沙
	Lint				m_robotNum;			// 0,不加机器人，1，2，3加机器人数量
	CardValue			m_cardValue[CARD_COUNT];
	std::vector<Lint>	m_playType;			//玩法
	Lint				m_coins;			// 玩家的金币

	LMsgLMG2CNEnterCoinDesk() :LMsg(MSG_LMG_2_CN_ENTER_COIN_DESK)
	{
		m_gateId = 0;
		m_state = 0;
		m_robotNum = 0;
		m_coins = 0;
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_gateId);
		buff.Read(m_ip);
		m_usert.Read(buff);
		buff.Read(m_state);
		buff.Read(m_robotNum);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Read(m_cardValue[i].m_color);
			buff.Read(m_cardValue[i].m_number);
		}
		int playTypeCount = 0;
		buff.Read(playTypeCount);
		for (int i = 0 ; i < playTypeCount; i ++ )
		{
			Lint playType;
			buff.Read(playType);
			m_playType.push_back(playType);
		}
		buff.Read(m_coins);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_gateId);
		buff.Write(m_ip);
		m_usert.Write(buff);
		buff.Write(m_state);
		buff.Write(m_robotNum);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Write(m_cardValue[i].m_color);
			buff.Write(m_cardValue[i].m_number);
		}
		buff.Write((int)m_playType.size());
		for (Lsize i = 0 ; i < m_playType.size(); i ++ )
		{
			buff.Write(m_playType[i]);
		}
		buff.Write(m_coins);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CNEnterCoinDesk();
	}
};

struct LMsgCN2LMGModifyUserCoins :public LMsg
{
	Lint			m_userid;
	Lstring			m_strUUID;
	Lint			isAddCoins;
	Lint			coinsNum;
	Lint			operType;	// 参考COINS_OPER_TYPE
	LMsgCN2LMGModifyUserCoins() :LMsg(MSG_CN_2_LMG_MODIFY_USER_COINS)
	{
		m_userid = 0;
		isAddCoins = 0;
		coinsNum = 0;
		operType = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_strUUID);
		buff.Read(isAddCoins);
		buff.Read(coinsNum);
		buff.Read(operType);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_strUUID);
		buff.Write(isAddCoins);
		buff.Write(coinsNum);
		buff.Write(operType);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCN2LMGModifyUserCoins();
	}
};

struct LMsgLMG2CNGetCoins :public LMsg
{
	Lstring			m_strUUID;
	Lint			m_userid;
	Lint			m_gateid;

	LMsgLMG2CNGetCoins() :LMsg(MSG_LMG_2_CN_GET_COINS)
	{
		m_userid = 0;
		m_gateid = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userid);
		buff.Read(m_gateid);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userid);
		buff.Write(m_gateid);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CNGetCoins();
	}
};

struct LMsgLMG2GLCoinsServerInfo :public LMsg
{
	CoinsInfo	m_conis;

	LMsgLMG2GLCoinsServerInfo() :LMsg(MSG_LMG_2_GL_COINS_SERVER_INFO)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_conis.m_ip);
		buff.Read(m_conis.m_port);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_conis.m_ip);
		buff.Write(m_conis.m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2GLCoinsServerInfo();
	}
};

struct LMsgG2CNLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;

	LMsgG2CNLogin() :LMsg(MSG_G_2_CN_LOGIN)
	{
		m_id = 0;
		m_port = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2CNLogin();
	}
};

struct LMsgL2CNLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;

	LMsgL2CNLogin() :LMsg(MSG_L_2_CN_LOGIN)
	{
		m_id = 0;
		m_port = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CNLogin();
	}
};

#define DIFFOPOUTTIME 10	// 服务器倒计时和客户端倒计时的差距

struct LMsgCN2LCreateCoinDesk : public LMsg
{
	struct User
	{
		User()
		{
			m_gateId = 0;
			m_pos = INVAILD_POS;
		}

		Lint				m_gateId;
		Lstring				m_ip;
		LUser				m_usert;
		Lint				m_pos;

		bool Read(LBuff& buff)
		{
			buff.Read(m_gateId);
			buff.Read(m_ip);
			m_usert.Read(buff);
			buff.Read(m_pos);
			return true;
		}

		bool Write(LBuff& buff)
		{
			buff.Write(m_gateId);
			buff.Write(m_ip);
			m_usert.Write(buff);
			buff.Write(m_pos);
			return true;
		}
	};
	std::vector<User>  m_users;
	
	Lint                m_deskId;
	Lint				m_state;			// GameType
	Lint				m_robotNum;			// 0,不加机器人，1，2，3加机器人数量
	CardValue			m_cardValue[CARD_COUNT];
	std::vector<Lint>	m_playType;			//玩法
	Lint				m_baseScore;		// 底分
	Lint				m_changeOutTime;	// 换牌的超时时间 <=0 不限制
	Lint				m_opOutTime;		// 操作倒计时 单位秒 <=0 表示不倒计时


	LMsgCN2LCreateCoinDesk() :LMsg(MSG_CN_2_L_CREATE_COIN_DESK)
	{
		m_deskId = 0;
		m_state = 0;
		m_robotNum = 0;
		m_baseScore = 0;
		m_changeOutTime = 0;
		m_opOutTime = 0;
	}
	virtual bool Read(LBuff& buff)
	{
		int usercount = 0;
		buff.Read(usercount);
		for ( int i = 0; i < usercount; ++i )
		{
			m_users.push_back( User() );
			m_users.back().Read( buff );
		}
		
		buff.Read(m_deskId);
		buff.Read(m_state);
		buff.Read(m_robotNum);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Read(m_cardValue[i].m_color);
			buff.Read(m_cardValue[i].m_number);
		}
		Lint playTypeCount = 0;
		buff.Read(playTypeCount);
		for (Lint i = 0 ; i < playTypeCount; i ++ )
		{
			Lint playType;
			buff.Read(playType);
			m_playType.push_back(playType);
		}
		buff.Read(m_baseScore);
		buff.Read(m_changeOutTime);
		buff.Read(m_opOutTime);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write((int)m_users.size());
		for ( Lsize i = 0; i < m_users.size(); ++i )
		{
			m_users[i].Write( buff );
		}
		buff.Write(m_deskId);
		buff.Write(m_state);
		buff.Write(m_robotNum);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Write(m_cardValue[i].m_color);
			buff.Write(m_cardValue[i].m_number);
		}
		buff.Write((int)m_playType.size());
		for (Lsize i = 0 ; i < m_playType.size(); i ++ )
		{
			buff.Write(m_playType[i]);
		}
		buff.Write(m_baseScore);
		buff.Write(m_changeOutTime);
		buff.Write(m_opOutTime);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCN2LCreateCoinDesk();
	}
};

struct LMsgL2CNCreateCoinDeskRet: public LMsg
{
	enum ErrorCode
	{
		CoinDesk_NoError = 0,
		CoinDesk_SizeError,
		CoinDesk_UserError,
		CoinDesk_PosError,
		CoinDesk_Unknown,
	};
	Lint                m_deskId;
	Lint				m_errorCode;			//0=成功

	LMsgL2CNCreateCoinDeskRet() :LMsg(MSG_L_2_CN_CREATE_COIN_DESK_RET)
	{
		m_deskId = 0;
		m_errorCode = 0;
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_deskId);
		buff.Read(m_errorCode);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_deskId);
		buff.Write(m_errorCode);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CNCreateCoinDeskRet();
	}
};

struct LMsgL2CNEndCoinDesk :public LMsg
{
	Lint		m_deskId;
	Lint        m_score[4];     //金币场结算得分
	LBuffPtr	m_dataResult;		//结算消息 有金币服务器发给客户端

	LMsgL2CNEndCoinDesk() :LMsg(MSG_L_2_CN_END_COIN_DESK)
	{
		m_deskId = 0;
		memset(m_score,0,sizeof(m_score));
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_deskId);
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			buff.Read(m_score[i]);
		}
		m_dataResult = LBuffPtr(new LBuff);
		m_dataResult->Write(buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_deskId);
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			buff.Write(m_score[i]);
		}
		buff.Write(m_dataResult->Data() + m_dataResult->GetOffset(), m_dataResult->Size() - m_dataResult->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CNEndCoinDesk();
	}
};

struct LMsgC2SGoOnCoinsDesk :public LMsgSC
{
	Lint		m_state;				//玩法规则: 0 转转  1  长沙  101-血战到底  102-血流成河
	std::vector<Lint>	m_playType;		//玩法: 1-自摸加底 2-自摸加番

	LMsgC2SGoOnCoinsDesk() :LMsgSC(MSG_C_2_S_GOON_COINS_ROOM)
	{
		m_state = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_playType), array);
		if (array.type == msgpack::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				Lint v = 0;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_playType.push_back(v);
			}
		}
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SGoOnCoinsDesk(); }
};

struct LMsgS2CGoOnCoinsDeskRet :public LMsgSC
{
	Lint		m_errorCode;//0-成功，1-金币不够，2-玩家不处于结算状态，3-未知错误 4-创建桌子失败,5-创建参数有问题
	Lint		m_remainCount; //m_errorCode=1时有效 表示玩家可以领取的金币的次数

	LMsgS2CGoOnCoinsDeskRet() :LMsgSC(MSG_S_2_C_GOON_COINS_ROOM)
	{
		m_errorCode = 0;
		m_remainCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_remainCount), m_remainCount);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_remainCount), m_remainCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CGoOnCoinsDeskRet(); }
};

struct LMsgS2CKickCoinsDesk :public LMsgSC
{
	Lint		m_reasonCode;//0-准备超时，1-金币不够，2-玩家已在金币场，3-未知原因 4-房间被解散了

	LMsgS2CKickCoinsDesk() :LMsgSC(MSG_S_2_C_KICK_COINS_ROOM)
	{
		m_reasonCode = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_reasonCode), m_reasonCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_reasonCode), m_reasonCode);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CKickCoinsDesk(); }
};

struct LMsgS2CGiveCoins :public LMsgSC
{
	Lint		m_reasonCode;// 赠送原因 1-金币不足赠送
	Lint		m_coins;	 //金币数
	Lint		m_remainCount; // 剩余次数

	LMsgS2CGiveCoins() :LMsgSC(MSG_S_2_C_GIVE_COINS)
	{
		m_reasonCode = 0;
		m_coins = 0;
		m_remainCount = 0;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_reasonCode), m_reasonCode);
		WriteKeyValue(pack, NAME_TO_STR(m_coins), m_coins);
		WriteKeyValue(pack, NAME_TO_STR(m_remainCount), m_remainCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CGiveCoins(); }
};

struct LMsgC2SGetCoins :public LMsgSC
{
	Lint m_userid;	// 用户ID 目前为了方便需要客户端填写下

	LMsgC2SGetCoins() :LMsgSC(MSG_C_2_S_GET_COINS)
	{
		m_userid = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_userid), m_userid);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userid), m_userid);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SGetCoins(); }
};

struct LMsgS2CGetCoins :public LMsgSC
{
	Lint		m_result;// 0-成功 1-剩余次数为0
	Lint		m_coins;	 //金币数
	Lint		m_remainCount; // 剩余次数

	LMsgS2CGetCoins() :LMsgSC(MSG_S_2_C_GET_COINS)
	{
		m_result = 0;
		m_coins = 0;
		m_remainCount = 0;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_result), m_result);
		WriteKeyValue(pack, NAME_TO_STR(m_coins), m_coins);
		WriteKeyValue(pack, NAME_TO_STR(m_remainCount), m_remainCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CGetCoins(); }
};

// 金币桌其他玩家的金币数
struct LMsgS2COtherCoins :public LMsgSC
{
	Lint	m_pos;
	Lint	m_coins;

	LMsgS2COtherCoins() :LMsgSC(MSG_S_2_C_OTHER_COINS)
	{
		m_pos = INVAILD_POS;
		m_coins = 0;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_coins), m_coins);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2COtherCoins(); }
};

//////////////////////////////////////////////////////////////////////////
//logic 发送玩家请求 点赞记录
struct LMsgL2LBDReqCRELog:public LMsg
{
	Lstring m_strUUID;
	Lint	m_userId;
	Lint    m_time;

	LMsgL2LBDReqCRELog() :LMsg(MSG_L_2_LDB_CRE_LOG)
	{
		m_userId = 0;
		m_time = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_time);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_time);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LBDReqCRELog();
	}
};

//////////////////////////////////////////////////////////////////////////
//一局结束，保存 本次可点赞记录
struct LMsgL2LDBSaveCRELog :public LMsg
{
	Lstring   m_strUUID;
	Lint	  m_id[4];    //玩家id
	Lint      m_deskID;   //桌子id
	Lstring   m_strLogId; //log id
	Lint      m_time;     //比赛时间

	LMsgL2LDBSaveCRELog() :LMsg(MSG_L_2_LDB_ENDCRE_LOG)
	{
		memset(m_id,0,sizeof(m_id));
		m_deskID = 0;
		m_time = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			buff.Read(m_id[i]);
		}
		buff.Read(m_deskID);
		buff.Read(m_strLogId);
		buff.Read(m_time);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			buff.Write(m_id[i]);
		}
		buff.Write(m_deskID);
		buff.Write(m_strLogId);
		buff.Write(m_time);
		return true;
	}

	virtual LMsg* Clone()
	{ 
		return new LMsgL2LDBSaveCRELog(); 
	}
};

// typedef struct credit_log_s
// {
//  	Lint	  m_id[4];    //玩家id
//  	Lint      m_deskID;   //桌子id
//  	Lstring   m_strLogId;
//  	Lint      m_time;     //时间
// }CRELog;

//////////////////////////////////////////////////////////////////////////
//logicdb 2 logicManger 返回点赞记录
struct LMsgLDB2LM_RecordCRELog :public LMsg
{
	enum
	{
		Length = 64,                   //最大容纳长度
	};
	Lstring             m_strUUID;     //用户uuid
	Lint                m_userId;      //用户id
	Lint                m_user[Length];//记录中的需要点赞玩家id
	Lint                m_count;       //记录条数
	std::vector<std::string> m_record; //记录

	LMsgLDB2LM_RecordCRELog() :LMsg(MSG_LDB_2_LM_RET_CRELOGHIS)
	{
		m_record.clear();
		m_count = 0;
		m_userId = 0;
		memset(m_user,0,sizeof(m_user));
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		for(Lint i = 0; i < Length; ++i)
		{
			buff.Read(m_user[i]);
		}
		buff.Read(m_count);
		for(Lint i = 0; i < m_count; ++i)
		{
			std::string log;
			buff.Read(log);

			m_record.push_back(log);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		Lint count = m_record.size();
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		for(Lint i = 0; i < Length; ++i)
		{
			buff.Write(m_user[i]);
		}
		buff.Write(count);
		for(Lsize i = 0; i < m_record.size(); ++i)
		{
			std::string& log = m_record[i];
			buff.Write(log);
		}
		return true;
	}

	virtual LMsg* Clone()
	{ 
		return new LMsgLDB2LM_RecordCRELog(); 
	}
};

// logicdb 删除点赞记录
struct LMsgL2LDBDEL_GTU :public LMsg
{
	Lstring   m_strUUID;
	Lint	  m_userId;    //玩家id  
	Lstring   m_strLog;    //点赞记录json

	LMsgL2LDBDEL_GTU() :LMsg(MSG_L_2_LDB_REQ_DELGTU)
	{
		m_userId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
		buff.Read(m_strLog);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		buff.Write(m_strLog);
		return true;
	}

	virtual LMsg* Clone()
	{ 
		return new LMsgL2LDBDEL_GTU(); 
	}
};

//vip比赛结束
struct LMsgLM2CEN_ADD_CRE :public LMsg
{
	Lint      m_user[4];    //logicmanager2center 增加信用值

	LMsgLM2CEN_ADD_CRE() :LMsg(MSG_LM_2_CEN_ADD_CRE)
	{
		memset(m_user,0,sizeof(m_user));
	}

	virtual bool Read(LBuff& buff)
	{
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			buff.Read(m_user[i]);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			buff.Write(m_user[i]);
		}
		return true;
	}

	virtual LMsg* Clone()
	{ 
		return new LMsgLM2CEN_ADD_CRE(); 
	}
};

//centerserver 保存用户 点赞记录入库
struct LMsgC2C_ADD_CRE :public LMsg
{
	Lstring   m_strUUID;
	Lint	  m_userId;    //玩家id   //桌子id

	LMsgC2C_ADD_CRE() :LMsg(MSG_C_2_C_ADD_CRE)
	{
		m_userId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_strUUID);
		buff.Read(m_userId);
	
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_strUUID);
		buff.Write(m_userId);
		
		return true;
	}

	virtual LMsg* Clone()
	{ 
		return new LMsgC2C_ADD_CRE(); 
	}
};

struct LMsgL2LMGExchCard :public LMsg   
{
	Lint			m_userid;
	Lstring			m_strUUID;
	Lint			m_add;
	Lint			m_del;
	Lint			cardType;
	Lint			operType;
	Lstring			admin;
	LMsgL2LMGExchCard() :LMsg(MSG_LMG_2_L_EXCH_CARD)
	{
		m_userid = 0;
		m_add = 0;
		cardType = 0;
		m_del = 0;
		operType = 0;
		cardType = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_strUUID);
		buff.Read(m_add);
		buff.Read(m_del);
		buff.Read(cardType);
		buff.Read(operType);
		buff.Read(admin);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_strUUID);
		buff.Write(m_add);
		buff.Write(m_del);
		buff.Write(cardType);
		buff.Write(operType);
		buff.Write(admin);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGExchCard();
	}
};

struct LMsgL2LMGUserCompleteGame : public LMsg
{
	Lint			m_userId;
	Lint			m_startTime;
	Lint			m_totalCircleNum;
	Lint			m_playerNum;
	Lint			m_players[DESK_USER_COUNT];

	LMsgL2LMGUserCompleteGame() : LMsg(MSG_L_2_LMG_USER_COMPLETE_GAME) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_startTime);
		buff.Read(m_totalCircleNum);
		buff.Read(m_playerNum);
		for(int i = 0; i < m_playerNum && i < DESK_USER_COUNT; ++i)
			buff.Read(m_players[i]);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_startTime);
		buff.Write(m_totalCircleNum);
		buff.Write(m_playerNum);
		for (int i = 0; i < m_playerNum && i < DESK_USER_COUNT; ++i)
			buff.Write(m_players[i]);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGUserCompleteGame();
	}
};

struct LMsgL2LMGModifyPlayCount :public LMsg
{
	Lint			m_userid;
	Lint			m_curPlayCount;
	LMsgL2LMGModifyPlayCount() :LMsg(MSG_L_2_LMG_MODIFY_USER_PLAYCOUNT)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_curPlayCount);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_curPlayCount);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGModifyPlayCount();
	}
};
struct LMsgL2LMGUserCircleIncreased : public LMsg
{
	Lint			m_userid;
	LMsgL2LMGUserCircleIncreased() :LMsg(MSG_L_2_LMG_USER_CIRCLE_INCREASED)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGUserCircleIncreased();
	}
};

// LogicManager发送给CenterServer玩家申请绑定代理
struct LMsgLMG2CEAddAgent :public LMsg
{
	Lint			m_userid;		//申请绑定的玩家
	Lint			m_agentId;		//绑定的代理人
	Lstring			m_strUUID;		//申请人的
	LMsgLMG2CEAddAgent() :LMsg(MSG_LM_2_CE_ADD_AGENT)
	{
		m_userid = 0;
		m_agentId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_agentId);
		buff.Read(m_strUUID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_agentId);
		buff.Write(m_strUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CEAddAgent();
	}
};

//CenterServer返回给LogicManager绑定代理结果
struct LMsgCE2LMGAddAgent :public LMsg
{
	Lint			m_result;		//0成功
	Lint			m_userid;		//申请绑定的玩家
	Lint			m_agentId;		//绑定的代理人
	Lstring			m_strUUID;		//申请人的
	LMsgCE2LMGAddAgent() :LMsg(MSG_CE_2_LM_ADD_AGENT)
	{
		m_result = 0;
		m_userid = 0;
		m_agentId = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_result);
		buff.Read(m_userid);
		buff.Read(m_agentId);
		buff.Read(m_strUUID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_result);
		buff.Write(m_userid);
		buff.Write(m_agentId);
		buff.Write(m_strUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LMGAddAgent();
	}
};

// LogicManager从CenterServer获取排行榜信息
struct LMsgLMG2CERank :public LMsg
{
	Lint			m_rankType;			//排行榜类型 1本周榜 2本月榜 3上周榜 4上月榜
	Lint			m_rankCount;		//排名数量
	LMsgLMG2CERank() :LMsg(MSG_LM_2_CE_RANK)
	{
		m_rankType = 0;
		m_rankCount = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_rankType);
		buff.Read(m_rankCount);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_rankType);
		buff.Write(m_rankCount);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CERank();
	}
};

struct RankData
{
	Lint id;
	Lint score;

	bool Read(LBuff& buff)
	{
		buff.Read(id);
		buff.Read(score);
		return true;
	}

	bool Write(LBuff& buff)
	{
		buff.Write(id);
		buff.Write(score);
		return true;
	}
};

// CenterServer返回排行榜信息给LogicManager
struct LMsgCE2LMGRank :public LMsg
{
	Lint					m_rankType;			//排行榜类型 1本周榜 2本月榜 3上周榜 4上月榜
	Lint					m_rankCount;		//排名数量
	std::vector<RankData>	m_rankData;
	std::vector<int>		m_rankRewardId;		//已经领取过上次奖励的玩家的ID
	LMsgCE2LMGRank() :LMsg(MSG_CE_2_LM_RANK)
	{
		m_rankType = 0;
		m_rankCount = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_rankType);
		buff.Read(m_rankCount);
		m_rankData.reserve(m_rankCount);
		for(int i=0; i<m_rankCount; ++i)
		{
			RankData rd;
			rd.Read(buff);
			m_rankData.push_back(rd);
		}
		int count = 0;
		buff.Read(count);
		for(int i=0; i<count; ++i)
		{
			int id;
			buff.Read(id);
			m_rankRewardId.push_back(id);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		m_rankCount = m_rankData.size();
		buff.Write(m_rankType);
		buff.Write(m_rankCount);
		for(int i=0; i<m_rankCount; ++i)
		{
			m_rankData[i].Write(buff);
		}
		int count = m_rankRewardId.size();
		buff.Write(count);
		for(int i=0; i<count; ++i)
		{
			buff.Write(m_rankRewardId[i]);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LMGRank();
	}
};

struct LMsgLMG2CEGetRankReward :public LMsg
{
	Lint			m_rankType;			//排行榜类型 1本周榜 2本月榜 3上周榜 4上月榜
	Lint			m_uid;				//玩家
	LMsgLMG2CEGetRankReward() :LMsg(MSG_LM_2_CE_GET_RANK_REWARD)
	{
		m_rankType = 0;
		m_uid = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_rankType);
		buff.Read(m_uid);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_rankType);
		buff.Write(m_uid);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CEGetRankReward();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic -> logicdb
//每局完成会发送一次 方便记录各种任务需要的数据
struct LMsgL2LDBTaskInfo:public LMsg
{
	Lint	m_userId[4];		// 玩家ID 第一个是房主
	Lint	m_hu[4];			// 和牌方式 0没有胡 1自摸 2收炮 3点炮		同LMsgS2CGameOver消息中m_win一致
	Lint	m_zhuang;			// 当前庄家ID
	Lint	m_create;			// 0 不计算开房次数 1 计算

	LMsgL2LDBTaskInfo() :LMsg(MSG_L_2_LDB_TASK_INFO)
	{
		memset(m_userId, 0, sizeof(m_userId));
		memset(m_hu, 0, sizeof(m_hu));
		m_zhuang = 0;
		m_create = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		for(int i=0; i<4; ++i)
			buff.Read(m_userId[i]);

		for(int i=0; i<4; ++i)
			buff.Read(m_hu[i]);

		buff.Read(m_zhuang);
		buff.Read(m_create);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		for(int i=0; i<4; ++i)
			buff.Write(m_userId[i]);

		for(int i=0; i<4; ++i)
			buff.Write(m_hu[i]);

		buff.Write(m_zhuang);
		buff.Write(m_create);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBTaskInfo();
	}
};


#endif