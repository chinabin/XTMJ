#include "GameHandler.h"
#include "Desk.h"
#include "Card.h"
#include "LTime.h"
#include "LVideo.h"
#include "RoomVip.h"
#include "CH_HN_ZhengZhou.h"

class GH_HeNan_ZhengZhou : public GameHandler
{
public:
	virtual bool startup(Desk *desk);

	virtual void shutdown(void);

	virtual void SetDeskPlay();

	// 玩家买跑
	virtual void HanderUserChange(User* pUser, LMsgC2SBottomPour* msg);

	virtual void ProcessRobot(Lint pos, User * pUser);

	virtual void HanderUserSpeak( User* pUser, LMsgC2SUserSpeak* msg );

	virtual void HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg);

	virtual void HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg);

	virtual void OnUserReconnect(User* pUser);

	virtual void OnGameOver(Lint result,Lint bombpos);

protected:
	void DeakCard();

	void CheckStartPlayCard();

	//摸牌
	void SetPlayIng(Lint pos,bool needGetCard,bool gang, bool needThink, bool canhu);

	// 计算玩家得分
	void calcUserScore(Lint result, Lint gold[], Lint& bombCount, Lint winPos[]);

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

	Lint			m_zhuangpos;   //庄家位置
	Lint			m_curPos;      //当前操作玩家

	Lint			m_endStartPos;    //海底牌结束位置
	Lint			m_endStartCurPos; //海底牌当前位置
	bool			m_needGetCard;

	CardVector		m_deskCard;       //桌子上剩余的牌

	VideoLog		m_video;						//录像

	OperateState	m_gameInfo;
	

	Lint            m_qiangganghu;

	std::vector<CardValue>	m_ghostCardReal;			//鬼牌
	Card			m_ghostCardFlop;					//翻出的鬼牌 发送给客户端

	PengInfoVector m_pengInfo[DESK_USER_COUNT];	//碰牌
	std::vector<Lint> m_pengGangInfo[DESK_USER_COUNT];	//杠牌包分
};

DECLARE_GAME_HANDLER_CREATOR(MJHeNanZhengZhou, GH_HeNan_ZhengZhou)
