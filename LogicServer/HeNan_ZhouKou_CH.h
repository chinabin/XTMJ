#ifndef _CARD_HENAN_ZHOUKOU_H_
#define _CARD_HENAN_ZHOUKOU_H_

#include "CardBase.h"

class HeNan_ZhouKou_CH : public CardBase
{
public:
	HeNan_ZhouKou_CH();
	virtual ~HeNan_ZhouKou_CH(){};

	static	HeNan_ZhouKou_CH*	Instance()
	{
		if (!mInstance)
		{
			mInstance = new HeNan_ZhouKou_CH();
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

public:

	// 绝门 返回绝门数量
	Lint CheckQueMen(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& agangcard,CardVector& mgangcard,Card* card, Card* ghost);
private:
	static	HeNan_ZhouKou_CH* mInstance;
};

#define gCB_HeNan_ZhouKou HeNan_ZhouKou_CH::Instance()

#endif

