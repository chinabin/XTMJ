#ifndef _CARD_HENAN_ZHUANZHUAN_H_
#define _CARD_HENAN_ZHUANZHUAN_H_

#include "Card.h"
#include "LMsg.h"
#include "LTime.h"

class CH_HeNan_ZhuanZhuan : public CardManager
{
public:
	virtual ~CH_HeNan_ZhuanZhuan(){};
	
	static	CH_HeNan_ZhuanZhuan	*Instance()
	{
		if (!mInstance)
		{
			mInstance = new CH_HeNan_ZhuanZhuan();
		}
		return mInstance;
	}
	
	static bool				CardSortFun(Card* c1,Card* c2);

	virtual bool		Init();
	virtual bool		Final();

	//DEBUG 发牌
	bool				GetSpecialOneTwoThreeFour(CardVector& src, CardVector& one, CardVector& two, CardVector& three, CardVector& four);
	//发牌
	void				DealCard(CardVector hand_card[],Lint user_count,CardVector& rest_card,Lint game_type, PlayType& playtype);
	//特定牌局 发牌
	void				DealCard2(CardVector hand_card[],Lint user_count,CardVector& rest_card,const Card specialCard[CARD_COUNT],Lint game_type, PlayType& playtype);
	//对牌进行排序，万，筒，条，红中
	void				SortCard(CardVector& vec);
	//把des中的相，从src中删除
	void				EraseCard(CardVector& src,CardVector& des);
	//把pCard从src中删除
	void				EraseCard(CardVector& src,Card* pCard);

	

	//删除src 与pCard 数据一样的牌 N张
	bool				EraseCard(CardVector& src,Card* pCard,Lint n);

	//删除一手牌中，相同的牌字，比如2个三万，删除其中一个
	void				EarseSameCard(CardVector& src);

	bool				CheckStartHu(CardVector& handcard,std::vector<StartHuCard>& vec);

	//别人出牌 检测操作类型
	ThinkVec			CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo);
	ThinkVec			CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, CardVector& outCards,OperateState& gameInfo,LMsgS2CUserGangThinkCard& send_message);

	//自己摸牌 检测操作类型
	ThinkVec			CheckGetCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,Card* getCard, OperateState& gameInfo);
	ThinkVec			CheckGetCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,CardVector& getCards, OperateState& gameInfo);

	//检测是否胡牌
	bool				CheckCanHu(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo,std::vector<Lint>& vec, bool isGetCard);
	//检测是否杠 
	bool				CheckCanGang(CardVector& handcard,  Card* outCard, OperateState& gameInfo);
	//检测是否碰 																	  
	bool				CheckCanPeng(CardVector& handcard,  Card* outCard, OperateState& gameInfo);

	//	bool				CheckHu(CardVector handcard,Card* curCard,bool needJiang, bool isGetCard);

	bool				xiaohu_CheckHu(CardVector handcard,Card* curCard,bool needJiang,bool is_out_card,bool hava_laizi=false);
	bool                Yongzhou_CheckBaseHu(CardVector handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* curCard, bool is_out_card, Card * wangbaCard, std::vector<HU_PAI_LEIXING> &playtypes);
	bool                XiangTan_CheckBaseHu(CardVector handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* curCard, bool is_out_card, Card * wangbaCard, Card * wangbaCard2, std::vector<HU_PAI_LEIXING> &playtypes);
	bool                YiYang_CheckTing(CardVector handcard, CardVector pengCard, CardVector agangCard, CardVector mgangCard, CardVector eatCard);
	bool				xiaohu_CheckTing(CardVector handcard,bool needJiang);
	bool                XiangTanCheckTing(CardVector handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* curCard, bool isGetCard, OperateState& gameInfo);
	bool				CheckTing(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,OperateState& gameInfo);
private:
	bool				explorer_zheng_function(std::vector<Lint> cards,Lint index,Lint Lai_num, Lint *wangbashunzi = NULL, Card *Wangba = NULL);
	Lint				judge_ke(std::vector<Lint>& targe_vector,Lint index);
	Lint				judge_shun(std::vector<Lint>& targe_vector,Lint index);
	bool				have_play_type(Lint playtype,std::vector<Lint> playtypes);
public:
	//永州麻将七巧对，龙巧对，硬巧对
	bool  CheckqiaoduiWithWangba(CardVector& handcard, HU_PAI_LEIXING& special, Card* outCard, bool isGetCard, OperateState& gameInfo);
	//检测大胡
	//检测小七对,是否是豪华，special = 1 
	//检测清一色
	bool				CheckQingyise(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, std::vector<Lint>& vec);
	//检测乱将胡
	bool				CheckLuanjianghu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard);
	//检测碰碰胡
	bool				CheckPengpenghu(CardVector& handcard,CardVector& eatCard,Card* outCard, Card *WangBa1 = NULL, Card *WangBa2 = NULL);
	//检测全球人
	bool				CheckQuanqiuren(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard);
	bool                CheckQuanqiuren2(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard);

	//起手4个红中赖子
	bool				CheckQishouLaizi(CardVector& handcard);

	//起手4个王霸
	bool				CheckQishouWangBa(CardVector& handcard, OperateState& gameInfo);

	//枪杠胡
	//bool				CheckQiangganghu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//检测杠上开花
	bool				CheckGangshangkaihua(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//抢杠胡
	bool				CheckGangshangpao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//海底捞
	bool				CheckHaiDiLao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//海底炮			
	bool				CheckHaiDiPao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);


	//邵阳玩法
public:
	//检测同一色
	bool                CheckXiangTongSe(CardVector& handcard, Card* outCard);

	bool                BaseQingyise(CardVector & handcard, CardVector & eatCard, CardVector & pengCard, CardVector & gangcard, CardVector & mgangcard, Card * outCard, Card* Wangba1, Card* Wangba2);
	//检测清一色
	bool				CheckQingyise(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard);
	//检测清一色大碰对
	bool				CheckQingyiseDapengdui(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard);
	//门清			
	bool				CheckMenQing(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard);
	//风一色			
	bool				CheckFengYiSe(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard);
	//十三幺			
	bool				CheckShiSanYiao(CardVector& handcard, Card* outCard);

	//永州麻将
public:
	bool				CheckShiSanLan(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, bool& isqifeng);

public:
	//检查吃
	bool				CheckCanChi(CardVector& handcard,Card* outCard,  CardVector& vec, OperateState& gameInfo);
	//检测暗杠 
	bool				CheckAnGang(CardVector& handcard,  CardVector& vec, OperateState& gameInfo);

	//检测明杠 
	bool				CheckMGang(CardVector& handcard,CardVector& pengCard,  CardVector& vec,OperateState& gameInfo);

	bool				IsContinue(CardVector& result);

	bool				IsSame(CardVector& result);

	bool				IsSame(Card* c1,Card* c2);

	bool				IsNineOne(Card* c);

	bool				HaveTheCard(CardVector& result,Card* thecard);

	Card*				GetCard(Lint m_color,Lint m_number);

	bool                CheckXiaoqidui2(CardVector& handcard, Lint& special, Card* outCard, bool isGetCard, OperateState& gameInfo);

	bool				CheckXiaoqidui(CardVector& handcard, Lint& special, Card* outCard, bool isGetCard, OperateState& gameInfo);
private:
	void				SwapCardBySpecial(CardVector& cvIn, const Card specialCard[CARD_COUNT]);
private:
	Card			wan_base_card[BASE_CARD_COUNT];
	Card			bing_base_card[BASE_CARD_COUNT];
	Card			tiao_base_card[BASE_CARD_COUNT];
	Card			wind_card[WIND_CARD_COUNT];
	Card			Lai_zi_card[LAI_ZI_COUNT];

	CardVector		m_3_feng;	//全部3副牌加风
	CardVector		m_3_laizi;	//全部3副牌加癞子
	CardVector		m_3;		//全部3副牌
	//CardVector		m_wan_feng; //万牌加风 二人转转用到

private:
	static	CH_HeNan_ZhuanZhuan	*mInstance;
};

#define gCM_HeNan_ZhuanZhuan CH_HeNan_ZhuanZhuan::Instance()

#endif
