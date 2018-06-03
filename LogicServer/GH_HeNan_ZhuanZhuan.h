#ifndef _GH_HENAM_ZHUANGZHUANG_H__
#define _GH_HENAM_ZHUANGZHUANG_H__

#include "GameHandler.h"
#include "Desk.h"
#include "Card.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"

class GH_HeNan_ZhuanZhuan : public GameHandler
{
public:
	bool startup(Desk *desk);
	void Shutdown(void);
	void DeakCard();
	void SetDeskPlay();
	void ProcessRobot(Lint pos, User * pUser);
	void HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg);
	void HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg);
	void OnUserReconnect(User* pUser);
	void CheckStartPlayCard();
	void SetPlayIng(Lint pos,bool needGetCard,bool gang, bool needThink, bool canhu,bool first_think=false);
	bool calcScore(Lint result,Lint winpos[],Lint bombpos, Lint gold[], Lint& bombCount,std::vector<Lint>& winPos, LMsgS2CGameOver& over);
	void SetThinkIng(bool is_thinkQiangGang=false);
	void CheckThink();
	void ThinkEnd();
	void OnGameOver(Lint result,Lint winpos[],Lint bombpos, CardVector winCards[]);
	void VideoThink(Lint pos);
	void VideoDoing(Lint op, Lint pos, Lint card_color, Lint card_number);

private:
	Desk			*m_desk;
	Card*			m_curOutCard;							//当前出出来的牌
	Card*			m_curGetCard;							//当前获取的牌
	Lint			m_louHuCard[DESK_USER_COUNT];			//是否有漏胡记录
	Lint			m_startHuRet[DESK_USER_COUNT];			//起手胡牌
	ThinkTool		m_thinkInfo[DESK_USER_COUNT];//当前打牌思考状态
	ThinkUnit		m_thinkRet[DESK_USER_COUNT];//玩家返回思考结果
	StartHuVector	m_startHu[DESK_USER_COUNT];//起手胡牌
	CardVector		m_handCard[DESK_USER_COUNT];//玩家手上的牌
	CardVector		m_outCard[DESK_USER_COUNT];	//玩家出的牌
	CardVector		m_pengCard[DESK_USER_COUNT];//玩家碰的牌，
	CardVector		m_minggangCard[DESK_USER_COUNT];//玩家明杠的牌
	CardVector		m_angangCard[DESK_USER_COUNT];//玩家暗杠的牌
	CardVector		m_eatCard[DESK_USER_COUNT];//玩家吃的牌
	Lint			m_angang[DESK_USER_COUNT];//暗杠数量
	Lint			m_minggang[DESK_USER_COUNT];//明杠数量
	Lint			m_diangang[DESK_USER_COUNT];//点杠数量
	Lint			m_adiangang[DESK_USER_COUNT];//被点杠数量
	Lint			m_gangPos[DESK_USER_COUNT];	//某位玩家有过杠
	Lint			m_playerHuInfo[DESK_USER_COUNT];		//玩家胡牌信息， 因为可以多胡 放炮的人只保存了位置
	Lint			m_playerBombInfo[DESK_USER_COUNT];	//玩家放炮信息，[玩家位置对应的炮的类型]
	Lint			m_beforePos;//之前操作的位置
	Lint			m_beforeType;//之前操作的类型
	Lint			m_gold;
	Lint			m_zhuangpos;//庄家位置
	Lint			m_curPos;						//当前操作玩家
	bool			m_isThinkQiangGang;				//是否在思考抢杠胡
	Lint			m_endStartPos;					//海底牌结束位置
	Lint			m_endStartCurPos;				//海底牌当前位置
	bool			m_needGetCard;
	CardVector		m_deskCard;		//桌子上剩余的牌
	CardVector		m_curOutCards;	//当前出出来的牌		(为杠写的)
	bool			m_first_turn[DESK_USER_COUNT]; //第一轮判断
	VideoLog		m_video;						//录像
	OperateState	mGameInfo;
};

DECLARE_GAME_HANDLER_CREATOR(MJHeNanZhuanZhuanFour, GH_HeNan_ZhuanZhuan)

#endif //_GH_HENAM_ZHUANGZHUANG_H__

