#ifndef _CARD_WuHan_XIANTAO_H_
#define _CARD_WuHan_XIANTAO_H_

#include "CardBase.h"

class CH_WuHan_XianTao : public CardBase
{
public:
	CH_WuHan_XianTao();
	virtual ~CH_WuHan_XianTao(){};

	static	CH_WuHan_XianTao*	Instance()
	{
		if (!mInstance)
		{
			mInstance = new CH_WuHan_XianTao();
		}
		return mInstance;
	}

	//别人出牌 检测操作类型
	virtual ThinkVec	CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo);

	//自己摸牌 检测操作类型
	virtual ThinkVec	CheckGetCardOperator(CardVector& handcard, CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,Card* getCard,OperateState& gameInfo);

	//检测抢杠胡
	virtual bool		CheckQiangganghu(OperateState& gameInfo);

	//检测是否胡牌
	virtual bool		CheckCanHu(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec);

	//
	virtual Lint		CheckTing(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, OperateState& gameInfo, std::vector<TingCard>& winCards, const Card* ignore = NULL);

protected:
	//不带癞子
	void				CheckCanHuNormal(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec);

	//带癞子
	void				CheckCanHuHun(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec);

	//四个鬼牌
	bool				CheckFourGhost(CardVector& handcard, Card* outCard, bool is_get_card, std::vector<Card>& ghostCard);

	bool				matchHu_ShunKe(const CardVector& cards, Lint laiZiCount, bool isneed258, std::vector<Card>* pMatch) const;

	Lint				checkLaiZi(CardVector& handcard, Card* outCard, bool is_get_card, std::vector<Card>& ghostCard, CardVector& tempCard);

private:
	static	CH_WuHan_XianTao* mInstance;
};

#define gCB_WuHan_XianTao CH_WuHan_XianTao::Instance()

#endif

