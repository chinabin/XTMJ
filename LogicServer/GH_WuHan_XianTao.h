#include "GameHandler.h"
#include "Desk.h"
#include "Card.h"
#include "LTime.h"
#include "LVideo.h"
#include "RoomVip.h"
#include "CH_WH_XianTao.h"

class GH_WuHan_XianTao : public GameHandler
{
public:
	bool startup(Desk *desk)override;

	void shutdown(void)override;

	void SetDeskPlay()override;

	// 玩家买跑
	void HanderUserChange(User* pUser, LMsgC2SBottomPour* msg)override;

	void ProcessRobot(Lint pos, User * pUser)override;

	void HanderUserSpeak( User* pUser, LMsgC2SUserSpeak* msg )override;

	void HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg)override;

	void HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg)override;

	void OnUserReconnect(User* pUser)override;

	void OnGameOver(Lint result,Lint bombpos)override;

protected:
	void DeakCard();

	void CheckStartPlayCard();

	//摸牌
	void SetPlayIng(Lint pos,bool needGetCard,bool gang, bool needThink, bool canhu);

	// 计算玩家得分
	void calcUserScore(Lint result, Lint gold[], Lint& bombCount, Lint winPos[]);

	bool checkOtherPlayCanHu(int pos);

	void SetThinkIng();

	void CheckThink();

	void ThinkEnd();
	
	void VideoThink(Lint pos);

	void VideoDoing(Lint op, Lint pos, Lint card_color, Lint card_number);

	bool isGuoHandGang(Card* card,Lint pos);

	//////////////////////////////////////////////////////////////////////////
	void InitHunCard();

	bool IsPouredOver();

	void BroadcastBottomPour();

	bool CheckIfBigHu(std::vector<Lint>& vec);

	Lint GetHuBaseSocore(HuInfo* pHuInfo);
	void GetGangSocore(Lint gold[]);
	void GetPaoSocore(Lint result, Lint gold[], Lint& bombCount, Lint winPos[]);

	Lint FindPengGangPos(Lint curPos, const CardValue card);
	//////////////////////////////////////////////////////////////////////////

	//听牌 参照漯河
	bool IsTing( Lint pos )
	{
		if ( pos >= 0 && pos < 4 )
		{
			return !m_tingCard[pos].empty();
		}
		return false;
	}

	bool isDeskCardEmpty();		// 判断桌上是否还有牌 由于规则原因 不能直接判断 m_deskCard.empty()

	void printThinkInfo(ThinkTool thinkInfo)
	{
		ThinkVec thinkData = thinkInfo.m_thinkData;
		for (ThinkUnit thinkUnit : thinkData)
		{
			thinkUnit.toString();
		}
	}
private:
	Card*			m_louHuCard[DESK_USER_COUNT];		//是否有漏胡记录 *

	Lint			m_startHuRet[DESK_USER_COUNT];	//起手胡牌 *

	ThinkTool		m_thinkInfo[DESK_USER_COUNT];	//当前打牌思考状态

	ThinkUnit		m_thinkRet[DESK_USER_COUNT];	//玩家返回思考结果

	StartHuVector	m_startHu[DESK_USER_COUNT];	//起手胡牌 *

	CardVector		m_handCard[DESK_USER_COUNT];//玩家手上的牌

	CardVector		m_outCard[DESK_USER_COUNT];	//玩家出的牌

	CardVector		m_pengCard[DESK_USER_COUNT];//玩家碰的牌，

	CardVector		m_gangCard[DESK_USER_COUNT];//玩家明杠的牌

	CardVector		m_abombCard[DESK_USER_COUNT];//玩家按杠的牌      *

	CardVector		m_eatCard[DESK_USER_COUNT];//玩家吃的牌          *

	Lint			m_angang[DESK_USER_COUNT];//暗杠数量
	Lint			m_minggang[DESK_USER_COUNT];//明扛数量 包括點槓和碰槓
	Lint			m_diangang[DESK_USER_COUNT];//明杠数量 别人点的
	Lint			m_adiangang[DESK_USER_COUNT];//点别人杠数量
	Lint			m_baseScore[DESK_USER_COUNT];//每个玩家的倍数

	Lint			m_gangPos[DESK_USER_COUNT];	//某位玩家有过杠

	HuInfo			m_playerHuInfo[DESK_USER_COUNT];		//玩家胡牌信息， 因为可以多胡 放炮的人只保存了位置
	bool			m_guoShouHu[DESK_USER_COUNT];  // 玩家过手胡限制
	CardVector		m_gsCard[DESK_USER_COUNT];     // 过手杠

	Lint            m_isPiao[DESK_USER_COUNT];//四家跑数
	bool            m_isFly[DESK_USER_COUNT];//已下跑的玩家

	Desk			*m_desk;

	Card*			m_curOutCard;//当前出出来的牌
	Card*			m_curGetCard;//当前获取的牌
	Lint			m_beforePos;   //之前操作的位置
	THINK_OPERATOR	m_beforeType;  //之前操作的类型
	Lint            m_beforeOpScore[DESK_USER_COUNT];
	Lint            m_totalScore[DESK_USER_COUNT]; // 总分

	Lint			m_paoZhuangPos;	//带跑玩法中 除了连庄以外 需要轮流坐庄
	Lint			m_zhuangpos;   //庄家位置
	Lint			m_curPos;      //当前操作玩家
	Lint			m_zhuangChangeCount;//记录换庄次数 带跑玩法换4次算一圈

	Lint			m_endStartPos;    //海底牌结束位置
	Lint			m_endStartCurPos; //海底牌当前位置
	bool			m_needGetCard;
	bool            b_userPlayCard[DESK_USER_COUNT];

	CardVector		m_deskCard;       //桌子上剩余的牌

	VideoLog		m_video;						//录像

	OperateState	m_gameInfo;
	

	Lint            m_qiangganghu;

	std::vector<CardValue>	m_ghostCardReal;			//癞子 发送给客户端
	Card			m_ghostCardFlop;					//癞子前一张
	bool            m_playGhostCard = false;            //是否有打过癞子

	PengInfoVector m_gangInfo[DESK_USER_COUNT];			// 杠牌，明杠谁的
	PengInfoVector m_pengInfo[DESK_USER_COUNT];	//碰牌
	std::vector<Lint> m_pengGangInfo[DESK_USER_COUNT];	//杠牌包分
	std::vector<Lint> m_mingGangInfo[DESK_USER_COUNT];		// 杠牌算分用，暗刻转杠

	//听牌 参照漯河
	std::vector<TingCard>	m_tingCard[4];					// 玩家听的牌
	Lint					m_tingPos[4];					// 记录每个玩家听牌时桌面已出的牌张数+1，方便客户端将听牌时打出的牌盖住

	bool					m_changeZhuang;					// 换庄标志，带跑玩法时 换庄要重新设置跑分
	bool					m_pt_ting;						// 标记玩法是否需要听牌才能胡 避免每次调用m_playtype进行复杂检测
	bool					m_pt_hun;						// 带混
	bool					m_pt_feng;						// 带风
	bool					m_pt_pao;						// 带跑玩法
	Lint					m_pt_zhuangDouble[4];			// 平胡玩法中庄家输赢加倍 默认1 加倍2

	Lint					m_paoGangScore[4];				// 带跑玩法 杠跑分
	Lint					m_paoBeforeScore[4];			// 带跑玩法 前跑分
	Lint					m_paoAfterScore[4];				// 带跑玩法 后跑分
	Lint					m_paoTieGang[4];				// 带跑玩法中，庄家选择是否铁杠跑 只以庄家为准 0不开启铁杠跑 1开启铁杠跑

	Lint					m_zhuangHuGang[4];				// 平胡玩法中庄家胡杠加底分，只有庄家有分，方便计算 默认0

	//结算的时候统计分数
	Lint					m_resultAnGangScore[4];			// 暗杠得分
	Lint					m_resultMingGangScore[4];		// 明杠得分
	Lint					m_resultADianGangScore[4];		// 点别人杠扣分

	std::vector<ScoreInfo> m_scoreInfo[4];		//得分信息

	Lint		m_cardPos1;		// 桌面起始摸牌的位置 0-3
	Lint		m_cardPos2;		// 2-12
	Lint		m_cardPos3;		// 1-6
};

DECLARE_GAME_HANDLER_CREATOR(MJWuHanXianTao, GH_WuHan_XianTao)
