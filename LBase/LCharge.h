#ifndef _L_CHARGE_H_
#define _L_CHARGE_H_

#include "LBuff.h"

//房卡类型
// enum CARD_TYPE
// {
// 	CARD_TYPE_4 = 1,//4局卡
// 	CARD_TYPE_8 = 2,//8局卡
// 	CARD_TYPE_16 = 3,//16局卡
// 	CARD_TYPE_Other = 100,   //数量
// };

//现在不区分房卡 统一只算数量 一定要连续
//没说明的是房主出房卡
enum CARD_TYPE
{
	CARD_TYPE_NONE		= 0,
	//商丘玩法
	CARD_TYPE_4 		= 1,			//商丘4局
	CARD_TYPE_8			= 2,			//商丘8局
	CARD_TYPE_10		= 3,			//商丘10局，每人出房卡
	CARD_TYPE_16		= 4,			//16局
	//周口带跑玩法
	CARD_TYPE_ZK_1		= 5,			// 1圈
	CARD_TYPE_ZK_2		= 6,			// 2圈
	CARD_TYPE_ZK_4		= 7,			// 每人出房卡
	//周口平胡玩法
	CARD_TYPE_ZK_PH_4	= 8,
	CARD_TYPE_ZK_PH_8	= 9,			// 每人出房卡
	CARD_TYPE_ZK_PH_16	= 10,			// 每人出房卡

	CARD_TYPE_MAX,

	CARD_TYPE_DEFAULT = CARD_TYPE_8,			// 有些地方的消息出错时就使用默认值

	//这个不管，旧代码的东西
	CARD_TYPE_Other = 100,   //数量
};

//根据圈数获取对应CARD_TYPE
//Lint GetCardTypeByCircle(int n);

//获取所需房卡数量 gameType商丘周口等  cardType几局 count所需数量
//返回是否所有玩家都扣
bool NeedCardCount(const Lint cardType, Lint& count);

//获取可以打几圈
Lint GetCircle(const Lint gameType, const Lint cardType);

//对应CARD_TYPE 玩对应局数需要的房卡数量
//extern Lint CARD_TYPE_NEED_COUNT[CARD_TYPE_MAX];
//对应CARD_TYPE 打多少圈
//extern Lint CARD_TYPE_CIRCLE[CARD_TYPE_MAX];

enum CARDS_OPER_TYPE
{
	CARDS_OPER_TYPE_INIT = 0,//新手赠送
	CARDS_OPER_TYPE_CHARGE = 1,//充值活动
	CARDS_OPER_TYPE_FREE_GIVE = 2,//免费赠送
	CARDS_OPER_TYPE_CREATE_ROOM = 3,//创建房间扣除
	CARDS_OPER_TYPE_ACTIVE = 4,		//活动送房卡
	CARDS_OPER_TYPE_INVITED = 5,    //邀请好友
	CARDS_OPER_TYPE_EXCHANGE = 6,   //换金币
	CARDS_OPER_TYPE_ACTIVE_DRAW = 7,//转盘消耗
	CARDS_OPER_TYPE_RANK = 8,		//排行榜奖励
	CARDS_OPER_TYPE_BIND_AGENT = 9,	//绑定代理赠送
	CARDS_OPER_TYPE_SHARE = 10,		// 分享送房卡
	CARDS_OPER_TYPE_TASK = 11,		// 任务奖励房卡
	CARDS_OPER_TYPE_FREE_CHARGE = 12, // 后台免费充值
	CARDS_OPER_TYPE_USER_TRANS = 13,  // 用户间免费赠送
};

enum COINS_OPER_TYPE
{
	COINS_OPER_TYPE_DEBUG = -1,//新手赠送
	COINS_OPER_TYPE_CHARGE = 0,//充值
	COINS_OPER_TYPE_FREE_GIVE = 1,//免费赠送
	COINS_OPER_TYPE_RESULT = 2,//房间结算
	COINS_OPER_TYPE_SELF_GIVE = 3,	//玩家自己领取
	COINS_OPER_TYPE_CHANGE_CARD = 4,   //换卡
};

class LCharge
{
public:
	LCharge();
	~LCharge();

	void	Read(LBuff& buff);
	void	Write(LBuff& buff);
public:
	Lint	m_time;
	Lint	m_cardType;
	Lint	m_cardNum;
	Lint	m_type;
	Lint	m_userId;
	Lstring	m_admin;
};
#endif