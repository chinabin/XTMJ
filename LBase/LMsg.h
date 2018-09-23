#ifndef _L_MSG_H_
#define _L_MSG_H_

#include "GameDefine.h"
#include "LSocket.h"
#include "LSingleton.h"
#include "LTool.h"
#include "LLog.h"
#include "LVideo.h"


struct LMsg;

class LMsgFactory : public LSingleton<LMsgFactory>
{
public:
	virtual bool Init();

	virtual bool Final();

	void RegestMsg(LMsg* pMsg);

	LMsg* CreateMsg(int msgId);

private:
	std::map<int,LMsg*> m_msgMap;
};


struct LMsg
{
	LMsg(int id);
	virtual~LMsg();

	LBuffPtr		GetSendBuff();

	void			WriteMsgId(LBuff& buff);
	void			ReadMsgId(LBuff& buff);

	virtual bool	Write(LBuff& buff);
	virtual bool	Write(msgpack::packer<msgpack::sbuffer>& pack);
	virtual bool	Read(LBuff& buff);
	virtual bool	Read(msgpack::object& obj);
	virtual LMsg*	Clone();
	virtual Lint	preAllocSendMemorySize();

	LBuffPtr			m_send;
	LSocketPtr			m_sp;
	Lint				m_msgId;
	bool				m_isMsgPack;
};

//客户端 服务器交互的消息
struct LMsgSC :public LMsg
{
	LMsgSC(int id) :LMsg(id)
	{
		m_isMsgPack = true;
	}
};
//玩家状态信息定义
enum LGUserMsgState
{
	LGU_STATE_CENTER,		//玩家在大厅，发到Center
	LGU_STATE_DESK,			//玩家在桌子，发到Logic
	LGU_STATE_COIN,			//玩家在金币场，发到CoinsServer
	LGU_STATE_COINDESK,		//玩家在金币场桌子，发到Logic
};
//服务器端跟客户端通讯的消息号
enum LMSG_ID
{
	MSG_ERROR_MSG = 0,//错误消息

	MSG_C_2_S_MSG = 1,//客户端发给服务器
	MSG_S_2_C_MSG = 2,//服务器返回结果

	MSG_C_2_S_LOGIN =	11, //客户端请求登录
	MSG_S_2_C_LOGIN =	12, //服务器返回客户登录结果

	MSG_S_2_C_ITEM_INFO = 13,  //返回玩家物品信息
	MSG_S_2_C_HORSE_INFO = 14,  //跑马灯信息

	MSG_C_2_S_HEART = 15, //心跳包
	MSG_S_2_C_HEART = 16, //心跳包

	MSG_C_2_S_NOTICE = 17, //请求服务器通知
	MSG_S_2_C_NOTICE = 18, //服务器返回通知

	//vip场相关消息
	MSG_C_2_S_CREATE_ROOM = 20,//玩家请求创建房间
	MSG_S_2_C_CREATE_ROOM = 21,//玩家请求创建房间结果

	MSG_C_2_S_ADD_ROOM = 22,//玩家请求加入房间
	MSG_S_2_C_ADD_ROOM = 23,//玩家请求加入房间结果

	MSG_C_2_S_LEAVE_ROOM = 24,//玩家请求退出房间
	MSG_S_2_C_LEAVE_ROOM = 25,//玩家请求退出房间结果

	MSG_C_2_S_RESET_ROOM = 26,//申请解算房间
	MSG_S_2_C_RESET_ROOM = 27,// 服务器广播申请解算房间结果

	MSG_C_2_S_RESET_ROOM_SELECT = 28,//玩家执行申请操作
	
	MSG_S_2_C_INTO_DESK = 30,//服务器通知玩家进入桌子
	MSG_S_2_C_ADD_USER = 31,//桌子添加玩家
	MSG_S_2_C_DEL_USER = 32,//桌子删除玩家

	MSG_S_2_C_DESK_STATE = 35,//发送桌子状态

	MSG_C_2_S_READY = 36,//客户端玩家准备
	MSG_S_2_C_READY = 37,//服务器广播某个玩家准备

	MSG_S_2_C_USER_ONLINE_INFO = 40,//服务器广播玩家在线状态

	MSG_S_2_C_VIP_INFO = 41,//同步桌子信息

	MSG_C_2_S_COINS_ROOM_LIST  = 42, //预留
	MSG_S_2_C_COINS_ROOM_LIST  = 43, //预留
	MSG_C_2_S_ENTER_COINS_ROOM = 44, //玩家请求进入金币房间
	MSG_S_2_C_ENTER_COINS_ROOM = 45, //玩家请求创建房间结果

	MSG_S_2_C_START = 50,//服务器发送游戏开始消息
	MSG_S_2_C_GET_CARD = 51,//服务器发送玩家摸牌，准备出牌的消息
	
	MSG_C_2_S_PLAY_CARD = 52,//客户端请求出牌
	MSG_S_2_C_PLAY_CARD = 53,//服务器广播玩家出牌

	MSG_S_2_C_USER_THINK = 54,//服务器通知玩家思考

	MSG_C_2_S_USER_OPER = 55,//玩家操作结果
	MSG_S_2_C_USER_OPER = 56,//服务器广播玩家操作结果

	MSG_C_2_S_USER_SPEAK = 57,//玩家请求说话
	MSG_S_2_C_USER_SPEAK = 58,//服务器广播玩家说话

	MSG_S_2_C_GAME_OVER = 60,//服务器通知本场游戏结束

	MSG_C_2_S_USER_OUT_ROOM = 61,//玩家请求退出房间
	MSG_S_2_C_USER_OUT_ROOM = 62,//玩家退出房间结果

	MSG_C_2_S_USER_AIOPER = 63,  //客户端请求自动决策		已作废
	MSG_S_2_C_USER_AIOPER = 64,  //服务器自动帮决策/出牌	已作废

	MSG_S_2_C_START_HU = 65,//玩家起手胡
	MSG_C_2_S_START_HU_SELECT = 66,//玩家选择起手胡
	MSG_S_2_C_START_HU_SELECT = 67,//玩家选择起手胡结果
	
	MSG_S_2_C_GANG_RESULT = 68,//玩家杠牌结果

	MSG_S_2_C_END_CARD_SELECT = 69,//服务器广播海底牌选择
	MSG_C_2_S_END_SELECT = 70,//玩家选择结果
	MSG_S_2_C_END_GET = 71,//服务器广播玩家摸海底牌
	MSG_S_2_C_REMOVE_QIANGGANG_CARD = 72,//移除抢杠胡的手牌			已作废

	MSG_S_2_C_FANPIGU_BUPAI = 73, //服务器发送翻屁股补牌消息		已作废
	MSG_C_2_S_FANPIGU_BUPAI_RESULT = 74, //玩家翻屁股补牌结果消息	已作废
	MSG_S_2_C_FANPIGU_BROADCAST = 75, //翻屁股新牌广播				已作废

	MSG_S_2_C_VIP_END = 80,//游戏结束

	MSG_C_2_S_VIP_LOG = 90,//玩家请求vip房间对账单
	MSG_S_2_C_VIP_LOG = 91,//玩家请求vip房间对账单结果

	MSG_C_2_S_VIDEO = 92,//玩家请求录像
	MSG_S_2_C_VIDEO = 93,//玩家请求录像

	MSG_S_2_C_ACTIVE_INFO = 94,				//通知客户端活动状态  已作废
	MSG_C_2_S_REQUEST_DRAW = 95,			//客户端请求抽卡   已作废
	MSG_S_2_C_FINISH_DRAW = 96,				//通知客户端抽卡结果  已作废
	MSG_C_2_S_WRITE_PHONE = 97,				//客户端请求写入电话  已作废
	MSG_S_2_C_WRITE_PHONE = 98,				//通知客户端写入电话结果  已作废
	MSG_C_2_S_ACTIVE_LOG = 99,				//客户端请求活动日志  已作废
	MSG_S_2_C_ACTIVE_LOG = 100,				//返馈客户端参与活动日志  已作废
	MSG_C_2_S_ACTIVE_INFO = 101,			//客户端请求活动信息  已作废
	MSG_S_2_C_ACTIVE_OPEN = 102,			//通知客户端当前是否有活动  已作废

	MSG_S_2_C_USER_DINGQUE_START = 110,			//通知玩家定缺 已作废
	MSG_C_2_S_USER_DINGQUE = 111,			//玩家回复定缺 已作废

	MSG_S_2_C_START_BOTTOM_POUR = 112,			//通知玩家开始下跑
	MSG_C_2_S_BOTTOM_POUR = 113,				//玩家下跑	
	MSG_S_2_C_USER_BOTTOM_POUR_INFO = 114,		//某个玩家玩家下跑数据
	MSG_S_2_C_ALLUSER_BOTTOM_POUR_INFO = 115,	//所有玩家玩家下跑数据

	MSG_S_2_C_USER_DINGQUE_COMPLETE = 116,	//所有玩家定缺完毕 已作废

	MSG_C_2_S_ROOM_LOG = 117,//玩家请求vip房间对账单
	MSG_S_2_C_ROOM_LOG = 118,//玩家请求vip房间对账单结果

	// Add HuNanZhuanZhuan by hanlu 02242017 Begin
	MSG_S_2_C_GANG_THINK	= 119,
	MSG_S_2_C_ZHA_BIRD		= 120,
	MSG_S_2_C_CiPai			= 121,
	MSG_S_2_C_BaoCiTime     = 122,     
	// Add HuNanZhuanZhuan by hanlu 02242017 End


	//--------------------------------------------------------------
	MSG_C_2_S_GET_USER_INFO = 130,		// 请求获取指定玩家的信息
	MSG_S_2_C_USER_INFO = 131,			// 服务器返回玩家信息

	MSG_C_2_S_GET_INVITING_INFO = 132,	      // 请求获取邀请信息
	MSG_S_2_C_INVITING_INFO = 133,		      // 服务器返回邀请信息

	MSG_C_2_S_BIND_INVITER = 134,		      // 请求绑定邀请人
	MSG_S_2_C_BIND_INVITER = 135,		      // 服务器返回绑定邀请人结果
	MSG_C_2_S_USER_SHARED = 136,		// 玩家进行了分享
	MSG_C_2_S_REQ_LUCKY_DRAW_NUM = 137,	// 请求玩家抽奖次数
	MSG_S_2_C_LUCKY_DRAW_NUM = 138,		// 服务器推送玩家抽奖次数

	MSG_C_2_S_TANGCARD = 139,			// 玩家发起廊起请求消息
	MSG_S_2_C_TANGCARD_RET = 140,		// 服务器发回廊起请求验证结果

	MSG_C_2_S_REQ_CREHIS = 141,         //玩家请求点赞记录
	MSG_S_2_C_RET_CREHIS = 142,         //服务器返回客户端 点赞记录

	MSG_C_2_S_REQ_GTU = 143,           //客户端 点赞
	MSG_S_2_C_RET_GTU = 144,           //服务器返回点赞结果

	MSG_C_2_S_REQ_EXCH = 145,          //换金币
	MSG_S_2_C_RET_EXCH = 146,          //结果
	//MSG_S_2_C_VIP_LOG = 91,//玩家请求vip房间对账单结果

	//////////////////////////////////////////////////////////////////////////
	//client 跟 server 之间消息交互
	//////////////////////////////////////////////////////////////////////////
	MSG_C_2_S_LOGIN_CENTER = 150,//客户端请求登录中心服务器
	MSG_S_2_C_LOGIN_CENTER = 150,//客户端请求登录中心服务器

	MSG_C_2_S_LOGIN_SERVER = 160,//客户端请求登录服务器
	MSG_S_2_C_LOGIN_SERVER = 160,//客户端请求登录服务器

	MSG_S_2_C_GHOST_CARD = 161,//服务器广播鬼牌

	//client and gate
	MSG_C_2_S_LOGIN_GATE = 170,	//客户端登录Gate
	MSG_S_2_C_LOGIN_GATE = 171,	//Gate回客户端登录消息

	// 工会相关接口
	MSG_C_2_S_GONGHUI_INFO = 180, // 客戶端请求用户工会信息
	MSG_S_2_C_GONGHUI_INFO = 181, // 服务器返回用户工会信息

	MSG_C_2_S_GONGHUI_APPLY = 182, // 客户端申请加入工会
	MSG_C_2_S_GONGHUI_USEROP = 184, // 会长操作工会用户

	MSG_S_2_C_GONGHUI_OPRESULT = 183,  // 通用返回消息类型，适用于只返回操作结果的消息

	MSG_C_2_S_GONGHUI_ROOMOP = 186, // 会长申请创建、删除工会策略
	//MSG_C_2_S_GONGHUI_DELETEROOM = 187,  // 删除工会房间策略

	MSG_C_2_S_GONGHUI_QUERYDESK = 188, // 查询工会房间信息(已结算的)
	MSG_S_2_C_GONGHUI_QUERYDESK = 189,

	MSG_LMG_2_L_CREATE_GONGHUIDESK = 190, // 创建工会房间

	// 工会房间状态变更消息，自动发给已登录的用户相关工会信息
	MSG_L_2_LMG_GONGHUIDESK_CHANGE = 191,  // LogicServer通知LogicManager变更消息
	MSG_S_2_C_GONGHUIDESK_CHANGE = 192,  // LogicManager通知工会下的所有人员变更消息

	MSG_C_2_S_GONGHUI_QUERYAPPLYINFO = 193, // 查询申请加入工会信息
	MSG_S_2_C_GONGHUI_QUERYAPPLYINFO = 194,

	MSG_C_2_S_QUERYDESK_HISTORY      = 195, // 查询桌子的打牌记录
	MSG_S_2_C_QUERYDESK_HISTORY      = 196, // 返回桌子的打牌记录

	MSG_S_2_C_GONHUISTATE_CHANGE     = 197, // 工会状态变更，比如新用户申请等

	// 活动 200 - 300
	MSG_S_2_C_ACTIVITY_INFO = 200,			//通用的活动内容 推动给客户端
	MSG_C_2_S_ACTIVITY_WRITE_PHONE = 201,	//填写活动相关的电话号码
	MSG_S_2_C_ACTIVITY_WRITE_PHONE = 202,
	MSG_C_2_S_ACTIVITY_REQUEST_LOG = 203,	//请求中奖纪录
	MSG_S_2_C_ACTIVITY_REPLY_LOG = 204,		//返回中奖纪录

	// 转盘
	MSG_C_2_S_ACTIVITY_REQUEST_DRAW_OPEN = 211,	//请求打开转盘
	MSG_S_2_C_ACTIVITY_REPLY_DRAW_OPEN = 212,	//返回请求打开转盘
	MSG_C_2_S_ACTIVITY_REQUEST_DRAW = 213,	//客户端请求抽卡
	MSG_S_2_C_ACTIVITY_REPLY_DRAW = 214,	//通知客户端抽卡结果
	
	// 分享
	MSG_C_2_S_ACTIVITY_REQUEST_SHARE = 220,	//客户端请求分享 
	MSG_S_2_C_ACTIVITY_FINISH_SHARE = 221,	//通知客户端分享结果

	// 排行榜
	MSG_C_2_S_UPDATE_RANK = 230,			// 客户端获取排行榜数据
	MSG_S_2_C_UPDATE_RANK = 231,			// 服务器返回排行榜数据
	MSG_C_2_S_GET_RANK_REWARD = 232,		// 客户端获取排行榜奖励
	MSG_S_2_C_GET_RANK_REWARD = 233,		// 返回结果

	MSG_C_2_S_LOCATION = 240,				// 同步位置信息
	MSG_S_2_C_LOCATION = 241,

	// 分享领钻石 之前的消息都不管
	MSG_C_2_S_SHARE = 242,
	MSG_S_2_C_SHARE = 243,

	MSG_C_2_S_TASK_INFO = 244,			// 获取任务信息
	MSG_S_2_C_TASK_INFO = 245,
	MSG_C_2_S_TASK_REWARD = 246,		// 领取任务奖励
	MSG_S_2_C_TASK_REWARD = 247,

	//客户端包头验证头
	MSG_C_2_S_VERIFY_HEAD = 500,

	MSG_S_2_C_MAX = 4096, //服务器客户端之间最大消息号

	//////////////////////////////////////////////////////////////////////////
	//下面这是服务器与服务器之间的消息交互
	//////////////////////////////////////////////////////////////////////////
	//logic 跟 center 之间的交互

	MSG_L_2_CE_LOGIN = 6000, //logic登录center
	
	MSG_L_2_CE_GATE_INFO = 6002, //logic同步gate信息到center

	MSG_CE_2_L_GONHUI_INFO = 6001, // center同步工会信息
	MSG_CE_2_L_USER_ID_INFO = 6003, //center同步玩家id信息

	MSG_CE_2_L_USER_LOGIN = 6004, //center同步玩家请求登录的消息

	MSG_L_2_CE_MODIFY_USER_NEW = 6005, //logic同步玩家更新的消息

	MSG_L_2_CE_ROOM_SAVE = 6006, //logic同步更新room的消息s

	MSG_L_2_CE_USER_LOGIN = 6011,			//同步Center玩家当前登陆某服务器

	MSG_L_2_CE_USER_LOGOUT = 6012,			//同步Center玩家当前登出某服务器

	MSG_CE_2_L_REQ_LOG_INFO = 6013, //center发送玩家请求战绩信息			---已废

	MSG_L_2_CE_REQ_VIDEO = 6014, //logic发送玩家请求录像					---已废

	MSG_CE_2_L_REQ_VIDEO = 6015, //center发送玩家请求录像					---已废

	MSG_CE_2_L_GM_CHARGE = 6020, //center发送gm充值请求

	MSG_CE_2_L_GM_HORSE = 6021, //center发送gm跑马灯

	MSG_CE_2_L_GM_BUYINFO = 6022, //center发送gm够买提示

	MSG_CE_2_L_GM_HIDE = 6023, //center发送gmyincang

	MSG_CE_2_L_GM_COINS = 6024, //center发送gm充值金币

	MSG_LMG_2_CE_SAVE_CARD = 6400, //logicmanager发给center来保存房卡		---暂时未增加该功能

	MSG_CE_2_L_SET_GAME_FREE = 6401,	//设置游戏免费

	MSG_CE_2_L_SET_DRAWACTIVE = 6402,	//设置抽奖活动配置 已作废

	MSG_CE_2_L_SET_PXACTIVE = 6403,		//设置牌型活动配置

	MSG_CE_2_L_SET_OUGCACTIVE = 6404,	//设置老玩家送卡活动配置

	MSG_CE_2_L_SET_EXCHACTIVE = 6405,	//设置老玩家送卡活动配置

	MSG_CE_2_L_SET_ACTIVITY = 6406,		//设置活动
	//////////////////////////////////////////////////////////////////////////
	//gete 跟 logic 之间的交互
	MSG_G_2_L_LOGIN = 7000, //gate登录logic		由LogicManager转发

	MSG_G_2_L_USER_MSG = 8000, //gate转发玩家消息到logic
	MSG_L_2_G_USER_MSG = 8001, //logic转发玩家消息到gate

	MSG_G_2_L_USER_OUT_MSG = 8002, //gate转发玩家退出消息到logic
	MSG_L_2_G_USER_OUT_MSG = 8003, //logic转发玩家退出消息到gate

	MSG_L_2_G_MODIFY_USER_STATE = 8006,	//修改玩家状态 gate to logic 


//////////////////////////////////////////////////////////////////////////
//LogicManager 与 Logic
	MSG_L_2_LMG_LOGIN = 11001,				//逻辑管理登陆
	MSG_LMG_2_L_RLOG_INFO = 11002,			//远程日志信息

	MSG_LMG_2_L_CREATE_DESK = 11011,		//创建房间	只有玩家不在Logic上才创建
	MSG_LMG_2_L_ADDTO_DESK = 11013,			//加入房间	只有玩家不在Logic上才加入
	MSG_LMG_2_L_USER_LOGIN = 11016,			//玩家重新登陆

	MSG_L_2_LMG_MODIFY_USER_STATE = 11017,			//修改玩家状态  logicmanager to logic
	MSG_L_2_LMG_MODIFY_USER_CARD = 11018,			//修改玩家卡数
	MSG_L_2_LMG_ADD_USER_PLAYCOUNT = 11019,			//修改玩家把数
	MSG_L_2_LMG_RECYLE_DESKID = 11020,				//创建失败，或桌子解散时

	MSG_LMG_2_L_PAIXING_ACTIVE = 11021,				//LMtoL牌型活动信息

	MSG_LMG_2_L_EXCH_CARD = 11022,

	MSG_L_2_LMG_USER_CIRCLE_INCREASED	= 11023,		//玩家又玩了一局
	MSG_L_2_LMG_USER_COMPLETE_GAME		= 11024,		//玩家完成一把游戏
	MSG_L_2_LMG_SPORT_RESULT			= 11025,		//比赛结果返回 
	MSG_L_2_LMG_MODIFY_USER_PLAYCOUNT	= 11026,

//LogicManager 与 Gate
	MSG_G_2_LMG_LOGIN = 12001,			//逻辑管理登陆
	MSG_LMG_2_G_SYNC_LOGIC = 12010,		//同步逻辑信息
	MSG_LMG_2_G_HORSE_INFO = 12011,		//跑马灯


//LogicManager 与 Gate 与 Logic
	MSG_LMG_2_GL_COINS_SERVER_INFO = 13001,     //金币服务器的信息
	

	//////////////////////////////////////////////////////////////////////////
	//logicmanager 和 logicdb 交互消息
	MSG_LMG_2_LDB_USER_LOGIN = 14901,	//用户登陆
	MSG_LDB_2_LMG_USER_MSG   = 14902,	//发送给用户的消息

	MSG_LMG_2_LDB_LOGIN = 14999,	//LogicManager 登入LogicDB
	

	//logic 和 logicdb 直接的消息交互
	MSG_L_2_LDB_LOGIN = 15000, //logic 登录 logicdb

	MSG_L_2_LDB_VIP_LOG = 15001,//logic 请求 vip战绩
	MSG_LDB_2_L_VIP_LOG = 15002,//logicdb 返回 vip战绩

	MSG_L_2_LDB_VIDEO  = 15003,//logic 请求 video
	MSG_LDB_2_L_VIDEO  = 15004,//logicdb 返回 video

	MSG_L_2_LDB_ROOM_SAVE = 15005, //logic同步更新room的消息
	MSG_L_2_LDB_VIDEO_SAVE = 15006, //logic同步更新video的消息

	MSG_L_2_LDB_REQ_ACTIVE = 15007,		//Logic请求活动  已作废
	MSG_LDB_2_L_REQ_ACTIVE = 15008,		//Center返回请求活动  已作废
	MSG_L_2_LDB_SYC_DRAW = 15009,			//Center返回请求活动  已作废
	MSG_L_2_LDB_SYC_PHONE = 15010,		//Center返回请求活动  已作废

	MSG_LDB_2_L_LOG_INFO = 15011, //center同步未完成的桌子信息

	//MSG_L_2_LDB_ROOM_LOG = 15014,//logic 请求room vip战绩
	MSG_LDB_2_L_ROOM_LOG = 15015,//logicdb 返回room vip战绩

	MSG_LM_2_LDB_BIND_INVITER = 15016,		      // 请求绑定邀请人
	MSG_LDB_2_LMG_REQ_BINDING_RELATIONSHIPS = 15017,

	MSG_LM_2_LDB_REQ_INFO = 15020,         //
	MSG_LDB_2_LM_RET_INFO = 15021,

	MSG_L_2_LDB_CRE_LOG = 15022,//logic 请求 vip战绩
	//MSG_LDB_2_L_VIP_LOG = 15002,//logicdb 返回 vip战绩

	MSG_L_2_LDB_ENDCRE_LOG = 15024,      //logicdb保存 点赞日志
	MSG_LDB_2_LM_RET_CRELOGHIS = 15025,  //logicdb返回点赞记录

	MSG_L_2_LDB_REQ_DELGTU = 15026,      //logicdb删除点赞记录

	MSG_L_2_LDB_TASK_INFO = 15027,			// 记录玩家任务信息

	
	//////////////////////////////////////////////////////////////////////////

	//LogicManager 与 CoinsServer
	MSG_CN_2_LMG_LOGIN = 16001,			//金币服务登陆Manager
	MSG_LMG_2_CN_LOGIN = 16002,

	MSG_CN_2_LMG_FREE_DESK_REQ = 16003,		//请求桌子号
	MSG_LMG_2_CN_FREE_DESK_REPLY = 16004,	//请求桌子回复
	MSG_CN_2_LMG_RECYCLE_DESK  = 16005,		// 释放一些桌子

	MSG_LMG_2_CN_USER_LOGIN = 16007,	//玩家在桌内时的登陆

	MSG_LMG_2_CN_ENTER_COIN_DESK = 16011,	//
	MSG_CN_2_LMG_MODIFY_USER_COINS = 16012,	//修改玩家金币

	MSG_LMG_2_CN_GET_COINS = 16013,		 //玩家领取金币 Manager转发的消息
	

	// CoinsServer 与 Gate
	MSG_G_2_CN_LOGIN = 16101,			//逻辑管理金币服务器

	// CoinsServer 与 Logic
	MSG_L_2_CN_LOGIN = 16201,			//逻辑管理金币服务器

	MSG_CN_2_L_CREATE_COIN_DESK = 16211,  //创建金币桌子
	MSG_L_2_CN_CREATE_COIN_DESK_RET = 16212,  //创建桌子返回
	MSG_L_2_CN_END_COIN_DESK = 16213,     //结算

	// CoinServer 与 Client
	MSG_C_2_S_GOON_COINS_ROOM = 16401,  //玩家金币场继续游戏
	MSG_S_2_C_GOON_COINS_ROOM = 16402,  //玩家金币场继续游戏 回复
	MSG_S_2_C_KICK_COINS_ROOM = 16403,	 //玩家被踢出房间
	MSG_S_2_C_GIVE_COINS = 16404,		 //给玩家赠送金币 服务器主动
	MSG_C_2_S_GET_COINS = 16405,		 //玩家领取金币
	MSG_S_2_C_GET_COINS = 16406,		 //返回玩家领取金币
	MSG_S_2_C_OTHER_COINS = 16407,		 //返回玩家领取金币

	//LM 2 CENTER
	MSG_LM_2_CEN_ADD_CRE = 16800,    //点赞
	MSG_C_2_C_ADD_CRE = 16801,       //点赞

	MSG_LM_2_CE_ADD_AGENT = 16802,		// 添加代理人信息
	MSG_CE_2_LM_ADD_AGENT = 16803,		// 返回添加代理人信息

	MSG_LM_2_CE_RANK = 16804,			// 获取排行榜信息
	MSG_CE_2_LM_RANK = 16805,			// 返回排行榜信息
	MSG_LM_2_CE_GET_RANK_REWARD = 16806,// 领取奖励

	/////////////////////////////////////////////////////////////////////////

	//LoginServer传递消息命令
	MSG_FROM_LOGINSERVER = 20000,	//来自login server 的消息
	MSG_TO_LOGINSERVER   = 20001,	//回给login server 的消息	

	//公用消息
	MSG_CONVERT_CLIENTMSG	= 50000,	//客户端数据包转换

	MSG_HEARDBEAT_REQUEST	= 50100,	//心跳发送
	MSG_HEARDBEAT_REPLY		= 50101,	//心跳回包	

	MSG_NEWUSER_VERIFY_REQUEST	= 50200,	//新用户校验请求
	MSG_NEWUSER_VERIFY_REPLY	= 50201,	//新用户校验返回

};

//////////////////////////////////////////////////////////////////////////
//socket断开的消息
#define MSG_CLIENT_KICK 0x7FFFFFFF

struct LMsgKick:public LMsg
{
	LMsgKick():LMsg(MSG_CLIENT_KICK){}

	virtual LMsg* Clone()
	{
		return new LMsgKick();
	}
};

//////////////////////////////////////////////////////////////////////////
//所有socket连上的消息
#define MSG_CLIENT_IN 0x7FFFFFFE

struct LMsgIn:public LMsg
{
	LMsgIn():LMsg(MSG_CLIENT_IN){}

	virtual LMsg* Clone()
	{
		return new LMsgIn();
	}
};

//////////////////////////////////////////////////////////////////////////
//http消息号
#define MSG_HTTP 0x7FFFFFFD

struct LMsgHttp:public LMsg
{
	std::string m_head;//消息头
	std::string m_body;//消息体

	LMsgHttp():LMsg(MSG_HTTP){}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_head);
		buff.Write(m_body);
		return true;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_head);
		buff.Read(m_body);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgHttp();
	}
};


// Add HuNanZhuanZhuan by hanlu 02242017 Begin
struct BirdInfo
{
	Lint		m_pos;
	CardValue	m_card;

	MSGPACK_DEFINE(m_pos, m_card); 

	BirdInfo():m_pos(-1){}
	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack,NAME_TO_STR(m_pos),m_pos);
		m_card.Write(pack);
		return true;
	}
};
// Add HuNanZhuanZhuan by hanlu 02242017 End

//////////////////////////////////////////////////////////////////////////
//客户端实体消息验证头
struct LMsgC2SVerifyHead : public LMsgSC
{
	Lstring m_strUserId;	//用户id
	int		m_iMd5Begin;	//消息实体计算MD5的起始位置
	int		m_iMd5End;		//消息实体计算MD5的结束位置
	Lstring m_strMd5;		//消息实体MD5值
	Lstring m_strVerifyKey;		//服务器给的一串验证码
	int		m_lMsgOrder;	//消息实体序列。每个消息包都需要增值
	LMsgC2SVerifyHead() : LMsgSC(MSG_C_2_S_VERIFY_HEAD)
	{
		m_iMd5Begin = -1;
		m_iMd5End   = -1;
		m_lMsgOrder = 0xFFFFFFFF;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "m_strUserId", m_strUserId);
		ReadMapData(obj, "m_iMd5Begin", m_iMd5Begin);
		ReadMapData(obj, "m_iMd5End", m_iMd5End);
		ReadMapData(obj, "m_strMd5", m_strMd5);
		ReadMapData(obj, "m_strVerifyKey", m_strVerifyKey);
		ReadMapData(obj, "m_lMsgOrder", m_lMsgOrder);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserId", m_strUserId);
		WriteKeyValue(pack, "m_iMd5Begin", m_iMd5Begin);
		WriteKeyValue(pack, "m_iMd5End", m_iMd5End);
		WriteKeyValue(pack, "m_strMd5", m_strMd5);
		WriteKeyValue(pack, "m_strVerifyKey", m_strVerifyKey);
		WriteKeyValue(pack, "m_lMsgOrder", m_lMsgOrder);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SVerifyHead();
	}
};

//客户端发送消息
struct LMsgC2SMsg :public LMsgSC
{
	Lstring			m_openId;
	Lstring			m_nike;
	Lstring			m_sign;
	Lstring			m_plate;
	Lstring			m_accessToken;
	Lstring			m_refreshToken;
	Lstring			m_md5;
	Lint			m_severID;
	Lstring			m_uuid;
	Lint			m_sex;
	Lstring			m_imageUrl;
	Lstring			m_nikename;

	LMsgC2SMsg() :LMsgSC(MSG_C_2_S_MSG)
	{
		m_severID = 0;
		m_sex = 1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "m_openId", m_openId);
		ReadMapData(obj, "m_nike", m_nike);
		ReadMapData(obj, "m_sign", m_sign);
		ReadMapData(obj, "m_plate", m_plate);
		ReadMapData(obj, "m_accessToken", m_accessToken);
		ReadMapData(obj, "m_refreshToken", m_refreshToken);
		ReadMapData(obj, "m_md5", m_md5);
		ReadMapData(obj, "m_severID", m_severID);
		ReadMapData(obj, "m_uuid", m_uuid);
		ReadMapData(obj, "m_sex", m_sex);
		ReadMapData(obj, "m_imageUrl", m_imageUrl);
		ReadMapData(obj, "m_nikename", m_nikename);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,13);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_openId", m_openId);
		WriteKeyValue(pack, "m_nike", m_nike);
		WriteKeyValue(pack, "m_sign", m_sign);
		WriteKeyValue(pack, "m_plate", m_plate);
		WriteKeyValue(pack, "m_accessToken", m_accessToken);
		WriteKeyValue(pack, "m_refreshToken", m_refreshToken);
		WriteKeyValue(pack, "m_md5", m_md5);
		WriteKeyValue(pack, "m_severID", m_severID);
		WriteKeyValue(pack, "m_uuid", m_uuid);
		WriteKeyValue(pack, "m_sex", m_sex);
		WriteKeyValue(pack, "m_imageUrl", m_imageUrl);
		WriteKeyValue(pack, "m_nikename", m_nikename);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SMsg();
	}
};

//////////////////////////////////////////////////////////////////////////
//客户端发送消息
struct LMsgS2CMsg :public LMsgSC
{
	Lint			m_errorCode;//0-成功，1-服务器还没启动成功 2-微信登陆失败 3- 微信返回失败 4-创建角色失败 5- 在原APP未退,6-错误的地区号,7-MD5验证失败,
	Lint			m_seed;
	Lint			m_id;
	Lstring			m_gateIp;
	Lshort			m_gatePort;
	Lstring			m_errorMsg;	//若登陆失败，返回玩家错误原因;
	Lint            m_totalPlayNum;

	LMsgS2CMsg() :LMsgSC(MSG_S_2_C_MSG)
	{
		 m_errorCode = -1;
		 m_seed = 0;
		 m_id = 0;
		 m_gatePort = 0;
		 m_totalPlayNum = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "m_errorCode", m_errorCode);
		ReadMapData(obj, "m_seed", m_seed);
		ReadMapData(obj, "m_id", m_id);
		ReadMapData(obj, "m_gateIp", m_gateIp);
		ReadMapData(obj, "m_gatePort", m_gatePort);
		ReadMapData(obj, "m_errorMsg", m_errorMsg);
		ReadMapData(obj, "m_totalPlayNum",m_totalPlayNum);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_errorCode", m_errorCode);
		WriteKeyValue(pack, "m_seed", m_seed);
		WriteKeyValue(pack, "m_id", m_id);
		WriteKeyValue(pack, "m_gateIp", m_gateIp);
		WriteKeyValue(pack, "m_gatePort", m_gatePort);
		WriteKeyValue(pack, "m_errorMsg", m_errorMsg);
	    WriteKeyValue(pack, "m_totalPlayNum",m_totalPlayNum);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CMsg();
	}
};

/////////////////////
//客户端发送登录Gate消息
struct LMsgC2SLoginGate : public LMsgSC
{
	Lstring m_strUserUUID;

	LMsgC2SLoginGate() : LMsgSC(MSG_C_2_S_LOGIN_GATE){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SLoginGate();
	}
};

//Gate回客户端登录信息
struct LMsgS2CLoginGate : public LMsgSC
{
	Lstring m_strUserUUID;
	Lstring m_strKey;
	unsigned long m_uMsgOrder;

	LMsgS2CLoginGate() : LMsgSC(MSG_S_2_C_LOGIN_GATE)
	{
		m_uMsgOrder = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_strKey), m_strKey);
		ReadMapData(obj, NAME_TO_STR(m_uMsgOrder), m_uMsgOrder);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, "m_strKey", m_strKey);
		WriteKeyValue(pack, "m_uMsgOrder", m_uMsgOrder);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CLoginGate();
	}
};

//////////////////////////////////////////////////////////////////////////
//客户端发送心跳包
struct LMsgC2SHeart:public LMsgSC
{
	//Lint	m_time;//时间戳

	LMsgC2SHeart() :LMsgSC(MSG_C_2_S_HEART){}

	virtual bool Read(msgpack::object& obj)
	{
		//buff.Read(m_time);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SHeart();}
};


//////////////////////////////////////////////////////////////////////////
//服务器发送心跳包
struct LMsgS2CHeart :public LMsgSC
{
	Lint	m_time;//时间戳

	LMsgS2CHeart() :LMsgSC(MSG_S_2_C_HEART)
	{
		m_time = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CHeart(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端请求通知
struct LMsgC2SNotice :public LMsgSC
{
	
	LMsgC2SNotice() :LMsgSC(MSG_C_2_S_NOTICE){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SNotice(); }
};


//////////////////////////////////////////////////////////////////////////
//服务器返回通知内容
struct Notice
{
	Notice()
	{
		m_time = 0;
	}

	Lint	m_time;//时间戳
	Lstring	m_title;//标题
	Lstring m_content;//内容

	MSGPACK_DEFINE(m_time, m_title, m_content); 
};

struct LMsgS2CNotice :public LMsgSC
{
	Lint	m_count;//时间戳
	Notice	m_data[20];

	LMsgS2CNotice() :LMsgSC(MSG_S_2_C_NOTICE)
	{
		m_count = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		m_count = 0;

		msgpack::object array;
		ReadMapData(obj, "array", array);
		if (array.type == msgpack::type::ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				msgpack::object pv = *(array.via.array.ptr+i);
				pv.convert(m_data[i]);
				++m_count;
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKey(pack, "array");
		std::vector<Notice>vce;
		for (Lint i = 0; i < m_count&&i < 20; ++i)
		{
			vce.push_back(m_data[i]);
		}
		WriteKey(pack,vce);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CNotice(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求登录大厅

struct LMsgC2SLogin:public LMsgSC
{
	Lint	m_seed;
	Lint	m_id;
	Lstring	m_md5;

	LMsgC2SLogin() :LMsgSC(MSG_C_2_S_LOGIN)
	{
		m_seed = 0;
		m_id = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_seed), m_seed);
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_md5), m_md5);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_seed), m_seed);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//返回玩家登陆结果消息
struct LMsgS2CLogin:public LMsgSC
{
	Lint		m_errorCode;//登录错误码，0-登录成功,1-帐号未注册，2-帐号已登录，3-未知错误
	Lint		m_id;
	Lstring		m_nike;
	Lint		m_exp;
	Lint		m_sex;
	Lstring		m_face;//
	Lstring		m_provice;
	Lstring		m_city;
	Lstring		m_ip;
	Lint		m_new;//0-不是，1-是
	Lint		m_card1;//新手奖励房卡1
	Lint		m_card2;//新手奖励房卡2
	Lint		m_card3;//新手奖励房卡3
	Lint		m_state;//0-进入大厅，1-进入桌子
	Lstring		m_buyInfo;//够买信息
	Lint		m_hide;//购买隐藏0-隐藏 1- 不隐藏
	Lint		m_gm;//0-不是，1-是GM
	Lint        m_oldUser;
	Lint		m_loginInterval;	// 距离上次登录多少天, 老用户登录送房卡活动使用字段
	Lint		m_inviterId;		// 代理人

	LMsgS2CLogin() :LMsgSC(MSG_S_2_C_LOGIN)
	{
		m_errorCode = -1;
		m_id = 0;
		m_exp = 0;
		m_sex = 1;
		m_new = 0;
		m_card1 = 0;
		m_card2 = 0;
		m_card3 = 0;
		m_state = 0;
		m_hide = 0;
		m_gm = 0;
		m_oldUser = 1;
		m_loginInterval = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_nike), m_nike);
		ReadMapData(obj, NAME_TO_STR(m_exp), m_exp);
		ReadMapData(obj, NAME_TO_STR(m_sex), m_sex);
		ReadMapData(obj, NAME_TO_STR(m_face), m_face);
		ReadMapData(obj, NAME_TO_STR(m_provice), m_provice);
		ReadMapData(obj, NAME_TO_STR(m_city), m_city);
		ReadMapData(obj, NAME_TO_STR(m_ip), m_ip);
		ReadMapData(obj, NAME_TO_STR(m_new), m_new);
		ReadMapData(obj, NAME_TO_STR(m_card1), m_card1);
		ReadMapData(obj, NAME_TO_STR(m_card2), m_card2);
		ReadMapData(obj, NAME_TO_STR(m_card3), m_card3);
		ReadMapData(obj, NAME_TO_STR(m_oldUser),m_oldUser);
		ReadMapData(obj, NAME_TO_STR(m_loginInterval),m_loginInterval);
		ReadMapData(obj, NAME_TO_STR(m_inviterId),m_inviterId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,21);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_nike), m_nike);
		WriteKeyValue(pack, NAME_TO_STR(m_exp), m_exp);
		WriteKeyValue(pack, NAME_TO_STR(m_sex), m_sex);
		WriteKeyValue(pack, NAME_TO_STR(m_face), m_face);
		WriteKeyValue(pack, NAME_TO_STR(m_provice), m_provice);
		WriteKeyValue(pack, NAME_TO_STR(m_city), m_city);
		WriteKeyValue(pack, NAME_TO_STR(m_ip), m_ip);
		WriteKeyValue(pack, NAME_TO_STR(m_new), m_new);
		WriteKeyValue(pack, NAME_TO_STR(m_card1), m_card1);
		WriteKeyValue(pack, NAME_TO_STR(m_card2), m_card2);
		WriteKeyValue(pack, NAME_TO_STR(m_card3), m_card3);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);
		WriteKeyValue(pack, NAME_TO_STR(m_buyInfo), m_buyInfo);
		WriteKeyValue(pack, NAME_TO_STR(m_hide), m_hide);
		WriteKeyValue(pack, NAME_TO_STR(m_gm), m_gm);
		WriteKeyValue(pack, NAME_TO_STR(m_oldUser), m_oldUser);
		WriteKeyValue(pack, NAME_TO_STR(m_loginInterval), m_loginInterval);
		WriteKeyValue(pack, NAME_TO_STR(m_inviterId), m_inviterId);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CLogin();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回玩家物品信息
struct LMsgS2CItemInfo:public LMsgSC
{
	Lint	m_card1;
	Lint	m_card2;
	Lint	m_card3;
	Lint	m_coins;
	Lint    m_credit;

	LMsgS2CItemInfo() :LMsgSC(MSG_S_2_C_ITEM_INFO)
	{
		m_card1 = 0;
		m_card2 = 0;
		m_card3 = 0;
		m_coins = 0;
		m_credit = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_card1), m_card1);
		ReadMapData(obj, NAME_TO_STR(m_card2), m_card2);
		ReadMapData(obj, NAME_TO_STR(m_card3), m_card3);
		ReadMapData(obj, NAME_TO_STR(m_coins), m_coins);
		ReadMapData(obj, NAME_TO_STR(m_credit), m_credit);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_card1), m_card1);
		WriteKeyValue(pack, NAME_TO_STR(m_card2), m_card2);
		WriteKeyValue(pack, NAME_TO_STR(m_card3), m_card3);
		WriteKeyValue(pack, NAME_TO_STR(m_coins), m_coins);
		WriteKeyValue(pack, NAME_TO_STR(m_credit), m_credit);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CItemInfo();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回跑马灯信息
struct LMsgS2CHorseInfo :public LMsgSC
{
	enum
	{
		TInfo_Horse = 0,
		TInfo_Free = 1,
		TInfo_Exch = 2,
	};

	Lstring	m_str;
	Lint    m_type;

	LMsgS2CHorseInfo() :LMsgSC(MSG_S_2_C_HORSE_INFO)
	{
		m_type = 0;   //默认0为跑马灯消息推送，1为限时免费
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_str), m_str);
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_str), m_str);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CHorseInfo(); }
};

//////////////////////////////////////////////////////////////////////////
//服务器返回玩家进入桌子
struct LMsgS2CIntoDesk:public LMsgSC
{
	Lint			m_deskId;
	Lint			m_pos;	//我的位置，0-3 分别代表东，南，西，北
	Lint			m_ready;//我是否准备，0-未准备，1-已经准备
	Lint			m_score;//我的积分
	Lint			m_coins;//我的金币
	Lint            m_credits; //信用值
	Lint			m_state;//玩法  0 转转 3 长沙 101 血流  102 血战
	Lint			m_maxCircle;//最大局数
	Lint			m_changeOutTime;// 换牌的超时时间 <=0 不限制
	Lint			m_opOutTime; // 桌内操作超时时间 <=0 不限制
	Lint			m_baseScore; // 基础分
	std::vector<Lint>	m_playtype;

	LMsgS2CIntoDesk():LMsgSC(MSG_S_2_C_INTO_DESK)
	{
		m_deskId = 0;
		m_pos = INVAILD_POS;
		m_ready = 0;
		m_score = 0;
		m_coins = 0;
		m_state = 0;
		m_maxCircle = 0;
		m_changeOutTime = 0;
		m_opOutTime = 0;
		m_baseScore = 1;
		m_credits = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_ready), m_ready);
		ReadMapData(obj, NAME_TO_STR(m_score), m_score);
		ReadMapData(obj, NAME_TO_STR(m_coins), m_coins);
		ReadMapData(obj, NAME_TO_STR(m_credits), m_credits);
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		ReadMapData(obj, NAME_TO_STR(m_maxCircle), m_maxCircle);
		ReadMapData(obj, NAME_TO_STR(m_playtype), m_playtype);
		ReadMapData(obj, NAME_TO_STR(m_changeOutTime), m_changeOutTime);
		ReadMapData(obj, NAME_TO_STR(m_opOutTime), m_opOutTime);
		ReadMapData(obj, NAME_TO_STR(m_baseScore), m_baseScore);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 13);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_ready), m_ready);
		WriteKeyValue(pack, NAME_TO_STR(m_score), m_score);
		WriteKeyValue(pack, NAME_TO_STR(m_coins), m_coins);
		WriteKeyValue(pack, NAME_TO_STR(m_credits), m_credits);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);
		WriteKeyValue(pack, NAME_TO_STR(m_maxCircle), m_maxCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_playtype), m_playtype);
		WriteKeyValue(pack, NAME_TO_STR(m_changeOutTime), m_changeOutTime);
		WriteKeyValue(pack, NAME_TO_STR(m_opOutTime), m_opOutTime);
		WriteKeyValue(pack, NAME_TO_STR(m_baseScore), m_baseScore);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CIntoDesk();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回桌子添加一个玩家
struct LMsgS2CDeskAddUser:public LMsgSC
{
	Lint			m_userId;
	Lstring			m_nike;//玩家昵称
	Lstring			m_face;//玩家图形，为空就为默认图形
	Lint			m_sex;//玩家性别
	Lint			m_pos;	//位置，0-3 分别代表东，南，西，北
	Lint			m_ready;//0-未准备，1-已准备
	Lint			m_score;//玩家积分
	Lstring			m_ip;
	Lstring			m_location;
	bool			m_online;//是否在线，true在线，false不在线
	Lint			m_coins;//玩家金币
	Lint            m_credits; //信用值

	LMsgS2CDeskAddUser() :LMsgSC(MSG_S_2_C_ADD_USER)
	{
		m_userId = 0;
		m_sex = 1;
		m_pos = INVAILD_POS;
		m_ready = 0;
		m_score = 0;
		m_online = false;
		m_coins = 0;
		m_credits = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_nike), m_nike);
		ReadMapData(obj, NAME_TO_STR(m_face), m_face);
		ReadMapData(obj, NAME_TO_STR(m_sex), m_sex);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_ready), m_ready);
		ReadMapData(obj, NAME_TO_STR(m_score), m_score);
		ReadMapData(obj, NAME_TO_STR(m_ip), m_ip);
		ReadMapData(obj, NAME_TO_STR(m_location), m_location);
		ReadMapData(obj, NAME_TO_STR(m_online), m_online);
		ReadMapData(obj, NAME_TO_STR(m_coins), m_coins);
		ReadMapData(obj, NAME_TO_STR(m_credits), m_credits);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 13);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_nike), m_nike);
		WriteKeyValue(pack, NAME_TO_STR(m_face), m_face);
		WriteKeyValue(pack, NAME_TO_STR(m_sex), m_sex);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_ready), m_ready);
		WriteKeyValue(pack, NAME_TO_STR(m_score), m_score);
		WriteKeyValue(pack, NAME_TO_STR(m_ip), m_ip);
		WriteKeyValue(pack, NAME_TO_STR(m_location), m_location);
		WriteKeyValue(pack, NAME_TO_STR(m_online), m_online);
		WriteKeyValue(pack, NAME_TO_STR(m_coins), m_coins);
		WriteKeyValue(pack, NAME_TO_STR(m_credits), m_credits);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CDeskAddUser();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回删除玩家
struct LMsgS2CDeskDelUser:public LMsgSC
{
	Lint			m_pos;//删除玩家位置 如果pos == 我的位置，代表我自己被踢出桌子，把桌子其他人清掉

	LMsgS2CDeskDelUser() :LMsgSC(MSG_S_2_C_DEL_USER)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CDeskDelUser();}
};

//////////////////////////////////////////////////////////////////////////
//客户端请求推出房间
struct LMsgC2SLeaveDesk:public LMsgSC
{
	Lint			m_pos;//我的位置

	LMsgC2SLeaveDesk() :LMsgSC(MSG_C_2_S_LEAVE_ROOM)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SLeaveDesk();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回结果
struct LMsgS2CLeaveDesk:public LMsgSC
{
	Lint			m_errorCode;//0-成功，1-失败

	LMsgS2CLeaveDesk() :LMsgSC(MSG_S_2_C_LEAVE_ROOM)
	{
		m_errorCode = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CLeaveDesk();}
};

//////////////////////////////////////////////////////////////////////////
//客户端申请解算房间
struct LMsgC2SResetDesk:public LMsgSC
{
	Lint	m_pos;//位置
	LMsgC2SResetDesk():LMsgSC(MSG_C_2_S_RESET_ROOM)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SResetDesk();}
};

//////////////////////////////////////////////////////////////////////////
struct LMsgS2CResetDesk:public LMsgSC
{
	Lint					m_errorCode;//0-等待操作中，1-未开始直接解算，2-三个人同意，解算成功，3-时间到，解算成功，4-有一个人拒绝，解算失败
	Lint					m_time;//倒计时
	Lstring					m_applay;//申请的玩家
	Lstring					m_refluse;//拒绝的玩家,有一个玩家拒绝，则解算房间失败
	Lint					m_flag;//0-等待我同意或者拒绝，1-我已经同意或者拒绝                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
	std::vector<Lstring>	m_agree;//同意的玩家
	std::vector<Lstring>	m_wait;//等待操作的玩家
	

	LMsgS2CResetDesk() :LMsgSC(MSG_S_2_C_RESET_ROOM)
	{
		m_errorCode = -1;
		m_time = 0;
		m_flag = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_apply), m_applay);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		
		WriteKey(pack,"m_agree");
		WriteKey(pack, m_agree);

		WriteKey(pack,"m_refuse");
		WriteKey(pack,m_refluse);

		WriteKey(pack, "m_wait");
		WriteKey(pack, m_wait);

		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CResetDesk();}
};

//////////////////////////////////////////////////////////////////////////
//客户端同意或者拒绝操作
struct LMsgC2SSelectResetDesk:public LMsgSC
{
	Lint	m_pos;//位置
	Lint	m_flag;//1-同意，2-拒绝
	LMsgC2SSelectResetDesk():LMsgSC(MSG_C_2_S_RESET_ROOM_SELECT)
	{
		m_pos = INVAILD_POS;
		m_flag = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SSelectResetDesk();}
};

struct TingData
{
	struct TingCard
	{
		CardValue m_card;
		Lint m_hufan;

		MSGPACK_DEFINE(m_card, m_hufan);
	};

	CardValue	m_card;
	std::vector<TingCard> m_tingcard;

	MSGPACK_DEFINE(m_card, m_tingcard);
};

//////////////////////////////////////////////////////////////////////////

struct PengInfo
{
	CardValue card;
	Lint	pos;
	PengInfo():pos(0){}

	MSGPACK_DEFINE(card, pos);
};
typedef std::vector<PengInfo> PengInfoVector;

//服务器发送桌子状态，这个消息主要用于重连用
struct LMsgS2CDeskState:public LMsgSC
{
#define   MAX_CARD_NUM   128
	Lint		m_state;  //当前桌子状态 1-等待，2-游戏中，会有后面的数据
	Lint		m_time;   //倒计时
	Lint		m_flag;   //0-等待别人，1-该我出牌，2-该我思考
	Lint		m_zhuang; //庄的位置
	Lint		m_dCount; //桌面剩余的牌
	Lint		m_pos;    //我的位置
	Lint		m_cardCount[4];//0-3号玩家手上的牌数量
	Lint		m_myPos;
	Lint        m_score[4];

	Lint		m_cardPos1;		// 桌面起始摸牌的位置 0-3
	Lint		m_cardPos2;		// 2-12
	Lint		m_cardPos3;		// 1-6

	Lint		m_gangCount;	// 所有玩家杠的数量

	//我手上的牌
	CardValue	m_cardValue[14];

	//碰的牌
	Lint		m_pCount[4];
	CardValue	m_pCard[4][MAX_CARD_NUM];
	
	//暗杠的牌
	Lint		m_aCount[4];
	CardValue	m_aCard[4][MAX_CARD_NUM];

	//明杠的牌
	Lint		m_mCount[4];
	CardValue	m_mCard[4][MAX_CARD_NUM];

	//出出去的牌
	Lint		m_oCount[4];
	CardValue	m_oCard[4][MAX_CARD_NUM];

	//吃的牌
	Lint		m_eCount[4];
	CardValue	m_eCard[4][MAX_CARD_NUM];

	//花牌
	Lint		m_fCount[4];
	CardValue	m_fCard[4][8];

	//亮四打一玩法中每个人亮出的牌
	Lint		m_sCount[4];
	CardValue	m_sCard[4][4];
	//0没有出过牌 1出过牌
	Lint		m_outState[4];

	CardValue	m_ghostCard;

	Lint		m_ting[4];
	std::vector<TingData::TingCard>    m_tingCard;	// 自己听牌后的数据
	Lint		m_tingPos[4];						// 记录每个玩家听牌时桌面已出的牌张数+1，方便客户端将听牌时打出的牌盖住

	Lint		m_user_count;
	CardValue	m_au;
	CardValue	m_winCard[4];	// 玩家的胡牌

	PengInfoVector m_pengInfo[4];	// 具体的碰杠指向
	PengInfoVector m_gangInfo[4];

	LMsgS2CDeskState() :LMsgSC(MSG_S_2_C_DESK_STATE)
	{
		m_state = 0;
		m_time = 0;
		m_flag = 0;
		m_zhuang = 0;
		m_dCount = 0;
		m_pos = INVAILD_POS;
		m_myPos = INVAILD_POS;
		m_user_count = 0;
		m_cardPos1 = 0;
		m_cardPos2 = 2;
		m_cardPos3 = 1;
		m_gangCount = 0;

		memset( m_cardCount, 0, sizeof(m_cardCount) );
		memset( m_score, 0, sizeof(m_score));
		memset( m_pCount, 0, sizeof(m_pCount) );
		memset( m_aCount, 0, sizeof(m_aCount) );
		memset( m_mCount, 0, sizeof(m_mCount) );
		memset( m_oCount, 0, sizeof(m_oCount) );
		memset( m_eCount, 0, sizeof(m_eCount) );
		memset( m_fCount, 0, sizeof(m_fCount) );
		memset( m_sCount, 0, sizeof(m_sCount) );
		memset( m_outState, 0, sizeof(m_outState));
		memset( m_ting, 0, sizeof(m_ting) );
		memset( m_tingPos, 0, sizeof(m_tingPos) );
		memset(&m_au, 0x00, sizeof(m_au));
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_zhuang), m_zhuang);
		ReadMapData(obj, NAME_TO_STR(m_dCount), m_dCount);

		//////////////////////////////////////////////////////////////////////////
		// au
		ReadMapData(obj, NAME_TO_STR(m_au_color), m_au.m_color);
		ReadMapData(obj, NAME_TO_STR(m_au_number), m_au.m_number);
		//////////////////////////////////////////////////////////////////////////

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_myCard), array);
		if (array.type == msgpack::type::ARRAY)//MSGPACK_OBJECT_ARRAY)
		{
			m_cardCount[m_pos] = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_cardValue[i]);	
			}
		}
		m_ghostCard.Read(obj);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 59);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);

		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_zhuang), m_zhuang);
		WriteKeyValue(pack, NAME_TO_STR(m_dCount), m_dCount);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_user_count), m_user_count);
		
		//////////////////////////////////////////////////////////////////////////
		// au
		WriteKeyValue(pack, NAME_TO_STR(m_au_color), m_au.m_color);
		WriteKeyValue(pack, NAME_TO_STR(m_au_number), m_au.m_number);
		//////////////////////////////////////////////////////////////////////////

		WriteKeyValue(pack, NAME_TO_STR(m_cardPos1), m_cardPos1);
		WriteKeyValue(pack, NAME_TO_STR(m_cardPos2), m_cardPos2);
		WriteKeyValue(pack, NAME_TO_STR(m_cardPos3), m_cardPos3);
		WriteKeyValue(pack, NAME_TO_STR(m_gangCount), m_gangCount);

		std::vector<CardValue> vec;
		WriteKey(pack, "m_CardCount");
		std::vector<Lint> veci;
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_cardCount[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_myCard");
		vec.clear();
		for (Lint i = 0; i < m_cardCount[m_myPos]; ++i)
		{
			vec.push_back(m_cardValue[i]);
		}
		WriteKey(pack,vec);

		WriteKey(pack, "m_score");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_score[i]);
		}
		WriteKey(pack,veci);

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name,"m_pCard%d",i);
			WriteKey(pack, name);
			vec.clear();
			for (Lint j = 0; j < m_pCount[i]; j+=3)
			{
				vec.push_back(m_pCard[i][j]);
			}
			WriteKey(pack,vec);
		}

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name,"m_aCard%d",i);
			WriteKey(pack, name);
			vec.clear();
			for (Lint j = 0; j < m_aCount[i]; j+=4)
			{
				vec.push_back(m_aCard[i][j]);
			}
			WriteKey(pack,vec);
		}

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name,"m_mCard%d",i);
			WriteKey(pack, name);
			vec.clear();
			for (Lint j = 0; j < m_mCount[i]; j+=4)
			{
				vec.push_back(m_mCard[i][j]);
			}
			WriteKey(pack,vec);
		}

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "m_oCard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_oCount[i]; ++j)
			{
				vec.push_back(m_oCard[i][j]);
			}
			WriteKey(pack,vec);
		}
		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "m_eCard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_eCount[i]; ++j)
			{
				vec.push_back(m_eCard[i][j]);
			}
			WriteKey(pack,vec);
		}

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "m_fCard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_fCount[i]; ++j)
			{
				vec.push_back(m_fCard[i][j]);
			}
			WriteKey(pack,vec);
		}

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "m_sCard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_sCount[i]; ++j)
			{
				vec.push_back(m_sCard[i][j]);
			}
			WriteKey(pack,vec);
		}

		WriteKey(pack, "m_outState");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_outState[i]);
		}
		WriteKey(pack,veci);

		m_ghostCard.Write(pack);

		WriteKey(pack, "m_ting");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_ting[i]);
		}
		WriteKey(pack,veci);

		WriteKeyValue(pack, NAME_TO_STR(m_tingCard), m_tingCard);

		WriteKey(pack, "m_tingPos");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_tingPos[i]);
		}
		WriteKey(pack,veci);
		
		WriteKey(pack, "m_winCard");
		vec.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			vec.push_back(m_winCard[i]);
		}
		WriteKey(pack,vec);
		
		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "m_pengInfo%d", i);
			WriteKeyValue(pack, std::string(name), m_pengInfo[i]);
		}

		for (Lint i = 0; i < 4; ++i)
		{
			char name[32];
			sprintf(name, "m_gangInfo%d", i);
			WriteKeyValue(pack, std::string(name), m_gangInfo[i]);
		}

		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CDeskState(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求准备
struct LMsgC2SUserReady:public LMsgSC
{
	Lint m_pos;

	LMsgC2SUserReady() :LMsgSC(MSG_C_2_S_READY)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserReady();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回玩家准备
struct LMsgS2CUserReady:public LMsgSC
{
	Lint			m_pos;//准备玩家的位置

	LMsgS2CUserReady() :LMsgSC(MSG_S_2_C_READY)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserReady();}
};


//////////////////////////////////////////////////////////////////////////
//服务器返回玩家准备
struct LMsgS2CUserOnlineInfo:public LMsgSC
{
	Lint			m_flag;//0-掉线了，1-回来了
	Lint			m_pos;//

	LMsgS2CUserOnlineInfo() :LMsgSC(MSG_S_2_C_USER_ONLINE_INFO)
	{
		m_flag = 0;
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CUserOnlineInfo(); }
};
//////////////////////////////////////////////////////////////////////////
//服务器广播游戏开始

struct LMsgS2CPlayStart:public LMsgSC
{
	Lint		m_zhuang;
	Lint		m_dCount;//桌面剩余的牌	
	Lint		m_cardCount[4];//0-3号玩家手上的牌数量
	Lint		m_pos;
	Lint		m_cardPos1;		// 桌面起始摸牌的位置 0-3
	Lint		m_cardPos2;		// 2-12
	Lint		m_cardPos3;		// 周口玩法中 赖子位置 1-6
	Lint		m_gangCount;	// // 所有玩家杠的数量 这儿不用填写 只是为了客户端处理
	std::vector<Lint> m_score;

	//我手上的牌
	CardValue	m_cardValue[14];
	std::vector<CardValue> m_flowerCards[DESK_USER_COUNT];	// 每个人翻出的花牌
	std::vector<CardValue> m_showCards[DESK_USER_COUNT]; 	// 亮四打一玩法中每个人亮出的牌
	std::vector<TingData::TingCard>    m_tingCard;			// 自己听牌后的数据 非报听玩法的时候也要听的数据

	LMsgS2CPlayStart() :LMsgSC(MSG_S_2_C_START)
	{
		m_zhuang = INVAILD_POS;
		m_dCount = 0;
		m_pos = INVAILD_POS;
		for ( int i = 0; i < 4; ++i )
		{
			m_cardCount[i] = 0;
		}
		m_cardPos1 = 0;
		m_cardPos2 = 2;
		m_cardPos3 = 3;
		m_gangCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_myCard), array);
		if (array.type == msgpack::type::ARRAY)//MSGPACK_OBJECT_ARRAY)
		{
			//m_cardCount[0] = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_cardValue[i]);
			}
		}

		msgpack::object array1;
		ReadMapData(obj, NAME_TO_STR(m_CardCount), array1);
		if (array1.type == msgpack::type::ARRAY)//MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array1.via.array.size; ++i)
			{
				msgpack::object& pv = *(array1.via.array.ptr + i);
				pv.convert(m_cardCount[i]);
			}
		}

		ReadMapData(obj, NAME_TO_STR(m_zhuang), m_zhuang);
		ReadMapData(obj, NAME_TO_STR(m_dCount), m_dCount);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 19);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_score), m_score);

		WriteKey(pack, "m_myCard");
		std::vector<CardValue>vce;
		for (Lint i = 0; i < m_cardCount[m_pos]; ++i)
		{
			vce.push_back(m_cardValue[i]);
		}
		WriteKey(pack,vce);

		WriteKey(pack, "m_CardCount");
		std::vector<Lint>vcei;
		for(Lint i = 0 ; i < 4;++i)
		{
			vcei.push_back(m_cardCount[i]);
		}
		WriteKey(pack,vcei);

		WriteKeyValue(pack, NAME_TO_STR(m_dCount),m_dCount);
		WriteKeyValue(pack, NAME_TO_STR(m_zhuang), m_zhuang);
		WriteKeyValue(pack, NAME_TO_STR(m_cardPos1), m_cardPos1);
		WriteKeyValue(pack, NAME_TO_STR(m_cardPos2), m_cardPos2);
		WriteKeyValue(pack, NAME_TO_STR(m_cardPos3), m_cardPos3);
		WriteKeyValue(pack, NAME_TO_STR(m_gangCount), m_gangCount);

		for (Lint idx = 0; idx < DESK_USER_COUNT; ++idx)
		{
			char name[32];
			sprintf_s(name, sizeof(name), "m_fCard%d", idx);
			WriteKey(pack, name);
			WriteKey(pack, m_flowerCards[idx]);
		}

		for (Lint idx = 0; idx < DESK_USER_COUNT; ++idx)
		{
			char name[32];
			sprintf_s(name, sizeof(name), "m_sCard%d", idx);
			WriteKey(pack, name);
			WriteKey(pack, m_showCards[idx]);
		}

		WriteKeyValue(pack, NAME_TO_STR(m_tingCard), m_tingCard);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CPlayStart();}
};

//////////////////////////////////////////////////////////////////////////
//服务器广播玩家摸牌

struct ThinkData
{
	Lint	m_type;
	std::vector<CardValue> m_card;

	ThinkData()
	{
		m_type = 0;
	}

	MSGPACK_DEFINE(m_type, m_card); 
};



struct LMsgS2COutCard:public LMsgSC
{
	Lint		m_pos;//当前出牌中的玩家
	Lint		m_time;//出牌倒计时
	Lint		m_deskCard;//桌面剩余的牌
	Lint		m_flag;//当前是否需要摸牌，0-需要，1-不需要
	Lint		m_gang;//是否杠过，0-没有，1-杠过
	Lint		m_end;//是否海底 0-不是，1-是
	CardValue	m_curCard;//摸上来的牌，这个只有 m_pos == mypos m_flag == 0 的时候才有

	std::vector<ThinkData> m_think;//思考的内容
	std::vector<TingData> m_ting;	//听牌数据
	std::vector<CardValue> m_flowerCard;		//摸牌时摸到的花

	LMsgS2COutCard() :LMsgSC(MSG_S_2_C_GET_CARD)
	{
		m_pos = INVAILD_POS;
		m_time = 0;
		m_deskCard = 0;
		m_flag = 0;
		m_gang = 0;
		m_end = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_dCount), m_deskCard);
		m_curCard.Read(obj);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_think), m_think);
		ReadMapData(obj, NAME_TO_STR(m_ting), m_ting);
		ReadMapData(obj, NAME_TO_STR(m_gang), m_gang);
		ReadMapData(obj, NAME_TO_STR(m_end), m_end);
		//
		msgpack::object array1;
		ReadMapData(obj, NAME_TO_STR(m_think), array1);
		if (array1.type == msgpack::type::ARRAY)//MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array1.via.array.size; ++i)
			{
				ThinkData v;
				msgpack::object& pv = *(array1.via.array.ptr + i);
				pv.convert(v);
				m_think.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 12);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_dCount), m_deskCard);
		m_curCard.Write(pack);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_think);
		WriteKeyValue(pack, NAME_TO_STR(m_ting), m_ting);
		WriteKeyValue(pack, NAME_TO_STR(m_gang), m_gang);
		WriteKeyValue(pack, NAME_TO_STR(m_end), m_end);
		WriteKeyValue(pack, NAME_TO_STR(m_flowerCard), m_flowerCard);

		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2COutCard(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端摸牌后的操作
struct LMsgC2SUserPlay:public LMsgSC
{
	ThinkData m_thinkInfo;

	LMsgC2SUserPlay() :LMsgSC(MSG_C_2_S_PLAY_CARD)
	{

	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_type), m_thinkInfo.m_type);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_think), array);
		if (array.type == msgpack::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_thinkInfo.m_card.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_thinkInfo.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_thinkInfo.m_card);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserPlay();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回操作结果
struct LMsgS2CUserPlay:public LMsgSC
{
	enum
	{
		OutSuccessed = 0,    //0:无错误
		OutNotYourTurn = 1,  //1:不应该由你出牌(当前出牌人由m_pos指定)
		OutOtherTing = 2,    //2:在卡心五中使用,出的牌是别人廊起听的牌,不允许出（除非玩家也廊起了）
		OutTingError = 3,    //3:在卡心五中使用,已经廊起的玩家，打出的牌不是摸到的牌
	};

	Lint		m_errorCode;   //0: success	
	Lint		m_flag;////0-自摸，1-出牌,2-暗牌,3-明杠(自己碰过之后),4-提
//	Lint		m_qiangGangflag; //标示是否该杠\补操作被抢 0-未被抢/无需等待胡牌 1-被抢 2-等待胡牌
	Lint		m_pos;//出牌玩家的位置
	Lint		m_beforePos;		// 这个只在碰转杠的时候标记是碰的谁的
	ThinkData	m_card;
	std::vector<Lint>	m_hu;
	Lint		m_cardCount;			//手牌数量
	std::vector<CardValue> m_cardValue;	//推倒的手牌
	CardValue m_huCard;					//自摸胡的那张牌
	std::vector<Lint>        m_addScore;          //当前牌局积分
	
	LMsgS2CUserPlay() :LMsgSC(MSG_S_2_C_PLAY_CARD)
	{
		m_errorCode = -1;
		m_flag = 0;
//		m_qiangGangflag=2;
		m_pos = INVAILD_POS;
		m_cardCount = 0;
		m_addScore.push_back(0);
		m_addScore.push_back(0);
		m_addScore.push_back(0);
		m_addScore.push_back(0);
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		/*ReadMapData(obj, NAME_TO_STR(m_card), m_card);
		ReadMapData(obj, NAME_TO_STR(m_hu), m_hu);
		ReadMapData(obj, NAME_TO_STR(m_cardCount), m_cardCount);
		ReadMapData(obj, NAME_TO_STR(m_cardValue), m_cardValue);
		m_huCard.Read(obj);*/
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 12);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_card.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_beforePos), m_beforePos);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_card.m_card);
		WriteKeyValue(pack, NAME_TO_STR(m_hu), m_hu);
		WriteKeyValue(pack, NAME_TO_STR(m_cardCount), m_cardCount);
		WriteKeyValue(pack, NAME_TO_STR(m_cardValue), m_cardValue);
		m_huCard.Write(pack);
		WriteKeyValue(pack, NAME_TO_STR(m_addScore), m_addScore);

		LLOG_DEBUG("scoreInfo=%d,%d,%d,%d.", m_addScore[0], m_addScore[1], m_addScore[2], m_addScore[3]);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserPlay();}
};

//通知玩家开始下跑
struct LMsgS2CStartBottomPour:public LMsgSC
{
	bool m_need_bottom;
	LMsgS2CStartBottomPour() :LMsgSC(MSG_S_2_C_START_BOTTOM_POUR),m_need_bottom(false){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_need_bottom), m_need_bottom);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_need_bottom), m_need_bottom);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CStartBottomPour();}
};


//玩家下跑
struct LMsgC2SBottomPour:public LMsgSC
{
	Lint m_nPour;			//周口玩法中 平胡下分用这个 其他的看各自实现
	Lint m_nTiePour;		//0不开启铁杠跑 1开启铁杠跑
	Lint m_nGangPour;		//1-5分
	Lint m_nQianPour;		//
	Lint m_nHouPour;		//

	LMsgC2SBottomPour() :LMsgSC(MSG_C_2_S_BOTTOM_POUR),m_nPour(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_nPour), m_nPour);
		ReadMapData(obj, NAME_TO_STR(m_nTiePour), m_nTiePour);
		ReadMapData(obj, NAME_TO_STR(m_nGangPour), m_nGangPour);
		ReadMapData(obj, NAME_TO_STR(m_nQianPour), m_nQianPour);
		ReadMapData(obj, NAME_TO_STR(m_nHouPour), m_nHouPour);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_nPour), m_nPour);
		WriteKeyValue(pack, NAME_TO_STR(m_nTiePour), m_nTiePour);
		WriteKeyValue(pack, NAME_TO_STR(m_nGangPour), m_nGangPour);
		WriteKeyValue(pack, NAME_TO_STR(m_nQianPour), m_nQianPour);
		WriteKeyValue(pack, NAME_TO_STR(m_nHouPour), m_nHouPour);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SBottomPour();}
};

//某个玩家下跑数据
struct LMsgS2CUserBottomPourInfo:public LMsgSC
{
	Lint m_nPos;	//玩家位置
	Lint m_nPour;	//下跑数据
	Lint m_nTiePour;		//0不开启铁杠跑 1开启铁杠跑
	Lint m_nGangPour;		//1-5分
	Lint m_nQianPour;		//
	Lint m_nHouPour;		//

	LMsgS2CUserBottomPourInfo() :LMsgSC(MSG_S_2_C_USER_BOTTOM_POUR_INFO),m_nPos(0),m_nPour(0)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_nPos), m_nPos);
		ReadMapData(obj, NAME_TO_STR(m_nPour), m_nPour);
		ReadMapData(obj, NAME_TO_STR(m_nTiePour), m_nTiePour);
		ReadMapData(obj, NAME_TO_STR(m_nGangPour), m_nGangPour);
		ReadMapData(obj, NAME_TO_STR(m_nQianPour), m_nQianPour);
		ReadMapData(obj, NAME_TO_STR(m_nHouPour), m_nHouPour);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_nPos), m_nPos);
		WriteKeyValue(pack, NAME_TO_STR(m_nPour), m_nPour);
		WriteKeyValue(pack, NAME_TO_STR(m_nTiePour), m_nTiePour);
		WriteKeyValue(pack, NAME_TO_STR(m_nGangPour), m_nGangPour);
		WriteKeyValue(pack, NAME_TO_STR(m_nQianPour), m_nQianPour);
		WriteKeyValue(pack, NAME_TO_STR(m_nHouPour), m_nHouPour);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserBottomPourInfo();}
};

//所有玩家下跑数据
struct LMsgS2CAllUserBottomPourInfo:public LMsgSC
{
	Lint m_nPour[DESK_USER_COUNT];
	Lint m_nTiePour[DESK_USER_COUNT];		//0不开启铁杠跑 1开启铁杠跑
	Lint m_nGangPour[DESK_USER_COUNT];		//1-5分
	Lint m_nQianPour[DESK_USER_COUNT];		//
	Lint m_nHouPour[DESK_USER_COUNT];		//
	bool m_bPoured[DESK_USER_COUNT];
	LMsgS2CAllUserBottomPourInfo() :LMsgSC(MSG_S_2_C_ALLUSER_BOTTOM_POUR_INFO)
	{
		memset(m_nPour,0,sizeof(m_nPour));
		memset(m_bPoured,0,sizeof(m_bPoured));
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKey(pack, "m_nPour");
		std::vector<Lint> veci;
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_nPour[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_nTiePour");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_nTiePour[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_nGangPour");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_nGangPour[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_nQianPour");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_nQianPour[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_nHouPour");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_nHouPour[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_bPoured");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_bPoured[i]);
		}
		WriteKey(pack, veci);

		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CAllUserBottomPourInfo();}
};


//////////////////////////////////////////////////////////////////////////
//服务器发送玩家思考信息

struct LMsgS2CThink:public LMsgSC
{
	Lint				m_time;//思考倒计时
	Lint				m_flag;//0-等待其他玩家思考中，1-当前我正在思考
	CardValue			m_card;
	std::vector<ThinkData>	m_think;


	LMsgS2CThink() :LMsgSC(MSG_S_2_C_USER_THINK)
	{
		m_time = 0;
		m_flag = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_think), m_think);
		m_card.Read(obj);
		//
		msgpack::object array1;
		ReadMapData(obj, NAME_TO_STR(m_think), array1);
		if (array1.type == msgpack::type::ARRAY)
		{
			for (Lsize i = 0; i < array1.via.array.size; ++i)
			{
				ThinkData v;
				msgpack::object& pv = *(array1.via.array.ptr + i);
				pv.convert(v);
				m_think.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_think);
		m_card.Write(pack);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CThink();}
};

//////////////////////////////////////////////////////////////////////////
//客户端发送思考操作结果
struct LMsgC2SUserOper:public LMsgSC
{
	ThinkData m_think; 
	//Lint m_think;//思考结果0-过，1-胡，2-杠，3-碰,4-补

	LMsgC2SUserOper() :LMsgSC(MSG_C_2_S_USER_OPER){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_type), m_think.m_type);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_think), array);
		if (array.type == msgpack::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_think.m_card.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_think.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_think.m_card);

		return true;
	}


	virtual LMsg* Clone(){return new LMsgC2SUserOper();}
};

//////////////////////////////////////////////////////////////////////////
//服务广播操作结果
struct LMsgS2CUserOper:public LMsgSC
{
	Lint		m_errorCode;//0-成功，1-失败
	Lint		m_pos;//pos=我自己的时候，需要根据类型响应不同的操作
	Lint		m_beforePos;			// 碰牌或者杠牌时 碰杠谁的
	ThinkData	m_think; 
	CardValue	m_card; //玩家碰，杠，胡的张牌
	std::vector<Lint>	m_hu;
	Lint        m_hType;                //1代表抢杠胡，否则为其他
	Lint		m_cardCount;			//手牌数量
	std::vector<CardValue> m_cardValue;	//推倒的手牌
	std::vector<Lint>        m_addScore;          //当前牌局积分
	
	LMsgS2CUserOper() :LMsgSC(MSG_S_2_C_USER_OPER)
	{
		m_errorCode = -1;
		m_pos = INVAILD_POS;
		m_hType = 0;
		m_cardCount = 0;
		m_addScore.push_back(0);
		m_addScore.push_back(0);
		m_addScore.push_back(0);
		m_addScore.push_back(0);
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_beforePos), m_beforePos);
		ReadMapData(obj, NAME_TO_STR(m_think), m_think);
		m_card.Read(obj);
		ReadMapData(obj, NAME_TO_STR(m_hu), m_hu);
		ReadMapData(obj, NAME_TO_STR(m_hType), m_hType);
		ReadMapData(obj, NAME_TO_STR(m_cardCount), m_cardCount);
		ReadMapData(obj, NAME_TO_STR(m_cardValue), m_cardValue);
		ReadMapData(obj, NAME_TO_STR(m_addScore), m_addScore);
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 12);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_beforePos), m_beforePos);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_think);
		m_card.Write(pack);
		WriteKeyValue(pack, NAME_TO_STR(m_hu), m_hu);
		WriteKeyValue(pack, NAME_TO_STR(m_hType), m_hType);
		WriteKeyValue(pack, NAME_TO_STR(m_cardCount), m_cardCount);
		WriteKeyValue(pack, NAME_TO_STR(m_addScore), m_addScore);
		WriteKeyValue(pack, NAME_TO_STR(m_cardValue), m_cardValue);
		LLOG_DEBUG("scoreInfo=%d,%d,%d,%d.", m_addScore[0], m_addScore[1], m_addScore[2], m_addScore[3]);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserOper();}
};

//////////////////////////////////////////////////////////////////////////

//结算时的得分统计
struct ScoreInfo
{
	int m_pos;			// 对方位置
	int m_type;			// 计分类型 自摸1 被自摸2 接炮3 点炮4 暗杠5 被暗杠6 接杠7 点杠8
	int m_score;		// 得分

	MSGPACK_DEFINE(m_pos, m_type, m_score);
};

//服务器广播游戏结束
struct LMsgS2CGameOver:public LMsgSC
{
	Lint		m_result;//0-自摸，1-点炮，2-慌庄
	Lint		m_win[4];//4家胡牌情况，0-没胡，1-自摸，2-收炮，3-点炮
	Lint		m_score[4];//东 南 西 北 各家输赢积分，这里需要更新玩家积分
	Lint		m_agang[4];//暗杠数量
	Lint		m_mgang[4];//明暗杠数量
	Lint		m_dgang[4];//点杠暗杠数量
	Lint		m_flower[4];      // 补花数量
	Lint		m_juemen[4];
	Lint		m_dumen[4];
	Lint		m_menqing[4];
	Lint		m_anke[4];
	Lint		m_zimo[4];

	Lint		m_startHu[4];//起手胡牌
	std::vector<Lint>		m_hu[4];//胡牌的数组
	Lint		m_zhuangPos;	//庄位置

	Lint		m_fanShu[4];	//番数
	//四家手上的牌
	Lint		m_count[4];
	CardValue	m_card[4][14];
	std::vector<CardValue> m_hucards[4];	//胡的牌
	Lint		m_end;
	std::vector<BirdInfo>	m_bird_infos;	//鸟牌

	std::vector<CardValue> m_ghostCard;		//鬼牌

	Lint		m_user_count;
	Lint		m_dmgang[4];//被点杠数量

	std::vector<ScoreInfo> m_scoreInfo[4];		//得分信息
	CardValue   m_lastCard;  // 最后一张牌

	LMsgS2CGameOver() :LMsgSC(MSG_S_2_C_GAME_OVER)
	{
		m_result = -1;
		for ( int i = 0; i < 4; ++i )
		{
			m_win[i] = 0;
			m_score[i] = 0;
			m_agang[i] = 0;
			m_mgang[i] = 0;
			m_dgang[i] = 0;
			m_flower[i] = 0;
			m_startHu[i] = 0;
			m_fanShu[i] = 0;
			m_count[i] = 0;
			m_dmgang[i] = 0;
			m_juemen[i] = 0;
			m_dumen[i] = 0;
			m_menqing[i] = 0;
			m_anke[i] = 0;
			m_zimo[i] = 0;
		}
		m_zhuangPos		= 0;
		m_end			= 0;
		m_user_count	= 4;
	}

	virtual bool Read(msgpack::object& obj)
	{	
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 38);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_result), m_result);
		WriteKeyValue(pack, NAME_TO_STR(m_end), m_end);
		WriteKeyValue(pack, NAME_TO_STR(m_user_count), m_user_count);
		WriteKeyValue(pack, NAME_TO_STR(m_zhuangPos), m_zhuangPos);
		WriteKeyValue(pack, NAME_TO_STR(m_ghostCard), m_ghostCard);

		WriteKey(pack, "m_win");
		std::vector<Lint> veci;
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_win[i]);
		}
		WriteKey(pack,veci);

		for(Lint i = 0 ; i < 4; ++i)
		{
			char name[32];
			sprintf_s(name,sizeof(name),"m_hu%d",i+1);
			WriteKey(pack, name);
			WriteKey(pack,m_hu[i]);
		}

		for(Lint i = 0 ; i < 4; ++i)
		{
			char name[32];
			sprintf_s(name,sizeof(name),"m_scoreInfo%d",i+1);
			WriteKey(pack, name);
			WriteKey(pack,m_scoreInfo[i]);
		}

		WriteKey(pack, "m_agang");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_agang[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_dmgang");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dmgang[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_startHu");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_startHu[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_mgang");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_mgang[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_dgang");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dgang[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_flower");
		veci.clear();
		for (Lint idx = 0; idx < 4; ++idx)
		{
			veci.push_back(m_flower[idx]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_juemen");
		veci.clear();
		for (Lint idx = 0; idx < 4; ++idx)
		{
			veci.push_back(m_juemen[idx]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_dumen");
		veci.clear();
		for (Lint idx = 0; idx < 4; ++idx)
		{
			veci.push_back(m_dumen[idx]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_menqing");
		veci.clear();
		for (Lint idx = 0; idx < 4; ++idx)
		{
			veci.push_back(m_menqing[idx]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_anke");
		veci.clear();
		for (Lint idx = 0; idx < 4; ++idx)
		{
			veci.push_back(m_anke[idx]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_zimo");
		veci.clear();
		for (Lint idx = 0; idx < 4; ++idx)
		{
			veci.push_back(m_zimo[idx]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_score");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_score[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "array0");
		std::vector<CardValue> vec;
		for (Lint i = 0; i < m_count[0]; ++i)
		{
			vec.push_back(m_card[0][i]);
		}
		WriteKey(pack, vec);

		WriteKey(pack, "array1");
		vec.clear();
		for (Lint i = 0; i < m_count[1]; ++i)
		{
			vec.push_back(m_card[1][i]);
		}
		WriteKey(pack, vec);

		WriteKey(pack, "array2");
		vec.clear();
		for (Lint i = 0; i < m_count[2]; ++i)
		{
			vec.push_back(m_card[2][i]);
		}
		WriteKey(pack, vec);

		WriteKey(pack, "array3");
		vec.clear();
		for (Lint i = 0; i < m_count[3]; ++i)
		{
			vec.push_back(m_card[3][i]);
		}
		WriteKey(pack, vec);

		WriteKey(pack, "m_fanShu");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_fanShu[i]);
		}
		WriteKey(pack,veci);
		
		WriteKeyValue(pack, NAME_TO_STR(m_hucards1), m_hucards[0]);
		WriteKeyValue(pack, NAME_TO_STR(m_hucards2), m_hucards[1]);
		WriteKeyValue(pack, NAME_TO_STR(m_hucards3), m_hucards[2]);
		WriteKeyValue(pack, NAME_TO_STR(m_hucards4), m_hucards[3]);
		WriteKeyValue(pack, NAME_TO_STR(m_bird_infos), m_bird_infos);
		WriteKeyValue(pack, NAME_TO_STR(m_lastCard), m_lastCard);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CGameOver();}
};


//////////////////////////////////////////////////////////////////////////
//客户端请求退出房间
struct LMsgC2SUserOutRoom:public LMsgSC
{
	Lint m_pos;//准备玩家的位置

	LMsgC2SUserOutRoom():LMsgSC(MSG_C_2_S_USER_OUT_ROOM)
	{
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);

		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserOutRoom();}
};

//////////////////////////////////////////////////////////////////////////
//服务返回退出结果
struct LMsgS2CUserOutRoom:public LMsgSC
{
	Lint		m_errorCode;//0-成功，1-失败
	Lint		m_pos;
	
	LMsgS2CUserOutRoom():LMsgSC(MSG_S_2_C_USER_OUT_ROOM)
	{
		m_errorCode = -1;
		m_pos = INVAILD_POS;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserOutRoom();}
};

struct LMsgC2SUserAIOper:public LMsgSC
{
	LMsgC2SUserAIOper() :LMsgSC(MSG_C_2_S_USER_AIOPER)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserAIOper();}
};

//客户端摸牌后的操作
struct LMsgS2CUserAIOper:public LMsgSC
{
	ThinkData m_thinkInfo;

	LMsgS2CUserAIOper() :LMsgSC(MSG_S_2_C_USER_AIOPER)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_thinkInfo.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_thinkInfo.m_card);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserAIOper();}
};

//////////////////////////////////////////////////////////////////////////
//服务器发送起手胡消息

struct StartHuInfo
{
	Lint	m_type;
	std::vector<CardValue> m_card;

	StartHuInfo()
	{
		m_type = 0;
	}

	MSGPACK_DEFINE(m_type, m_card); 
};

struct LMsgS2CUserStartHu:public LMsgSC
{
	std::vector<StartHuInfo> m_card;

	LMsgS2CUserStartHu():LMsgSC(MSG_S_2_C_START_HU){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);

		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserStartHu();}
};

//////////////////////////////////////////////////////////////////////////
//服务返回退出结果
struct LMsgC2SUserStartHuSelect:public LMsgSC
{
	Lint		m_type;
	std::vector<CardValue> m_card;

	LMsgC2SUserStartHuSelect():LMsgSC(MSG_C_2_S_START_HU_SELECT)
	{
		m_type = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{

		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		//ReadMapData(obj, NAME_TO_STR(m_card), m_card);
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_card), array);
		if (array.type == msgpack::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_card.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserStartHuSelect();}
};

//////////////////////////////////////////////////////////////////////////
//服务广播玩家操作
struct LMsgS2CUserStartHuSelect:public LMsgSC
{
	Lint		m_pos;
	Lint		m_type;
	std::vector<CardValue> m_card;

	LMsgS2CUserStartHuSelect():LMsgSC(MSG_S_2_C_START_HU_SELECT)
	{
		m_pos = INVAILD_POS;
		m_type = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserStartHuSelect();}
};

//////////////////////////////////////////////////////////////////////////
//玩家杠牌
struct LMsgS2CUserGangCard:public LMsgSC
{
	Lint		m_pos;
	Lint		m_flag;//0-没有自摸，1-自摸
	std::vector<CardValue> m_card;

	LMsgS2CUserGangCard():LMsgSC(MSG_S_2_C_GANG_RESULT)
	{
		m_pos = INVAILD_POS;
		m_flag = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserGangCard();}
};

//////////////////////////////////////////////////////////////////////////
//服务器广播玩家选择海底牌
struct LMsgS2CUserEndCardSelect:public LMsgSC
{
	Lint		m_pos;
	Lint		m_time;

	LMsgS2CUserEndCardSelect():LMsgSC(MSG_S_2_C_END_CARD_SELECT)
	{
		m_pos = INVAILD_POS;
		m_time = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserEndCardSelect();}
};

//////////////////////////////////////////////////////////////////////////
//客户的选择海底
struct LMsgC2SUserEndCardSelect:public LMsgSC
{
	Lint		m_flag;//0-不要，1-要

	LMsgC2SUserEndCardSelect():LMsgSC(MSG_C_2_S_END_SELECT)
	{
		m_flag = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserEndCardSelect();}
};


//////////////////////////////////////////////////////////////////////////
//服务器广播玩家摸海底
struct LMsgS2CUserEndCardGet:public LMsgSC
{
	Lint		m_pos;	//位置
	Lint		m_flag;//是否自摸 0-没有，1-自摸
	CardValue	m_card;

	LMsgS2CUserEndCardGet():LMsgSC(MSG_S_2_C_END_GET)
	{
		m_pos = INVAILD_POS;
		m_flag = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		m_card.Write(pack);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserEndCardGet();}
};

//玩家杠牌
struct LMsgS2CRemoveQiangGangCard:public LMsgSC
{
	Lint		m_pos;
	Lint		m_flag;		//0-手牌， 1未用
	CardValue	m_card;

	LMsgS2CRemoveQiangGangCard():LMsgSC(MSG_S_2_C_REMOVE_QIANGGANG_CARD)
	{
		m_pos = INVAILD_POS;
		m_flag = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		m_card.Write(pack);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CRemoveQiangGangCard();}
};
//////////////////////////////////////////////////////////////////////////
//客户端请求说话
struct LMsgC2SUserSpeak:public LMsgSC
{
	Lint		m_type;//说话类型，0-表情，1-语音,2-文字
	Lint		m_id;	//表情id
	Lstring		m_musicUrl; //语音地址链接
	Lstring		m_msg;//聊天文字

	LMsgC2SUserSpeak() :LMsgSC(MSG_C_2_S_USER_SPEAK)
	{
		m_type = -1;
		m_id = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_musicUrl), m_musicUrl);
		ReadMapData(obj, NAME_TO_STR(m_msg), m_msg);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_musicUrl), m_musicUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_msg), m_msg);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SUserSpeak(); }
};

//////////////////////////////////////////////////////////////////////////
//服务广播操作结果
struct LMsgS2CUserSpeak:public LMsgSC
{
	Lint		m_userId;//说话人id
	Lint		m_pos;//说话人的位置
	Lint		m_type;//说话类型
	Lint		m_id;//类型id
	Lstring		m_musicUrl;//语音url链接
	Lstring		m_msg;//聊天文字

	LMsgS2CUserSpeak() :LMsgSC(MSG_S_2_C_USER_SPEAK)
	{
		m_userId = 0;
		m_pos = INVAILD_POS;
		m_type = -1;
		m_id = 0;
	}

	
	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		//ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_musicUrl), m_musicUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_msg), m_msg);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CUserSpeak(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端请求创建桌子
struct LMsgC2SCreateDesk :public LMsgSC
{
	Lint		m_flag;					//房间类型，1-2圈，2-四圈，3-8圈
	Lstring		m_secret;				//房间密码，如果为空，服务器会随机一个密码
	Lint		m_gold;					//房间抵住，单位元，最低为1元，最多不能超过100
	Lint		m_state;				//玩法规则: 0 转转  1  长沙  101-血战到底  102-血流成河
	Lint		m_robotNum;				// 0,不加机器人，1，2，3加机器人数量
	Lint        m_credits;              //最大信用值
	CardValue	m_cardValue[CARD_COUNT];
	std::vector<Lint>	m_playType;		//玩法: 1-自摸加底 2-自摸加番
	LMsgC2SCreateDesk() :LMsgSC(MSG_C_2_S_CREATE_ROOM)
	{
		m_flag = 0;		
		m_gold = 0;		
		m_state = 0;	
		m_credits = 0;
		m_robotNum = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_secret), m_secret);
		ReadMapData(obj, NAME_TO_STR(m_gold), m_gold);
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		ReadMapData(obj, NAME_TO_STR(m_robotNum), m_robotNum);
		ReadMapData(obj, NAME_TO_STR(m_credits), m_credits);
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_cardValue), array);
		if (array.type == msgpack::type::ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < CARD_COUNT; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_cardValue[i]);
			}
		}
		ReadMapData(obj, NAME_TO_STR(m_playType), array);
		if (array.type == msgpack::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				Lint v;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_playType.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_secret), m_secret);
		WriteKeyValue(pack, NAME_TO_STR(m_gold), m_gold);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);
		WriteKeyValue(pack, NAME_TO_STR(m_robotNum), m_robotNum);
		WriteKeyValue(pack, NAME_TO_STR(m_credits), m_credits);
		WriteKeyValue(pack, NAME_TO_STR(m_playType), m_playType);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SCreateDesk(); }
};

//////////////////////////////////////////////////////////////////////////
//玩家请求创建桌子结果
struct LMsgS2CCreateDeskRet :public LMsgSC
{
	enum ErrorCode
	{
		Err_CreditNotEnough = 6,
	};
	Lint		m_errorCode;//0-成功，1-房卡不够，2-玩家已有存在的桌子，3-桌子号已被使用,4-创建桌子失败,5-创建参数有问题,6-人品不够,信用有误
	Lint		m_deskId;//桌子id

	LMsgS2CCreateDeskRet() :LMsgSC(MSG_S_2_C_CREATE_ROOM)
	{
		m_errorCode = 0;
		m_deskId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CCreateDeskRet(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端加入桌子
struct LMsgC2SAddDesk :public LMsgSC
{
	Lint		m_deskId;//桌子id

	LMsgC2SAddDesk() :LMsgSC(MSG_C_2_S_ADD_ROOM)
	{
		m_deskId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SAddDesk(); }
};

//////////////////////////////////////////////////////////////////////////
//玩家请求加入桌子结果
struct LMsgS2CAddDeskRet :public LMsgSC
{
	enum ErrorCode
	{
		Err_CreditNotEnough = 6,
	};

	Lint		m_errorCode;//0-成功，1-人已满,2-房间不存在,3-房卡不足 4-IP相同 6-信用不够 7-用户不在工会
	Lint		m_deskId;//桌子号码
	
	LMsgS2CAddDeskRet() :LMsgSC(MSG_S_2_C_ADD_ROOM)
	{
		m_errorCode = -1;
		m_deskId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CAddDeskRet(); }
};

struct LMsgC2SEnterCoinsDesk :public LMsgSC
{
	Lint		m_state;				//玩法规则: 0 转转  1  长沙  101-血战到底  102-血流成河
	Lint		m_robotNum;				// 0,不加机器人，1，2，3加机器人数量
	CardValue	m_cardValue[CARD_COUNT];
	std::vector<Lint>	m_playType;		//玩法: 1-自摸加底 2-自摸加番
	Lint		m_coins;				// 玩家的金币
	LMsgC2SEnterCoinsDesk() :LMsgSC(MSG_C_2_S_ENTER_COINS_ROOM)
	{
		m_state = 0;
		m_robotNum = 0;
		m_coins = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		ReadMapData(obj, NAME_TO_STR(m_robotNum), m_robotNum);
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_cardValue), array);
		if (array.type == msgpack::type::ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < CARD_COUNT; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_cardValue[i]);
			}
		}
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
		ReadMapData(obj, NAME_TO_STR(m_coins), m_coins);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SEnterCoinsDesk(); }
};

struct LMsgS2CEnterCoinsDeskRet :public LMsgSC
{
	Lint		m_errorCode;//0-成功，1-金币不够，2-玩家已在金币场，3-未知错误 4-创建桌子失败,5-创建参数有问题 6-人数太多
	Lint		m_remainCount; //m_errorCode=1时有效 表示玩家可以领取的金币的次数

	LMsgS2CEnterCoinsDeskRet() :LMsgSC(MSG_S_2_C_ENTER_COINS_ROOM)
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

	virtual LMsg* Clone(){ return new LMsgS2CEnterCoinsDeskRet(); }
};

//////////////////////////////////////////////////////////////////////////
//vip比赛结束
struct LMsgS2CVipEnd :public LMsgSC
{
	Lint	m_id[4];//玩家id
	Lstring	m_nike[4];//游戏玩家昵称
	Lint	m_gold[4];//输赢积分总数
	Lint	m_zimo[4];//自摸次数
	Lint	m_bomb[4];//放炮次数
	Lint	m_win[4];//收炮次数
	Lint	m_mgang[4];//明杠次数
	Lint	m_agang[4];//暗杠的次数

	Lint	m_dzimo[4];//大胡自摸次数
	Lint	m_dbomb[4];//大胡放炮次数
	Lint	m_dwin[4];//大胡收炮次数

	std::vector<Lint> m_oncegold[4]; //[bing] 每局的得分
	std::vector<Lint> m_playPara;	//玩法参数
	Lint	m_deskid;  //房间号
	Lint    m_playMode; //玩法类型
	Lint	m_maxCircle;	//总局数 8 / 16
	//Lstring m_roomMasterNike;  //[bing] 房主名字


	LMsgS2CVipEnd() :LMsgSC(MSG_S_2_C_VIP_END)
	{
		memset(m_id, 0, sizeof(m_id));
		memset(m_gold, 0, sizeof(m_gold));
		memset(m_zimo, 0, sizeof(m_zimo));
		memset(m_bomb, 0, sizeof(m_bomb));
		memset(m_win, 0, sizeof(m_win));
		memset(m_mgang, 0, sizeof(m_mgang));
		memset(m_agang, 0, sizeof(m_agang));
		memset(m_dzimo, 0, sizeof(m_dzimo));
		memset(m_dbomb, 0, sizeof(m_dbomb));
		memset(m_dwin, 0, sizeof(m_dwin));

		m_nike[0] = "";
		m_nike[1] = "";
		m_nike[2] = "";
		m_nike[3] = "";

		m_deskid = 0;
		m_playMode = 0;
		m_maxCircle = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 20);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

		WriteKey(pack,"m_nike");
		std::vector<Lstring>vecs;
		for(Lint i = 0; i < 4; ++i)
		{
			vecs.push_back(m_nike[i]);
		}
		WriteKey(pack,vecs);

		WriteKey(pack,"m_id");
		std::vector<Lint>veci;
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_id[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_gold");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_gold[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_zimo");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_zimo[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_bomb");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_bomb[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_win");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_win[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_mgang");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_mgang[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_agang");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_agang[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_dzimo");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dzimo[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_dbomb");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dbomb[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_dwin");
		veci.clear();
		for(Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dwin[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_oncegold_1");
		WriteKey(pack,m_oncegold[0]);

		WriteKey(pack,"m_oncegold_2");
		WriteKey(pack,m_oncegold[1]);

		WriteKey(pack,"m_oncegold_3");
		WriteKey(pack,m_oncegold[2]);

		WriteKey(pack,"m_oncegold_4");
		WriteKey(pack,m_oncegold[3]);

		WriteKeyValue( pack, "m_deskid", m_deskid );

		/*if( 0 == m_playMode )
		m_playMode = 1;*/
		WriteKeyValue( pack, "m_playMode", m_playMode );

		WriteKeyValue( pack, "m_playPara", m_playPara );

		WriteKeyValue( pack, "m_maxCircle", m_maxCircle );

		//WriteKeyValue( pack, "m_roomMasterNike", m_roomMasterNike );

		return true;
	}


	virtual LMsg* Clone(){ return new LMsgS2CVipEnd(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求对账单
struct LMsgC2SVipLog :public LMsgSC
{
	Lint		m_time;//id
	Lint		m_userId;//请求某个玩家的战绩
	
	LMsgC2SVipLog() :LMsgSC(MSG_C_2_S_VIP_LOG)
	{
		m_time = 0;
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SVipLog(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求对账单2
//Client 2 LogicManager 2 LogicDB
struct LMsgC2SRoomLog :public LMsgSC
{
	Lstring		m_strUUID; //客户端没有传递，由服务器填写
	Lint		m_userId; //请求某个玩家的战绩
	Lint        m_pos;    //pos1
	Lint        m_time;   //time
	Lint        m_deskId; //桌子id

	LMsgC2SRoomLog() :LMsgSC(MSG_C_2_S_ROOM_LOG)
	{
		m_strUUID = "";
		m_userId = 0;
		m_pos = 0;
		m_deskId = 0;
		m_time = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID),m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId),m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SRoomLog(); }
};

//////////////////////////////////////////////////////////////////////////
//vip对账结果

struct VipLog
{
	Lint	m_time;
	Lstring	m_videoId;
	Lint	m_score[4];

	VipLog()
	{
		m_time = 0;
		memset( m_score, 0, sizeof(m_score) );
	}
};

struct VipLogItemInfo
{
	Lstring m_id;
	Lint m_time;
	Lint m_flag;		//房间类型
	Lint m_userid[4];
	Lstring m_nike[4];
	Lint m_score[4];
	Lint m_reset;//是否解散
	Lint m_deskId;//是否解散
	Lint m_size;
	VipLog m_item[32];

	VipLogItemInfo()
	{
		m_time = 0;
		m_flag = 0;
		memset( m_userid, 0, sizeof(m_userid) );
		memset( m_score, 0, sizeof(m_score) );
		m_reset = 0;
		m_deskId = 0;
		m_size = 0;
	}
};

struct LMsgS2CVipLog:public LMsgSC
{
	Lint				m_size;//长度
	VipLogItemInfo		m_info[20];//log信息
	
	LMsgS2CVipLog() :LMsgSC(MSG_S_2_C_VIP_LOG)
	{
		m_size = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

		WriteKey(pack,"m_data");
		WriteArray(pack,m_size);
		for(Lint i = 0 ; i < m_size; ++i)
		{
			WriteMap(pack,8);
			WriteKeyValue(pack,"m_id",m_info[i].m_id);
			WriteKeyValue(pack,"m_time",m_info[i].m_time);
		//	WriteKeyValue(pack,"m_flag",m_info[i].m_flag);
			WriteKeyValue(pack,"m_reset",m_info[i].m_reset);
			WriteKeyValue(pack,"m_deskId",m_info[i].m_deskId);

			std::vector<Lstring> vecs;
			std::vector<Lint>veci,vec;
			for(Lint j = 0 ; j < 4; ++j)
			{
				vecs.push_back(m_info[i].m_nike[j]);
				veci.push_back(m_info[i].m_score[j]);
				vec.push_back(m_info[i].m_userid[j]);
			}
			WriteKey(pack,"m_userid");
			WriteKey(pack,vec);

			WriteKey(pack,"m_nike");
			WriteKey(pack,vecs);

			WriteKey(pack,"m_score");
			WriteKey(pack,veci);

			WriteKey(pack,"m_match");
			WriteArray(pack,m_info[i].m_size);
			for(Lint k = 0 ; k < m_info[i].m_size; ++k)
			{
				WriteMap(pack,3);
				WriteKeyValue(pack,"m_time",m_info[i].m_item[k].m_time);
				WriteKeyValue(pack,"m_videoId",m_info[i].m_item[k].m_videoId);

				std::vector<Lint> vec;
				for(Lint n = 0 ; n < 4; ++n)
				{
					vec.push_back(m_info[i].m_item[k].m_score[n]);
				}
				WriteKey(pack,"m_score");
				WriteKey(pack,vec);
			}
		}
	
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CVipLog(); }
};

//LogicManager 2 Client
struct LMsgS2CRoomLog:public LMsgSC
{
	Lint m_size;
	Lint m_flag;		//房间类型
	VipLog m_item[32];

	LMsgS2CRoomLog() :LMsgSC(MSG_S_2_C_ROOM_LOG)
	{
		m_flag = 0;
		m_size = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack,"m_flag",m_flag);
		WriteKey(pack,"m_data");
		WriteArray(pack,m_size);
		for(Lint k = 0 ; k < m_size && k < 32; ++k)
		{
			WriteMap(pack,3);
			WriteKeyValue(pack,"m_time",m_item[k].m_time);
			WriteKeyValue(pack,"m_videoId",m_item[k].m_videoId);

			std::vector<Lint> vec;
			for(Lint n = 0 ; n < 4; ++n)
			{
				vec.push_back(m_item[k].m_score[n]);
			}
			WriteKey(pack,"m_score");
			WriteKey(pack,vec);
		}
		
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CRoomLog(); }
};

//inviter-----------------------------------------------
struct UserBasicInfo
{
	Lint		m_id;
	Lstring		m_unionId;
	Lstring		m_nickName;
	Lstring		m_headImgUrl;
	UserBasicInfo()
	{
		m_id = 0;
		m_unionId = "";
		m_nickName = "";
		m_headImgUrl = "";
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_unionId), m_unionId);
		WriteKeyValue(pack, NAME_TO_STR(m_nickName), m_nickName);
		WriteKeyValue(pack, NAME_TO_STR(m_headImgUrl), m_headImgUrl);
		return true;
	}
};

struct InviteeInfo
{
	UserBasicInfo		m_info;
	Lint				m_taskFinished;		// 被邀请者任务是否完成，0：没有完成，非0：完成
	InviteeInfo():m_info()
	{
		m_taskFinished = 0;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_info.m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_unionId), m_info.m_unionId);
		WriteKeyValue(pack, NAME_TO_STR(m_nickName), m_info.m_nickName);
		WriteKeyValue(pack, NAME_TO_STR(m_headImgUrl), m_info.m_headImgUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_taskFinished), m_taskFinished);
		return true;
	}
};

struct LMsgS2CInvitingInfo : public LMsgSC
{
	struct Reward
	{
		Lint	m_type;
		Lint	m_count;
		Reward():m_type(0),m_count(0){}
	};

	Reward						m_reward;
	UserBasicInfo				m_inviter;
	std::vector<InviteeInfo>	m_invitees;

	LMsgS2CInvitingInfo() : LMsgSC(MSG_S_2_C_INVITING_INFO) ,m_inviter()
	{

	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

		WriteKey(pack, NAME_TO_STR(m_reward));
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_reward.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_count), m_reward.m_count);

		WriteKey(pack, NAME_TO_STR(m_inviter));
		m_inviter.Write(pack);

		WriteKey(pack, NAME_TO_STR(m_invitees));
		WriteArray(pack, m_invitees.size());

		for (Lsize i = 0; i < m_invitees.size(); ++i)
			m_invitees[i].Write(pack);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CInvitingInfo; }
};

struct LMsgC2SBindInviter : public LMsgSC
{
	Lint				m_inviterId;
	LMsgC2SBindInviter() : LMsgSC(MSG_C_2_S_BIND_INVITER) 
	{
		m_inviterId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_inviterId), m_inviterId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_inviterId), m_inviterId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SBindInviter; }
};

struct LMsgS2CBindInviter : public LMsgSC
{
	ErrorCode			m_error;
	UserBasicInfo		m_inviter;

	LMsgS2CBindInviter() : LMsgSC(MSG_S_2_C_BIND_INVITER),m_inviter()
	{
		m_error = Unknown;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_error), (Lint)m_error);
		WriteKey(pack, NAME_TO_STR(m_inviter));
		m_inviter.Write(pack);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CBindInviter; }
};
////-end

struct LMsgC2SGetUserInfo : public LMsgSC
{
	Lint		m_userId;

	LMsgC2SGetUserInfo() : LMsgSC(MSG_C_2_S_GET_USER_INFO), m_userId(-1) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SGetUserInfo; }
};

struct LMsgS2CUserInfo : public LMsgSC
{
	ErrorCode			m_error;
	UserBasicInfo		m_user;

	LMsgS2CUserInfo() : LMsgSC(MSG_S_2_C_USER_INFO) 
	{
		m_error = Unknown;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_error), (Lint)m_error);
		WriteKey(pack, NAME_TO_STR(m_user));
		m_user.Write(pack);

		return true;
	}

	virtual LMsg* Clone() {return new LMsgS2CUserInfo;}
};

struct LMsgC2SGetInvitingInfo : public LMsgSC
{
	Lstring  m_strUUID;		//由服务器填写
	Lint     m_userId;
	LMsgC2SGetInvitingInfo() : LMsgSC(MSG_C_2_S_GET_INVITING_INFO) 
	{
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SGetInvitingInfo; }
};

//------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
//客户端请求录像
struct LMsgC2SVideo :public LMsgSC
{
	Lstring		m_videoId;//id

	LMsgC2SVideo() :LMsgSC(MSG_C_2_S_VIDEO){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_videoId), m_videoId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_videoId), m_videoId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SVideo(); }
};

//////////////////////////////////////////////////////////////////////////
//vip对账结果

struct LMsgS2CVideo:public LMsgSC
{
	VideoLog		m_video;//log信息
	Lint			m_sex[4];
	Lstring			m_nike[4];
	Lstring			m_imageUrl[4];
	Lint			m_zhuang;
	Lint			m_deskId;
	Lint			m_curCircle;
	Lint			m_maxCircle;
	Lint			m_flag;
	Lint			m_score[4];

	LMsgS2CVideo() :LMsgSC(MSG_S_2_C_VIDEO)
	{
		memset( m_sex, 0, sizeof(m_sex) );
		m_zhuang = 0;
		m_deskId = 0;
		m_curCircle = 0;
		m_maxCircle = 0;
		m_flag = 0;
		memset( m_score, 0, sizeof(m_score) );
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 18);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

		WriteKeyValue(pack, NAME_TO_STR(m_zhuang), m_zhuang);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(m_curCircle), m_curCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_maxCircle), m_maxCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_flag);

		std::vector<Lint>vecid;
		std::vector<Lint>vecsex;
		std::vector<Lint>vecscore;
		std::vector<Lstring>vecnike;
		std::vector<Lstring>vecurl;

		for(Lint i = 0 ; i < 4; ++i)
		{
			if ( m_video.m_userId[i] == 0 )
			{
				continue;
			}
			vecid.push_back(m_video.m_userId[i]);
			vecnike.push_back(m_nike[i]);
			vecsex.push_back(m_sex[i]);
			vecurl.push_back(m_imageUrl[i]);
			vecscore.push_back(m_score[i]);
		}

		WriteKey(pack,"m_score");
		WriteKey(pack,vecscore);

		WriteKey(pack,"m_userid");
		WriteKey(pack,vecid);

		WriteKey(pack,"m_nike");
		WriteKey(pack, vecnike);

		WriteKey(pack, "m_sex");
		WriteKey(pack, vecsex);

		WriteKey(pack, "m_imageUrl");
		WriteKey(pack, vecurl);

		WriteKey(pack,"m_time");
		WriteKey(pack,m_video.m_time);

		WriteKey(pack,"m_oper");
		WriteKey(pack,m_video.m_oper);

		WriteKey(pack,"m_playtype");
		WriteKey(pack,m_video.m_playType);

		for(Lint i = 0 ; i < 4; ++i)
		{
			std::stringstream ss ;
			ss << "m_card" << i;
			WriteKey(pack,ss.str());
			WriteKey(pack,m_video.m_handCard[i]);
		}

		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CVideo(); }
};

//////////////////////////////////////////////////////////////////////////
//同步vip房间信息
struct LMsgS2CVipInfo :public LMsgSC
{
	Lint			m_curCircle;//当前局数
	Lint			m_curMaxCircle;//最大局数
	std::vector<Lint>	m_playtype;

	LMsgS2CVipInfo() :LMsgSC(MSG_S_2_C_VIP_INFO)
	{
		m_curCircle = 0;
		m_curMaxCircle = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		//ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_curCircle), m_curCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_curMaxCircle), m_curMaxCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_playtype), m_playtype);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CVipInfo(); }
};


// 服务器通知玩家开始定缺
struct LMsgS2CUserStartDingQue:public LMsgSC
{
	Lint		m_state[4];		//玩家定缺的状态，0：没订过缺	1：订过缺

	LMsgS2CUserStartDingQue() :LMsgSC(MSG_S_2_C_USER_DINGQUE_START)
	{
		memset( m_state, 0, sizeof(m_state) );
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

		WriteKey(pack, "m_state");
		std::vector<Lint> vec;
		for (Lint i = 0; i < 4; ++i)
		{
			vec.push_back(m_state[i]);
		}
		WriteKey(pack,vec);

		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CUserStartDingQue(); }
};

//服务器通知玩家定缺完毕
struct LMsgS2CUserCompleteDingQue:public LMsgSC
{
	Lint		m_color[4];		//玩家定缺的颜色

	LMsgS2CUserCompleteDingQue() :LMsgSC(MSG_S_2_C_USER_DINGQUE_COMPLETE)
	{
		memset( m_color, 0, sizeof(m_color) );
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		
		WriteKey(pack, "m_color");
		std::vector<Lint> vec;
		for (Lint i = 0; i < 4; ++i)
		{
			vec.push_back(m_color[i]);
		}
		WriteKey(pack,vec);

		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CUserCompleteDingQue(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端定缺
struct LMsgC2SUserDingQue:public LMsgSC
{
	Lint		m_pos;		//玩家的位置
	Lint		m_color;	// 定缺花色

	LMsgC2SUserDingQue() :LMsgSC(MSG_C_2_S_USER_DINGQUE)
	{
		m_pos = INVAILD_POS;
		m_color = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_color), m_color);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserDingQue();}
};


// 服务器通知玩家换牌
//struct LMsgS2CUserChange:public LMsgSC
//{
//	Lint		m_time;		// 10:请求玩家 0:表示玩家换牌成功
//	Lint		m_pos;		//换牌玩家位置
//	CardValue	m_card[3];	// m_time=0时有效  目前只有金币场填写因为有倒计时
//
//	LMsgS2CUserChange() :LMsgSC(MSG_S_2_C_USER_CHANGE)
//	{
//		m_time = 0;
//		m_pos = INVAILD_POS;
//	}
//
//	virtual bool Read(msgpack::object& obj)
//	{
//		return true;
//	}
//
//	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
//	{
//		WriteMap(pack, 4);
//		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
//		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
//		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
//		std::vector<CardValue> vec;
//		WriteKey(pack, "m_card");
//		for (Lint i = 0; i < 3; ++i)
//		{
//			vec.push_back(m_card[i]);
//		}
//		WriteKey(pack,vec);
//		return true;
//	}
//
//	virtual LMsg* Clone(){ return new LMsgS2CUserChange(); }
//};

//////////////////////////////////////////////////////////////////////////
//客户端换牌
//struct LMsgC2SUserChange:public LMsgSC
//{
//	Lint		m_pos;		//玩家的位置
//	CardValue	m_card[3];	//玩家换的牌
//
//	LMsgC2SUserChange() :LMsgSC(MSG_C_2_S_USER_CHANGE)
//	{
//		m_pos = INVAILD_POS;
//	}
//
//	virtual bool Read(msgpack::object& obj)
//	{
//		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
//		//ReadMapData(obj, NAME_TO_STR(m_card), m_card);
//		msgpack::object array;
//		ReadMapData(obj, NAME_TO_STR(m_card), array);
//		if (array.type == msgpack::type::ARRAY && array.via.array.size == 3)
//		{
//			Lint m_count = array.via.array.size;
//			for (Lsize i = 0; i < array.via.array.size; ++i)
//			{
//				CardValue v;
//				msgpack::object& obj =  *(array.via.array.ptr+i);
//				obj.convert(v);
//				m_card[i] = v;
//			}
//		}
//
//		return true;
//	}
//
//	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
//	{
//		return true;
//	}
//
//	virtual LMsg* Clone(){return new LMsgC2SUserChange();}
//};

// 服务器通知玩家换牌结果
//struct LMsgS2CUserChangeResult:public LMsgSC
//{
//	Lint		m_flag;		//换牌规则
//	CardValue	m_card[3];	//玩家换的牌
//
//	LMsgS2CUserChangeResult() :LMsgSC(MSG_S_2_C_USER_CHANGERESULT)
//	{
//		m_flag = 0;
//	}
//
//	virtual bool Read(msgpack::object& obj)
//	{
//		return true;
//	}
//
//	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
//	{
//		WriteMap(pack, 3);
//		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
//		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
//
//		std::vector<CardValue> vec;
//		WriteKey(pack, "m_card");
//		for (Lint i = 0; i < 3; ++i)
//		{
//			vec.push_back(m_card[i]);
//		}
//		WriteKey(pack,vec);
//		return true;
//	}
//
//	virtual LMsg* Clone(){ return new LMsgS2CUserChangeResult(); }
//};

// 单局结算
struct SingleResult
{
	Lint m_winPos;
	Lint m_dianPos;
	std::vector<Lint> m_fanType;
	Lint m_genCount;
	CardValue m_winCard;

	SingleResult()
	{
		m_winPos = INVAILD_POS;
		m_dianPos = INVAILD_POS;
		m_fanType.clear();
		m_genCount = 0;
		m_winCard.m_color = 0;
		m_winCard.m_number = 0;
	}

	MSGPACK_DEFINE(m_winPos, m_dianPos, m_fanType,m_genCount, m_winCard); 

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_winPos), m_winPos);
		WriteKeyValue(pack, NAME_TO_STR(m_dianPos), m_dianPos);
		WriteKeyValue(pack, NAME_TO_STR(m_fanType), m_fanType);
		WriteKeyValue(pack, NAME_TO_STR(m_genCount), m_genCount);
		WriteKeyValue(pack, NAME_TO_STR(m_winCard), m_winCard);
		return true;
	}
};

// 服务器通知玩家单次胡牌信息
//struct LMsgS2CGameResult:public LMsgSC
//{
//	std::vector<SingleResult>	m_result;
//
//	LMsgS2CGameResult() :LMsgSC(MSG_S_2_C_GAMEREULT){}
//
//	virtual bool Read(msgpack::object& obj)
//	{
//		return true;
//	}
//
//	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
//	{
//		WriteMap(pack, 2);
//		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
//		//WriteKeyValue(pack, NAME_TO_STR(m_result), m_result);
//
//		std::vector<SingleResult> vec;
//		WriteKey(pack, "m_result");
//		for (Lsize i = 0; i < m_result.size(); ++i)
//		{
//			vec.push_back(m_result[i]);
//		}
//		WriteKey(pack,vec);
//		return true;
//	}
//
//	virtual LMsg* Clone(){ return new LMsgS2CGameResult(); }
//};

struct LMsgC2STangCard : public LMsgSC
{
	CardValue			m_cardValue[14];  //躺牌
	CardValue           m_outCard;

	LMsgC2STangCard() :LMsgSC(MSG_C_2_S_TANGCARD)
	{
	}
	virtual bool Read(msgpack::object& obj)
	{
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_cardValue), array);
		if (array.type == msgpack::type::ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				msgpack::object& pv =  *(array.via.array.ptr+i);
				pv.convert(m_cardValue[i]);
			}
		}

		ReadMapData(obj, NAME_TO_STR(m_outCard), array);
		if (array.type == msgpack::type::ARRAY)
		{
			msgpack::object& pv =  *(array.via.array.ptr);
			pv.convert(m_outCard);
		}
		return true;
	}
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2STangCard();
	}
};

struct LMsgS2CTangCardRet : public LMsgSC
{
	enum
	{
		TSuccessed   = 0,    //躺成功
		TPosError    = 1,    //pos错误
		TStatusError = 2,    //状态错误
		TCardError   = 3,    //躺牌错误,躺牌不在手牌中 ，或为空
		TFailed      = 4,    //躺不下去
	};

	Lint				m_result;   //0=success other=failed
	Lint                m_pos;      // 躺牌玩家的pos
	Lint                m_tangCount;
	CardValue	        m_cardValue[14];	//玩家 躺下的牌
	CardValue			m_outCard;			//玩家躺牌出去的牌
	Lint                m_tingCount;
	CardValue	        m_tingCards[CARD_COUNT/4];	//玩家听的牌

	LMsgS2CTangCardRet() :LMsgSC(MSG_S_2_C_TANGCARD_RET)
	{	
		m_result = -1;	
		m_pos = INVAILD_POS;
		m_tangCount = 0;
		m_tingCount = 0;
	}
	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_result), m_result);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);

		std::vector<CardValue> vec;
		WriteKey(pack, "m_cardValue");
		for (Lint i = 0; i < m_tangCount && i < 14; ++i)
		{
			vec.push_back(m_cardValue[i]);
		}
		WriteKey(pack,vec);

		WriteKeyValue(pack, NAME_TO_STR(m_outCard), m_outCard);

		vec.clear();
		WriteKey(pack, "m_tingCards");
		for (Lint i = 0; i < m_tingCount && i < 14; ++i)
		{
			vec.push_back(m_tingCards[i]);
		}
		WriteKey(pack,vec);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CTangCardRet();
	}
};

//////////////////////////////////////////////////////////////////////////
//客户端请求 可点赞记录
struct LMsgC2SCRELog :public LMsgSC
{
	Lint		m_time;   //暂未用
	Lint		m_userId; //用户id

	LMsgC2SCRELog() :LMsgSC(MSG_C_2_S_REQ_CREHIS)
	{
		m_time = 0;
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SCRELog(); }
};

//用户基本信息for 点赞客户端显示头像 等信息
struct UserBaseInfo_s     
{
	Lint    m_userId;
	Lstring m_strNike;
	Lstring m_strHeadUrl;
	Lint    m_sex;

	UserBaseInfo_s()
	{
		m_sex = 0;
		m_userId = 0;
	}

	MSGPACK_DEFINE(m_userId, m_strNike, m_strHeadUrl, m_sex); 

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_strNike), m_strNike);
		WriteKeyValue(pack, NAME_TO_STR(m_strHeadUrl), m_strHeadUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_sex), m_sex);
		return true;
	}
};

//服务器 返回 可点赞记录
struct LMsgS2CCRELogRet :public LMsgSC
{
	std::vector<std::string> m_record;   //点赞记录 
	std::vector<UserBaseInfo_s> m_info;  //用户基本信息


	LMsgS2CCRELogRet() :LMsgSC(MSG_S_2_C_RET_CREHIS)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_record), m_record);

		std::vector<UserBaseInfo_s> vec;
		WriteKey(pack, "m_info");
		for (Lsize i = 0; i < m_info.size(); ++i)
		{
			vec.push_back(m_info[i]);
		}
		WriteKey(pack,vec);
	
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CCRELogRet(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求 评价点赞
struct LMsgC2SREQ_GTU :public LMsgSC
{
	Lint        m_index;                   //索引
	Lint		m_userId;                  //当前请求玩家id
	Lstring     m_onelog;                  //可点赞记录
	std::vector<Lint>        m_userList;   //用户列表

	LMsgC2SREQ_GTU() :LMsgSC(MSG_C_2_S_REQ_GTU)
	{
		m_userId = 0;
		m_index = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_index), m_index);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_onelog), m_onelog);
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_userList), array);
		if (array.type == msgpack::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				Lint v = 0;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_userList.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SREQ_GTU(); }
};

//////////////////////////////////////////////////////////////////////////
//服务器 回复 客户端请求点赞成功
struct LMsgS2CREQ_GTU_RET :public LMsgSC
{
	Lint        m_index;   //索引

	LMsgS2CREQ_GTU_RET() :LMsgSC(MSG_S_2_C_RET_GTU)
	{
		m_index = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_index), m_index);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CREQ_GTU_RET(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端请求 评价点赞
struct LMsgC2SREQ_Exch :public LMsgSC
{
	Lint        m_card;                   //索引

	LMsgC2SREQ_Exch() :LMsgSC(MSG_C_2_S_REQ_EXCH)
	{
		m_card = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_card), m_card);
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SREQ_Exch(); }
};

//////////////////////////////////////////////////////////////////////////
//服务器 回复 客户端请求点赞成功
struct LMsgS2CREQ_EXCH_RET :public LMsgSC
{
	Lint        m_card;   //索引

	LMsgS2CREQ_EXCH_RET() :LMsgSC(MSG_S_2_C_RET_EXCH)
	{
		m_card = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CREQ_EXCH_RET(); }
};

// 和后的ID对应
enum ActivityID
{
	ActivityID_Draw	= 1002,	// 新装盘
	ActivityID_Share	= 1003,	// 分享
};

struct LMsgS2CActivityInfo:public LMsgSC
{
	typedef struct Value
	{
		Lstring m_key;
		Lstring m_value;
		MSGPACK_DEFINE(m_key,m_value);
	}ATTRI;

	struct Info
	{
		Lint m_id;		   // ActivityID类型
		Lstring m_info;	   // 活动内容 根据m_id来填充和解析
		std::vector<struct Value> m_attri;  //根据m_id来填充解析

		Info()
		{
			m_id = 0;
		}

		MSGPACK_DEFINE(m_id, m_info,m_attri);
	};

	std::vector<Info> m_activities;

	LMsgS2CActivityInfo() : LMsgSC(MSG_S_2_C_ACTIVITY_INFO)
	{
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		Lint m_num = (Lint)m_activities.size();
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_num), m_num);
		WriteKeyValue(pack, NAME_TO_STR(m_activities), m_activities);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActivityInfo(); }
};

struct LMsgC2SActivityPhone : public LMsgSC
{
	Lstring		m_strUUID;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用

	Lint		m_AutoId;		// 服务器的给客户端的ID
	Lstring		m_PhoneNum;		// 电话号码

	LMsgC2SActivityPhone() :LMsgSC(MSG_C_2_S_ACTIVITY_WRITE_PHONE)
	{
		m_userId = 0;
		m_AutoId = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_AutoId), m_AutoId);
		ReadMapData(obj, NAME_TO_STR(m_PhoneNum), m_PhoneNum);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_AutoId), m_AutoId);
		WriteKeyValue(pack, NAME_TO_STR(m_PhoneNum), m_PhoneNum);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SActivityPhone(); }
};

struct LMsgS2CActivityPhone:public LMsgSC
{
	Lint		m_errorCode;			//结果 0成功 其他失败
	LMsgS2CActivityPhone() :LMsgSC(MSG_S_2_C_ACTIVITY_WRITE_PHONE)
	{
		m_errorCode = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActivityPhone(); }
};

struct LMsgC2SActivityRequestLog : public LMsgSC
{
	Lstring		m_strUUID;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用

	Lint		m_activeId;	// 活动ID ActivityID类型 若为0表示查询玩家所有的活动纪录

	LMsgC2SActivityRequestLog() :LMsgSC(MSG_C_2_S_ACTIVITY_REQUEST_LOG)
	{
		m_userId = 0;
		m_activeId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_activeId), m_activeId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_activeId), m_activeId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SActivityRequestLog(); }
};

struct ActivityLog
{
	ActivityLog()
	{
		AutoId = 0;
		Id = 0;
		SpendType = 0;
		SpendCount = 0;
		GiftType = 0;
		GiftCount = 0;
		Date = 0;
	}

	Lint AutoId;
	Lint Id;
	Lint SpendType;
	Lint SpendCount;
	Lstring GiftName;
	Lint GiftType;
	Lint GiftCount;
	Lint Date;
	Lstring Phone;

	// 下面这种写法 客户端看不到变量名 直接安装顺序来取值 所以顺序很重要
	MSGPACK_DEFINE(AutoId, Id, SpendType, SpendCount, GiftName, GiftType, GiftCount, Date, Phone);
};

struct LMsgS2CActivityReplyLog:public LMsgSC
{
	Lint		m_errorCode;			//结果 0成功 其他失败
	std::vector<ActivityLog> m_logs;

	LMsgS2CActivityReplyLog() :LMsgSC(MSG_S_2_C_ACTIVITY_REPLY_LOG)
	{
		m_errorCode = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_logs), m_logs);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_logs), m_logs);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActivityReplyLog(); }
};

struct LMsgC2SActivityRequestDrawOpen:public LMsgSC
{
	Lstring		m_strUUID;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用

	LMsgC2SActivityRequestDrawOpen() :LMsgSC(MSG_C_2_S_ACTIVITY_REQUEST_DRAW_OPEN)
	{
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SActivityRequestDrawOpen(); }
};

// 装盘
struct ActivityDrawWinUser
{
	Lstring m_userId;	// 玩家id
	Lstring m_giftName;	// 中奖的名称
	MSGPACK_DEFINE(m_userId,m_giftName);
};

struct LMsgS2CActivityReplyDrawOpen:public LMsgSC
{
	Lint		m_errorCode;	// 转盘结果 0成功 -1未知错误 1当前没有这个转盘活动
	// m_errorCode=0 时 下面的值有效
	std::vector<ActivityDrawWinUser> m_winUsers;
	Lint		m_SpendType;	// 消耗的类型 0免费 1房卡（单位个）
	Lint		m_SpendCount;	// 消耗的数量

	LMsgS2CActivityReplyDrawOpen() :LMsgSC(MSG_S_2_C_ACTIVITY_REPLY_DRAW_OPEN)
	{
		m_errorCode = -1;
		m_SpendType = 0;
		m_SpendCount = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_winUsers), m_winUsers);
		ReadMapData(obj, NAME_TO_STR(m_SpendType), m_SpendType);
		ReadMapData(obj, NAME_TO_STR(m_SpendCount), m_SpendCount);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_winUsers), m_winUsers);
		WriteKeyValue(pack, NAME_TO_STR(m_SpendType), m_SpendType);
		WriteKeyValue(pack, NAME_TO_STR(m_SpendCount), m_SpendCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActivityReplyDrawOpen(); }
};

struct LMsgC2SActivityRequestDraw:public LMsgSC
{
	Lstring		m_strUUID;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_cardNum;	// 玩家的卡 客户端不需要填充 服务器内部转发使用

	LMsgC2SActivityRequestDraw() :LMsgSC(MSG_C_2_S_ACTIVITY_REQUEST_DRAW)
	{
		m_userId = 0;
		m_cardNum = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_cardNum), m_cardNum);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_cardNum), m_cardNum);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SActivityRequestDraw(); }
};

struct LMsgS2CActivityReplyDraw:public LMsgSC
{
	Lint		m_errorCode;	// 转盘结果 0成功 -1未知错误 1消耗的物品不足（一般卡不足） 2 抽奖活动已经关闭或者未开启
	// 结果为0时 下面的值有效
	Lint		m_GiftIndex;	// 奖励索引
	Lstring		m_GiftName;		// 奖励名称
	Lint		m_GiftType;		// 奖励类型 参照ActivityDrawGift 1房卡（单位个） 2话费（单位元） 3 乐视电视（单位个） 4 IPad min4（单位个） 5 iPhone7 plus（单位个）
	Lint		m_GiftCount;	// 奖励数量
	Lint		m_NeedPhoneNum;	// 是否需要电话
	Lint		m_AutoId;		// 填写电话号码时回执使用

	LMsgS2CActivityReplyDraw() :LMsgSC(MSG_S_2_C_ACTIVITY_REPLY_DRAW)
	{
		m_errorCode = -1;
		m_GiftIndex = 0;
		m_GiftType = 0;
		m_GiftCount = 0;
		m_NeedPhoneNum = false;
		m_AutoId = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_GiftIndex), m_GiftIndex);
		ReadMapData(obj, NAME_TO_STR(m_GiftName), m_GiftName);
		ReadMapData(obj, NAME_TO_STR(m_GiftType), m_GiftType);
		ReadMapData(obj, NAME_TO_STR(m_GiftCount), m_GiftCount);
		ReadMapData(obj, NAME_TO_STR(m_NeedPhoneNum), m_NeedPhoneNum);
		ReadMapData(obj, NAME_TO_STR(m_AutoId), m_AutoId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_GiftIndex), m_GiftIndex);
		WriteKeyValue(pack, NAME_TO_STR(m_GiftName), m_GiftName);
		WriteKeyValue(pack, NAME_TO_STR(m_GiftType), m_GiftType);
		WriteKeyValue(pack, NAME_TO_STR(m_GiftCount), m_GiftCount);
		WriteKeyValue(pack, NAME_TO_STR(m_NeedPhoneNum), m_NeedPhoneNum);
		WriteKeyValue(pack, NAME_TO_STR(m_AutoId), m_AutoId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActivityReplyDraw(); }
};

//客户端 分享朋友圈 成功
struct LMsgC2SActivityRequestShare:public LMsgSC
{
	Lstring		m_strUUID;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用

	LMsgC2SActivityRequestShare() :LMsgSC(MSG_C_2_S_ACTIVITY_REQUEST_SHARE)
	{
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
 		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
 		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SActivityRequestShare(); }
};

struct LMsgS2CActivityFinishShare:public LMsgSC
{
	Lint		m_ErrorCode;	// 转盘结果 0成功 -1未知错误
	Lint		m_GiftCount;	// 奖励房卡张数0=不奖励
	Lint		m_GiftType;		// 奖励类型 参照ActivityDrawGift

	LMsgS2CActivityFinishShare() :LMsgSC(MSG_S_2_C_ACTIVITY_FINISH_SHARE)
	{
		m_ErrorCode = -1;
		m_GiftCount = 0;
		m_GiftType = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_ErrorCode), m_ErrorCode);
		ReadMapData(obj, NAME_TO_STR(m_GiftCount), m_GiftCount);
		ReadMapData(obj, NAME_TO_STR(m_GiftType), m_GiftType);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_ErrorCode), m_ErrorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_GiftType), m_GiftType);
		WriteKeyValue(pack, NAME_TO_STR(m_GiftCount), m_GiftCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActivityFinishShare(); }
};

struct LMsgS2CGhostCard:public LMsgSC
{
	CardValue	m_ghostCard;//鬼牌
	LMsgS2CGhostCard() :LMsgSC(MSG_S_2_C_GHOST_CARD){}

	virtual bool Read(msgpack::object& obj)
	{
		m_ghostCard.Read(obj);;
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		m_ghostCard.Write(pack);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CGhostCard(); }
};

// Add HuNanZhuanZhuan by hanlu 02242017 Begin
struct gang_think{
	CardValue				m_gang_card;	//杠出的牌
	std::vector<ThinkData>	m_think;		//思考
	MSGPACK_DEFINE(m_gang_card, m_think); 
};

//玩家杠牌思考
struct LMsgS2CUserGangThinkCard : public LMsgSC
{
	Lint		m_pos;
	CardValue	m_gang;
	std::vector<gang_think> m_card;

	LMsgS2CUserGangThinkCard():LMsgSC(MSG_S_2_C_GANG_THINK),m_pos(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_gang), m_gang);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserGangThinkCard();}
};

//服务器广播玩家扎鸟信息
struct LMsgS2CZhaBirdShow:public LMsgSC
{
	Lint    m_type;  //0:扎鸟 1:飞鸟
	Lint	m_end_pos;	
	std::vector<BirdInfo>	m_bird_infos;	//鸟牌

	LMsgS2CZhaBirdShow():LMsgSC(MSG_S_2_C_ZHA_BIRD),m_end_pos(0),m_type(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_end_pos), m_end_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_bird_infos), m_bird_infos);


		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CZhaBirdShow();}
};

struct RankInfo
{
	Lint m_id;					// 玩家ID
	Lint m_count;				// 玩的局数
	Lstring m_nickName;			// 昵称
	Lstring m_imageUrl;			// 头像
	Lint m_diamond;				// 奖励的钻石数量

	MSGPACK_DEFINE(m_id, m_count, m_nickName, m_imageUrl, m_diamond);

	RankInfo()
	{
		m_id = 0;
		m_count = 0;
		m_diamond = 0;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_count), m_count);
		ReadMapData(obj, NAME_TO_STR(m_nickName), m_nickName);
		ReadMapData(obj, NAME_TO_STR(m_imageUrl), m_imageUrl);
		ReadMapData(obj, NAME_TO_STR(m_diamond), m_diamond);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_count), m_count);
		WriteKeyValue(pack, NAME_TO_STR(m_nickName), m_nickName);
		WriteKeyValue(pack, NAME_TO_STR(m_imageUrl), m_imageUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_diamond), m_diamond);
		return true;
	}
};

//客户端获取排行榜信息
struct LMsgC2SUpdateRank:public LMsgSC
{
	Lint m_rankType;		//排行榜类型 1本周榜 2本月榜 3上周榜 4上月榜
	LMsgC2SUpdateRank() :LMsgSC(MSG_C_2_S_UPDATE_RANK)
	{
		m_rankType = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_rankType), m_rankType);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_rankType), m_rankType);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SUpdateRank(); }
};

//返回排行榜数据
struct LMsgS2CUpdateRank:public LMsgSC
{
	Lint m_rankType;		// 排行榜类型 1本周榜 2本月榜 3上周榜 4上月榜
	Lint m_rankReward;		// 0未领取 1已领取
	Lint m_rankTime;		// 剩余多少秒
	std::vector<RankInfo>	m_randData;
	LMsgS2CUpdateRank() :LMsgSC(MSG_S_2_C_UPDATE_RANK)
	{
		m_rankType = 0;
		m_rankReward = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_rankType), m_rankType);
		ReadMapData(obj, NAME_TO_STR(m_rankReward), m_rankReward);
		ReadMapData(obj, NAME_TO_STR(m_randData), m_randData);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_rankType), m_rankType);
		WriteKeyValue(pack, NAME_TO_STR(m_rankReward), m_rankReward);
		WriteKeyValue(pack, NAME_TO_STR(m_rankTime), m_rankTime);
		WriteKeyValue(pack, NAME_TO_STR(m_randData), m_randData);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CUpdateRank(); }
};

//客户端领取奖励
struct LMsgC2SGetRankReward:public LMsgSC
{
	Lint m_rankType;		//排行榜类型 1本周榜 2本月榜 3上周榜 4上月榜
	LMsgC2SGetRankReward() :LMsgSC(MSG_C_2_S_GET_RANK_REWARD)
	{
		m_rankType = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_rankType), m_rankType);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_rankType), m_rankType);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SGetRankReward(); }
};

struct LMsgS2CGetRankReward:public LMsgSC
{
	Lint m_rankType;		// 排行榜类型 1本周榜 2本月榜 3上周榜 4上月榜
	Lint m_result;			// >0领取到的数量 -1不在排名中 -2已经领取过 -3没有对应的排名奖励，也就是排行榜数量大于配置的奖励数量

	LMsgS2CGetRankReward() :LMsgSC(MSG_S_2_C_GET_RANK_REWARD)
	{
		m_rankType = 0;
		m_result = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_rankType), m_rankType);
		ReadMapData(obj, NAME_TO_STR(m_rankReward), m_result);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_rankType), m_rankType);
		WriteKeyValue(pack, NAME_TO_STR(m_rankReward), m_result);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CGetRankReward(); }
};

struct LMsgC2SLocation :public LMsgSC
{
	Lstring		m_location;

	LMsgC2SLocation() :LMsgSC(MSG_C_2_S_LOCATION)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_location), m_location);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_location), m_location);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SLocation(); }
};

//////////////////////////////////////////////////////////////////////////
//玩家请求加入桌子结果
struct LMsgS2CLocation :public LMsgSC
{
	Lint		m_pos;		// 玩家牌桌位置
	Lstring		m_location;	// GPS位置

	LMsgS2CLocation() :LMsgSC(MSG_S_2_C_LOCATION)
	{
		m_pos = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_location), m_location);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_location), m_location);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CLocation(); }
};

// 分享领钻石
struct LMsgC2SShare:public LMsgSC
{
	Lstring		m_strUUID;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用

	LMsgC2SShare() :LMsgSC(MSG_C_2_S_SHARE)
	{
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
 		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
 		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SShare(); }
};

struct LMsgS2CShare:public LMsgSC
{
	Lint		m_ErrorCode;	// 0失败 1成功

	LMsgS2CShare() :LMsgSC(MSG_S_2_C_SHARE)
	{
		m_ErrorCode = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_ErrorCode), m_ErrorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_ErrorCode), m_ErrorCode);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CShare(); }
};

struct GameTask
{
	Lint m_taskId;			// 任务ID
	Lint m_taskType;		// 任务类型    对应 TaskType
	Lint m_taskTime;		// 任务时间    对应 TaskTime
	Lint m_taskCount;		// 所需次数
	Lint m_cardCount;		// 奖励房卡数量
	Lstring m_desc;			// 任务描述

	GameTask()
	{
		m_taskId = 0;
		m_taskType = 0;
		m_taskTime = 0;
		m_taskCount = 0;
		m_cardCount = 0;
	}

	bool operator!()const{ return m_taskId == 0; }
};

struct MsgTask
{
	Lint m_taskId;			// 任务ID
	Lint m_curCount;		// 当前次数
	Lint m_needCount;		// 所需次数
	Lint m_cardCount;		// 奖励房卡数量
	Lint m_get;				// 是否领取了奖励 0未领取 1已领取
	Lstring m_desc;			// 任务描述

	// 不需要发送的数据
	Lint m_taskType;		// 任务类型    对应 TaskType
	Lint m_taskTime;		// 任务时间    对应 TaskTime

	MSGPACK_DEFINE(m_taskId, m_curCount, m_needCount, m_cardCount, m_get, m_desc)

	MsgTask(const GameTask& task)
	{
		*this = task;
	}

	MsgTask& operator=(const GameTask& task)
	{
		m_taskId = task.m_taskId;
		m_needCount = task.m_taskCount;
		m_cardCount = task.m_cardCount;
		m_desc = task.m_desc;

		m_taskType = task.m_taskType;
		m_taskTime = task.m_taskTime;

		return *this;
	}
};

// 获取任务信息
struct LMsgC2STaskInfo:public LMsgSC
{
	Lstring		m_strUUID;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用

	LMsgC2STaskInfo() :LMsgSC(MSG_C_2_S_TASK_INFO)
	{
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
 		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
 		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2STaskInfo(); }
};

struct LMsgS2CTaskInfo:public LMsgSC
{
	std::vector<MsgTask> m_tasks;

	LMsgS2CTaskInfo() :LMsgSC(MSG_S_2_C_TASK_INFO)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
 		ReadMapData(obj, NAME_TO_STR(m_tasks), m_tasks);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_tasks), m_tasks);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CTaskInfo(); }
};

// 领取任务奖励
struct LMsgC2STaskReward:public LMsgSC
{
	Lstring		m_strUUID;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用
	Lint		m_taskId;	// 任务ID

	LMsgC2STaskReward() :LMsgSC(MSG_C_2_S_TASK_REWARD)
	{
		m_userId = 0;
		m_taskId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
 		ReadMapData(obj, NAME_TO_STR(m_strUUID), m_strUUID);
 		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_taskId), m_taskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_strUUID), m_strUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_taskId), m_taskId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2STaskReward(); }
};

struct LMsgS2CTaskReward:public LMsgSC
{
	Lint		m_ErrorCode;	// 0成功 1未完成 2已领取 3无任务 4服务器错误
	Lint		m_taskId;

	LMsgS2CTaskReward() :LMsgSC(MSG_S_2_C_TASK_REWARD)
	{
		m_ErrorCode = 0;
		m_taskId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_ErrorCode), m_ErrorCode);
		ReadMapData(obj, NAME_TO_STR(m_taskId), m_taskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_ErrorCode), m_ErrorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_taskId), m_taskId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CTaskReward(); }
};

struct PaiJuInfo
{
	Lint m_roomId;    // 房间ID
	Lint m_roomCounts;  // 局数,1-3分别表示4-8-10局
	Lint m_roomType;  // 407表示3人麻将，为408表示4人麻将，为406表示2人麻将
	Lint m_roomScore;  // 牌局底分，400、401、402、403、404分别表示1、2、4、3、5分
	Lstring m_roomState;  // 房间状态
	Lstring m_user1;
	Lstring m_user2;
	Lstring m_user3;
	Lstring m_user4;

	PaiJuInfo()
	{
		m_roomId = 0;
		m_roomCounts = 0;
	}

	MSGPACK_DEFINE(m_roomId, m_roomCounts, m_roomType, m_roomScore, m_roomState, m_user1, m_user2, m_user3, m_user4);

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_roomId), m_roomId);
		ReadMapData(obj, NAME_TO_STR(m_roomCounts), m_roomCounts);
		ReadMapData(obj, NAME_TO_STR(m_roomType), m_roomType);
		ReadMapData(obj, NAME_TO_STR(m_roomScore), m_roomScore);
		ReadMapData(obj, NAME_TO_STR(m_roomState), m_roomState);
		ReadMapData(obj, NAME_TO_STR(m_user1), m_user1);
		ReadMapData(obj, NAME_TO_STR(m_user2), m_user2);
		ReadMapData(obj, NAME_TO_STR(m_user3), m_user3);
		ReadMapData(obj, NAME_TO_STR(m_user4), m_user4);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_roomId), m_roomId);
		WriteKeyValue(pack, NAME_TO_STR(m_roomCounts), m_roomCounts);
		WriteKeyValue(pack, NAME_TO_STR(m_roomType), m_roomType);
		WriteKeyValue(pack, NAME_TO_STR(m_roomScore), m_roomScore);
		WriteKeyValue(pack, NAME_TO_STR(m_roomState), m_roomState);
		WriteKeyValue(pack, NAME_TO_STR(m_user1), m_user1);
		WriteKeyValue(pack, NAME_TO_STR(m_user2), m_user2);
		WriteKeyValue(pack, NAME_TO_STR(m_user3), m_user3);
		WriteKeyValue(pack, NAME_TO_STR(m_user4), m_user4);
		return true;
	}
};

struct GonghuiUser
{
	Lint id;
	Lstring name;

	MSGPACK_DEFINE(id, name);

	GonghuiUser()
	{
		id = 0;
		name = "";
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(id), id);
		ReadMapData(obj, NAME_TO_STR(name), name);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(id), id);
		WriteKeyValue(pack, NAME_TO_STR(name), name);
		return true;
	}
};

struct Gonghui
{
	Lint m_gonghuiId;
	Lstring m_gonghuiName;
	Lint m_adminUserId;
	Lstring m_roomPolicy;   // 工会房间策略：房间类型，房间底分，房间局数，房间个数
	Lint m_paijuCount;   // 当前工会下有多少房间(当前未去除已开始房间)
	std::vector<PaiJuInfo> m_paijuInfo;
	Lint m_userCount;    // 工会下有多少用户
	std::vector<GonghuiUser> m_userInfo;  // 用户的ID信息

	Gonghui()
	{
		m_gonghuiId = 0;
		m_gonghuiName = "";
		m_adminUserId = 0;
		m_userCount = 0;
		m_roomPolicy = "";
	}

	MSGPACK_DEFINE(m_gonghuiId, m_gonghuiName, m_adminUserId, m_roomPolicy, m_paijuCount, m_paijuInfo, m_userCount, m_userInfo);

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		ReadMapData(obj, NAME_TO_STR(m_gonghuiName), m_gonghuiName);
		ReadMapData(obj, NAME_TO_STR(m_adminUserId), m_adminUserId);
		ReadMapData(obj, NAME_TO_STR(m_roomPolicy), m_roomPolicy);
		ReadMapData(obj, NAME_TO_STR(m_paijuCount), m_paijuCount);
		ReadMapData(obj, NAME_TO_STR(m_paijuInfo), m_paijuInfo);
		ReadMapData(obj, NAME_TO_STR(m_userCount), m_userCount);
		ReadMapData(obj, NAME_TO_STR(m_userInfo), m_userInfo);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		WriteKeyValue(pack, NAME_TO_STR(m_gonghuiName), m_gonghuiName);
		WriteKeyValue(pack, NAME_TO_STR(m_adminUserId), m_adminUserId);
		WriteKeyValue(pack, NAME_TO_STR(m_roomPolicy), m_roomPolicy);
		WriteKeyValue(pack, NAME_TO_STR(m_paijuCount), m_paijuCount);
		WriteKeyValue(pack, NAME_TO_STR(m_paijuInfo), m_paijuInfo);
		WriteKeyValue(pack, NAME_TO_STR(m_userCount), m_userCount);
		WriteKeyValue(pack, NAME_TO_STR(m_userInfo), m_userInfo);
		return true;
	}

	bool Read(LBuff& buff)
	{
		buff.Read(m_gonghuiId);
		buff.Read(m_gonghuiName);
		buff.Read(m_adminUserId);
		buff.Read(m_roomPolicy);

		buff.Read(m_paijuCount);
		for (int i = 0; i < m_paijuCount; i++)
		{
			PaiJuInfo paiJu;
			buff.Read(paiJu.m_roomId);
			buff.Read(paiJu.m_roomCounts);
			buff.Read(paiJu.m_roomType);
			buff.Read(paiJu.m_roomScore);
			buff.Read(paiJu.m_roomState);
			buff.Read(paiJu.m_user1);
			buff.Read(paiJu.m_user2);
			buff.Read(paiJu.m_user3);
			buff.Read(paiJu.m_user4);
			m_paijuInfo.push_back(paiJu);
		}

		buff.Read(m_userCount);
		for (int j = 0; j < m_userCount; j++)
		{
			GonghuiUser tmpUser;
			buff.Read(tmpUser.id);
			buff.Read(tmpUser.name);
			m_userInfo.push_back(tmpUser);
		}
		return true;
	}

	bool Write(LBuff& buff)
	{
		buff.Write(m_gonghuiId);
		buff.Write(m_gonghuiName);
		buff.Write(m_adminUserId);
		buff.Write(m_roomPolicy);
		buff.Write(m_paijuCount);
		for (int i = 0; i < m_paijuCount; i++)
		{
			buff.Write(m_paijuInfo[i].m_roomId);
			buff.Write(m_paijuInfo[i].m_roomCounts);
			buff.Write(m_paijuInfo[i].m_roomType);
			buff.Write(m_paijuInfo[i].m_roomScore);
			buff.Write(m_paijuInfo[i].m_roomState);
			buff.Write(m_paijuInfo[i].m_user1);
			buff.Write(m_paijuInfo[i].m_user2);
			buff.Write(m_paijuInfo[i].m_user3);
			buff.Write(m_paijuInfo[i].m_user4);
		}

		buff.Write(m_userCount);
		for (int j = 0; j < m_userCount; j++)
		{
			buff.Write(m_userInfo[j].id);
			buff.Write(m_userInfo[j].name);
		}
		return true;
	}
};

struct LMsgCe2LGonghuiInfo : public LMsg
{
	Lint m_count;
	std::vector<Gonghui> m_gonghui;
	Lint	m_hasSentAll; //已经发送所有了的id。 0：没有 1：已经发送所有

	LMsgCe2LGonghuiInfo() : LMsg(MSG_CE_2_L_GONHUI_INFO)
	{
		m_count = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_count);
		for (Lint i = 0; i < m_count; ++i)
		{
			m_gonghui.push_back(Gonghui());
			m_gonghui.back().Read(buff);
		}
		buff.Read(m_hasSentAll);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		m_count = m_gonghui.size();
		buff.Write(m_count);
		for (Lint i = 0; i < m_count; ++i)
		{
			m_gonghui[i].Write(buff);
		}
		buff.Write(m_hasSentAll);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGonghuiInfo();
	}
};

// 工会相关消息定义在这里

struct LMsgC2SGonghuiInfo : public LMsgSC
{
	Lstring m_strUserUUID;
	Lint		m_userId;	// 客户端不需要填充 服务器内部转发使用

	LMsgC2SGonghuiInfo() : LMsgSC(MSG_C_2_S_GONGHUI_INFO)
	{
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SGonghuiInfo();
	}
};

struct LMsgS2CGonghuiInfo : public LMsgSC
{
	Lstring m_strUserUUID;
	Lint		m_userId;
	std::vector<Gonghui> m_gonghui;

	LMsgS2CGonghuiInfo() : LMsgSC(MSG_S_2_C_GONGHUI_INFO)
	{
		m_userId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_gonghui), m_gonghui);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_gonghui), m_gonghui);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CGonghuiInfo();
	}
};

struct LMsgC2SGonghuiApply : public LMsgSC
{
	Lstring m_strUserUUID;  // 客户端不需要填充 服务器内部转发使用
	Lint    m_gonghuiId;

	LMsgC2SGonghuiApply() : LMsgSC(MSG_C_2_S_GONGHUI_APPLY)
	{
		m_gonghuiId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SGonghuiApply();
	}
};

struct LMsgS2CGonghuiOPResult : public LMsgSC
{
	// 操作类型，对应不同的操作，比如申请加入工会、剔除用户等
	// 1同意用户加入工会
	// 2拒绝用户加入工会
	// 3删除工会用户
	// 4申请加入工会
	// 5创建工会房间策略
	// 6删除工会房间策略
	// 7解散工会
	Lint    m_opType; 
	// 0  操作成功
	// -1 工会不存在
	// -2 用户未申请加入工会
	// -3 工会房间策略非法
	// -4 创建工会房间失败
	// -5 房卡不足
	// -6 用户已经是工会成员
	// -7 新增用户到数据库失败
	// -8 从数据库中删除用户失败
	// -9 删除工会用户时，不能删除会长
	// -10 非工会会长不能解散工会
	// -11 解散工会失败
	// -12 此策略已经存在
	Lint    m_errorCode; // 操作结果： 0成功，其他失败

	LMsgS2CGonghuiOPResult() : LMsgSC(MSG_S_2_C_GONGHUI_OPRESULT)
	{
		m_errorCode = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_opType), m_opType);
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_opType), m_opType);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CGonghuiOPResult();
	}
};

struct LMsgC2SGonghuiUserOp : public LMsgSC
{
	Lstring m_strUserUUID;  // 客户端不需要填充 服务器内部转发使用
	// 1 同意用户加入工会,2 拒绝用户加入工会,3 从工会剔除用户,7 解散工会
	Lint	m_opType;
	Lint    m_gonghuiId;
	Lint    m_userId;

	LMsgC2SGonghuiUserOp() : LMsgSC(MSG_C_2_S_GONGHUI_USEROP)
	{
		m_opType = 0;
		m_gonghuiId = 0;
		m_userId = -1;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_opType), m_opType);
		ReadMapData(obj, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_opType), m_opType); 
		WriteKeyValue(pack, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SGonghuiUserOp();
	}
};

struct LMsgC2SQueryGonghuiDesk : public LMsgSC
{
	Lstring m_strUserUUID;  // 客户端不需要填充 服务器内部转发使用
	Lint    m_gonghuiId;
	Lint    m_beginPos;    // 按照时间排序，查询多少条数据，起始和结束
	Lint    m_endPos;

	LMsgC2SQueryGonghuiDesk() : LMsgSC(MSG_C_2_S_GONGHUI_QUERYDESK)
	{
		m_gonghuiId = 0;
		m_beginPos = 0;
		m_endPos = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		ReadMapData(obj, NAME_TO_STR(m_beginPos), m_beginPos);
		ReadMapData(obj, NAME_TO_STR(m_endPos), m_endPos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		WriteKeyValue(pack, NAME_TO_STR(m_beginPos), m_beginPos);
		WriteKeyValue(pack, NAME_TO_STR(m_endPos), m_endPos);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SQueryGonghuiDesk();
	}
};

struct RoomRecord
{
	Lint m_roomId;    // 房间ID
	Lint m_gonghuiId; // 工会ID
	Lint m_baseScore; // 牌局底分，400、401、402分别表示1、2、4分
	Lint m_cardType;  // 局数,1-3分别表示4-8-10局
	Lint m_playType;  // 407表示3人麻将，为408表示4人麻将
	Lint m_playNums;  // 实际房间打的局数
	Lstring m_roomState;  // 房间状态，正常结算，还是解散
	Lint m_userId[4];
	Lstring m_userName[4];
	Lint m_score[4];
	Lint m_playTime;

	RoomRecord()
	{
		m_roomId = 0;
		m_gonghuiId = 0;
		m_baseScore = 0;
		m_cardType = 0;
		m_playType = 0;
		m_playNums = 0;
		m_roomState = "";
		for (int i = 0; i < 4; i++)
		{
			m_score[i] = 0;
			m_userId[i] = 0;
			m_userName[i] = "";
		}
		m_playTime = 0;
	}

	MSGPACK_DEFINE(m_roomId, m_gonghuiId, m_baseScore, m_cardType, m_playType, m_playNums, m_roomState, m_userId, m_userName, m_score, m_playTime);

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_roomId), m_roomId);
		ReadMapData(obj, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		ReadMapData(obj, NAME_TO_STR(m_baseScore), m_baseScore);
		ReadMapData(obj, NAME_TO_STR(m_cardType), m_cardType);
		ReadMapData(obj, NAME_TO_STR(m_playType), m_playType);
		ReadMapData(obj, NAME_TO_STR(m_playNums), m_playNums);
		ReadMapData(obj, NAME_TO_STR(m_roomState), m_roomState);
		//ReadMapData(obj, NAME_TO_STR(m_user), m_user);
		//ReadMapData(obj, NAME_TO_STR(m_score), m_score);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 11);
		WriteKeyValue(pack, NAME_TO_STR(m_roomId), m_roomId);
		WriteKeyValue(pack, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		WriteKeyValue(pack, NAME_TO_STR(m_baseScore), m_baseScore);
		WriteKeyValue(pack, NAME_TO_STR(m_cardType), m_cardType);
		WriteKeyValue(pack, NAME_TO_STR(m_playType), m_playType);
		WriteKeyValue(pack, NAME_TO_STR(m_playNums), m_playNums);
		WriteKeyValue(pack, NAME_TO_STR(m_roomState), m_roomState);

		WriteKey(pack, "m_userId");
		std::vector<Lint> vce;
		for (Lint i = 0; i < 4; ++i)
		{
			vce.push_back(m_userId[i]);
		}
		WriteKey(pack, vce);

		WriteKey(pack, "m_userName");
		std::vector<Lstring> vce3;
		for (Lint i = 0; i < 4; ++i)
		{
			vce3.push_back(m_userName[i]);
		}
		WriteKey(pack, vce3);
		
		WriteKey(pack, "m_score");
		std::vector<Lint> vce2;
		for (Lint i = 0; i < 4; ++i)
		{
			vce2.push_back(m_score[i]);
		}
		WriteKey(pack, vce2);

		WriteKeyValue(pack, NAME_TO_STR(m_playTime), m_playTime);
		return true;
	}
};

struct LMsgS2CQueryGonghuiDesk : public LMsgSC
{
	Lstring m_strUserUUID;  // 客户端不需要填充 服务器内部转发使用
	Lint    m_gonghuiId;
	Lint    m_deskCounts;  // 返回当前查询的桌子数量，如果比实际查询要求的少，说明已经查询完
	std::vector<RoomRecord> m_records;

	LMsgS2CQueryGonghuiDesk() : LMsgSC(MSG_S_2_C_GONGHUI_QUERYDESK)
	{
		m_gonghuiId = 0;
		m_deskCounts = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		ReadMapData(obj, NAME_TO_STR(m_deskCounts), m_deskCounts);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskCounts), m_deskCounts);
		//WriteKeyValue(pack, NAME_TO_STR(m_records), m_records);
		WriteKey(pack, NAME_TO_STR(m_records));
		WriteArray(pack, m_records.size());
		for (Lint i = 0; i < m_records.size(); i++)
		{
			m_records[i].Write(pack);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CQueryGonghuiDesk();
	}
};

struct LMsgC2SGonghuiRoomOP : public LMsgSC
{
	Lstring m_strUserUUID;  // 客户端不需要填充 服务器内部转发使用
	Lint    m_gonghuiId;
	Lstring m_opType;     // 操作类型：add、delete分别代表新增和删除
	Lint    m_playType;   // 房间类型：407表示3人麻将，408表示4人麻将
	Lint    m_baseScoreType;  // 房间底分类型：400、401、402分别表示1、2、4分
	Lint    m_roomType; // 房间局数类型：1、2、3分别表示4、8、10局
	Lint    m_roomNumber;  // 房间个数

	LMsgC2SGonghuiRoomOP() : LMsgSC(MSG_C_2_S_GONGHUI_ROOMOP)
	{
		m_gonghuiId = 0;
		m_opType = "";
		m_playType = 0;
		m_baseScoreType = 0;
		m_roomType = 0;
		m_roomNumber = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		ReadMapData(obj, NAME_TO_STR(m_opType), m_opType);
		ReadMapData(obj, NAME_TO_STR(m_playType), m_playType);
		ReadMapData(obj, NAME_TO_STR(m_baseScoreType), m_baseScoreType);
		ReadMapData(obj, NAME_TO_STR(m_roomType), m_roomType);
		ReadMapData(obj, NAME_TO_STR(m_roomNumber), m_roomNumber);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		WriteKeyValue(pack, NAME_TO_STR(m_opType), m_opType);
		WriteKeyValue(pack, NAME_TO_STR(m_playType), m_playType);
		WriteKeyValue(pack, NAME_TO_STR(m_baseScoreType), m_baseScoreType);
		WriteKeyValue(pack, NAME_TO_STR(m_roomType), m_roomType);
		WriteKeyValue(pack, NAME_TO_STR(m_roomNumber), m_roomNumber);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SGonghuiRoomOP();
	}
};

struct LMsgS2CGonghuiDeskChange : public LMsgSC
{
	Lint m_gonghuiId;  // 工会ID
	Gonghui m_gonghui;  // 工会信息

	LMsgS2CGonghuiDeskChange() : LMsgSC(MSG_S_2_C_GONGHUIDESK_CHANGE)
	{
		m_gonghuiId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		ReadMapData(obj, NAME_TO_STR(m_gonghui), m_gonghui);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		WriteKeyValue(pack, NAME_TO_STR(m_gonghui), m_gonghui);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CGonghuiDeskChange();
	}
};

struct LMsgS2CQueryApplyInfo : public LMsgSC
{
	Lstring m_strUserUUID;  // 客户端不需要填充 服务器内部转发使用
	Lint    m_gonghuiId;
	Lint    m_opResult;   // 0 成功，1 工会不存在，2 当前用户不是工会会长，3 获取列表失败
	std::vector<GonghuiUser> m_gonghuiUser;

	LMsgS2CQueryApplyInfo() : LMsgSC(MSG_S_2_C_GONGHUI_QUERYAPPLYINFO)
	{
		m_gonghuiId = 0;
		m_opResult = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		ReadMapData(obj, NAME_TO_STR(m_opResult), m_opResult);
		ReadMapData(obj, NAME_TO_STR(m_gonghuiUser), m_gonghuiUser);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		WriteKeyValue(pack, NAME_TO_STR(m_opResult), m_opResult);
		WriteKeyValue(pack, NAME_TO_STR(m_gonghuiUser), m_gonghuiUser);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CQueryApplyInfo();
	}
};

struct LMsgC2SQueryApplyInfo : public LMsgSC
{
	Lstring m_strUserUUID;  // 客户端不需要填充 服务器内部转发使用
	Lint    m_gonghuiId;

	LMsgC2SQueryApplyInfo() : LMsgSC(MSG_C_2_S_GONGHUI_QUERYAPPLYINFO)
	{
		m_gonghuiId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_gonghuiId), m_gonghuiId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SQueryApplyInfo();
	}
};

struct LMsgC2SQueryDeskHistory : public LMsgSC
{
	Lstring m_strUserUUID;  // 客户端不需要填充 服务器内部转发使用
	Lint    m_deskId;

	LMsgC2SQueryDeskHistory() : LMsgSC(MSG_C_2_S_QUERYDESK_HISTORY)
	{
		m_deskId = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SQueryDeskHistory();
	}
};

struct LMsgS2CQueryDeskHistory : public LMsgSC
{
	Lint    m_opRet;        // 返回结果，0成功，-1获取数据失败
	Lstring m_strUserUUID;  // 客户端不需要填充 服务器内部转发使用
	Lint    m_matchSize;
	VipLog  m_match[20];

	LMsgS2CQueryDeskHistory() : LMsgSC(MSG_S_2_C_QUERYDESK_HISTORY)
	{
		m_opRet = 0;
		m_matchSize = 0;
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_strUserUUID), m_strUserUUID);
		ReadMapData(obj, NAME_TO_STR(m_opRet), m_opRet);
		ReadMapData(obj, NAME_TO_STR(m_matchSize), m_matchSize);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_strUserUUID", m_strUserUUID);
		WriteKeyValue(pack, NAME_TO_STR(m_opRet), m_opRet);
		WriteKeyValue(pack, NAME_TO_STR(m_matchSize), m_matchSize);
		
		WriteKey(pack, "m_match");
		WriteArray(pack, m_matchSize);
		for (Lint k = 0; k < m_matchSize; ++k)
		{
			WriteMap(pack, 3);
			WriteKeyValue(pack, "m_time", m_match[k].m_time);
			WriteKeyValue(pack, "m_videoId", m_match[k].m_videoId);

			std::vector<Lint> vec;
			for (Lint n = 0; n < 4; ++n)
			{
				vec.push_back(m_match[k].m_score[n]);
			}
			WriteKey(pack, "m_score");
			WriteKey(pack, vec);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CQueryDeskHistory();
	}
};

struct LMsgS2CGonghuiStateChagne : public LMsgSC
{
	std::vector<Lint> m_changeItems; // 每一个变更的项

	LMsgS2CGonghuiStateChagne() : LMsgSC(MSG_S_2_C_GONHUISTATE_CHANGE)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_changeItems), m_changeItems);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_changeItems), m_changeItems);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CGonghuiStateChagne();
	}
};
#endif