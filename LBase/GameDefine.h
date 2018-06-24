#ifndef _GAME_DEFINE_H_
#define _GAME_DEFINE_H_

#include "LBase.h"

#if defined(DEBUG) || defined(_DEBUG)
// 定义这个宏表示 不 启用心跳检查
//#define UNCHECKHEARTBEAT
#endif

#define CARD_COUNT		108		//136
#define INIT_CARD_NUM 	8		// 新手房卡数量

//排行榜
#define RANK_REWARD_INTERVAL	(86400*7)		// 目前是一周算一次排行榜奖励，以秒计算
#define RANK_UPDATE_INTERVAL	(3600)			// 排行榜刷新周期

enum CARD_COLOR
{
	CARD_COLOR_WAN = 1, 		// 万
	CARD_COLOR_TUO = 2,			// 饼
	CARD_COLOR_SUO = 3, 		// 条
	CARD_COLOR_ZI  = 4, 		// 东南西北中发白
	CARD_COLOR_FLOWER = 5,		// 春夏秋冬梅兰竹菊
};

//玩家思考类型
enum THINK_OPERATOR
{
	THINK_OPERATOR_DINGQUE = -3,
	THINK_OPERATOR_CHANGE = -2,
	THINK_OPERATOR_REOUT = -1, // 断线续完再次告诉玩家打出去的牌
	THINK_OPERATOR_NULL  = 0, 
	THINK_OPERATOR_OUT   = 1,
	THINK_OPERATOR_BOMB  = 2, // 胡
	THINK_OPERATOR_AGANG = 3, // 暗杠
	THINK_OPERATOR_MGANG = 4, // 明杠
	THINK_OPERATOR_PENG  = 5, // 碰
	THINK_OPERATOR_CHI   = 6, // 吃     目前没用到
	THINK_OPERATOR_ABU   = 7, // 暗杠
	THINK_OPERATOR_MBU   = 8, // 补杠
	THINK_OPERATOR_DIANGANGPAO = 9,// 玩家点杠后自摸胡算点炮胡操作
	THINK_OPERATOR_QIANGANGH = 10, //抢杠胡
	THINK_OPERATOR_TINGCARD = 11,  //听牌	
	THINK_OPERATOR_BAOCI    = 12,  //包呲	
	
};
#define DESK_USER_COUNT 4	//一桌玩家数量

#define INVAILD_POS		4  //无效的位置

#define	PLAY_CARD_TIME		10  //出牌时间
#define WAIT_READY_TIME		15  //准备时间

//桌子当前状态
enum DESK_STATE
{
	DESK_FREE =		0,	//桌子空闲状态
	DESK_WAIT,			//桌子等待状态
	DESK_PLAY,			//正在游戏中
	//	DESK_COUNT_RESULT,	//正在结算中
	//	DESK_RESET_ROOM		//正在申请解散房间中
};

//牌局当前状态
enum DESK_PLAY_STATE
{
	DESK_PLAY_START_HU,		//起手胡状态
	DESK_PLAY_GET_CARD,		//摸牌
	DESK_PLAY_THINK_CARD,	//思考
	DESK_PLAY_END_CARD,		//海底牌
	DESK_PLAY_DINGQUE,		//定缺
	DESK_PLAY_BOTTOM_POUR,		//下跑中
	DESK_PLAY_FANPIGU,		//翻屁股补牌
};

//家胡牌情况，0-没胡，1-自摸，2-收炮，3-点炮
enum WIN_TYPE
{
	WIN_INVALID = -1,
	WIN_ZIMO = 0,	//自摸
	WIN_BOMB,		//点炮
	WIN_NONE,		//慌庄
	WIN_DISS,       //解散房间
};

//胡牌方式
enum WIN_TYPE_SUB
{
	WIN_SUB_NONE = 0,
	WIN_SUB_ZIMO = 1,		//自摸
	WIN_SUB_BOMB = 2,		//收炮
	WIN_SUB_ABOMB = 3,		//点炮

	WIN_SUB_DZIMO = 4,		//大胡自摸
	WIN_SUB_DBOMB = 5,		//大胡收炮
	WIN_SUB_DABOMB = 6,		//大胡点炮
	WIN_SUB_TINGCARD = 7,   //有叫 3/4血战，2房
	
	WIN_SUB_TWO = 8,		//一炮双响
	WIN_SUB_THERE = 9,		//一炮三响
};

//起手胡牌类型
enum START_HU_INFO
{
	START_HU_NULL = 0,
	START_HU_QUEYISE=1,	//缺一色
	START_HU_BANBANHU,		//板板胡
	START_HU_DASIXI,		//大四喜
	START_HU_LIULIUSHUN,	//六六顺
};
//麻将类型
enum GameType
{
	MJHeNanZhengZhou  = 0,  //郑州
	MJHeNanXinYang	= 1,	//信阳
	MJHeNanHuaShui	= 2,	//滑水

	// Add HuNanZhuanZhuan by hanlu 02242017 Begin
	MJHeNanZhuanZhuanTwo	= 3,	//转转2人
	MJHeNanZhuanZhuanThree	= 4,	//转转3人
	MJHeNanZhuanZhuanFour	= 5,	//转转4人
	// Add HuNanZhuanZhuan by hanlu 02242017 End

	MJHeNanTuiDaoHu	= 6,	//推倒胡
	MJHeNanXinXiang	= 7,	//新乡
	MJHeNanJiaoZuo	= 8,	//焦作
		
	MJHeNanLuoYang			= 9,	//洛阳
	MJHeNanLuoHe			= 10,	//漯河
	MJHeNanZhouKou = 11,		//周口
	MJHeNanKaiFeng = 12,
	
	MJHeNanShangQiu2 = 13,		//商丘2人
	MJHeNanShangQiu3 = 14,
	MJHeNanShangQiu4 = 15,

	MJWuHanXianTao = 16,
};

//胡牌类型
enum HU_PAI_LEIXING
{
	HU_NULL = 0,					//没胡
	HU_XIAOHU = 1,					//小胡


	HU_SPECIAL_XIAOQIDUI=2,			//龙七对
	HU_XIAOQIDUI=3,					//七小对

	HU_QINGYISE=4,					//清一色
	HU_JIANGJIANGHU=5,				//将将胡
	HU_PENGPENGHU=6,				//对对胡
	HU_QUANQIUREN=7,				//全球人
	HU_GANG1=8,						//杠上开花
	HU_GANG2=9,						//杠上炮
	HU_END1=10,						//海底捞(扫底胡)
	HU_END2=11,						//海底炮
	HU_QIANGGANGHU=12,				//抢扛胡
	HU_QISHOUFOURLAN=13,			//起手四个赖子
	HU_DOUBLESPECIAL_XIAOQIDUI =14, //双豪华七小队
	HU_TIANHU=15,					//天胡
	HU_DIHU=16,						//地胡
	HU_DANDIAO = 17,				//单钓
	HU_JINGOUDIAO = 18,				//金钩钓

	HU_QINGDUI = 19,				//清对
	HU_QINGQIDUI = 20,				//清七对
	HU_QINGLONGQIDUI = 21,			//清龙七对

	HU_JIANGDUI = 22,               //将对
	HU_JIANGDUI_7 = 23,              //将7对
	HU_ONENINE = 24,                 //全幺九
	HU_MENQING = 25,				//门清
	HU_NOONENINE = 26,				//断幺九 中张

	HU_CA2TIAO = 27,                //4人2房 卡2条
	
	HU_CA5XIN = 28,                 //夹心5
	HU_ONE2NINE = 29,               //一条龙
	HU_SISTERPAIRS = 30,            //姊妹对
	HU_TRIPLESPECIAL_XIAOQIDUI = 31,//超超豪华七小队

	HU_PICI			= 32,			//河南洛阳 胡皮呲
	HU_MCI		= 33,			//明呲
	HU_ANCI		= 34,			//暗呲
	HU_BAOCI		= 35,			//包呲

	HU_GOLDGOUPAO = 41,             //宜宾 金钩炮
	HU_NOGOLDCARD = 42,             //无听用
	HU_MIDGOLD    = 43,             //本金
	HU_TING		  = 44,				//报听
	HU_ZIMOHU     = 50,
	HU_QINGSANDA = 80,				//清三搭
	HU_QIANSI    = 81,				//前四
	HU_HOUSI     = 82,				//后四

	HU_BADCARD = 101,                //烂牌 字不重复 花色牌隔开两张
	HU_QIXIN = 102,				    //七心 东南西北中发白各一个的烂牌
	HU_YAOCARD = 103,				//幺牌 全字和19组成的牌
	HU_JIAXINWU = 104,				//夹心五（只能听五）4番
	HU_HUNYISE = 105,				//混一色
	HU_DASANYUAN = 106,				//大三元
	HU_XIAOSANYUAN = 107,			//小三元
	HU_SHIFENG = 108,				//十风
	HU_SHISANYAO_FEIXIAOJI = 109,	//十三幺-云南飞小鸡
	HU_LONGZHUABEI = 110,			//龙爪背
	HU_SIYAOJI = 111,				//四幺鸡
	HU_GANGSHANGWUMEIHUA = 112,     //杠上五梅花
	HU_WUJI = 113,					//无鸡
	HU_XIAOJIGUIWEI = 114,			//小鸡归位
	HU_DOUBLEGANG = 115,			//两杠
	HU_QIDUI = 117,				//小七对
	HU_FOURGHOST = 122,			//四鬼胡
	HU_ALLTYPE = 149,				//总番型		 --保证最大

	GANG_MINGGANG = 150,			//明杠
	GANG_ANGANG = 151,			//暗杠
	GANG_BUGANG = 152,			//补杠
	GANG_ZHUANYI = 153,			//呼叫转移
	GANG_HANBAOG = 154,          //汉堡杠

	HU_PAIQIAN = 180,		//牌钱 平胡
	HU_XIAOPAO = 181,		//小跑

	HU_JIAZI = 182,					//夹子
	HU_DUYING = 183,				//独赢
	HU_QUEMEN = 184,			//缺门
	HU_BAZHANG = 185, 			//八张
	HU_SHIZHANG = 186,			//十张
	HU_QINGQUE = 187,			//清缺
};
//请求解散房间
enum RESET_ROOM_TYPE
{
	RESET_ROOM_TYPE_WAIT,
	RESET_ROOM_TYPE_AGREE,
	RESET_ROOM_TYPE_REFLUSE,
};

enum  PLAYTYPE
{
	PT_UNDEFINE=0,

	//郑州玩法选项
	PT_GHOSTCARDFLOP = 1,			//翻鬼（混牌）
	PT_QIXIAODUI = 2,				//可胡七对
	PT_KEQIANGGANG = 3,				//可抢杠胡
	PT_BOTTOM_POUR = 4,				//买跑
	PT_HUANGZHUANGNOTHUANGGANG = 5,	//荒庄不荒杠
	PT_DIANPAO = 6,					//是否可以点炮胡
	PT_FENGPAI = 7,					//是否带风牌
	
	// Add HuNanZhuanZhuan by hanlu 02242017 Begin
	PT_ZIMOHU				= 8,		//自摸胡
	PT_DIANPAOHU			= 9,		//点炮胡
	PT_NEEDWIND				= 10,		//是否有风牌
	PT_ZHUANGXIAN			= 11,		//庄闲
	PT_ZHANIAO2				= 12,		//扎2鸟
	PT_ZHANIAO4				= 13,		//扎4鸟
	PT_ZHANIAO6				= 14,		//扎6鸟
	PT_TWOPLAYER			= 15,		//两人
	PT_THREEPLAYER			= 16,		//三人
	PT_HAVEWIND				= 17,		//带风
	PT_ZHUANGCANEAT			= 18,		//可吃
	PT_ZhaNiao_2			= 19,		//抓2鸟翻倍
	PT_ZhuangXian_2			= 20,		//庄闲翻倍
	PT_XueZhan				= 21,		//血战玩法
	PT_XueZhan2fan			= 22,		//血战 2番
	PT_XueZhan4fan			= 23,		//血战 4番
	PT_QiShouHuCountNiao	= 24,		//起手胡算鸟
	PT_QiShouHuCountZhuang	= 25,		//起手胡算庄闲
	PT_EachDaHuCountNiao	= 26,		//每个大胡单算鸟
	PT_EachDaHuCountZhuang  = 27,		//每个大胡单算庄闲
	PT_FengLuanJiang		= 28,		//风乱将
	
	//滑水
	PT_HSDIANPAO			= 31,		//点炮胡
	PT_HSZIMOHU				= 32,		//自摸胡
	PT_HSYU2				= 33,		//2条鱼 
	PT_HSYU5				= 34,		//5条鱼 
	PT_HSYU8				= 35,		//8条鱼 
	PT_HSFENGPAI			= 36,		//是否带风牌
	PT_HSJIANGERWUBA		= 37,		//二五八将
	PT_HSHONGZHONG			= 38,		//红中
	PT_HSHUN				= 39,		//随机混
	//滑水

	//推倒胡
	PT_TDHDIANPAOHU			= 41,		//点炮胡
	PT_TDHZIMOHU			= 42,		//自摸胡
	PT_TDHFENGPAI			= 43,		//是否带风牌
	PT_TDHJIANGERWUBA		= 44,		//二五八将
	PT_TDHHONGZHONG			= 45,		//红中
	PT_TDHZHUANGFAN			= 46,		//庄家翻倍
	PT_TDHDIANPAODABAO		= 47,		//点炮大包
	//推倒胡

	//新乡
	PT_XXDIANPAOHU			= 51,		//点炮胡
	PT_XXHZIMOHU			= 52,		//自摸胡
	PT_XXJIANGERWUBA		= 53,		//二五八将
	//新乡

	//开封
	PT_KFDIANPAOHU			= 61,		//点炮胡
	PT_KFHZIMOHU			= 62,		//自摸胡
	//开封

	// Add HuNanZhuanZhuan by hanlu 02242017 End
	PT_HAS_TANG_MUSTHU		= 151,		//有听必胡
	PT_CIPAI_YING			= 152,		//硬呲
	PT_CIPAI_RUAN			= 153,		//软呲
	PT_UNNEEDWIND			= 154,      //不带风牌
	PT_LYBaseScoreGANGDI1        = 160,      // 杠底1分
	PT_LYBaseScoreGANGDI2        = 161,      // 杠底2分
	PT_LYBaseScoreGANGDI3        = 162,      // 杠底3分

	PT_LYBaseScoreZIMO1          = 170,      // 自摸1分
	PT_LYBaseScoreZIMO2          = 171,      // 自摸2分
	PT_LYBaseScoreZIMO3          = 172,      // 自摸3分
	PT_LYBaseScoreZIMO4          = 173,      // 自摸4分
	PT_LYBaseScoreZIMO5          = 174,      // 自摸5分

	PT_LYBaseScoreCI1            = 180,      // 次底1分
	PT_LYBaseScoreCI2            = 181,      // 次底2分
	PT_LYBaseScoreCI3            = 182,      // 次底3分
	PT_LYBaseScoreCI4            = 183,      // 次底4分
	PT_LYBaseScoreCI5            = 184,      // 次底5分
	PT_LYBaseScoreCI6            = 185,      // 次底6分
	PT_LYBaseScoreCI7            = 186,      // 次底7分
	PT_LYBaseScoreCI8            = 187,      // 次底8分
	PT_LYBaseScoreCI9            = 188,      // 次底9分
	PT_LYBaseScoreCI10           = 189,      // 次底10分	

	//信阳
	PT_SANZUIQIHU				= 300,		//三嘴起胡
	PT_KANPAI					= 301,		//坎牌连庄  这个暂时不用
	PT_HUQIDUI					= 302,		//可胡七对
	PT_QINGYISEJIAFAN			= 303,		//清一色七对清七对加番
	PT_DIANPAO_SHOUSANJIA		= 304,		//点炮收三家
	PT_HU_DIANPAO				= 305,		//点炮胡
	PT_HU_ZIMO					= 306,		//自摸胡

	//周口
	//ZK_PT_DUANMEN				= 307,							//断门
	//ZK_PT_TWO_FIVE_EIGHT_JIANG	= 308,							//258作将
	//ZK_PT_FENG					= 309,							//带风牌
	//ZK_PT_OUT_FENG_TING			= 310,							//出风报听(重要:如果勾选这个玩法选项必须默认勾选带风牌)
	//ZK_PT_ZHONG_LAIZI			= 311,							//红中赖子

	//------商丘------
	PT_SQ_YOUHUAWUCAI				= 351,						//有花无财
	PT_SQ_YOUCAIWUHUA				= 352,						//有财无花
	PT_SQ_DAZHONG					= 353,						//大众麻将

	PT_SQ_ANGANGSUOSI				= 354,						//暗杠锁死
	PT_SQ_YINGKOU					= 355,						//硬扣
	PT_SQ_WUFENG					= 356,						//不带风牌
	PT_SQ_LIANG4DA1					= 357,						//亮四打一

	PT_SQ_JUEMEN					= 358,						// 绝门
	PT_SQ_DUMEN						= 359,						// 独门
	PT_SQ_MENQING					= 360,						// 门清
	PT_SQ_ANKE						= 361,						// 暗刻
	PT_SQ_ZIMO						= 362,						// 自摸
	PT_SQ_LIMITIP					= 363,						// 限制IP，选择后 同IP不能进入
	PT_SQ_TING						= 364,						// 上听暗嘴玩法 不听牌不能胡
	//------商丘------


	//------周口------
	PT_ZK_GHOST						= 380,						// 带混牌 赖子
	PT_ZK_ZIMO						= 381,						// 只能自摸
	PT_ZK_ZHUANGHUGANG				= 382,						// 庄家和牌杠加底
	PT_ZK_DAIFENG					= 383,						// 带风牌
	PT_ZK_TING						= 384,						// 报听和牌
	PT_ZK_ZHUANGDOUBLE				= 385,						// 庄家输赢翻倍
	PT_ZK_DUANMEN					= 386,						// 打断门,缺一门
	//------周口------
};

enum  PlayBaseScore
{
	BaseScore_ZIMO =0,
	BaseScore_GANG,
	BaseScore_CI
};

enum ErrorCode
{
	ErrorNone = 0,
	UserNotExist = 1,
	TaskNotExist = 2,
	ExceedMaximum = 3,
	AlreadyBinded = 4,
	NotQualified = 5,
	TaskNotFinished = 6,
	Expired = 7,
	BindingSelf = 8,
	NoAgentPower = 9,			//没有代理权
	Unknown = 100
};

//任务类型 需要连续
enum TaskType
{
	TaskType_CreateDesk = 1,			// 创建房间

	TaskType_Max,
};

//任务时间 需要连续
enum TaskTime
{
	TaskTime_Once = 0,					// 一次性任务 只能做一次
	TaskTime_Day = 1,					// 每日任务

	TaskTime_Max,
};

// Add HuNanZhuanZhuan by hanlu 02242017 Begin
#define BASE_CARD_COUNT					36
#define WIND_CARD_COUNT					28
#define LAI_ZI_COUNT					4
#define CARD_ZHUANZHUAN_COUNT			136
#define MAX_HANDCARD_COUNT				13
// Add HuNanZhuanZhuan by hanlu 02242017 End
#endif