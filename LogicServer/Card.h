#ifndef _CARD_H_
#define _CARD_H_

#include "LBase.h"
#include "GameDefine.h"
#include "LLog.h"
#include "LVideo.h"

// struct Card
// {
// 	Lint m_number;	//牌的号码1-9
// 	Lint m_color;   //牌的花色1-3 1-万，2-筒，3-条
//
// 	Card()
// 	{
// 		m_number = 0;
// 		m_color = 0;
// 	}
//
// 	Card(Lint number,Lint color)
// 	{
// 		m_number = number;
// 		m_color = color;
// 	}
//
// 	Lint GetNCIndex()
// 	{
// 		return (m_color * 10 + m_number);
// 	}
//
// 	void InitCardbyNCIndex(Lint NCIndex)
// 	{
// 		m_number =  NCIndex % 10;;
// 		m_color = NCIndex / 10;;
// 	}
//
// 	// 不方便在CardValue中添加转换，在这儿加个这
// 	CardValue ToCardValue()
// 	{
// 		CardValue v;
// 		v.m_number = m_number;
// 		v.m_color = m_color;
// 		return v;
// 	}
//
// 	bool isValid() const
// 	{
// 		//return (m_color > 0) && (m_number > 0);
// 		switch(m_color)
// 		{
// 			case CARD_COLOR_WAN:
// 			case CARD_COLOR_TUO:
// 			case CARD_COLOR_SUO: return m_number > 0 && m_number < 10;
// 			case CARD_COLOR_ZI: return m_number > 0 && m_number < 8;
// 			case CARD_COLOR_FLOWER: return m_number > 0 && m_number < 9;
// 			default: return false;
// 		}
// 	}
//
// 	bool operator== (const Card& other) const
// 	{
// 		return m_color == other.m_color && m_number == other.m_number;
// 	}
// 	bool operator!= (const Card& other) const
// 	{
// 		return m_color != other.m_color || m_number != other.m_number;
// 	}
//
// 	bool operator== (const CardValue& other) const
// 	{
// 		return m_color == other.m_color && m_number == other.m_number;
// 	}
// 	bool operator!= (const CardValue& other) const
// 	{
// 		return m_color != other.m_color || m_number != other.m_number;
// 	}
// };

typedef CardValue Card;

typedef std::vector<Card*> CardVector;

struct ThinkUnit
{
	ThinkUnit()
	{
		m_type = THINK_OPERATOR_NULL;
		m_gen  = 0;
		m_isOnlyhu = false;
		m_score = 0;
	}

	void Clear()
	{
		m_type = THINK_OPERATOR_NULL;
		m_card.clear();
		m_hu.clear();
		m_score = 0;
	}

	void count_score()
	{
		m_score = 0;

		for (Luint x = 0; x < m_hu.size(); ++x)
		{
			switch(m_hu[x])
			{
			case HU_XIAOHU:					//小胡
				++m_score;
				break;
			case HU_SPECIAL_XIAOQIDUI:		//豪华小七对
				m_score+=12;
				break;
			case HU_XIAOQIDUI:				//小七对
				m_score+=6;
				break;
			case HU_QINGYISE:				//清一色
			case HU_JIANGJIANGHU:			//将将胡
			case HU_PENGPENGHU:				//碰碰胡
			case HU_QUANQIUREN:				//全球人
			case HU_GANG1:					//杠上开花
			case HU_GANG2:					//杠上炮
			case HU_END1:					//海底捞
			case HU_END2:					//海底炮
			case HU_QIANGGANGHU:			//抢扛胡
			case HU_TIANHU://天胡
			case HU_DIHU://地胡
				m_score+=6;
				break;
			case HU_DOUBLESPECIAL_XIAOQIDUI: //双豪华七小对
				m_score+=18;
				break;
			default:
				LLOG_ERROR("error hu type");
			}
		}
	}

	int GetHuFan( Lint fanshu[HU_ALLTYPE] ) const
	{
		if ( m_type != THINK_OPERATOR_BOMB )
		{
			return 0;
		}
		Lint fan = 0;
		for (Lsize k = 0; k < m_hu.size(); ++k)
		{
			if ( fanshu[m_hu[k]] > 0 )
			{
				fan += fanshu[m_hu[k]];
			}
		}
		return fan;
	}
	// 获取最大的番
	int GetMaxFan( Lint fanshu[HU_ALLTYPE] ) const
	{
		if ( m_type != THINK_OPERATOR_BOMB )
		{
			return 0;
		}
		Lint fan = 0;
		for (Lsize k = 0; k < m_hu.size(); ++k)
		{
			if ( fan < fanshu[m_hu[k]] )
			{
				fan = fanshu[m_hu[k]];
			}
		}
		return fan;
	}

	bool HasQianggh()
	{
		for(Lsize i = 0; i < m_hu.size(); ++i)
		{
			if(m_hu.at(i) == HU_QIANGGANGHU)
			{
				return true;
			}
		}
		return false;
	}

	THINK_OPERATOR		m_type;
	Lint				m_score;
	CardVector			m_card;
	std::vector<Lint>   m_hu;//胡牌的类型
	//for yb
	int                 m_gen;
	bool                m_isOnlyhu;
};

typedef std::vector<ThinkUnit> ThinkVec;

struct TingCard
{
	TingCard()
		: m_hufan(0)
		, m_gen(0)
	{
	}

	Card m_card;
	std::vector<int> m_hu; // 胡的番型
	Lint m_hufan;	// 胡的番数 包括根和最大番限制
	Lint m_gen;		// 根的数量

public:

	bool FindQiDui()
	{
		for(int i=0; i<m_hu.size(); ++i)
		{
			if(m_hu[i] == HU_QIDUI)
				return true;
		}
		return false;
	}
};

struct TingUnit
{
	Card m_card;
	std::vector<TingCard> m_tingcard;

public:

	bool FindQiDui()
	{
		for(int i=0; i<m_tingcard.size(); ++i)
		{
			if(m_tingcard[i].FindQiDui())
				return true;
		}
		return false;
	}

	bool FindGhost(Card ghost)
	{
		for(int i=0; i<m_tingcard.size(); ++i)
		{
			if(m_tingcard[i].m_card == ghost)
				return true;
		}
		return false;
	}
};

typedef std::vector<TingUnit> TingVec;

struct PlayType 
{
public:
	void clear() { m_playtype.clear(); }
	void setAllTypes( const std::vector<Lint>& v ) { m_playtype = v; }
	const std::vector<Lint>& getAllTypes() const { return m_playtype; }

	// 点炮胡是否胡 转转玩法
	bool gametypeDianPao() const;
	// 小七对 转转玩法
	bool gametypeQiXiaoDui() const;
	//是否需要风牌
	bool   gameNeedWind() const;

	bool GametypeGetGhost() const;
	
	bool GametypeQiangKang() const;
	
	bool hasPour() const;

	bool hasHuangzhuang() const;

	bool checkPlayType(PLAYTYPE playType) const;
	Luint checkZhaNiao() const;
	
protected:
	std::vector<Lint>	m_playtype;
};

struct OperateState
{
	//用于desk 和 card 之前的参数传递 参数太多了, 有扩展请扩展此处
	bool b_canHu;		// 是否可以胡
	bool b_onlyHu;		// 是否只能操作胡
	bool m_thinkGang;	// 单独处理是不是杠的牌
	bool bNoCard;
	bool bChaTing;
	bool m_firstPlayIng; // 是否第一次抓牌 之后庄家才能第一次抓牌
	bool m_firstOper;// 标记玩家是否第一次操作牌

	Lint m_deskState;	// 当前局牌状态
	Lint m_playerPos;	// 当前一个出牌位置
	Lint m_cardState;	// 当前一个出牌状态
	Lint m_endStartPos;	// 结束位置，即海底位置
	Lint m_MePos;		// 玩家的位置
	Lint m_dingQue;		// 玩家定缺
	Lint m_hasHu;		// 血流中使用 表示玩家是否胡牌的次数
	Lint m_upState;
	Lint m_showCount;	// 亮四打一玩法中 手上亮出的牌数量

	Card *m_pGscard;

	// 初始化赋值一次就可以
	void *m_gameHandler;
	PlayType m_playtype;	// 桌子玩法
	Lint* m_hufan;			// 番数配置表
	std::vector<Card> m_hCard;  //宜宾 混牌

	// Add HuNanZhuanZhuan by hanlu 02242017 Begin
	CardVector		m_gangcard;		//过手杠
	Lint			m_GameType;		//
	Lint			i_canHuScore;		// 是否可以胡
	bool			m_first_turn;	// 是否为玩家第一轮
	bool			b_canEat;		// 是否可以吃
	bool			m_QiXiaoDui;	// 可以胡七小队
	bool			m_pt_laizi;
	bool			m_tianhu;
	bool			m_dihu;
	bool			b_guoGang;
	CardVector		m_louhuCard;		//其他地方含义不管 在商丘里面表示亮四打一玩法中亮出的牌
	//Add HuNanZhuanZhuan by hanlu 02242017 End	
	bool			m_bCiPaiDispatched;// 直接派送的呲牌，而不是从桌上摸到的。
	Card			*m_CiPai;
	bool            m_is10deskcardLeft;// 桌上牌是否剩下最後１０張。用於判斷包茨。
	std::vector<Lint>    m_tingHuCardsIndex;// 要听胡的牌的card index
	std::vector<TingCard>*    m_tingCard;	// 要听胡的牌

	OperateState():m_deskState(0), m_playerPos(0), m_cardState(0), m_MePos(0),
		m_endStartPos(0), m_thinkGang(false), b_canHu(true), b_onlyHu(false),
		m_dingQue(0),m_hasHu(0),
		bNoCard(false),m_pGscard(NULL),bChaTing(false),m_upState(0),
		m_firstPlayIng(false),m_firstOper(false),
		m_gameHandler(NULL), m_hufan(NULL),
		// Add HuNanZhuanZhuan by hanlu 02242017 Begin
		m_GameType(0), m_first_turn(false), b_canEat(false), m_QiXiaoDui(false), 
		m_pt_laizi(false), m_tianhu(false), m_dihu(false), b_guoGang(false), i_canHuScore(0),
		//Add HuNanZhuanZhuan by hanlu 02242017 End
		m_bCiPaiDispatched(false), m_CiPai(NULL), m_is10deskcardLeft(false),m_showCount(0),m_tingCard(NULL)
	{
	}
	void Clear()
	{
		b_canHu = true;
		b_onlyHu = false;
		m_thinkGang = false;
		m_deskState = 0;
		m_playerPos = 0;
		m_cardState = 0;
		m_endStartPos = 0;
		m_MePos = 0;
		m_dingQue = 0;
		m_hasHu = 0;
		bNoCard = false;
		bChaTing = false;
		m_upState = 0;
		m_firstPlayIng = false;
		m_firstOper = false;

		m_showCount = 0;

		m_gameHandler = NULL;
		m_playtype.clear();
		m_hufan = NULL;
		m_hCard.clear();

		// Add HuNanZhuanZhuan by hanlu 02242017 Begin
		m_CiPai			= NULL;
		m_GameType		= 0;
		i_canHuScore    = 0;
		m_first_turn	= false;
		b_canEat		= false;
		m_QiXiaoDui		= false;	// 可以胡七小队
		m_pt_laizi		= false;
		m_tianhu		= false;
		m_dihu			= false;
		b_guoGang		= false;
		m_bCiPaiDispatched	= false;			
		m_gangcard.clear();
		m_louhuCard.clear();
		// Add HuNanZhuanZhuan by hanlu 02242017 End		
		m_tingCard = NULL;
	}

};
struct StartHuCard
{
	StartHuCard()
	{
		m_type = START_HU_NULL;
	}

	void	Clear()
	{
		m_type = START_HU_NULL;
		m_card.clear();
	}
	Lint	m_type;
	CardVector m_card;
};
typedef std::vector<StartHuCard> StartHuVector;

class CardManager
{
public:
	virtual ~CardManager(){};

public:
	virtual bool			Init();
	static	CardManager*	Instance()
	{
		if (!mInstance)
		{
			mInstance = new CardManager();
		}
		return mInstance;
	}
	
	//DEBUG 发牌

	//三人三房使用
	void				DealCardThrThr(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v5,int cardNum);
	void				DealCardThrThr2(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v5,int cardNum,const Card specialCard[]);
	//发牌
	virtual void				DealCard(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5, bool needWind);
	//特定牌局 发牌
	virtual void				DealCard2(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5,const Card specialCard[CARD_COUNT], bool needWind);
	//对牌进行排序，万，筒，条，红中
	virtual void				SortCard(CardVector& vec);
	//把des中的相，从src中删除
	virtual void				EraseCard(CardVector& src,CardVector& des);
	//把pCard从src中删除
	virtual void				EraseCard(CardVector& src,Card* pCard);

	//删除src 与pCard 数据一样的牌 N张
	virtual bool				EraseCard(CardVector& src,Card* pCard,Lint n);

	//删除一手牌中，相同的牌字，比如2个三万，删除其中一个
	virtual void				EarseSameCard(CardVector& src);

	//别人出牌 检测操作类型
	virtual ThinkVec			CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo);
	
	//自己摸牌 检测操作类型
	virtual ThinkVec			CheckGetCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,Card* getCard, OperateState& gameInfo);
	
	//检测是否胡牌	--- 检测胡牌，各自实现自已的。
	virtual bool				CheckCanHu(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo,std::vector<Lint>& vec, bool isGetCard);
	//检测是否杠 
	virtual bool				CheckCanGang(CardVector& handcard,  Card* outCard);
	//检测是否碰 
	virtual bool				CheckCanPeng(CardVector& handcard,  Card* outCard);


	//检测大胡
	//检测小七对,是否是豪华，special = 1 
	bool                       CheckXiaoqidui(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard,Lint& special);
	//检测清一色
	virtual bool				CheckQingyise(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, std::vector<Lint>& vec);
	//检测乱将胡
	virtual bool				CheckLuanjianghu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard);
	//检测碰碰胡
	virtual bool				CheckPengpenghu(CardVector& handcard,CardVector& eatCard,Card* outCard);
	//检测全球人
	virtual bool				CheckQuanqiuren(CardVector& handcard,Card* outCard);

	//枪杠胡
	virtual bool				CheckQiangganghu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//检测杠上开花
	virtual bool				CheckGangshangkaihua(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//抢杠胡
	virtual bool				CheckGangshangpao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//海底捞
	virtual bool				CheckHaiDiLao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//海底炮			
	virtual bool				CheckHaiDiPao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);

	int                         CheckJiangdui( CardVector& handcard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard);

	//检测暗杠 
	virtual bool				CheckAnGang(CardVector& handcard,  CardVector& vec);

	//检测明杠 
	virtual bool				CheckMGang(CardVector& handcard,CardVector& pengCard,  CardVector& vec);

	virtual bool				IsContinue(CardVector& result);

	virtual bool				IsSame(CardVector& result);

	virtual bool				IsSame(Card* c1,Card* c2);

	virtual bool				IsNineOne(Card* c);

	virtual Card*				GetCard(Lint color,Lint number);

	//金钩钓
	virtual bool				CheckJinGouDiao(CardVector& handcard, CardVector& eatCard,Card* outCard){return false;};

	// 判断手牌是否花猪
	virtual bool				IsHuaZhu(CardVector& handcard, Lint dingque){return false;};

	// 查听
	virtual Lint				CheckTing(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, OperateState& gameInfo, Lint fan[], Lint dingque, Card* winCard, Lint maxFan){return 0;};

	// 算根
	virtual Lint				CheckGen(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& agangcard,CardVector& mgangcard, Card* getCard);

	// 计算胡牌番数
	bool                        CheckOneNine( CardVector& tmp,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard);
	bool                        CheckNoOneNine( CardVector& tmp,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard ); // 断幺九
	bool                        CheckMenQing( CardVector& tmp,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard ); // 门前清

	bool                        CheckMidCard(const CardVector& handcard,const Card& mid,const Card* outCard );
	bool                        CheckOne2Nine(const CardVector& handcard,const Card* outCard);   //1条龙
	bool                        CheckSisterPairs(const CardVector& handcard,const Card* outCard);
	void				        SwapCardBySpecial(CardVector& cvIn, const Card specialCard[CARD_COUNT]);

	bool                        CheckHoo(const CardVector& handcard,const Card* outCard);
	bool                        CheckMyHoo( int allPai[3][10]);
	bool						Analyzefor(int* kindcard);
public:
	Card		m_card[CARD_COUNT];
	Card		m_card_Laizi[CARD_COUNT];
	CardVector	m_cardVec;
	CardVector	m_cardVec_Laizi;;
	static	CardManager* mInstance;
};

#define gCardMgr CardManager::Instance()

#endif