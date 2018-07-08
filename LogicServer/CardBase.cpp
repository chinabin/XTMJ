#include "CardBase.h"
#include "LTool.h"
#include "LLog.h"
#include "LTime.h"
#include "CounterManager.h"
#include "BaseCommonFunc.h"

struct HuPaiInfo
{
	int cards[9];
	int value;
	int count;

	HuPaiInfo()
	{
		memset(cards, 0, sizeof(int)*9);
		value = 0;
		count = 0;
	}

	void Convert()
	{
		value = 0;
		count = 0;
		for (int i = 8; i >= 0; --i)
		{
			value = value * 10 + cards[i];
			count += cards[i];
		}
	}

	bool IsValid()
	{
		for (int i = 0; i < 9; ++i)
		{
			if (cards[i] > 4)
				return false;
		}
		return true;
	}

	bool operator<(const HuPaiInfo& o)const
	{
		return value < o.value;
	}
};

class HuPai
{
public:
	HuPai();
	~HuPai();

	void Gen();

	int NeedHun(const int cards[9], int hun);

	void CheckHu(int cardsArr[], int cardCount, int hunCount);		// 测试代码用
	bool CheckHooWithHun(int cards[4][9], int hunnum);
	bool CheckHooWithHun(int cards[4][9], int cardsnum[4], const int color, const int hun);

	int NeedHunJiangShunKe(const int card[9], const bool bzipai, int hun);
	int NeedHunWithoutJiangShunKe(const int card[9], const bool bzipai, int hun);

	int byte2num(const int cards[]);

private:

	std::map<int, std::set<HuPaiInfo> > huPaiMap;
};

HuPai::HuPai()
{

}

HuPai::~HuPai()
{
}

int HuPai::byte2num(const int cards[])
{
	int value = 0;
	for (int i = 8; i >= 0; --i)
	{
		value = value * 10 + cards[i];
	}
	return value;
}

void HuPai::Gen()
{
	int fix[5][3] = {
		{ 3, 0, 0 },
		{ 0, 3, 0 },
		{ 0, 0, 3 },
		{ 1, 1, 1 },
		{ 0, 0, 0 },
	};

	std::set<int> numbers;		// 存储所有刻子顺子组合 保证不重复

	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			for (int k = 0; k < 5; ++k)
			{
				for (int m = 0; m < 5; ++m)
				{
					for (int b1 = 0; b1 < 7; ++b1)
					{
						for (int b2 = 0; b2 < 7; ++b2)
						{
							for (int b3 = 0; b3 < 7; ++b3)
							{
								for (int b4 = 0; b4 < 7; ++b4)
								{
									int value[9] = { 0 };

									HuPaiInfo info;

									info.cards[b1] += fix[i][0];
									info.cards[b1 + 1] += fix[i][1];
									info.cards[b1 + 2] += fix[i][2];

									info.cards[b2] += fix[j][0];
									info.cards[b2 + 1] += fix[j][1];
									info.cards[b2 + 2] += fix[j][2];

									info.cards[b3] += fix[k][0];
									info.cards[b3 + 1] += fix[k][1];
									info.cards[b3 + 2] += fix[k][2];

									info.cards[b4] += fix[m][0];
									info.cards[b4 + 1] += fix[m][1];
									info.cards[b4 + 2] += fix[m][2];

									if (info.IsValid())
									{
										info.Convert();
										auto ret = numbers.insert(info.value);
										if (ret.second)
										{
											huPaiMap[info.count].insert(info);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

int HuPai::NeedHun(const int cards[9], int hun)
{
	int cardsum = 0;
	for (int i = 0; i < 9; ++i)
		cardsum += cards[i];

	int cardInt = byte2num(cards);

	for (int i = cardsum; i <= cardsum + hun; ++i)
	{
		auto it = huPaiMap.find(i);
		if (it != huPaiMap.end())
		{
			std::set<HuPaiInfo>& arr = it->second;

			for (auto it2 = arr.begin(); it2 != arr.end(); ++it2)
			{
				int sub[9] = { 0 };
				for (int n = 0; n < 9; ++n)
				{
					sub[n] = cards[n] - (*it2).cards[n];
				}

				int subabs1 = 0;
				for (int n = 0; n < 9; ++n)
				{
					subabs1 += abs(sub[n]);
				}

				int subabs2 = 0;
				for (int n = 0; n < 9; ++n)
				{
					subabs2 += sub[n];
				}
				subabs2 = abs(subabs2);

				if (subabs1 == subabs2)
				{
					return subabs1;
				}
			}
		}
	}

	return 1000;
}

void HuPai::CheckHu(int cardsArr[], int cardCount, int hunCount)
{
	if ((cardCount + hunCount) % 3 != 2)
	{
		printf("HuPai::CheckHu 数量不对\n");
		return;
	}

	int cards[4][9] = { 0 };
	for (int i = 0; i < cardCount; ++i)
	{
		int color = cardsArr[i] / 10;
		int number = cardsArr[i] % 10;

		if (color <= 0 || color > 4 || number < 1 || number > 9)
		{
			printf("HuPai::CheckHu 牌有错误\n");
			return;
		}

		++cards[color - 1][number - 1];
	}

	if (CheckHooWithHun(cards, hunCount))
	{
		printf("HuPai::CheckHu 胡牌\n");
	}
	else
	{
		printf("HuPai::CheckHu 不能胡牌\n");
	}
}

bool HuPai::CheckHooWithHun(int cards[4][9], int hunnum)
{
	// 统计牌的数量
	int cardsnum[4] = { 0 };
	for (int i = 0; i < 4; ++i)
	{
		for (int k = 0; k < 9; ++k)
		{
			cardsnum[i] += cards[i][k];
		}
	}
	if ((cardsnum[0] + cardsnum[1] + cardsnum[2] + cardsnum[3] + hunnum) % 3 != 2)
	{
		return false;
	}

	for (int i = 0; i<4; ++i)
	{
		if (CheckHooWithHun(cards, cardsnum, i, hunnum))
			return true;
	}

	return false;
}

bool HuPai::CheckHooWithHun(int cards[4][9], int cardsnum[4], const int color, const int hun)
{
	int needHun[4] = { 0 };
	for (int i = 0; i<4; ++i)
	{
		if (color == i)
		{
			if (cardsnum[i] == 0)
				needHun[i] += 2;
			else
				needHun[i] = NeedHunJiangShunKe(cards[i], i >= 3, hun);
		}
		else
		{
			if (cardsnum[i] == 0)
				continue;

			int needhun = NeedHunWithoutJiangShunKe(cards[i], i >= 3, hun);
			needHun[i] += needhun;
		}
	}

	int need = 0;
	for (int i = 0; i<4; ++i)
	{
		need += needHun[i];
	}

	return need <= hun;
}

int HuPai::NeedHunWithoutJiangShunKe(const int card[9], const bool bzipai, int hun)
{
	int needhun = 0;
	if (bzipai)
	{
		int tcard[11] = { 0 };
		memcpy(tcard, card, sizeof(int) * 9);

		for (int i = 0; i < 7; ++i)
		{
			if (tcard[i] >= 3)
				tcard[i] -= 3;
		}

		for (int i = 0; i < 7; ++i)
		{
			if (tcard[i] == 2)
			{
				++needhun;
			}
			else if (tcard[i] == 1)
			{
				needhun += 2;
			}
		}
	}
	else
	{
		needhun = NeedHun(card, hun);
	}

	return needhun;
}

int HuPai::NeedHunJiangShunKe(const int card[9], const bool bzipai, int hun)
{
	int tneed[9] = { 0 };
	int tcard[11] = { 0 };
	memcpy(tcard, card, sizeof(int) * 9);

	for (int i = 0; i<9; ++i)
	{
		if (tcard[i] > 0)
		{
			int cost = 0;
			if (tcard[i] == 1)
			{
				--tcard[i];
				cost = 1;

				tneed[i] += 1;
			}
			else
			{
				tcard[i] -= 2;
				cost = 2;
			}

			int needhun = NeedHunWithoutJiangShunKe(tcard, bzipai, hun);
			tneed[i] += needhun;

			tcard[i] += cost;
		}
		else
		{
			tneed[i] += 2;
			int needhun = NeedHunWithoutJiangShunKe(tcard, bzipai, hun);
			tneed[i] += needhun;
		}
	}

	int needHun = tneed[0];
	for (int i = 0; i<9; ++i)
	{
		if (tneed[i] < needHun)
			needHun = tneed[i];
	}

	return needHun;
}

HuPai gHuPaiCheck;

static bool CardSortFun(Card* c1,Card* c2)
{
	if(c2->m_color > c1->m_color)
	{
		return true;
	}
	else if(c2->m_color == c1->m_color)
	{
		return c2->m_number > c1->m_number;
	}

	return false;
}

Card CardBase::m_card[CARD_COUNT];
CardVector	CardBase::m_cardNormal;
CardVector	CardBase::m_cardZi;
CardVector	CardBase::m_cardZhong;
CardVector	CardBase::m_cardFlower;

bool CardBase::Init()
{
	LLOG_ERROR("Card count=%d.", CARD_COUNT);
	Lint index = 0;
	// 万饼条
	for (Lint j = 1; j < 4; ++j)	//三种色
	{
		for (Lint k = 1; k < 10; ++k)  //9张牌
		{
			for(Lint i = 0; i < 4; ++i)		//循环加四次
			{
				m_card[index].m_color = j;
				m_card[index].m_number = k;
				m_cardNormal.push_back(&m_card[index]);
				++index;
			}
		}
	}

	gHuPaiCheck.Gen();
	return true;
}

void CardBase::DealCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card* specialCard, bool needwind, bool needzhong, bool needflower)
{
	CardVector mCards;
	mCards.insert(mCards.end(), m_cardNormal.begin(), m_cardNormal.end());
	
	if(needwind)
	{
		mCards.insert(mCards.end(), m_cardZi.begin(), m_cardZi.end());
	}
	else if(needzhong)
	{
		mCards.insert(mCards.end(), m_cardZhong.begin(), m_cardZhong.end());
	}
	
	if(needflower)
	{
		mCards.insert(mCards.end(), m_cardFlower.begin(), m_cardFlower.end());
	}

	//更新随机因子
	updateRandomFactor();

	CardVector cardtmp;
	Lint nSize = mCards.size();
	while (nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		cardtmp.push_back(mCards[seed1]);
		mCards.erase(mCards.begin() + seed1, mCards.begin() + seed1 + 1);
		nSize = mCards.size();
	}

	if(specialCard)
		SwapCardBySpecial(cardtmp, specialCard, needwind, needzhong, needflower);

	v1.insert(v1.begin(),cardtmp.begin(),cardtmp.begin()+13);
	v2.insert(v2.begin(),cardtmp.begin()+13,cardtmp.begin()+26);
	v3.insert(v3.begin(),cardtmp.begin()+26,cardtmp.begin()+39);
	v4.insert(v4.begin(),cardtmp.begin()+39,cardtmp.begin()+52);
	v5.insert(v5.begin(),cardtmp.begin()+52,cardtmp.end());
	
	std::reverse(v5.begin(),v5.end());		//逆序桌上牌
	
	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardBase::DealCardThree(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card* specialCard, bool needwind, bool needzhong, bool needflower)
{
	CardVector mCards;

	for (auto oneCard : m_cardNormal)
	{
		if (1 != oneCard->m_color)
		{
			mCards.push_back(oneCard);
		}
	}


	//更新随机因子
	updateRandomFactor();

	CardVector cardtmp;
	Lint nSize = mCards.size();
	while (nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		cardtmp.push_back(mCards[seed1]);
		mCards.erase(mCards.begin() + seed1, mCards.begin() + seed1 + 1);
		nSize = mCards.size();
	}

	if (specialCard)
		SwapCardBySpecial(cardtmp, specialCard, needwind, needzhong, needflower);

	v1.insert(v1.begin(), cardtmp.begin(), cardtmp.begin() + 13);
	v2.insert(v2.begin(), cardtmp.begin() + 13, cardtmp.begin() + 26);
	v3.insert(v3.begin(), cardtmp.begin() + 26, cardtmp.begin() + 39);
	v5.insert(v5.begin(), cardtmp.begin() + 39, cardtmp.end());

	std::reverse(v5.begin(), v5.end());		//逆序桌上牌

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
}

// void CardBase::DealCard(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5, bool needwind)
// {
// 	CardVector mCards;
// 	for(Lint i = 0; i < (needwind ? CARD_COUNT : 108); ++i)
// 	{
// 		mCards.push_back(&m_card[i]);
// 	}
// 
// 	//更新随机因子
// 	updateRandomFactor();
// 
// 	//随机算法修改
// 	CardVector cardtmp;
// 	Lint nSize = mCards.size();
// 	while (nSize > 0)
// 	{
// 		Lint seed1 = L_Rand(0, nSize - 1);
// 		cardtmp.push_back(mCards[seed1]);
// 		mCards[seed1] = mCards[nSize - 1];
// 		nSize --;
// 	}
// 
// 	v1.insert(v1.begin(),cardtmp.begin(),cardtmp.begin()+13);
// 	v2.insert(v2.begin(),cardtmp.begin()+13,cardtmp.begin()+26);
// 	v3.insert(v3.begin(),cardtmp.begin()+26,cardtmp.begin()+39);
// 	v4.insert(v4.begin(),cardtmp.begin()+39,cardtmp.begin()+52);
// 	v5.insert(v5.begin(),cardtmp.begin()+52,cardtmp.end());
// 
// 	SortCard(v1);
// 	SortCard(v2);
// 	SortCard(v3);
// 	SortCard(v4);
// }

// void CardBase::DealCard2(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5,const Card* specialCard, bool needwind)
// {
// 	CardVector mCards;
// 	for(Lint i = 0; i < (needwind ? CARD_COUNT : 108); ++i)
// 	{
// 		mCards.push_back(&m_card[i]);
// 	}
// 
// 	//更新随机因子
// 	updateRandomFactor();
// 
// 	CardVector cardtmp;
// 	Lint nSize = mCards.size();
// 	while (nSize > 0)
// 	{
// 		Lint seed1 = L_Rand(0, nSize - 1);
// 		cardtmp.push_back(mCards[seed1]);
// 		mCards.erase(mCards.begin() + seed1, mCards.begin() + seed1 + 1);
// 		nSize = mCards.size();
// 	}
// 	
// 	SwapCardBySpecial(cardtmp, specialCard, needwind);
// 
// 	v1.insert(v1.begin(),cardtmp.begin(),cardtmp.begin()+13);
// 	v2.insert(v2.begin(),cardtmp.begin()+13,cardtmp.begin()+26);
// 	v3.insert(v3.begin(),cardtmp.begin()+26,cardtmp.begin()+39);
// 	v4.insert(v4.begin(),cardtmp.begin()+39,cardtmp.begin()+52);
// 	v5.insert(v5.begin(),cardtmp.begin()+52,cardtmp.end());
// 	
// 	std::reverse(v5.begin(),v5.end());		//逆序桌上牌
// 	
// 	SortCard(v1);
// 	SortCard(v2);
// 	SortCard(v3);
// 	SortCard(v4);
// }


// void CardBase::DealCardThr(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v5, bool needwind)
// {
// 	CardVector mCards;
// 	for(Lint i = 0; i < (needwind ? CARD_COUNT : 108); ++i)
// 	{
// 		mCards.push_back(&m_card[i]);
// 	}
// 
// 	updateRandomFactor();
// 
// 	//随机算法修改
// 	CardVector cardtmp;
// 	Lint nSize = mCards.size();
// 	while (nSize > 0)
// 	{
// 		Lint seed1 = L_Rand(0, nSize - 1);
// 		cardtmp.push_back(mCards[seed1]);
// 		mCards[seed1] = mCards[nSize - 1];
// 		nSize --;
// 	}
// 
// 	v1.insert(v1.begin(),cardtmp.begin(),cardtmp.begin()+13);
// 	v2.insert(v2.begin(),cardtmp.begin()+13,cardtmp.begin()+26);
// 	v3.insert(v3.begin(),cardtmp.begin()+26,cardtmp.begin()+39);
// 
// 	v5.insert(v5.begin(),cardtmp.begin()+39,cardtmp.end());
// 
// 	SortCard(v1);
// 	SortCard(v2);
// 	SortCard(v3);
// }

// void CardBase::DealCardThr2(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v5,const Card* specialCard, bool needwind)
// {
// 	CardVector mCards;
// 	for(Lint i = 0; i < (needwind ? CARD_COUNT : 108); ++i)
// 	{
// 		mCards.push_back(&m_card[i]);
// 	}
// 
// 	CardVector cardtmp;
// 	Lint nSize = mCards.size();
// 	while (nSize > 0)
// 	{
// 		Lint seed1 = L_Rand(0, nSize - 1);
// 		cardtmp.push_back(mCards[seed1]);
// 		mCards.erase(mCards.begin() + seed1, mCards.begin() + seed1 + 1);
// 		nSize = mCards.size();
// 	}
// 
// 	SwapCardBySpecial(cardtmp, specialCard, needwind);
// 
// 	v1.insert(v1.begin(),cardtmp.begin(),cardtmp.begin()+13);
// 	v2.insert(v2.begin(),cardtmp.begin()+13,cardtmp.begin()+26);
// 	v3.insert(v3.begin(),cardtmp.begin()+26,cardtmp.begin()+39);
// 
// 	v5.insert(v5.begin(),cardtmp.begin()+39,cardtmp.end());
// 
// 	std::reverse(v5.begin(),v5.end());		//逆序桌上牌
// 
// 	SortCard(v1);
// 	SortCard(v2);
// 	SortCard(v3);
// }
// void CardBase::DealCardHongZhong(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5, bool needwind, bool needzhong)
// {
// 	CardVector mCards;
// 	for(Lint i = 0; i < (needwind ? CARD_COUNT : 108); ++i)
// 	{
// 		mCards.push_back(&m_card[i]);
// 	}
// 
// 	if(!needwind)
// 	{
// 		if(needzhong)
// 		{
// 			for(Lint i = 124; i < 128; ++i)//0-107万柄条，108-123，东南西北，红中124-127，发白128-135
// 			{
// 				mCards.push_back(&m_card[i]);
// 			}
// 		}
// 	}
// 
// 	//更新随机因子
// 	updateRandomFactor();
// 
// 	//随机算法修改
// 	CardVector cardtmp;
// 	Lint nSize = mCards.size();
// 	while (nSize > 0)
// 	{
// 		Lint seed1 = L_Rand(0, nSize - 1);
// 		cardtmp.push_back(mCards[seed1]);
// 		mCards[seed1] = mCards[nSize - 1];
// 		nSize --;
// 	}
// 
// 	v1.insert(v1.begin(),cardtmp.begin(),cardtmp.begin()+13);
// 	v2.insert(v2.begin(),cardtmp.begin()+13,cardtmp.begin()+26);
// 	v3.insert(v3.begin(),cardtmp.begin()+26,cardtmp.begin()+39);
// 	v4.insert(v4.begin(),cardtmp.begin()+39,cardtmp.begin()+52);
// 	v5.insert(v5.begin(),cardtmp.begin()+52,cardtmp.end());
// 
// 	SortCard(v1);
// 	SortCard(v2);
// 	SortCard(v3);
// 	SortCard(v4);
// }

// void CardBase::DealCardHongZhong2(CardVector& v1,CardVector& v2,CardVector& v3,CardVector& v4,CardVector& v5,const Card* specialCard, bool needwind, bool needzhong)
// {
// 	CardVector mCards;
// 	for(Lint i = 0; i < (needwind ? CARD_COUNT : 108); ++i)
// 	{
// 		mCards.push_back(&m_card[i]);
// 	}
// 
// 	if(!needwind)
// 	{
// 		if(needzhong)
// 		{
// 			for(Lint i = 124; i < 128; ++i)//0-107万柄条，108-123，东南西北，红中124-127，发白128-135
// 			{
// 				mCards.push_back(&m_card[i]);
// 			}
// 		}
// 	}
// 
// 	//更新随机因子
// 	updateRandomFactor();
// 
// 	CardVector cardtmp;
// 	Lint nSize = mCards.size();
// 	while (nSize > 0)
// 	{
// 		Lint seed1 = L_Rand(0, nSize - 1);
// 		cardtmp.push_back(mCards[seed1]);
// 		mCards.erase(mCards.begin() + seed1, mCards.begin() + seed1 + 1);
// 		nSize = mCards.size();
// 	}
// 
// 	if(needwind || needzhong)
// 		needwind = true;
// 
// 	SwapCardBySpecial(cardtmp, specialCard, needwind);
// 
// 	v1.insert(v1.begin(),cardtmp.begin(),cardtmp.begin()+13);
// 	v2.insert(v2.begin(),cardtmp.begin()+13,cardtmp.begin()+26);
// 	v3.insert(v3.begin(),cardtmp.begin()+26,cardtmp.begin()+39);
// 	v4.insert(v4.begin(),cardtmp.begin()+39,cardtmp.begin()+52);
// 	v5.insert(v5.begin(),cardtmp.begin()+52,cardtmp.end());
// 
// 	std::reverse(v5.begin(),v5.end());		//逆序桌上牌
// 
// 	SortCard(v1);
// 	SortCard(v2);
// 	SortCard(v3);
// 	SortCard(v4);
// }

/**
 * 配牌的时候只处理specialCard中前n个正确的牌
 * 
 */
void CardBase::SwapCardBySpecial(CardVector& cvIn, const Card* specialCard, bool needwind, bool needzhong, bool needflower)
{
	// 交换牌
	Lint swapCount = cvIn.size();
	for (Lint i = 0; i < swapCount; ++i)
	{
		if(!specialCard[i].isValid())
			break;

		if(needzhong)
		{
			if(specialCard[i].m_color == CARD_COLOR_ZI && specialCard[i].m_number != 5)
				break;
		}
		else if(!needwind)
		{
			if(specialCard[i].m_color == CARD_COLOR_ZI)
				break;
		}

		if(!needflower && specialCard[i].m_color == CARD_COLOR_FLOWER)
			break;

		for (Lint j = i + 1; j < swapCount; ++j)
		{
			if (*cvIn[j] == specialCard[i])
			{
				Card* tmp = cvIn[j];
				cvIn[j] = cvIn[i];
				cvIn[i] = tmp;	
				break;
			}
		}
	}
	return;
}

// void CardBase::SwapCardBySpecial(CardVector& cvIn, const Card* specialCard, bool needwind)
// {
// 	// 验证牌的合法性
// 	Lint nCardCount = cvIn.size();
// 	for (Lint i = 0; i < nCardCount; ++i)
// 	{
// 		// 验证牌的合法性
// 		bool bok = false;
// 		if ( specialCard[i].m_color == CARD_COLOR_WAN || specialCard[i].m_color == CARD_COLOR_TUO || specialCard[i].m_color == CARD_COLOR_SUO )
// 		{
// 			if ( specialCard[i].m_number > 0 && specialCard[i].m_number <= 9 )
// 			{
// 				bok = true;
// 			}
// 		}
// 		else if ( needwind && specialCard[i].m_color == CARD_COLOR_ZI )
// 		{
// 			if ( specialCard[i].m_number > 0 && specialCard[i].m_number <= 7 )
// 			{
// 				bok = true;
// 			}
// 		}
// 		if ( !bok )
// 		{
// 			continue;
// 		}
// 		for (Lint j = i + 1; j < nCardCount; ++j)
// 		{
// 			if (cvIn[j]->m_color == specialCard[i].m_color && cvIn[j]->m_number == specialCard[i].m_number)
// 			{
// 				Card* tmp = cvIn[j];
// 				cvIn[j] = cvIn[i];
// 				cvIn[i] = tmp;	
// 				break;
// 			}
// 		}
// 	}
// }

bool CardBase::IsSame(Card* c1,Card* c2)
{
	return c1->m_color==c2->m_color&&c1->m_number==c2->m_number;
}

void CardBase::SortCard(CardVector& vec)
{
	std::sort(vec.begin(),vec.end(),CardSortFun);
}

void CardBase::EraseCard(CardVector& src,CardVector& des)
{
	//这里直接比较地址是否相等
	CardVector::iterator it2 = des.begin();
	for(; it2 != des.end(); ++it2)
	{
		CardVector::iterator it1 = src.begin();
		for(; it1 != src.end();)
		{
			if((*it1) == (*it2))
			{
				it1 = src.erase(it1);
			}
			else
			{
				++it1;
			}
		}
	}
}

void CardBase::EraseCard(CardVector& src,Card* pCard)
{
	//这里直接比较地址是否相等
	for(Lsize i = 0 ; i  < src.size(); ++i)
	{
		if(src[i] == pCard)
		{
			src.erase(src.begin()+i);
			break;
		}
	}
}

//删除src 与pCard 数据一样的牌 N张
bool CardBase::EraseCard(CardVector& src,Card* pCard,Lint n)
{
	Lint cnt = 0;
	CardVector::iterator it2 = src.begin();
	for(; it2 != src.end();)
	{
		if(IsSame(pCard,*it2))
		{
			++cnt;
			it2 = src.erase(it2);
			if(cnt >= n)
				break;
		}
		else
		{
			++it2;
		}
	}
	return cnt != 0;
}

bool CardBase::CheckCanGang(CardVector& handcard,Card* outCard)
{
	int samecorlor = 0;
	for(Lsize i = 0 ;i < handcard.size(); ++i)
	{
		Card* pcard = handcard[i];
		if(*pcard == *outCard)
		{
			++samecorlor;
		}
	}
	return samecorlor >= 3;
}

int CardBase::CheckCanGangNeedHun(CardVector& handcard, Card* outCard)
{
	int samecorlor = 0;
	for(Lsize i = 0 ;i < handcard.size(); ++i)
	{
		Card* pcard = handcard[i];
		if(*pcard == *outCard)
		{
			++samecorlor;
		}
	}
	if ( samecorlor >= 3 )
	{
		return 0;
	}
	return 3 - samecorlor;
}

bool CardBase::CheckCanPeng(CardVector& handcard,Card* outCard)
{
	int samecorlor = 0;
	for(Lsize i = 0 ;i < handcard.size(); ++i)
	{
		Card* pcard = handcard[i];
		if(*pcard == *outCard)
		{
			++samecorlor;
		}
	}
	return samecorlor >= 2;
}

int CardBase::CheckCanPengNeedHun(CardVector& handcard, Card* outCard)
{
	int samecorlor = 0;
	for(Lsize i = 0 ;i < handcard.size(); ++i)
	{
		Card* pcard = handcard[i];
		if(*pcard == *outCard)
		{
			++samecorlor;
		}
	}
	if ( samecorlor >= 2 )
	{
		return 0;
	}
	return 2 - samecorlor;
}

bool CardBase::CheckCanChi(CardVector& handcard, Card* outCard, CardVector& vec)
{
	if(handcard.size() < 2 || !outCard)
	{
		return false;
	}
	Lint color = outCard->m_color;
	Lint number = outCard->m_number;
	if ( !( color == CARD_COLOR_WAN || color == CARD_COLOR_TUO || color == CARD_COLOR_SUO ) )
	{
		return false;
	}
	// 统计同一花色的牌
	Card* cards[4] = {0};	// 前后能形成吃的牌
	for ( CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( !(*it) )
		{
			continue;
		}
		if ( (*it)->m_color != color )
		{
			continue;
		}
		int n = ((*it)->m_number - number);
		if ( n == 0 )
		{
			continue;
		}
		n += (n < 0 ? 2 : 1);
		if ( n >= 0 && n < 4 )
		{
			cards[n] = *it;
		}
	}
	for ( int i = 0; i < 3; ++i )
	{
		if ( cards[i] && cards[i+1] )
		{
			vec.push_back(cards[i]);
			vec.push_back(cards[i+1]);
		}
	}
	return vec.size() != 0;
}

bool CardBase::CheckAnGang(CardVector& handcard, CardVector& vec)
{
	int cardnum[4][9] = {0};
	for ( CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( !(*it) )
		{
			continue;
		}
		if ( (*it)->m_color <= 0 || (*it)->m_color > 4 )
		{
			continue;
		}
		if ( (*it)->m_number <= 0 || (*it)->m_number > 9 )
		{
			continue;
		}

		cardnum[(*it)->m_color-1][(*it)->m_number-1]++;
		if ( cardnum[(*it)->m_color-1][(*it)->m_number-1] >= 4 )
		{
			vec.push_back(*it);
		}
	}
	return vec.size() != 0;
}

bool CardBase::CheckAnGangWithHun(CardVector& handcard, CardVector& huncards, std::vector< CardVector >& vec)
{
	int cardnum[4][9] = {0};
	Card* cards[4][9] = {0};
	for ( CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( !(*it) )
		{
			continue;
		}
		if ( (*it)->m_color <= 0 || (*it)->m_color > 4 )
		{
			continue;
		}
		if ( (*it)->m_number <= 0 || (*it)->m_number > 9 )
		{
			continue;
		}
		cardnum[(*it)->m_color-1][(*it)->m_number-1]++;
		cards[(*it)->m_color-1][(*it)->m_number-1] = *it;
	}
	for ( int i = 0; i < 4; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			if ( cardnum[i][k] >= 4 && cards[i][k] )
			{
				CardVector v;
				v.push_back( cards[i][k] );
				vec.push_back( v );
			}
			else if ( cardnum[i][k] > 0 && cards[i][k] )
			{
				int needhun = 4 - cardnum[i][k];
				if ( needhun <= (int)huncards.size() )
				{
					CardVector v;
					v.push_back( cards[i][k] );
					for ( int n = 0; n <3; ++n )
					{
						if ( n < needhun )
						{
							v.push_back( huncards[n] );
						}
						else
						{
							v.insert( v.begin(), cards[i][k] );	// 使用插入的方式 混往后面放
						}
					}
					vec.push_back( v );
				}
			}
		}
	}
	return vec.size() != 0;
}

bool CardBase::CheckMGang(CardVector& handcard, CardVector& pengCard, CardVector& vec)
{
	for(Lsize i = 0 ; i < pengCard.size(); i+=3)
	{
		Card* pcard = pengCard[i];
		for(Lsize j = 0 ; j < handcard.size(); ++j)
		{
			if(*pcard == *handcard[j])
			{
				vec.push_back(pcard);
				break;
			}
		}
	}
	return vec.size() != 0;
}

bool CardBase::CheckMGangWithHun(CardVector& handcard, CardVector& huncards, CardVector& pengCard, std::vector< CardVector >& vec)
{
	for(Lsize i = 0 ; i < pengCard.size(); i+=3)
	{
		Card* pcard = pengCard[i];
		bool bfindhand = false;
		for(Lsize j = 0 ; j < handcard.size(); ++j)
		{
			if(*pcard == *handcard[j])
			{
				CardVector v;
				v.push_back( pcard );
				vec.push_back( v );
				bfindhand = true;
				break;
			}
		}
		if ( !bfindhand && huncards.size() > 0 )
		{
			CardVector v;
			for ( int k = 0; k < 3; ++k )
			{
				if ( i+k < pengCard.size() )
				{
					v.push_back( pengCard[i+k] );
				}
			}
			v.push_back( huncards[0] );	// 带混的都是填充四张牌 相关的混牌后后面 这个顺序不可改动 玩家补杠时会从手里移除最后一张牌
			vec.push_back( v );
		}
	}
	return vec.size() != 0;
}

bool CardBase::CheckXiaoqidui(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, int& special, bool& isLongZhuaBei)
{
	isLongZhuaBei = false;
	special = 0;
	// 如果有吃 碰或者明杠肯定不是七对
	if ( eatCard.size() > 0 || pengCard.size() > 0 || gangcard.size() > 0 || mgangcard.size() > 0 )
	{
		return false;
	}

	int cardnum[4][9] = {0};
	for ( CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( !(*it) )
		{
			return false;
		}
		const int color = (*it)->m_color;
		const int number = (*it)->m_number;
		if ( color <= 0 || color > 4 )
		{
			return false;
		}
		if ( number <= 0 || number > 9 )
		{
			return false;
		}
		cardnum[color-1][number-1]++;
	}
	if ( outCard )
	{
		if ( outCard->m_color <= 0 || outCard->m_color > 4 )
		{
			return false;
		}
		if ( outCard->m_number <= 0 || outCard->m_number > 9 )
		{
			return false;
		}
		cardnum[outCard->m_color-1][outCard->m_number-1]++;
	}

	// 检查牌的个数要么是4 要么是2 要么是0
	int two = 0;	// 两张牌的个数
	int four = 0;	// 四张牌的个数
	for ( int i = 0; i < 4; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			if ( cardnum[i][k] == 0 )
			{
			}
			else if ( cardnum[i][k] == 4 )
			{
				++four;
				if(outCard && outCard->m_color - 1 == i && outCard->m_number - 1 == k)
				{
					isLongZhuaBei = true;
				}
			}
			else if ( cardnum[i][k] == 2 )
			{
				++two;
			}
			else
			{
				return false;
			}
		}
	}
	special = four;
	return true;
}

bool CardBase::CheckXiaoqidui(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, int& special)
{
	special = 0;
	// 如果有吃 碰或者明杠肯定不是七对
	if ( eatCard.size() > 0 || pengCard.size() > 0 || gangcard.size() > 0 || mgangcard.size() > 0 )
	{
		return false;
	}

	int cardnum[4][9] = {0};
	for ( CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( !(*it) )
		{
			return false;
		}
		const int color = (*it)->m_color;
		const int number = (*it)->m_number;
		if ( color <= 0 || color > 4 )
		{
			return false;
		}
		if ( number <= 0 || number > 9 )
		{
			return false;
		}
		cardnum[color-1][number-1]++;
	}
	if ( outCard )
	{
		if ( outCard->m_color <= 0 || outCard->m_color > 4 )
		{
			return false;
		}
		if ( outCard->m_number <= 0 || outCard->m_number > 9 )
		{
			return false;
		}
		cardnum[outCard->m_color-1][outCard->m_number-1]++;
	}

	// 检查牌的个数要么是4 要么是2 要么是0
	int two = 0;	// 两张牌的个数
	int four = 0;	// 四张牌的个数
	for ( int i = 0; i < 4; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			if ( cardnum[i][k] == 0 )
			{
			}
			else if ( cardnum[i][k] == 4 )
			{
				++four;
			}
			else if ( cardnum[i][k] == 2 )
			{
				++two;
			}
			else
			{
				return false;
			}
		}
	}
	special = four;
	return true;
}

bool CardBase::CheckXiaoqidui(const int handcard[4][9],CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard, int& special)
{
	special = 0;
	// 如果有吃 碰或者明杠肯定不是七对
	if ( eatCard.size() > 0 || pengCard.size() > 0 || gangcard.size() > 0 || mgangcard.size() > 0 )
	{
		return false;
	}

	// 检查牌的个数要么是4 要么是2 要么是0
	int two = 0;	// 两张牌的个数
	int four = 0;	// 四张牌的个数
	for ( int i = 0; i < 4; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			if ( handcard[i][k] == 0 )
			{
			}
			else if ( handcard[i][k] == 4 )
			{
				++four;
			}
			else if ( handcard[i][k] == 2 )
			{
				++two;
			}
			else
			{
				return false;
			}
		}
	}
	special = four;
	return true;
}

int CardBase::CheckXiaoqiduiNeedHun(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, int& special, std::vector<Card>& huncards)
{
	int cardnum[4][9] = {0};
	for ( CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( !(*it) )
		{
			return -1;
		}
		if ( (*it)->m_color <= 0 || (*it)->m_color > 4 )
		{
			return -1;
		}
		if ( (*it)->m_number <= 0 || (*it)->m_number > 9 )
		{
			return -1;
		}
		cardnum[(*it)->m_color-1][(*it)->m_number-1]++;
	}
	if ( outCard )
	{
		if ( outCard->m_color <= 0 || outCard->m_color > 4 )
		{
			return -1;
		}
		if ( outCard->m_number <= 0 || outCard->m_number > 9 )
		{
			return -1;
		}
		cardnum[outCard->m_color-1][outCard->m_number-1]++;
	}

	return CheckXiaoqiduiNeedHun( cardnum, eatCard, pengCard, gangcard, mgangcard, special, huncards );
}

int CardBase::CheckXiaoqiduiNeedHun(const int cardnum[4][9],CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard, int& special, std::vector<Card>& huncards)
{
	special = 0;
	// 如果有吃 碰或者明杠肯定不是七对
	if ( eatCard.size() > 0 || pengCard.size() > 0 || gangcard.size() > 0 || mgangcard.size() > 0 )
	{
		return -1;
	}

	huncards.clear();

	// 检查牌的个数 奇数加1
	int needhun = 0;
	for ( int i = 0; i < 4; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			if ( cardnum[i][k]%2 != 0 )
			{
				huncards.push_back( Card(k+1, i+1) );
				++needhun;
			}
			if ( cardnum[i][k] >= 3 )  // 混组成的也+1
			{
				++special;
			}
		}
	}
	return needhun;
}

//检测清一色
bool CardBase::CheckQingyise(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard)
{
	Lint color = -1;
	CardVector* cards[5] = { &handcard, &eatCard, &pengCard, &gangcard, &mgangcard };
	for ( int i = 0; i < 5; ++i )
	{
		int space = 1;
		// 吃碰杠只检查第一张牌 这样可以避免带混的牌 带混的牌放在了牌组的后面
		if ( i == 1 || i == 2 )
		{
			space = 3;
		}
		else if ( i == 3 || i == 4 )
		{
			space = 4;
		}
		int size = (int)cards[i]->size();
		for ( int k = 0; k < size; k+=space )
		{
			if ( !cards[i]->at(k) )
			{
				return false;
			}
			int color_ = cards[i]->at(k)->m_color;
			if ( color_ > 3 )	// 万饼条才算清一色 其他不算
			{
				return false;
			}
			if ( color == -1 )
			{
				color = color_;
				continue;
			}
			if ( color != color_ )
			{
				return false;
			}
		}
	}

	if ( outCard )
	{
		if ( outCard->m_color > 3 )	// 万饼条才算清一色 其他不算
		{
			return false;
		}
		if ( color == -1 )
		{
			color = outCard->m_color;
		}
		else if ( color != outCard->m_color )
		{
			return false;
		}
	}

	return true;
}

//检测混一色
bool CardBase::CheckHunyise(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard)
{
	Lint color = -1; bool hasZi = false;
	CardVector* cards[5] = { &handcard, &eatCard, &pengCard, &gangcard, &mgangcard };
	for ( int i = 0; i < 5; ++i )
	{
		int space = 1;
		// 吃碰杠只检查第一张牌 这样可以避免带混的牌 带混的牌放在了牌组的后面
		if ( i == 1 || i == 2 )
		{
			space = 3;
		}
		else if ( i == 3 || i == 4 )
		{
			space = 4;
		}
		int size = (int)cards[i]->size();
		for ( int k = 0; k < size; k+=space )
		{
			if ( !cards[i]->at(k) )
			{
				return false;
			}
			int color_ = cards[i]->at(k)->m_color;
			if( color_ == CARD_COLOR_ZI )
			{
				hasZi = true;
				continue;
			}
			if ( color_ > 3 )	// 万饼条才算清一色 其他不算
			{
				return false;
			}
			if ( color == -1 )
			{
				color = color_;
				continue;
			}
			if ( color != color_ )
			{
				return false;
			}
		}
	}

	if ( outCard )
	{
		if ( outCard->m_color == CARD_COLOR_ZI )
		{
			hasZi = true;
		}
		else if ( outCard->m_color > 3 )	// 万饼条才算清一色 其他不算
		{
			return false;
		}
		else if ( color == -1 )
		{
			color = outCard->m_color;
		}
		else if ( color != outCard->m_color )
		{
			return false;
		}
	}
	if ( color != -1 )
	{
		if(hasZi)
			return true;
		else
			return false;
	}
	return false;
}

//检测碰碰胡
bool CardBase::CheckPengpenghu(CardVector& handcard,CardVector& eatCard,Card* outCard)
{
	if(eatCard.size())
	{
		return false;
	}

	int cardnum[4][9] = {0};
	for( size_t i = 0; i < handcard.size(); ++i)
	{
		Card* pCard = handcard[i];
		if ( !pCard )
		{
			continue;
		}
		if ( pCard->m_color <= 0 || pCard->m_color > 4 )
		{
			continue;
		}
		if ( pCard->m_number <= 0 || pCard->m_number > 9 )
		{
			continue;
		}
		cardnum[pCard->m_color-1][pCard->m_number-1]++;
	}
	while(outCard)
	{
		if ( outCard->m_color <= 0 || outCard->m_color > 4 )
		{
			break;
		}
		if ( outCard->m_number <= 0 || outCard->m_number > 9 )
		{
			break;
		}
		cardnum[outCard->m_color-1][outCard->m_number-1]++;
		break;
	}
	return CheckPengpenghu( cardnum, eatCard );
}

bool CardBase::CheckPengpenghu(const int cardnum[4][9],CardVector& eatCard)
{
	if(eatCard.size())
	{
		return false;
	}
	// 统计相同牌的个数 在查听的时候 玩家手里可能会有五张一样的牌
	int numcount[5] = {0};
	for ( int i = 0; i < 4; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			if ( cardnum[i][k] == 0 )
			{
				continue;
			}
			if ( cardnum[i][k] > 0 && cardnum[i][k] < 6 )
			{
				numcount[cardnum[i][k]-1]++;
			}
			else
			{
				return false;
			}
		}
	}
	if ( numcount[0] > 0  )	// 存在单张的牌
	{
		return false;
	}
	if ( numcount[1] > 1 )	// 两张牌存在多个
	{
		return false;
	}
	if ( numcount[3] > 0 )	// 存在一个四张的牌
	{
		return false;
	}
	if ( numcount[4] > 1 )
	{
		return false;
	}
	if ( numcount[4] == 1  )	// 玩家手里可能有个杠 再加一个相同的牌组成将牌
	{
		return numcount[1] == 0;
	}
	return numcount[1] == 1;
}

int CardBase::CheckPengpenghuNeedHun(const int cardnum[4][9])
{
	int numcount[4] = {0};
	for ( int i = 0; i < 4; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			if ( cardnum[i][k] == 0 )
			{
				continue;
			}
			if ( cardnum[i][k] > 0 && cardnum[i][k] < 5 )
			{
				numcount[cardnum[i][k]-1]++;
			}
			else
			{
				return -1;
			}
		}
	}
	int needhun = 0;
	// 先考虑将牌相关
	if ( numcount[1] == 0 )	// 没有将
	{
		if ( numcount[0] > 0  )	// 存在单张的牌
		{
			needhun += 1;
			numcount[0]--;
		}
		else
		{
			needhun += 2;
		}
	}
	else if ( numcount[1] > 1 )	// 两张牌存在多个
	{
		needhun += numcount[1] - 1;	// 其中一个做将
	}

	// 单张的
	needhun += numcount[0]*2;

	if ( numcount[3] > 0 )	// 存在一个四张的牌
	{
		return -1;
	}
	return needhun;
}

//检测全球人
bool CardBase::CheckQuanqiuren(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard)
{
	// 有暗杠不算
	if ( gangcard.size() > 0 )
	{
		return false;
	}
	return CheckJinGouDiao(handcard, outCard);
}

//检测抢杠胡
bool CardBase::CheckQiangganghu(OperateState& gameInfo)
{
	if (!gameInfo.m_thinkGang && (gameInfo.m_cardState == THINK_OPERATOR_MBU && gameInfo.m_deskState != DESK_PLAY_END_CARD))
	{
		return true;
	}
	return false;
}

//杠上开花
bool CardBase::CheckGangshangkaihua(OperateState& gameInfo)
{
	//判断是不是自已杠的
	if (gameInfo.m_MePos == gameInfo.m_playerPos && gameInfo.m_thinkGang)
	{
		return true;
	}

	return false;
}

//杠上炮
bool CardBase::CheckGangshangpao(OperateState& gameInfo)
{
	if (!gameInfo.m_thinkGang && (gameInfo.m_cardState == THINK_OPERATOR_MBU && gameInfo.m_deskState != DESK_PLAY_END_CARD))
	{
		if(gameInfo.m_upState == THINK_OPERATOR_ABU || gameInfo.m_upState == THINK_OPERATOR_MGANG || gameInfo.m_upState == THINK_OPERATOR_MBU)
			return true;
	}
	//杠上炮，判断是不是别人的杠
	if (gameInfo.m_MePos != gameInfo.m_playerPos && gameInfo.m_thinkGang)
	{
		return true;
	}

	return false;
}

//海底捞
bool CardBase::CheckHaiDiLao(OperateState& gameInfo)
{
	if (gameInfo.m_MePos == gameInfo.m_playerPos && gameInfo.bNoCard)
	{
		return true;
	}
	return false;
}

//海底炮
bool CardBase::CheckHaiDiPao(OperateState& gameInfo)
{
	if (gameInfo.m_MePos != gameInfo.m_playerPos && gameInfo.bNoCard)
	{
		return true;
	}
	return false;
}

bool CardBase::Check258( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard )
{
	CardVector* cards[5] = { &handcard, &eatCard, &pengCard, &gangcard, &mgangcard };
	for ( int i = 0; i < 5; ++i )
	{
		for ( CardVector::iterator it = cards[i]->begin(); it != cards[i]->end(); ++it )
		{
			if ( !(*it) )
			{
				return false;
			}
			if ( (*it)->m_color <= 0 || (*it)->m_color > 3 )
			{
				return false;
			}
			if((*it)->m_number != 2 && (*it)->m_number != 5 && (*it)->m_number != 8)
			{
				return false;
			}
		}
	}
	if ( outCard )
	{
		if ( outCard->m_color <= 0 || outCard->m_color > 3 )
		{
			return false;
		}
		if(outCard->m_number != 2 && outCard->m_number != 5 && outCard->m_number != 8)
		{
			return false;
		}
	}
	return true;
}

bool CardBase::CheckJinGouDiao( CardVector& handcard, Card* outCard)
{
	if ( handcard.size() == 1 && outCard )
	{
		return (*handcard.front() == *outCard);
	}
	if ( handcard.size() == 2 && !outCard )
	{
		return (*handcard.front() == *handcard.back());
	}
	return false;
}

Lint CardBase::CheckGen( CardVector& handcard, CardVector& pengCard, CardVector& agangcard, CardVector& mgangcard, CardVector& eatCard, Card* getCard )
{
	// 统计牌数
	int cardnum[4][9] = {0};
	CardVector* cards[5] = { &handcard, &pengCard, &agangcard, &mgangcard, &eatCard };
	for ( int i = 0; i < 5; ++i )
	{
		for ( CardVector::iterator it = cards[i]->begin(); it != cards[i]->end(); ++it )
		{
			if ( !(*it) )
			{
				continue;
			}
			if ( (*it)->m_color <= 0 || (*it)->m_color > 4 )
			{
				continue;
			}
			if ( (*it)->m_number <= 0 || (*it)->m_number > 9 )
			{
				continue;
			}
			cardnum[(*it)->m_color-1][(*it)->m_number-1]++;
		}
	}

	while ( getCard )
	{
		if ( getCard->m_color <= 0 || getCard->m_color > 3 )
		{
			break;
		}
		if ( getCard->m_number <= 0 || getCard->m_number > 9 )
		{
			break;
		}
		cardnum[getCard->m_color-1][getCard->m_number-1]++;
		break;
	}
	
	int gen = 0;
	for ( int i = 0; i < 4; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			if ( cardnum[i][k] >= 4 )
			{
				++gen;
			}
		}
	}
	return gen;
}

bool CardBase::CheckOneNine( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard )
{
	auto tmp = handcard;
	if(outCard)
	{
		tmp.push_back(outCard);
	}
	// 简单检查数据的合法性
	if ( eatCard.size()%3 != 0 || pengCard.size()%3 != 0 || gangcard.size()%4 != 0 || mgangcard.size()%4 != 0 )
	{
		return false;
	}
	// 吃碰牌检查
	CardVector* group1[2] = { &eatCard, &pengCard };
	for ( int i = 0; i < 2; ++i )
	{
		for ( size_t k = 0; k < group1[i]->size(); k+=3 )
		{
			if ( !group1[i]->at(k) || !group1[i]->at(k+2) )
			{
				return false;
			}
			Lint color = group1[i]->at(k)->m_color;
			Lint number1 = group1[i]->at(k)->m_number;
			Lint number3 = group1[i]->at(k+2)->m_number;
			if ( !( color == CARD_COLOR_WAN || color == CARD_COLOR_TUO || color == CARD_COLOR_SUO ) )
			{
				return false;
			}
			if ( !(number1 == 1 || number1 == 9 || number3 == 1 || number3 == 9 ) )
			{
				return false;
			}
		}
	}
	// 杠牌检查
	CardVector* group2[2] = { &gangcard, &mgangcard };
	for ( int i = 0; i < 2; ++i )
	{
		for ( Lsize k = 0; k < group2[i]->size(); k+=4 )
		{
			if ( !group2[i]->at(k) )
			{
				return false;
			}
			if ( !(group2[i]->at(k)->m_color == CARD_COLOR_WAN || group2[i]->at(k)->m_color == CARD_COLOR_TUO || group2[i]->at(k)->m_color == CARD_COLOR_SUO ) )
			{
				return false;
			}
			if ( !(group2[i]->at(k)->m_number == 1 || group2[i]->at(k)->m_number == 9) )
			{
				return false;
			}
		}
	}

	// 判断手牌中是否能能组合成 1 or 9
	// 按花色统计牌数
	int cardnum[3][9] = {0};
	for ( CardVector::iterator it = tmp.begin(); it != tmp.end(); ++it )
	{
		if ( !(*it) )
		{
			continue;
		}
		if ( (*it)->m_color == CARD_COLOR_WAN || (*it)->m_color == CARD_COLOR_TUO || (*it)->m_color == CARD_COLOR_SUO )
		{
			cardnum[(*it)->m_color-1][(*it)->m_number-1]++;
		}
	}
	int handvalidsize = 0;
	// 判断的原则就是 2==3<=1 7==8<=9 handsize=1+2+3+7+8+9==handcard.size()
	for ( int i = 0; i < 3; ++i )
	{
		if ( !(cardnum[i][1] == cardnum[i][2] && cardnum[i][1] <= cardnum[i][0]) )	// 2==3<=1
		{
			return false;
		}
		if ( !(cardnum[i][6] == cardnum[i][7] && cardnum[i][6] <= cardnum[i][8]) )	// 7==8<=9
		{
			return false;
		}
		handvalidsize += ( cardnum[i][1] + cardnum[i][2] + cardnum[i][0] );
		handvalidsize += ( cardnum[i][6] + cardnum[i][7] + cardnum[i][8] );
	}
	return handvalidsize == (int)tmp.size();
}

bool CardBase::CheckNoOneNine( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard )
{
	// 简单检查数据的合法性
	if ( eatCard.size()%3 != 0 || pengCard.size()%3 != 0 || gangcard.size()%4 != 0 || mgangcard.size()%4 != 0 )
	{
		return false;
	}
	// 判断牌组中是否都含有 1 or 9
	CardVector* group[4] = { &eatCard, &pengCard, &gangcard, &mgangcard };
	for ( int i = 0; i < 4; ++i )
	{
		for ( size_t k = 0; k < group[i]->size(); ++k )
		{
			if ( !group[i]->at(k) )
			{
				return false;
			}
			if ( group[i]->at(k)->m_color == CARD_COLOR_WAN || group[i]->at(k)->m_color == CARD_COLOR_TUO || group[i]->at(k)->m_color == CARD_COLOR_SUO )
			{
				if ( group[i]->at(k)->m_number == 1 || group[i]->at(k)->m_number == 9 )
				{
					return false;
				}
			}
		}
	}
	// 判断手牌中是否有幺九
	for ( CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( !(*it) )
		{
			continue;
		}
		if ( (*it)->m_color == CARD_COLOR_WAN || (*it)->m_color == CARD_COLOR_TUO || (*it)->m_color == CARD_COLOR_SUO )
		{
			if ( (*it)->m_number == 1 || (*it)->m_number == 9 )
			{
				return false;
			}
		}
	}
	if ( outCard )
	{
		if ( outCard->m_color == CARD_COLOR_WAN || outCard->m_color == CARD_COLOR_TUO || outCard->m_color == CARD_COLOR_SUO )
		{
			if ( outCard->m_number == 1 || outCard->m_number == 9 )
			{
				return false;
			}
		}
	}
	return true;
}

bool CardBase::CheckMenQing(CardVector& eatCard,CardVector& pengCard,CardVector& mgangcard)
{
	// 简单检查数据的合法性
// 	if ( eatCard.size()%3 != 0 || pengCard.size()%3 != 0 || mgangcard.size()%4 != 0 )
// 	{
// 		return false;
// 	}
	
	// 如果有吃 碰或者明杠肯定不是门前清
	if ( eatCard.size() > 0 || pengCard.size() > 0 || mgangcard.size() > 0 )
	{
		return false;
	}
	return true;
}

bool CardBase::CheckMidCard(const CardVector& handcard,const Card& mid,const Card* outCard)
{
	auto tmp = handcard;
	if(outCard && *outCard == mid)
	{
		SortCard(tmp);

		Lint num = mid.m_number - 1;
		for(auto it = tmp.begin(); it != tmp.end(); )
		{
			if((*it)->m_color == mid.m_color && (*it)->m_number == num)
			{
				it = tmp.erase(it);
				if(num == mid.m_number - 1)
					num = mid.m_number+1;
				else if(num == mid.m_number+1)
				{
					num = 0;
					break;
				}
			}
			else
				++ it;
		}

		if(num == 0 && CheckHoo(tmp,NULL))
		{
			return true;
		}
	}
	return false;
}

bool CardBase::CheckOne2Nine(const CardVector& handcard,const Card* curCard)
{
	int cards[4][9];
	int cardsnum[4] = {0};
	memset(cards,0,sizeof(cards));

	for(size_t i = 0; i < handcard.size(); ++i)
	{
		const int color = handcard.at(i)->m_color;
		const int number = handcard.at(i)->m_number;
		if(color > 0 && color < 5 && number > 0 && number < 10)
		{
			cards[color-1][number-1]++;
			cardsnum[color-1]++;
		}
	}

	if(curCard)
	{
		const int color = curCard->m_color;
		const int number = curCard->m_number;
		if(color > 0 && color < 5 && number > 0 && number < 10)
		{
			cards[color-1][number-1]++;
			cardsnum[color-1]++;
		}
	}

	for(int i = 0; i < 4; ++i)
	{
		if(cardsnum[i] >= 9)  //3种花色 中只能找到一种 9张牌的
		{
			for(int j = 0; j < 9; ++j)
			{
				if(cards[i][j])
				{
					cards[i][j]--;
				}
				else
				{
					return false;
				}
			}

			if(CheckHoo(cards))
			{
				return true;
			}
			break;
		}
	}
	return false;
}

bool CardBase::CheckSisterPairs( const CardVector& handcard, const Card* curCard )
{
	int mycard[3][9];
	int cardsnum[4] = {0};
	memset(mycard,0,sizeof(mycard));

	for(size_t i = 0; i < handcard.size(); ++i)
	{
		const int color = handcard.at(i)->m_color;
		const int number = handcard.at(i)->m_number;
		if(color > 0 && color < 4 && number > 0 && number < 10)
		{
			mycard[color-1][number-1]++;
			cardsnum[color-1]++;
		}
	}

	if(curCard)
	{
		const int  color = curCard->m_color;
		const int  number = curCard->m_number;
		if(color > 0 && color < 4 && number > 0 && number < 10)
		{
			mycard[color-1][number-1]++;
			cardsnum[color-1]++;
		}
	}

	for(int i = 0; i < 3; ++i)
	{
		if(cardsnum[i] >= 6)
		{
			for(int j = 0; j < 9-2; ++j)
			{
				if(mycard[i][j] > 1 && mycard[i][j+1] > 1 && mycard[i][j+2] > 1)
				{
					//remove the sisters
					mycard[i][j] -= 2;
					mycard[i][j+1] -= 2;
					mycard[i][j+2] -= 2;
					if(CheckHoo(mycard))    //满足条件 return
					{
						return true;
					}
					//reback the sisters
					mycard[i][j] += 2;
					mycard[i][j+1] += 2;
					mycard[i][j+2] += 2;
				}
			}
		}
	}
	return false;
}

bool CardBase::CheckBadCard( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, bool& isqixin )
{
	int cardnum[4][9] = {0};
	for ( CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( !(*it) )
		{
			return false;
		}
		if ( (*it)->m_color <= 0 || (*it)->m_color > 4 )
		{
			return false;
		}
		if ( (*it)->m_number <= 0 || (*it)->m_number > 9 )
		{
			return false;
		}
		cardnum[(*it)->m_color-1][(*it)->m_number-1]++;
		// 只能有一张
		if ( cardnum[(*it)->m_color-1][(*it)->m_number-1] > 1 )
		{
			return false;
		}
	}
	if ( outCard )
	{
		if ( outCard->m_number <= 0 || outCard->m_number > 9 )
		{
			return false;
		}
		cardnum[outCard->m_color-1][outCard->m_number-1]++;
		// 只能有一张
		if ( cardnum[outCard->m_color-1][outCard->m_number-1] > 1 )
		{
			return false;
		}
	}
	return CheckBadCard( cardnum, eatCard, pengCard, gangcard, mgangcard, isqixin );
}

bool CardBase::CheckBadCard( const int handcard[4][9], CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, bool& isqixin )
{
	// 有吃碰杠肯定不是烂牌
	CardVector* cards[4] = { &eatCard, &pengCard, &gangcard, &mgangcard };
	for ( int i = 0; i < 4; ++i )
	{
		if ( cards[i]->size() > 0 )
		{
			return false;
		}
	}

	// 147 258 369单个组合的才是烂牌
	for ( int i = 0; i < 3; ++i )
	{
		bool has[3] = {false};
		for ( int k = 0; k < 9; ++k )
		{
			if ( handcard[i][k] > 1 )	// 只能有一张
			{
				return false;
			}
			if ( handcard[i][k] > 0 )
			{
				if ( k == 0 || k == 3 || k == 6 )
				{
					if ( has[1] || has[2] )
					{
						return false;
					}
					has[0] = true;
				}
				else if ( k == 1 || k == 4 || k == 7 )
				{
					if ( has[0] || has[2] )
					{
						return false;
					}
					has[1] = true;
				}
				else if ( k == 2 || k == 5 || k == 8 )
				{
					if ( has[0] || has[1] )
					{
						return false;
					}
					has[2] = true;
				}
			}
		}
	}

	// 检查是否东南西北是否全部存在
	isqixin = true;
	for ( int k = 0; k < 7; ++k )
	{
		if ( handcard[3][k] > 1 ) // 只能有一张
		{
			return false;
		}
		if ( handcard[3][k] == 0 )
		{
			isqixin = false;
		}
	}
	return true;
}

bool CardBase::CheckBadCardWithHun( CardVector& handcard, int hunnum, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, bool& isqixin )
{
	// 统计手牌 除了混所有的手牌
	int cardnum[4][9];
	memset(cardnum,0,sizeof(cardnum));
	for(size_t i = 0; i < handcard.size(); ++i)
	{
		const int color = handcard[i]->m_color;
		const int number = handcard[i]->m_number;
		if(color > 0 && color < 5 && number > 0 && number < 10)
		{
			cardnum[color-1][number-1]++;
			// 只能有一张
			if ( cardnum[color-1][number-1] > 1 )
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	if(outCard)
	{
		const int color = outCard->m_color;
		const int number = outCard->m_number;
		if(color > 0 && color < 5 && number > 0 && number < 10)
		{
			cardnum[color-1][number-1]++;
			// 只能有一张
			if ( cardnum[color-1][number-1] > 1 )
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	if ( CheckBadCard( cardnum, eatCard, pengCard, gangcard, mgangcard, isqixin ) )
	{
		if ( !isqixin )
		{
			// 判断字牌数和混数加起来是否等于全部的字牌数 等于就是七心
			int zinum = hunnum;
			for ( int k = 0; k < 7; ++k )
			{
				if ( cardnum[3][k] != 0 )
				{
					++zinum;
				}
			}
			if ( zinum >= 7 )
			{
				isqixin = true;
			}
		}
		return true;
	}
	return false;
}

bool CardBase::CheckBadCard2( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, bool& isqixin )
{
	int cardnum[4][9] = {0};
	for ( CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( !(*it) )
		{
			return false;
		}
		if ( (*it)->m_color <= 0 || (*it)->m_color > 4 )
		{
			return false;
		}
		if ( (*it)->m_number <= 0 || (*it)->m_number > 9 )
		{
			return false;
		}
		cardnum[(*it)->m_color-1][(*it)->m_number-1]++;
		// 只能有一张
		if ( cardnum[(*it)->m_color-1][(*it)->m_number-1] > 1 )
		{
			return false;
		}
	}
	if ( outCard )
	{
		if ( outCard->m_number <= 0 || outCard->m_number > 9 )
		{
			return false;
		}
		cardnum[outCard->m_color-1][outCard->m_number-1]++;
		// 只能有一张
		if ( cardnum[outCard->m_color-1][outCard->m_number-1] > 1 )
		{
			return false;
		}
	}
	return CheckBadCard2( cardnum, eatCard, pengCard, gangcard, mgangcard, isqixin );
}

bool CardBase::CheckBadCard2( const int handcard[4][9], CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, bool& isqixin )
{
	// 有吃碰杠肯定不是烂牌
	CardVector* cards[4] = { &eatCard, &pengCard, &gangcard, &mgangcard };
	for ( int i = 0; i < 4; ++i )
	{
		if ( cards[i]->size() > 0 )
		{
			return false;
		}
	}

	// 检查数字牌的间隔是否都隔开两种
	for ( int i = 0; i < 3; ++i )
	{
		Lint last_number = -3;
		for ( int k = 0; k < 9; ++k )
		{
			if ( handcard[i][k] > 0 )
			{
				if ( k - last_number <= 2 )
				{
					return false;
				}
				last_number = k;
			}
		}
	}

	// 检查是否东南西北是否全部存在
	isqixin = true;
	for ( int k = 0; k < 7; ++k )
	{
		if ( handcard[3][k] > 1 ) // 只能有一张
		{
			return false;
		}
		if ( handcard[3][k] == 0 )
		{
			isqixin = false;
		}
	}
	return true;
}

bool CardBase::CheckBadCardWithHun2( CardVector& handcard, int hunnum, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, bool& isqixin )
{
	// 统计手牌 除了混所有的手牌
	int cardnum[4][9];
	memset(cardnum,0,sizeof(cardnum));
	for(size_t i = 0; i < handcard.size(); ++i)
	{
		const int color = handcard[i]->m_color;
		const int number = handcard[i]->m_number;
		if(color > 0 && color < 5 && number > 0 && number < 10)
		{
			cardnum[color-1][number-1]++;
			// 只能有一张
			if ( cardnum[color-1][number-1] > 1 )
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	if(outCard)
	{
		const int color = outCard->m_color;
		const int number = outCard->m_number;
		if(color > 0 && color < 5 && number > 0 && number < 10)
		{
			cardnum[color-1][number-1]++;
			// 只能有一张
			if ( cardnum[color-1][number-1] > 1 )
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	if ( CheckBadCard2( cardnum, eatCard, pengCard, gangcard, mgangcard, isqixin ) )
	{
		if ( !isqixin )
		{
			// 判断字牌数和混数加起来是否等于全部的字牌数 等于就是七心
			int zinum = hunnum;
			for ( int k = 0; k < 7; ++k )
			{
				if ( cardnum[3][k] != 0 )
				{
					++zinum;
				}
			}
			if ( zinum == 7 )
			{
				isqixin = true;
			}
		}
		return true;
	}
	return false;
}

bool CardBase::YaoCard( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard,CardVector& mgangcard, Card* outCard )
{
	// 有吃碰杠肯定不是幺牌
	CardVector* cards[4] = { &eatCard, &pengCard, &gangcard, &mgangcard };
	for ( int i = 0; i < 4; ++i )
	{
		if ( cards[i]->size() > 0 )
		{
			return false;
		}
	}
	// 手里的牌只能是1 9 和字牌  全是字也是幺牌
	for ( CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( !(*it) )
		{
			return false;
		}

		if ( (*it)->m_color > 0 && (*it)->m_color <= 3 )
		{
			if ( !((*it)->m_number == 1 || (*it)->m_number == 9) )
			{
				return false;
			}
		}
		else
		{
			if ( (*it)->m_color != 4 )
			{
				return false;
			}
		}
	}
	if ( outCard )
	{
		if ( outCard->m_color > 0 && outCard->m_color <= 3 )
		{
			if ( !(outCard->m_number == 1 || outCard->m_number == 9) )
			{
				return false;
			}
		}
		else
		{
			if ( outCard->m_color != 4 )
			{
				return false;
			}
		}
	}
	return true;
}

// 检查大小三元
bool CardBase::CheckDXSanYuan( CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, Lint& zike )
{
	zike = 0;
	Lint ZhongFaBai[3] = { 0 };	// 中发白
	CardVector* cards[4] = { &handcard, &pengCard, &gangcard, &mgangcard };
	for ( int i = 0; i < 4; ++i )
	{
		for ( CardVector::iterator it = cards[i]->begin(); it != cards[i]->end(); ++it )
		{
			if ( !(*it) )
			{
				return false;
			}
			if ( (*it)->m_color !=  CARD_COLOR_ZI || (*it)->m_number < 5 || (*it)->m_number > 7)
			{
				continue;
			}
			ZhongFaBai[(*it)->m_number - 5]++;
		}
	}

	if(outCard && outCard->m_color == CARD_COLOR_ZI && outCard->m_number <= 7 && outCard->m_number >= 5) 
	{
		ZhongFaBai[outCard->m_number - 5]++;
	}

	// 统计刻的数量
	Lint nXiao = 0;
	for ( int i = 0; i < 3; ++i )
	{
		if(ZhongFaBai[i]/3 == 1)
		{
			++zike;
		}
		else if(ZhongFaBai[i]/2 == 1)
		{
			++nXiao;
		}
	}

	if(zike == 3 || (zike == 2 && nXiao == 1))
	{
		return true;
	}
	return false;
}

bool CardBase::CheckDXSanYuanWithHun( const int cards[4][9], int hunnum, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Lint& zike, Lint& needhun )
{
	int handcards[4][9];
	memcpy( handcards, cards, sizeof(handcards) );
	return CheckDXSanYuanWithHun2( handcards, hunnum, eatCard, pengCard, gangcard, mgangcard, zike, needhun );
}

bool CardBase::CheckDXSanYuanWithHun2( int cards[4][9], int hunnum, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Lint& zike, Lint& needhun )
{
	// 需要的中发白
	int needzongfabai[3];
	for ( int i = 0; i < 3; ++i )
	{
		needzongfabai[i] = 3;
	}

	CardVector* groups[3] = { &pengCard, &gangcard, &mgangcard };
	for ( int i = 0; i < 3; ++i )
	{
		int size = groups[i]->size();
		int dif = (i == 1 ? 3 : 4);
		for ( int k = 0; k < size; k+=dif )
		{
			if ( groups[i]->at(k) )
			{
				continue;
			}
			if ( groups[i]->at(k)->m_color !=  CARD_COLOR_ZI || groups[i]->at(k)->m_number < 5 || groups[i]->at(k)->m_number > 7)
			{
				continue;
			}
			needzongfabai[groups[i]->at(k)->m_number-5] -= 3;
		}
	}

	// 从手牌中减去需要的中发白
	for ( int i = 5; i <= 7; ++i )
	{
		if ( needzongfabai[i-5] > 0 )
		{
			if ( cards[3][i-1] >= needzongfabai[i-5] )
			{
				cards[3][i-1] -= needzongfabai[i-5];
				needzongfabai[i-5] = 0;
			}
			else
			{
				needzongfabai[i-5] -= cards[3][i-1];
				cards[3][i-1] = 0;
			}
		}
	}
	needhun = 0;
	for ( int i = 0; i < 3; ++i )
	{
		needhun += needzongfabai[i];
	}

	zike = 0;
	// 从手中删除必要的牌 查看是否依然能小胡
	// 大三元
	if ( needhun <= hunnum && CheckHooWithHun( cards, hunnum - needhun ) )
	{
		zike = 3;
		return true;
	}
	// 小三元 考虑需要将
	needhun -= 1;

	int otherneedhun = 0;	// 其他牌成顺刻需要的混
	for ( int i = 0; i < 4; ++i )
	{
		otherneedhun += ToBeShunKeNeedHun( cards[i], i >= 3 );
	}
	if ( otherneedhun <= (hunnum - needhun) )
	{
		zike = 2;
		return true;
	}
	return false;
}

TingVec CardBase::CheckGetCardTing( CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo )
{
	TingVec result;
	// 
	CardVector tmp = handcard;
	if (getCard)
	{
		tmp.push_back(getCard);
	}

	static int calccount = 0;
	static Llong calctime = 0;
	++calccount;
	LTime precur;
	// 多线程计算
	gCounterManager.CalcTing( tmp, pengCard, agangCard, mgangCard, eatCard, gameInfo, this, result );
	//gCounterManager.CalcTingWithHun(tmp, pengCard, agangCard, mgangCard, eatCard, gameInfo, this, gameInfo.m_hCard,result);

	LTime now;
	calctime += now.MSecs() - precur.MSecs();
	if ( calccount == 10000 )
	{
		LLOG_ERROR("[CardBase::CheckGetCardTing] %d calc spend %lld MSecs", calccount, calctime);
		calccount = 0;
		calctime = 0;
	}

// 	bool checked[4][9] = {false};
// 	Lsize size = tmp.size();
// 	for ( Lsize i = 0; i < size; ++i )
// 	{
// 		if ( !tmp[i] )
// 		{
// 			continue;
// 		}
// 		Lint color = tmp[i]->m_color;
// 		Lint number = tmp[i]->m_number;
// 		if ( (color <= 0 || color > 4) || (number <= 0 || number > 9) )
// 		{
// 			continue;
// 		}
// 		if ( checked[color-1][number-1] )
// 		{
// 			continue;
// 		}
// 		checked[color-1][number-1] = true;
// 
// 		TingUnit unit;
// 		unit.m_card = *tmp[i];	// 表示要打出去的牌
// 
// 		// 组织手牌
// 		CardVector cards;
// 		cards.reserve( tmp.size() - 1 );
// 		for ( Lsize k = 0; k < size; ++k )
// 		{
// 			if ( k != i )
// 			{
// 				cards.push_back( tmp[k] );
// 			}
// 		}
// 
// 		CheckTing( cards, pengCard, agangCard, mgangCard, eatCard, gameInfo, unit.m_tingcard );
// 		if ( unit.m_tingcard.size() > 0 )
// 		{
// 			result.push_back( unit );
// 		}
// 	}
	return result;
}

bool CardBase::CheckHoo( const CardVector& handcard, const Card* curCard )
{
	int cards[4][9];
	memset(cards,0,sizeof(cards));

	for(size_t i = 0; i < handcard.size(); ++i)
	{
		const int color = handcard.at(i)->m_color;
		const int number = handcard.at(i)->m_number;
		if(color > 0 && color < 5 && number > 0 && number < 10)
		{
			cards[color-1][number-1]++;
		}
	}

	if(curCard)
	{
		const int color = curCard->m_color;
		const int number = curCard->m_number;
		if(color > 0 && color < 5 && number > 0 && number < 10)
		{
			cards[color-1][number-1]++;
		}
	}

	return CheckHoo(cards);
}

bool CardBase::CheckHoo258( const CardVector& handcard, bool b258)
{
	int cards[4][9];
	memset(cards,0,sizeof(cards));

	for(size_t i = 0; i < handcard.size(); ++i)
	{
		const int color = handcard.at(i)->m_color;
		const int number = handcard.at(i)->m_number;
		if(color > 0 && color < 5 && number > 0 && number < 10)
		{
			cards[color-1][number-1]++;
		}
	}

	if( b258 )
	{
		CardValue jiangCard;
		jiangCard.m_color = 0;
		jiangCard.m_number = 0;
		bool bret = CheckHoo(cards, jiangCard);

		if(bret && jiangCard.m_color > 0 && jiangCard.m_color < 5 && jiangCard.m_number > 0 && jiangCard.m_number < 10)
		{
			if((jiangCard.m_color != 4) && (jiangCard.m_number == 2 || jiangCard.m_number == 5 || jiangCard.m_number == 8))
				return true;
		}
		
		return false;
	}
	
	return CheckHoo(cards);
}

bool CardBase::CheckHoo( int cards[4][9] )
{
	// 统计牌的数量
	int cardsnum[4] = {0};
	for( int i = 0; i < 4 ; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			cardsnum[i] += cards[i][k];
		}
	}
	if ( (cardsnum[0] + cardsnum[1] + cardsnum[2] + cardsnum[3])%3 != 2 )
	{
		return false;
	}

	int jiangPos = -1;
	for( int i = 0; i < 4 ; ++i)
	{
		int yuShu = cardsnum[i]%3;
		if ( yuShu == 1)
		{
			return false;
		}
		if ( yuShu == 2 )
		{
			if ( jiangPos != -1 )
			{
				return false;
			}
			jiangPos=i;
		}
	}

	if( jiangPos == -1 )
	{
		return false;
	}

	// 判断非将位置的牌是否能组碰 顺
	for( int i = 0 ; i < 4; ++i)
	{        
		if ( i != jiangPos)
		{
			if ( !ToBeShunKe(cards[i], i >= 3) )
			{
				return false;
			}
		}
	}

	bool success=false;
	for( int i = 0; i < 9; ++i )
	{
		if (cards[jiangPos][i] >= 2)
		{
			cards[jiangPos][i] -= 2;
			if( ToBeShunKe(cards[jiangPos], jiangPos >= 3) )
			{
				success = true;
			}
			cards[jiangPos][i]+=2;
			if (success)
			{
				break;
			}
		}
	}
	return success;
}

// 检查胡 3n+2牌型，获取胡牌的将牌
bool CardBase::CheckHoo( int cards[4][9], CardValue& jiangCard )
{
	// 统计牌的数量
	int cardsnum[4] = {0};
	for( int i = 0; i < 4 ; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			cardsnum[i] += cards[i][k];
		}
	}
	if ( (cardsnum[0] + cardsnum[1] + cardsnum[2] + cardsnum[3])%3 != 2 )
	{
		return false;
	}

	int jiangPos = -1;
	for( int i = 0; i < 4 ; ++i)
	{
		int yuShu = cardsnum[i]%3;
		if ( yuShu == 1)
		{
			return false;
		}
		if ( yuShu == 2 )
		{
			if ( jiangPos != -1 )
			{
				return false;
			}
			jiangPos=i;
		}
	}

	if( jiangPos == -1 )
	{
		return false;
	}

	// 判断非将位置的牌是否能组碰 顺
	for( int i = 0 ; i < 4; ++i)
	{        
		if ( i != jiangPos)
		{
			if ( !ToBeShunKe(cards[i], i >= 3) )
			{
				return false;
			}
		}
	}

	bool success=false;
	for( int i = 0; i < 9; ++i )
	{
		if (cards[jiangPos][i] >= 2)
		{
			cards[jiangPos][i] -= 2;
			if( ToBeShunKe(cards[jiangPos], jiangPos >= 3) )
			{
				success = true;
				jiangCard.m_color = jiangPos + 1;
				jiangCard.m_number = i + 1;
			}
			cards[jiangPos][i]+=2;
			if (success)
			{
				break;
			}
		}
	}
	return success;
}

// card[9]为各色card number的个数。
bool CardBase::ToBeShunKe( const int card[9], bool bzipai )
{
	int tcard[11] = {0};
	memcpy( &tcard[0], card, sizeof(int)*9 );

	for( int i = 0; i < 9; )
	{
		// 先组刻
		if( tcard[i] >= 3 )
		{
			tcard[i] -= 3;
		}

		if( tcard[i] == 0 )
		{
			++i;
			continue;
		}
		// 字牌必须都是刻
		if ( bzipai )
		{
			return false;
		}

		// 组顺子
		if( tcard[i+1] > 0 && tcard[i+2] > 0 )
		{
			--tcard[i];
			--tcard[i+1];
			--tcard[i+2];
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool CardBase::CheckHooWithHun( const CardVector& handcard, const Card* outCard, int hunnum )
{
	int cards[4][9];   
	memset(cards,0,sizeof(cards));
	for(size_t i = 0; i < handcard.size(); ++i)
	{
		const int color = handcard.at(i)->m_color;
		const int number = handcard.at(i)->m_number;
		if(color > 0 && color < 5 && number > 0 && number < 10)
		{
			cards[color-1][number-1]++;
		}
	}
	if ( outCard )
	{
		const int color = outCard->m_color;
		const int number = outCard->m_number;
		if(color > 0 && color < 5 && number > 0 && number < 10)
		{
			cards[color-1][number-1]++;
		}
	}
	return CheckHooWithHun( cards, hunnum );
}

bool CardBase::CheckHooWithHun258( const CardVector& handcard, int hunnum, bool b258 )
{
	int cards[4][9];   
	memset(cards,0,sizeof(cards));
	for(size_t i = 0; i < handcard.size(); ++i)
	{
		const int color = handcard.at(i)->m_color;
		const int number = handcard.at(i)->m_number;
		if(color > 0 && color < 5 && number > 0 && number < 10)
		{
			cards[color-1][number-1]++;
		}
	}

	if( b258 )
	{
		CardValue jiangCard;
		jiangCard.m_color = 0;
		jiangCard.m_number = 0;
		bool bret = CheckHooWithHun258(cards, hunnum, jiangCard);

		if(bret)
		{
			if(jiangCard.m_color == 0 && jiangCard.m_number == 0)//任意将
				return true;

			if(jiangCard.m_color > 0 && jiangCard.m_color < 5 && jiangCard.m_number > 0 && jiangCard.m_number < 10)
			{
				if((jiangCard.m_color != 4) && (jiangCard.m_number == 2 || jiangCard.m_number == 5 || jiangCard.m_number == 8))
					return true;
			}
		}

		return false;
	}
	return CheckHooWithHun( cards, hunnum );
}

// bool CardBase::CheckHooWithHun( int cards[4][9], int hunnum )
// {
// 	// 统计牌的数量
// 	int cardsnum[4] = {0};
// 	for( int i = 0; i < 4 ; ++i )
// 	{
// 		for ( int k = 0; k < 9; ++k )
// 		{
// 			cardsnum[i] += cards[i][k];
// 		}
// 	}
// 	if ( (cardsnum[0] + cardsnum[1] + cardsnum[2] + cardsnum[3] + hunnum)%3 != 2 )
// 	{
// 		return false;
// 	}
//
// 	int needhun[4] = {0};
// 	for ( int i = 0; i < 4; ++i )
// 	{
// 		needhun[i] = ToBeShunKeNeedHun( cards[i], i >= 3 );
// 	}
// 	// 混的数量大于需要混的数量 肯定能胡
// 	if ( needhun[0] + needhun[1] + needhun[2] + needhun[3] < hunnum )
// 	{
// 		return true;
// 	}
// 	// 逐个测试蒋的位置
// 	for ( int i = 0; i < 4; ++i )
// 	{
// 		int hunnum2 = hunnum;	// 除去非蒋位置所需要的混 剩余混的数量
// 		for ( int k = 0; k < 4; ++k )
// 		{
// 			if ( k != i )
// 			{
// 				hunnum2 -= needhun[k];
// 			}
// 		}
// 		if ( hunnum2 >= 0 )
// 		{
// 			if ( needhun[i] == 0 && hunnum2 == 2 )	// 测试两个混当蒋
// 			{
// 				return true;
// 			}
//
// 			for( int k = 0; k < 9 ; ++k )	// 测试没有混当蒋
// 			{
// 				if ( cards[i][k] >= 2 )
// 				{
// 					cards[i][k] -= 2;
// 					if ( ToBeShunKeNeedHun( cards[i], i >= 3 ) <= hunnum2 )
// 					{
// 						cards[i][k] += 2;
// 						return true;
// 					}
// 					cards[i][k] += 2;
// 				}
// 				else if ( cards[i][k] >= 1 ) // 测试一个混当蒋
// 				{
// 					cards[i][k] -= 1;
// 					if ( ToBeShunKeNeedHun( cards[i], i >= 3 ) <= (hunnum2-1) )
// 					{
// 						cards[i][k] += 1;
// 						return true;
// 					}
// 					cards[i][k] += 1;
// 				}
// 			}
// 		}
// 	}
// 	return false;
// }

//这个是先测试顺子 再测试刻子
// int _NeedHunByShunKe(const int card[9], const bool bzipai)
// {
// 	int needHun = 0;
// 	int tcard[11] = {0};
// 	memcpy(tcard, card, sizeof(int)*9);
//
// 	for (int i = 0; i<9;)
// 	{
// 		if (tcard[i] >= 3)
// 		{
// 			tcard[i] -= 3;
// 		}
//
// 		if (!bzipai && tcard[i] > 0 && tcard[i + 1] > 0 && tcard[i + 2] > 0)
// 		{
// 			--tcard[i];
// 			--tcard[i + 1];
// 			--tcard[i + 2];
//
// 			continue;		// 可能还有 继续循环检测
// 		}
//
// 		if (!bzipai && tcard[i] > 0 && tcard[i + 2] > 0)
// 		{
// 			--tcard[i];
// 			//--tcard[i + 1];
// 			--tcard[i + 2];
// 			++needHun;
//
// 			continue;		// 可能还有 继续循环检测
// 		}
//
// 		++i;
// 	}
//
// 	if(!bzipai)
// 	{
// 		for(int i=0; i<9;)
// 		{
// 			if(tcard[i] > 0)
// 			{
// 				if(tcard[i+1] > 0)
// 				{
// 					--tcard[i];
// 					--tcard[i+1];
// 					++needHun;
// 					continue;
// 				}
// 				else if(tcard[i+2] > 0)
// 				{
// 					--tcard[i];
// 					--tcard[i+2];
// 					++needHun;
// 					continue;
// 				}
// 			}
//
// 			++i;
// 		}
// 	}
//
// 	for(int i=0; i<9; ++i)
// 	{
// 		if(tcard[i] == 1)
// 		{
// 			needHun += 2;
// 		}
// 		else if(tcard[i] == 2)
// 		{
// 			++needHun;
// 		}
// 	}
//
// 	return needHun;
// }

//这个是先测试顺子 再测试刻子
int __NeedHunByShunKe(const int card[9], const bool bzipai)
{
	int needHun = 0;
	int tcard[11] = { 0 };
	memcpy(tcard, card, sizeof(int) * 9);

	for (int i = 0; i<9; ++i)
	{
		if (tcard[i] >= 3)
		{
			tcard[i] -= 3;
			continue;
		}

		if (tcard[i] >= 2)
		{
			tcard[i] -= 2;
			++needHun;
			continue;
		}
	}

	if (!bzipai)
	{
		for (int i = 0; i<9;)
		{
			if (tcard[i] > 0 && tcard[i + 1] > 0 && tcard[i + 2] > 0)
			{
				--tcard[i];
				--tcard[i + 1];
				--tcard[i + 2];

				continue;
			}
			if (tcard[i] > 0)
			{
				if (tcard[i + 1] > 0)
				{
					--tcard[i];
					--tcard[i + 1];
					++needHun;
					continue;
				}
				else if (tcard[i + 2] > 0)
				{
					--tcard[i];
					--tcard[i + 2];
					++needHun;
					continue;
				}
			}

			++i;
		}
	}

	for (int i = 0; i<9; ++i)
	{
		if (tcard[i] == 1)
		{
			needHun += 2;
		}
		else if (tcard[i] == 2)
		{
			++needHun;
		}
	}

	return needHun;
}

//这个是先测试刻子 再测试顺子
// int _NeedHunByKeShun(const int card[9], const bool bzipai)
// {
// 	int needHun = 0;
// 	int tcard[11] = {0};
// 	memcpy(tcard, card, sizeof(int)*9);
//
// 	for (int i = 0; i<9;)
// 	{
// 		if (tcard[i] >= 3)
// 		{
// 			tcard[i] -= 3;
// 		}
//
// 		if (!bzipai && tcard[i] > 0 && tcard[i + 1] > 0 && tcard[i + 2] > 0)
// 		{
// 			--tcard[i];
// 			--tcard[i + 1];
// 			--tcard[i + 2];
//
// 			continue;		// 可能还有 继续循环检测
// 		}
//
// 		++i;
// 	}
//
// 	for(int i=0; i<9; ++i)
// 	{
// 		if(tcard[i] == 2)
// 		{
// 			tcard[i] -= 2;
// 			++needHun;
// 		}
// 	}
//
// 	if(!bzipai)
// 	{
// 		for(int i=0; i<9;)
// 		{
// 			if(tcard[i] > 0)
// 			{
// 				if(tcard[i+1] > 0)
// 				{
// 					--tcard[i];
// 					--tcard[i+1];
// 					++needHun;
// 					continue;
// 				}
// 				else if(tcard[i+2] > 0)
// 				{
// 					--tcard[i];
// 					--tcard[i+2];
// 					++needHun;
// 					continue;
// 				}
// 			}
//
// 			++i;
// 		}
// 	}
//
// 	for(int i=0; i<9; ++i)
// 	{
// 		if(tcard[i] > 0)
// 		{
// 			needHun += 2;
// 		}
// 	}
//
// 	return needHun;
// }

//这个是先测试刻子 再测试顺子
int __NeedHunByKeShun(const int card[9], const bool bzipai)
{
	int needHun = 0;
	int tcard[11] = { 0 };
	memcpy(tcard, card, sizeof(int) * 9);

	if (!bzipai)
	{
		for (int i = 0; i<9;)
		{
			if (tcard[i] > 0 && tcard[i + 1] > 0 && tcard[i + 2] > 0)
			{
				--tcard[i];
				--tcard[i + 1];
				--tcard[i + 2];

				continue;		// 可能还有 继续循环检测
			}

			if (tcard[i] > 0)
			{
				if (tcard[i + 1] > 0)
				{
					--tcard[i];
					--tcard[i + 1];
					++needHun;
					continue;
				}
				else if (tcard[i + 2] > 0)
				{
					--tcard[i];
					--tcard[i + 2];
					++needHun;
					continue;
				}
			}

			++i;
		}
	}

	for (int i = 0; i<9; ++i)
	{
		if (tcard[i] >= 3)
		{
			tcard[i] -= 3;
		}
		else if (tcard[i] == 2)
		{
			tcard[i] -= 2;
			++needHun;
		}
	}

	for (int i = 0; i<9; ++i)
	{
		if (tcard[i] > 0)
		{
			needHun += 2;
		}
	}

	return needHun;
}

// 测试此花色不带将最少需要几张赖子
int _NeedHunWithoutJiangShunKe(const int card[9], const bool bzipai)
{
	int needhun1 = __NeedHunByShunKe(card, bzipai);
	int needhun2 = __NeedHunByKeShun(card, bzipai);
	if(needhun1 < needhun2)
		return needhun1;
	else
		return needhun2;
}

// 测试此花色带将时需要几张癞子
int _NeedHunJiangShunKe(const int card[9], const bool bzipai)
{
	int tneed[9] = {0};
	int tcard[11] = {0};
	memcpy(tcard, card, sizeof(int)*9);

	for(int i=0; i<9; ++i)
	{
		if(tcard[i] > 0)
		{
			int cost = 0;
			if(tcard[i] == 1)
			{
				--tcard[i];
				cost = 1;

				tneed[i] += 1;
			}
			else
			{
				tcard[i] -= 2;
				cost = 2;
			}

// 			int needhun1 = _NeedHunByShunKe(tcard, bzipai);
// 			int needhun2 = _NeedHunByKeShun(tcard, bzipai);
//
// 			if(needhun1 < needhun2)
// 			{
// 				tneed[i]  += needhun1;
// 			}
// 			else
// 			{
// 				tneed[i]  += needhun2;
// 			}

			int needhun = _NeedHunWithoutJiangShunKe(tcard, bzipai);
			tneed[i] += needhun;

			tcard[i] += cost;
		}
		else
		{
			tneed[i] += 2;
// 			int needhun1 = _NeedHunByShunKe(tcard, bzipai);
// 			int needhun2 = _NeedHunByKeShun(tcard, bzipai);
//
// 			if(needhun1 < needhun2)
// 			{
// 				tneed[i]  += needhun1;
// 			}
// 			else
// 			{
// 				tneed[i]  += needhun2;
// 			}

			int needhun = _NeedHunWithoutJiangShunKe(tcard, bzipai);
			tneed[i] += needhun;
		}
	}

	int needHun = tneed[0];
	for(int i=0; i<9; ++i)
	{
		if(tneed[i] < needHun)
			needHun = tneed[i];
	}

	return needHun;
}

//cards[4][9]	所有手牌
//count			对应花色的牌数量
//color			作为将的花色
//hun			混牌数量
bool _CheckHooWithHun(int cards[4][9], int cardsnum[4], const int color, const int hun)
{
// 	int tempHunnum = hun;
// 	for ( int i = 0; i < 4; ++i )
// 	{
// 		tempHunnum = CheckShunKe(cards[i], cardsnum[i], color == i, i>=3, tempHunnum);
// 		if(tempHunnum < 0)
// 			return false;
// 	}
//
// 	return true;

	int needHun[4]={0};
	for(int i=0; i<4; ++i)
	{
		if(color == i)
		{
			needHun[i] = _NeedHunJiangShunKe(cards[i], i>=3);
		}
		else
		{
// 			int needhun1 = _NeedHunByShunKe(cards[i], i>=3);
// 			int needhun2 = _NeedHunByKeShun(cards[i], i>=3);
//
// 			if(needhun1 < needhun2)
// 			{
// 				needHun[i]  += needhun1;
// 			}
// 			else
// 			{
// 				needHun[i]  += needhun2;
// 			}

			int needhun = _NeedHunWithoutJiangShunKe(cards[i], i>=3);
			needHun[i] += needhun;
		}
	}

	int need = 0;
	for(int i=0; i<4; ++i)
	{
		need += needHun[i];
	}

	return need <= hun;
}

bool CardBase::CheckHooWithHun( int cards[4][9], int hunnum )
{
	if (2 <= hunnum)
	{
		// 癞子数量大于1个时，不能胡
		return false;
	}
	// 统计牌的数量
	int cardsnum[4] = {0};
	for( int i = 0; i < 4 ; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			cardsnum[i] += cards[i][k];
		}
	}
	if ( (cardsnum[0] + cardsnum[1] + cardsnum[2] + cardsnum[3] + hunnum)%3 != 2 )
	{
		return false;
	}

	for(int i=0; i<4; ++i)
	{
// 		if(_CheckHooWithHun(cards, cardsnum, i, hunnum))
// 			return true;

		if(gHuPaiCheck.CheckHooWithHun(cards, cardsnum, i, hunnum))
			return true;
	}

	return false;
}

bool CardBase::CheckHooWithHun258( int cards[4][9], int hunnum , CardValue& jiangCard )
{
	// 统计牌的数量
	jiangCard.m_color = 0;
	jiangCard.m_number = 0;

	int cardsnum[4] = {0};
	for( int i = 0; i < 4 ; ++i )
	{
		for ( int k = 0; k < 9; ++k )
		{
			cardsnum[i] += cards[i][k];
		}
	}
	if ( (cardsnum[0] + cardsnum[1] + cardsnum[2] + cardsnum[3] + hunnum)%3 != 2 )
	{
		return false;
	}

	int needhun[4] = {0};
	for ( int i = 0; i < 4; ++i )
	{
		needhun[i] = ToBeShunKeNeedHun( cards[i], i >= 3 );
	}
	// 混的数量大于需要混的数量 肯定能胡
	if ( needhun[0] + needhun[1] + needhun[2] + needhun[3] < hunnum )
	{
		jiangCard.m_color = 0;//任意将
		jiangCard.m_number = 0;
		return true;
	}
	// 逐个测试蒋的位置
	for ( int i = 0; i < 4; ++i )
	{
		int hunnum2 = hunnum;	// 除去非蒋位置所需要的混 剩余混的数量
		for ( int k = 0; k < 4; ++k )
		{
			if ( k != i )
			{
				hunnum2 -= needhun[k];
			}
		}
		if ( hunnum2 >= 0 )
		{
			if ( needhun[i] == 0 && hunnum2 == 2 )	// 测试两个混当蒋
			{
				jiangCard.m_color = 0;//任意将
				jiangCard.m_number = 0;
				return true;
			}

			for( int k = 0; k < 9 ; ++k )	// 测试没有混当蒋
			{
				if ( cards[i][k] >= 2 )
				{
					cards[i][k] -= 2;
					if ( ToBeShunKeNeedHun( cards[i], i >= 3 ) <= hunnum2 )
					{
						cards[i][k] += 2;
						jiangCard.m_color =  i + 1;
						jiangCard.m_number =  k + 1;
						return true;
					}
					cards[i][k] += 2;
				}
				else if ( cards[i][k] >= 1 ) // 测试一个混当蒋
				{
					cards[i][k] -= 1;
					if ( ToBeShunKeNeedHun( cards[i], i >= 3 ) <= (hunnum2-1) )
					{
						cards[i][k] += 1;
						jiangCard.m_color =  i + 1;
						jiangCard.m_number =  k + 1;
						return true;
					}
					cards[i][k] += 1;
				}
			}
		}
	}
	return false;
}

int CardBase::ToBeShunKeNeedHun( const int card[9], bool bzipai )
{
	int needhun[6] = {0};
	{
		int cardindex[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		needhun[0] = ToBeShunKeNeedHun( card, bzipai, cardindex );
	}
	{
		int cardindex[9] = { 1, 4, 7, 2, 5, 8, 3, 6, 9 };
		needhun[1] = ToBeShunKeNeedHun( card, bzipai, cardindex );
	}
	{
		int cardindex[9] = { 2, 5, 8, 1, 4, 7, 3, 6, 9 };
		needhun[2] = ToBeShunKeNeedHun( card, bzipai, cardindex );
	}
	{
		int cardindex[9] = { 2, 5, 8, 3, 6, 9, 1, 4, 7 };
		needhun[3] = ToBeShunKeNeedHun( card, bzipai, cardindex );
	}
	{
		int cardindex[9] = { 3, 6, 9, 2, 5, 8, 1, 4, 7 };
		needhun[4] = ToBeShunKeNeedHun( card, bzipai, cardindex );
	}
	{
		int cardindex[9] = { 3, 6, 9, 1, 4, 7, 2, 5, 8 };
		needhun[5] = ToBeShunKeNeedHun( card, bzipai, cardindex );
	}
	int minneedhun = needhun[0];
	for ( int i = 1; i < 6; ++i )
	{
		if ( minneedhun > needhun[i] )
		{
			minneedhun = needhun[i];
		}
	}
	return minneedhun;
}
int CardBase::ToBeShunKeNeedHun( const int card[9], bool bzipai, int cardindex[9] )
{
	int tcard[11] = {0};
	memcpy( &tcard[0], card, sizeof(int)*9 );

	for( int i = 0; i < 9; )
	{
		int index = cardindex[i] - 1;
		// 先组刻
		if( tcard[index] >= 3 )
		{
			tcard[index] -= 3;
		}

		// 组顺子
		if( !bzipai && tcard[index] > 0 && tcard[index+1] > 0 && tcard[index+2] > 0 )
		{
			--tcard[index];
			--tcard[index+1];
			--tcard[index+2];
			continue; // 防止还有一个一样的顺
		}
		++i;
	}

	int needhun = 0;
	// 剩余的牌组顺刻
	for( int i = 0; i < 9; ++i )
	{
		// 先组顺
		if ( !bzipai )
		{
			int num = tcard[i];
			for ( int k = 0; k < num; ++k )
			{
				if ( tcard[i+1] > 0 )
				{
					tcard[i]--;
					tcard[i+1]--;
					++needhun;
				}
				else if ( tcard[i+2] > 0 )
				{
					tcard[i]--;
					tcard[i+2]--;
					++needhun;
				}
			}
		}
		// 组刻
		if ( tcard[i] == 1 )
		{
			tcard[i]--;
			needhun += 2;
		}
		else if ( tcard[i] == 2 )
		{
			tcard[i] -= 2;
			++needhun;
		}
	}
	return needhun;
}
int	CardBase::ToBeShunKeNeedHunEx( const int card[9], bool bzipai, int cardindex[9] )
{
	int tcard[11] = {0};
	int dnxbCnt = 0;
	int zfbCnt	= 0;
	int subIdxDXNB[4];
	int subIdxZFB[3];
	int tmpIdx = -1;

	memset(subIdxDXNB, 0x00, sizeof(int)*4);
	memset(subIdxZFB,  0x00, sizeof(int)*3);
	memcpy( &tcard[0], card, sizeof(int)*9 );

	for( int i = 0; i < 9; )
	{
		int index = cardindex[i] - 1;
		
		//风夹将 将夹风
		if (bzipai) 
		{
			if (tcard[index] > 0) 
			{
				if (index < 4)	//东南西北
				{
					subIdxDXNB[dnxbCnt] = index;
					++dnxbCnt;
				}
				else			//中发白
				{
					subIdxZFB[zfbCnt] = index;
					++zfbCnt;
				}
				
				if (dnxbCnt > 2 && zfbCnt > 0)
				{
					for (int k = 0; k < 4; ++k)
					{
						if (subIdxDXNB[k] > 0)
						{
							tmpIdx = k;
							subIdxDXNB[k] = 0;
							break;
						}
					}
					--tcard[tmpIdx];
					--tcard[tmpIdx+1];
					--tcard[zfbCnt-1];
				}
				else if (zfbCnt > 2 && dnxbCnt > 0)
				{
					for (int n = 0; n < 3; ++n)
					{
						if (subIdxZFB[n] > 0)
						{
							tmpIdx = n;
							subIdxZFB[n] = 0;
							break;
						}
					}
					--tcard[tmpIdx];
					--tcard[tmpIdx+1];
					--tcard[dnxbCnt-1];
				}
			}
		}
		
		// 先组刻
		if( tcard[index] >= 3 )
		{
			tcard[index] -= 3;
		}

		// 组顺子
		if( !bzipai && tcard[index] > 0 && tcard[index+1] > 0 && tcard[index+2] > 0 )
		{
			--tcard[index];
			--tcard[index+1];
			--tcard[index+2];
			continue;	// 防止还有一个一样的顺
		}
		++i;
	}



	int needhun = 0;
	// 剩余的牌组顺刻
	for( int i = 0; i < 9; ++i )
	{
		// 先组顺
		if ( !bzipai )
		{
			int num = tcard[i];
			for ( int k = 0; k < num; ++k )
			{
				if ( tcard[i+1] > 0 )
				{
					tcard[i]--;
					tcard[i+1]--;
					++needhun;
				}
				else if ( tcard[i+2] > 0 )
				{
					tcard[i]--;
					tcard[i+2]--;
					++needhun;
				}
			}
		}
		else
		{
			int num = tcard[i];
			int tmpLoop = i < 4 ? 4 : 3;

			for (int k = 0; k < num; ++k )
			{
				for (int n = k+1; n < tmpLoop-k; ++n)
				{
					if (tcard[n] > 0)
					{
						tcard[i]--;
						tcard[n]--;
						++needhun;
					}
				}
			}

		}

		// 组刻
		if ( tcard[i] == 1 )
		{
			tcard[i]--;
			needhun += 2;
		}
		else if ( tcard[i] == 2 )
		{
			tcard[i] -= 2;
			++needhun;
		}
	}
	return needhun;
}

bool CardBase::matchHu_ShunKe(const CardVector& cards, Lint laiZiCount, std::vector<Card>* pMatch, std::vector<Card>* pJiangVec,bool jiang258)
{
	if( cards.empty() && (2 == laiZiCount || 5 == laiZiCount) )
	{
		//手里牌为空直接胡吧 因为手里只剩下赖子牌做将了
		if (pJiangVec && pMatch)
		{
			pJiangVec->push_back(Card(1, 2));
			pMatch->push_back(Card(1, 2));
		}

		return true;
	}

	bool bRet = false;

	Luchar szCards[52] = {0};	// 万00桶00条00东00南00西00北00中00发00白

	//重新组织数据
	for (CardVector::size_type index = 0; index < cards.size(); ++index)
	{
		if (cards[index]->m_color < 4)
		{
			szCards[(cards[index]->m_color - 1) * 11 + cards[index]->m_number - 1] += 1;
		}
		else
		{
			szCards[(cards[index]->m_color - 1) * 11 + (cards[index]->m_number - 1) * 3] += 1;
		}
	}

	//临时函数 使用癞子完全匹配牌形以形成顺子或刻子
	std::function<bool(Luchar*, Lint, Lint, std::set<Luchar*>*)> matchLaiZi = 
		[&](Luchar* cards, Lint count, Lint laiZiCount, std::set<Luchar*>* pTingMatch)->bool
	{
		bool result = false;

		//匹配刻子
		if (0 < count && 0 < cards[0] && 3 <= cards[0] + laiZiCount)
		{
			Luchar bkCardCount	= cards[0];
			laiZiCount -= std::max(3 - cards[0], 0);
			cards[0]	= std::max(cards[0] - 3, 0);

			if (matchLaiZi(cards, count, laiZiCount, pTingMatch))
			{
				result = true;
				if (NULL != pTingMatch && bkCardCount < 3)
				{
					pTingMatch->insert(cards);
				}
			}

			cards[0]	= bkCardCount;
			laiZiCount	+= std::max(3 - cards[0], 0);
		}

		//匹配顺子
		if (2 < count && (!result || NULL != pTingMatch))
		{
			//连续的三个位置上至少两个存在牌数据
			if (2 <= (Lint)(0 < cards[0]) + (Lint)(0 < cards[1]) + (Lint)(0 < cards[2]))
			{
				if ((0 < cards[0] && 0 < cards[1] && 0 < cards[2]) || 0 < laiZiCount)
				{
					Luchar bkFirstCount		= cards[0];
					Luchar bkSecondCount	= cards[1];
					Luchar bkThirdCount		= cards[2];
					laiZiCount -= cards[0] == 0 ? 1 : 0;
					laiZiCount -= cards[1] == 0 ? 1 : 0;
					laiZiCount -= cards[2] == 0 ? 1 : 0;
					cards[0] = std::max(cards[0] - 1, 0);
					cards[1] = std::max(cards[1] - 1, 0);
					cards[2] = std::max(cards[2] - 1, 0);

					if (matchLaiZi(cards, count, laiZiCount, pTingMatch))
					{
						result = true;
						if (NULL != pTingMatch && (bkFirstCount == 0 || bkSecondCount == 0 || bkThirdCount == 0))
						{
							pTingMatch->insert(bkFirstCount == 0 ? cards : (bkSecondCount == 0 ? (cards + 1) : (cards + 2)));
						}
					}

					cards[0] = bkFirstCount;
					cards[1] = bkSecondCount;
					cards[2] = bkThirdCount;
					laiZiCount += cards[0] == 0 ? 1 : 0;
					laiZiCount += cards[1] == 0 ? 1 : 0;
					laiZiCount += cards[2] == 0 ? 1 : 0;
				}
			}
		}//匹配顺子

		//后移位置
		if (0 < count && 0 == cards[0] && (!result || NULL != pTingMatch))
		{
			if (matchLaiZi(cards + 1, count - 1, laiZiCount, pTingMatch))
			{
				result = true;
			}
		}

		//满足需要
		if (count <= 0)
		{
			result = true;
		}

		return result;
	}; //临时函数 matchLaiZi()

	std::set<Luchar*> tingCards;
	std::set<Luchar*>* pTingMatch = NULL != pMatch ? &tingCards : NULL;

	std::set<Luchar> jiangCards;
	Lint cardCount = sizeof(szCards) / sizeof(szCards[0]);

	//检测是否满足胡牌
	if ((cards.size() + laiZiCount) % 3 == 2)	//考虑将牌
	{
		for (Lint index = 0; index < cardCount && (!bRet || NULL != pTingMatch); ++index)
		{
			//单胡将牌的情况
			if (0 < szCards[index] && 0 < laiZiCount)
			{
				//[bing] 使用2,5,8将牌				
				if (jiang258)
				{
					if (!(index == 1 || index == 4 || index == 7
						|| index == 12 || index == 15 || index == 18
						|| index == 23 || index == 26 || index == 29))
						continue;
				}
				

				szCards[index] -= 1;
				laiZiCount -= 1;
				if (matchLaiZi(szCards, cardCount, laiZiCount, pTingMatch))
				{
					bRet = true;
					if (NULL != pTingMatch)
					{
						pTingMatch->insert(szCards + index);
					}

					if (pJiangVec && (index == 1 || index == 4 || index == 7
						|| index == 12 || index == 15 || index == 18
						|| index == 23 || index == 26 || index == 29))
					{
						//258将牌列出来
						jiangCards.insert((Luchar)index);
					}
				} 
				szCards[index] += 1;
				laiZiCount += 1;
			}
			//已经存在将牌的情况
			if ((!bRet || NULL != pMatch) && 2 <= szCards[index])
			{
				//[bing] 使用2,5,8将牌
				
				if (jiang258)
				{
					if (!(index == 1 || index == 4 || index == 7
						|| index == 12 || index == 15 || index == 18
						|| index == 23 || index == 26 || index == 29))
						continue;
				}

				szCards[index] -= 2;

				if (matchLaiZi(szCards, cardCount, laiZiCount, pTingMatch))
				{
					bRet = true;

					if (pJiangVec && (index == 1 || index == 4 || index == 7
						|| index == 12 || index == 15 || index == 18
						|| index == 23 || index == 26 || index == 29))
					{
						//258将牌列出来
						jiangCards.insert((Luchar)index);
					}
				}

				//bRet = matchLaiZi(szCards, cardCount, laiZiCount, pTingMatch) || bRet;
				szCards[index] += 2;
			}
		}
	}
	else if ((cards.size() + laiZiCount) % 3 == 0)	//不考虑将牌
	{
		bRet = matchLaiZi(szCards, cardCount, laiZiCount, pTingMatch);
	}


	//将数组转换为常规牌数据
	if (bRet && NULL != pMatch)
	{
		Lint cardColor	= 0;
		Lint cardNumber	= 0;
		//Lint jianPaiStart = (CARD_COLOR_FENG_JIAN - 1) * 11;
		Lint jianPaiStart = 3 * 11;
		std::set<Luchar*>::iterator iterTing = tingCards.begin();
		for (; iterTing != tingCards.end(); ++iterTing)
		{
			Lint cardIndex = *iterTing - szCards;
			if (jianPaiStart <= cardIndex)
			{
				//pMatch->push_back(Card(4, (cardIndex - jianPaiStart) / 3 + 1));
				pMatch->push_back(Card(((cardIndex - jianPaiStart) / 3 + 1),4));
			}
			else
			{
				cardColor = cardIndex / 11 + 1;
				cardNumber = (cardIndex - (cardColor - 1) * 11) + 1;
				if (1 <= cardNumber && cardNumber <= 9)
				{
					//pMatch->push_back(Card(cardColor, cardNumber));
					pMatch->push_back(Card(cardNumber,cardColor));
				}
			}
		}
	}

	if (bRet && NULL != pJiangVec)
	{
		Lint cardColor = 0;
		Lint cardNumber = 0;
		std::set<Luchar>::iterator jiangIT = jiangCards.begin();
		for (; jiangIT != jiangCards.end(); ++jiangIT)
		{
			Lint cardIndex = *jiangIT;

			cardColor = cardIndex / 11 + 1;
			cardNumber = (cardIndex - (cardColor - 1) * 11) + 1;
			if (1 <= cardNumber && cardNumber <= 9)
			{
				//pJiangVec->push_back(Card(cardColor, cardNumber));
				pJiangVec->push_back(Card(cardNumber,cardColor));
			}
		}
	}

	return bRet;
}
bool CardBase::CheckTing(CardVector& handcard, std::map<Lint, std::vector<Lint>>& tingCardMap,FilterTingFunc filterFunc,bool jiang258)
{
	tingCardMap.clear();

	Lint ntmpCardIdx = 0;

	//[bing] 可能需要排序 mark
	SortCard(handcard);

	for(Lsize nHandCardIdx = 0; nHandCardIdx < handcard.size(); ++nHandCardIdx)
	{
		if( ntmpCardIdx == handcard[nHandCardIdx]->GetNCIndex() )
			continue;
		ntmpCardIdx = handcard[nHandCardIdx]->GetNCIndex();

		CardVector tmpHandCard(handcard);
		CardVector::iterator IT = tmpHandCard.begin();
		while( IT != tmpHandCard.end() )
		{
			if( *IT == tmpHandCard[nHandCardIdx] )
			{
				tmpHandCard.erase(IT);
				break;
			}

			++IT;
		}

		Lint nDelCardCN = handcard[nHandCardIdx]->GetNCIndex();

		std::vector<Card> TingCardVec;
		if(true == matchHu_ShunKe(tmpHandCard, 1, &TingCardVec,NULL,jiang258))
		{
			//如果能听平胡
			std::vector<Lint>& CardNCVec = tingCardMap[nDelCardCN];
			for(Lsize i = 0; i < TingCardVec.size(); ++i)
			{
				CardNCVec.push_back(TingCardVec[i].GetNCIndex());
			}
		}
	}
	filterFunc(tingCardMap);
#ifdef _DEBUG
	//输出用
	std::map<Lint, std::vector<Lint>>::iterator IT = tingCardMap.begin();
	while(IT != tingCardMap.end())
	{
		std::stringstream ss;
		std::vector<Lint>& tmpTingCardVec = IT->second;
		for(Lsize i = 0; i < tmpTingCardVec.size(); ++i)
		{
			ss << tmpTingCardVec[i] << ",";
		}

		LLOG_ERROR("ting: delcard %d - %s", IT->first, move(ss.str()).c_str());

		++IT;
	}
#endif

	return (tingCardMap.empty() != true);
}

bool CardBase::CheckQueYiMen( CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& agangcard,CardVector& mgangcard,Card* outCard )
{
	CardVector tmpCards;
	tmpCards.insert(tmpCards.begin(), handcard.begin(), handcard.end());
	tmpCards.insert(tmpCards.begin(), eatCard.begin(), eatCard.end());
	tmpCards.insert(tmpCards.begin(), pengCard.begin(), pengCard.end());
	tmpCards.insert(tmpCards.begin(), agangcard.begin(), agangcard.end());
	tmpCards.insert(tmpCards.begin(), mgangcard.begin(), mgangcard.end());
	if (outCard)
	{
		tmpCards.push_back(outCard);
	}
	

	bool bRet = false;
	if (!tmpCards.empty())
	{		
		std::set<Lint> setColors;	
		for(size_t i = 0; i < tmpCards.size(); ++i)
		{
			const Lint color = tmpCards.at(i)->m_color;			
			setColors.insert(color);
		}

		// 缺一种花色。
		if ((setColors.count(CARD_COLOR_ZI) == 0) || (setColors.count(CARD_COLOR_WAN) == 0) ||
			(setColors.count(CARD_COLOR_TUO) == 0) || (setColors.count(CARD_COLOR_SUO) == 0))
		{
			bRet = true;
		}
	}

	return bRet;
}
