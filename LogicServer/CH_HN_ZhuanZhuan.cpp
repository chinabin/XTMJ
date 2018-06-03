#include "CH_HN_ZhuanZhuan.h"

CH_HeNan_ZhuanZhuan* CH_HeNan_ZhuanZhuan::mInstance = NULL;

bool CH_HeNan_ZhuanZhuan::CardSortFun(Card* c1,Card* c2)
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


bool CH_HeNan_ZhuanZhuan::Init()
{
	//万
	Lint index = 0;
	for (Lint k = 0; k < 9; ++k)  //9张牌
	{
		for(Lint i = 0; i < 4 ;++i)		//循环加四次
		{

			wan_base_card[index].m_color = 1;
			wan_base_card[index].m_number = k + 1;
			++index;
		}
	}

	//饼
	index = 0;
	for (Lint k = 0; k < 9; ++k)  //9张牌
	{
		for(Lint i = 0; i < 4 ;++i)		//循环加四次
		{

			bing_base_card[index].m_color = 2;
			bing_base_card[index].m_number = k + 1;
			++index;
		}
	}

	//条
	index = 0;
	for (Lint k = 0; k < 9; ++k)  //9张牌
	{
		for(Lint i = 0; i < 4 ;++i)		//循环加四次
		{

			tiao_base_card[index].m_color = 3;
			tiao_base_card[index].m_number = k + 1;
			++index;
		}
	}

	index = 0;
	//风+字
	for (Lint j = 0; j < 7; ++j)	
	{
		for(Lint i = 0; i < 4 ;++i)		//循环加四次
		{
			wind_card[index].m_color = 4;
			wind_card[index].m_number = j+1;
			++index;
		}	
	}

	index = 0;
	for(Lint i = 0; i < 4 ;++i)		//循环加四次
	{
		Lai_zi_card[index].m_color = 4;
		Lai_zi_card[index].m_number = 5;
		++index;
	}	

	for(Lint i = 0 ; i < BASE_CARD_COUNT; ++i)
	{
		m_3_feng.push_back(&wan_base_card[i]);
		m_3_feng.push_back(&bing_base_card[i]);
		m_3_feng.push_back(&tiao_base_card[i]);

		m_3_laizi.push_back(&wan_base_card[i]);
		m_3_laizi.push_back(&bing_base_card[i]);
		m_3_laizi.push_back(&tiao_base_card[i]);
		//m_wan_feng.push_back(&wan_base_card[i]);

		m_3.push_back(&wan_base_card[i]);
		m_3.push_back(&bing_base_card[i]);
		m_3.push_back(&tiao_base_card[i]);
	}

	for(Lint i = 0 ; i < WIND_CARD_COUNT; ++i)
	{
		m_3_feng.push_back(&wind_card[i]);
		//m_wan_feng.push_back(&wind_card[i]);
	}
	for(Lint i = 0 ; i < LAI_ZI_COUNT; ++i)
	{
		m_3_laizi.push_back(&Lai_zi_card[i]);
	}
	SortCard(m_3_feng);
	SortCard(m_3_laizi);
	//SortCard(m_wan_feng);
	SortCard(m_3);
	return true;
}

bool CH_HeNan_ZhuanZhuan::Final()
{
	return true;
}

void CH_HeNan_ZhuanZhuan::SwapCardBySpecial(CardVector& cvIn, const Card specialCard[CARD_COUNT])
{
	Lint nCardCount = cvIn.size();
	for (Lint i = 0; i < nCardCount; ++i)
	{
		if (specialCard[i].m_color == 0 || specialCard[i].m_number == 0)
		{
			break;
		}
		for (Lint j = i + 1; j < nCardCount; ++j)
		{
			if (cvIn[j]->m_color == specialCard[i].m_color && cvIn[j]->m_number == specialCard[i].m_number)
			{
				Card* tmp = cvIn[j];
				cvIn[j] = cvIn[i];
				cvIn[i] = tmp;	
				break;
			}
		}
	}
}

void CH_HeNan_ZhuanZhuan::DealCard2(CardVector hand_card[],Lint user_count,CardVector& rest_card,const Card specialCard[CARD_COUNT],Lint game_type, PlayType& playtype)
{

	CardVector mCards;
	Lint choose=0;
	
	/*if (playtype.checkPlayType(PT_TWOPLAYER))
	{
	mCards = m_wan_feng;
	}
	else */
	{
		if (playtype.checkPlayType(PT_HAVEWIND))
		{
			mCards = m_3_feng;
			choose = 1;
		}
		else if (playtype.checkPlayType(PT_NEEDWIND))
		{
			mCards = m_3_laizi;
			choose = 2;
		}
		else {
			mCards = m_3;
			choose = 3;
		}
	}
	

	CardVector cardtmp;
	Lint nSize = mCards.size();
	while (nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		cardtmp.push_back(mCards[seed1]);
		mCards.erase(mCards.begin() + seed1, mCards.begin() + seed1 + 1);
		nSize = mCards.size();
	}

	SwapCardBySpecial(cardtmp, specialCard);

	for(int x=0;x<user_count;++x)
	{
		hand_card[x].insert(hand_card[x].end(),cardtmp.begin()+13*x,cardtmp.begin()+13*(x+1));
		SortCard(hand_card[x]);
	}
	rest_card.insert(rest_card.end(),cardtmp.begin()+13*user_count,cardtmp.end());
	std::reverse(rest_card.begin(),rest_card.end());		//逆序桌上牌
}

void CH_HeNan_ZhuanZhuan::DealCard(CardVector hand_card[],Lint user_count,CardVector& rest_card, Lint game_type, PlayType& playtype)
{
	CardVector mCards;
	Lint choose=0;
	
	//if (playtype.checkPlayType(PT_TWOPLAYER))
	//{
	//	mCards = m_wan_feng;
	//}
	//else
	{
		if (playtype.checkPlayType(PT_HAVEWIND))
		{
			mCards = m_3_feng;
			choose = 1;
		}
		else if (playtype.checkPlayType(PT_NEEDWIND))
		{
			mCards = m_3_laizi;
			choose = 2;
		}
		else {
			mCards = m_3;
			choose = 3;
		}
	}

	CardVector cardtmp;
	Lint nSize = mCards.size();

	//设置随机因子
	LTime timeNow;
	srand(timeNow.Secs());
	while (nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		cardtmp.push_back(mCards[seed1]);
		mCards[seed1] = mCards[nSize - 1];
		nSize --;
	}

	switch(choose)
	{
	//case 0:
	//	m_wan_feng=cardtmp;
	//	break;
	case 1:
		m_3_feng=cardtmp;
		break;
	case 2:
		m_3_laizi=cardtmp;
		break;
	case 3:
		m_3 = cardtmp;
		break;
	}

	for(int x=0;x<user_count;++x)
	{
		hand_card[x].insert(hand_card[x].end(),cardtmp.begin()+13*x,cardtmp.begin()+13*(x+1));
		SortCard(hand_card[x]);
	}
	rest_card.insert(rest_card.end(),cardtmp.begin()+13*user_count,cardtmp.end());
	LLOG_DEBUG("1cardtmp:%d,handcard:%d,rest_card:%d\n", cardtmp.size(), hand_card[0].size(), rest_card.size());
}

void CH_HeNan_ZhuanZhuan::SortCard(CardVector& vec)
{
	std::sort(vec.begin(),vec.end(),CardSortFun);
}

void CH_HeNan_ZhuanZhuan::EraseCard(CardVector& src,CardVector& des)
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

void CH_HeNan_ZhuanZhuan::EraseCard(CardVector& src,Card* pCard)
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
bool CH_HeNan_ZhuanZhuan::EraseCard(CardVector& src, Card* pCard, Lint n)
{
	Lint cnt = 0;
	CardVector::iterator it2 = src.begin();

	for(; it2 != src.end();)
	{
		if(cnt >= n)
			break;

		if(IsSame(pCard,*it2))
		{
			++cnt;
			it2 = src.erase(it2);
		}
		else
		{
			++it2;
		}
	}

	return true;
}

void CH_HeNan_ZhuanZhuan::EarseSameCard(CardVector& src)
{
	if(src.empty())
	{
		return;
	}

	Lint color = src.front()->m_color;
	Lint number = src.front()->m_number;
	CardVector::iterator it = src.begin()+1;
	for(; it != src.end();)
	{
		if(color == (*it)->m_color && number == (*it)->m_number)
		{
			it = src.erase(it);
		}
		else
		{
			color = (*it)->m_color;
			number= (*it)->m_number;
			++it;
		}
	}
}

ThinkVec CH_HeNan_ZhuanZhuan::CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo)
{
	ThinkVec result;
	ThinkUnit unit;
	if(gameInfo.b_canHu&&CheckCanHu(handcard,pengCard,agangCard,mgangCard,eatCard, outCard,gameInfo,unit.m_hu, false))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		unit.m_card.push_back(outCard);
		
		if (gameInfo.m_GameType == MJHeNanZhuanZhuanFour)
		{
			unit.count_score();
			if (unit.m_score>gameInfo.i_canHuScore)
				result.push_back(unit);
		}
		else
		{
			result.push_back(unit);
		}
	}
	
	if (!gameInfo.b_onlyHu)
	{
		if(CheckCanGang(handcard,outCard,gameInfo))
		{
			unit.Clear();
			unit.m_type = THINK_OPERATOR_MBU;
			unit.m_card.push_back(outCard);
			result.push_back(unit);
			gameInfo.m_gangcard.push_back(outCard);
		}

		if(!gameInfo.b_guoGang &&CheckCanPeng(handcard,outCard,gameInfo))
		{
			unit.Clear();   
			unit.m_type = THINK_OPERATOR_PENG;
			unit.m_card.push_back(outCard);
			result.push_back(unit);
		}

		CardVector vec;
		if(!gameInfo.b_guoGang &&gameInfo.b_canEat && CheckCanChi(handcard,outCard,vec,gameInfo))
		{
			for(Lsize i = 0 ; i < vec.size(); i += 3)
			{
				LLOG_DEBUG("canEat vec: %d,%d,,%d,%d,,%d,%d", vec[0]->m_color, vec[0]->m_number, vec[1]->m_color, vec[1]->m_number, vec[2]->m_color, vec[2]->m_number);
				unit.Clear();
				unit.m_type = THINK_OPERATOR_CHI;
				unit.m_card.push_back(vec[i]);
				unit.m_card.push_back(vec[i+1]);
				unit.m_card.push_back(vec[i+2]);
				result.push_back(unit);
			}
		}
	}

	return result;
}

ThinkVec CH_HeNan_ZhuanZhuan::CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,
	CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, 
	CardVector& outCards,OperateState& gameInfo, LMsgS2CUserGangThinkCard& send_message)
{
	ThinkVec		result;
	ThinkUnit		unit;
	Lint			hu_count = 0;
	gang_think		the_gank_think[4];
	
	for (Luint i = 0; i < outCards.size(); ++i)
	{
		if(!outCards[i])
		{
			return result;
		}

		the_gank_think[i].m_gang_card.m_color = outCards[i]->m_color;
		the_gank_think[i].m_gang_card.m_number = outCards[i]->m_number;

		unit.Clear();
		if(gameInfo.b_canHu && !gCM_HeNan_ZhuanZhuan->HaveTheCard(gameInfo.m_louhuCard, outCards[i]) && CheckCanHu(handcard,pengCard,agangCard,mgangCard,eatCard, outCards[i],gameInfo,unit.m_hu,false))
		{
			if (hu_count> 0)
			{
				result[0].m_card.push_back(outCards[i]);
				for (Luint i = 0; i < unit.m_hu.size(); ++i)
				{
					result[0].m_hu.push_back(unit.m_hu[i]);
				}
			}
			else
			{
				++hu_count;
				unit.m_type = THINK_OPERATOR_BOMB;
				unit.m_card.push_back(outCards[i]);
				result.push_back(unit);
			}
			ThinkData think;
			think.m_type = THINK_OPERATOR_BOMB;
			think.m_card.push_back(the_gank_think[i].m_gang_card);
			the_gank_think[i].m_think.push_back(think);
		}
	}

	if (!result.empty())
	{
		result[0].count_score();
		if (result[0].m_score > gameInfo.i_canHuScore)
		{

		}
		else {
			result.clear();
			for (Luint x = 0; x < send_message.m_card.size(); ++x)
			{
				send_message.m_card[x].m_think.clear();
			}
		}
	}


	for (Luint xx = 0; xx < outCards.size(); ++xx)
	{
		if (!gameInfo.b_onlyHu)
		{
			if(CheckCanGang(handcard,outCards[xx], gameInfo))
			{	
				unit.Clear();
				unit.m_type = THINK_OPERATOR_MBU;
				unit.m_card.push_back(outCards[xx]);
				result.push_back(unit);

				ThinkData think;
				think.m_type = THINK_OPERATOR_MBU;
				think.m_card.push_back(the_gank_think[xx].m_gang_card);
				the_gank_think[xx].m_think.push_back(think);
			}
			if(!gameInfo.b_guoGang && CheckCanPeng(handcard,outCards[xx], gameInfo))
			{
				unit.Clear();
				unit.m_type = THINK_OPERATOR_PENG;
				unit.m_card.push_back(outCards[xx]);
				result.push_back(unit);

				ThinkData think;
				think.m_type = THINK_OPERATOR_PENG;
				think.m_card.push_back(the_gank_think[xx].m_gang_card);
				the_gank_think[xx].m_think.push_back(think);
			}

			CardVector vec;
			if(!gameInfo.b_guoGang && gameInfo.b_canEat && CheckCanChi(handcard,outCards[xx],vec, gameInfo))
			{
				for(Lsize i = 0 ; i+2 < vec.size(); i += 3)
				{
					unit.Clear();
					unit.m_type = THINK_OPERATOR_CHI;
					unit.m_card.push_back(vec[i]);
					unit.m_card.push_back(vec[i+1]);
					unit.m_card.push_back(vec[i+2]);
					result.push_back(unit);

					ThinkData think;
					think.m_type = THINK_OPERATOR_CHI;
					CardValue card;
					if(vec[i])
					{
						card.m_color = vec[i]->m_color;
						card.m_number = vec[i]->m_number;
						think.m_card.push_back(card);
					}
					if(vec[i+1])
					{
						card.m_color = vec[i+1]->m_color;
						card.m_number = vec[i+1]->m_number;
						think.m_card.push_back(card);
					}
					if(vec[i+2])
					{	
						card.m_color = vec[i+2]->m_color;
						card.m_number = vec[i+2]->m_number;
						think.m_card.push_back(card);
					}
					the_gank_think[xx].m_think.push_back(think);
				}
			}
		}
		send_message.m_card.push_back(the_gank_think[xx]);
	}

	return result;
}

ThinkVec CH_HeNan_ZhuanZhuan::CheckGetCardOperator(CardVector& handcard, CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,Card* getCard,OperateState& gameInfo)
{
	ThinkVec result;
	ThinkUnit unit;
	if(gameInfo.b_canHu&&CheckCanHu(handcard, pengCard,agangCard,mgangCard,eatCard,getCard,gameInfo,unit.m_hu,true))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		if (getCard)
		{
			unit.m_card.push_back(getCard);
		}
		result.push_back(unit);
	}

	if(!gameInfo.b_onlyHu)
	{
		CardVector tmp = handcard;
		if (getCard)
		{
			tmp.push_back(getCard);
		}
		SortCard(tmp);

		CardVector vec;
		if(CheckAnGang(tmp, vec, gameInfo))
		{
			for(Lsize i = 0 ; i < vec.size(); ++i)
			{
				unit.Clear();
				unit.m_type = THINK_OPERATOR_ABU;
				unit.m_card.push_back(vec[i]);
				result.push_back(unit);
			}
		}

		vec.clear();
		if(CheckMGang(tmp,pengCard,vec,gameInfo))
		{
			for(Lsize i = 0 ; i < vec.size(); ++i)
			{
				bool next=false;
				//不允许先碰后杠
				if (MJHeNanZhuanZhuanFour == gameInfo.m_GameType)
				{
					for(Luint x=0;x<gameInfo.m_gangcard.size();++x)
					{
						if(gameInfo.m_gangcard[x]->m_color==vec[i]->m_color&&gameInfo.m_gangcard[x]->m_number==vec[i]->m_number)
						{
							next=true;
							break;
						}
					}
				}

				if(!next)
				{
					unit.Clear();
					unit.m_type = THINK_OPERATOR_MBU;
					unit.m_card.push_back(vec[i]);
					result.push_back(unit);
				}
			} 
		}
	}

	return result;
}

ThinkVec CH_HeNan_ZhuanZhuan::CheckGetCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,CardVector& getCards, OperateState& gameInfo)
{
	ThinkVec result;
	ThinkUnit unit;
	for (Lsize i = 0; i < getCards.size(); ++i)
	{
		unit.Clear();
		if(gameInfo.b_canHu && CheckCanHu(handcard, pengCard,agangCard,mgangCard,eatCard,getCards[i],gameInfo,unit.m_hu,true))
		{
			if (result.size() > 0)
			{
				result[0].m_card.push_back(getCards[i]);
				for (Lsize i = 0; i < unit.m_hu.size(); ++i)
				{
					result[0].m_hu.push_back(unit.m_hu[i]);
				}
			}
			else
			{
				unit.m_type = THINK_OPERATOR_BOMB;
				unit.m_card.push_back(getCards[i]);
				result.push_back(unit);
			}
		}

		if(!gameInfo.b_onlyHu)
		{
			CardVector tmp = handcard;
			tmp.push_back(getCards[i]);
			SortCard(tmp);

			CardVector vec;
			if(CheckAnGang(tmp,vec,gameInfo))
			{
				for(Lsize i = 0 ; i < vec.size(); ++i)
				{
					unit.Clear();
					unit.m_type = THINK_OPERATOR_ABU;
					unit.m_card.push_back(vec[i]);
					result.push_back(unit);
				}
			}

			vec.clear();
			if(CheckMGang(tmp,pengCard,vec,gameInfo))
			{
				for(Lsize i = 0 ; i < vec.size(); ++i)
				{
					unit.Clear();
					unit.m_type = THINK_OPERATOR_MBU;
					unit.m_card.push_back(vec[i]);
					result.push_back(unit);
				} 
			}
		}
	}
	return result;
}

bool CH_HeNan_ZhuanZhuan::CheckStartHu(CardVector& handcard,std::vector<StartHuCard>& vec)
{
	vec.clear();

	//缺一色
	std::set<Lint> color_num;
	for(Lsize i = 0; i < handcard.size(); ++i)
		color_num.insert(handcard[i]->m_color);

	if(color_num.size() < 3)
	{
		StartHuCard info;
		info.m_type = START_HU_QUEYISE;
		vec.push_back(info);
	}

	//大四喜
	CardVector angang;
	OperateState x;
	CheckAnGang(handcard,angang, x);
	if(angang.size())
	{
		StartHuCard info;
		info.m_type = START_HU_DASIXI;
		for(Lsize i = 0 ; i < angang.size(); i += 1)
		{
			info.m_card.push_back(angang[i]);
		}
		vec.push_back(info);
	}

	//板板胡
	bool banban = true;
	for(Lsize i = 0 ; i < handcard.size(); ++i)
	{
		if(handcard[i]->m_number == 2 || handcard[i]->m_number == 5 ||
			handcard[i]->m_number == 8)
		{
			banban = false;
			break;
		}
	}

	if(banban)
	{
		StartHuCard info;
		info.m_type = START_HU_BANBANHU;
		vec.push_back(info);
	}

	//六六顺
	CardVector oneh, twoh, fourh, threeh;
	GetSpecialOneTwoThreeFour(handcard, oneh, twoh, threeh, fourh);
	if(threeh.size()/3 + fourh.size()/4 >= 2)
	{
		StartHuCard info;
		info.m_type = START_HU_LIULIUSHUN;
		for(Lsize i = 0 ; i < threeh.size(); i += 3)
		{
			info.m_card.push_back(threeh[i]);
		}
		for(Lsize i = 0 ; i < fourh.size(); i += 4)
		{
			info.m_card.push_back(fourh[i]);
		}
		vec.push_back(info);
	}

	return true;
}

bool CH_HeNan_ZhuanZhuan::CheckCanHu(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec, bool isGetCard)
{ 
	Lint isSpecial = 0;
	if(xiaohu_CheckHu(handcard,outCard,false,!isGetCard,gameInfo.m_pt_laizi))
	{
		vec.push_back(HU_XIAOHU);
	}
	else if (gameInfo.m_QiXiaoDui && CheckXiaoqidui(handcard, isSpecial, outCard, isGetCard,gameInfo))
	{
		vec.push_back(HU_XIAOHU);
	}
	else if(isGetCard&&gameInfo.m_pt_laizi&&gameInfo.m_first_turn &&CheckQishouLaizi(handcard))
	{
		vec.push_back(HU_XIAOHU);
	}
	else
	{
		;
	}
	
	return vec.size()!=0;
}


bool CH_HeNan_ZhuanZhuan::CheckCanGang(CardVector& handcard,Card* outCard, OperateState& gameInfo)
{
	if (outCard)
	{
		if (gameInfo.m_pt_laizi&&outCard->m_color == 4 && outCard->m_number == 5)
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	CardVector tmpVec;
	for(Lsize i = 0 ;i < handcard.size(); ++i)
	{
		Card* pcard = handcard[i];
		if(pcard->m_color == outCard->m_color && 
			pcard->m_number == outCard->m_number)
		{
			tmpVec.push_back(pcard);
		}
	}

	if(tmpVec.size()>0)
	{
		if(gameInfo.m_pt_laizi&&tmpVec[0]->m_color==4&&tmpVec[0]->m_number==5)
		{
			return false;
		}
	}

	return tmpVec.size() >= 3;
}

bool CH_HeNan_ZhuanZhuan::CheckCanPeng(CardVector& handcard,Card* outCard, OperateState& gameInfo)
{
	CardVector tmpVec;

	if(outCard)
	{
		if (gameInfo.m_pt_laizi&&outCard->m_color == 4 && outCard->m_number == 5)
		{
			return false;
		}
	}
	else 
	{
		return false;
	}

	for(Lsize i = 0 ;i < handcard.size(); ++i)
	{
		Card* pcard = handcard[i];
		if(pcard->m_color == outCard->m_color && 
			pcard->m_number == outCard->m_number)
		{
			tmpVec.push_back(pcard);
		}
	}

	return (tmpVec.size() >= 2);
}

bool CH_HeNan_ZhuanZhuan::CheckXiaoqidui2(CardVector& handcard, Lint& special, Card* outCard, bool isGetCard, OperateState& gameInfo)
{
	CardVector all = handcard;
	if (outCard)
	{
		all.push_back(outCard);
		gCM_HeNan_ZhuanZhuan->SortCard(all);
	}
	if (all.size() != 14)
	{
		return false;
	}
	CardVector one, tow, three, four;
	GetSpecialOneTwoThreeFour(all, one, tow, three, four);
	if (one.empty() && three.empty())
	{
		if (four.size() == 0)
		{
			special = 1;
		}
		if (four.size() == 4)
		{
			special = 2;
		}
		else if (four.size() == 8)
		{
			special = 3;
		}
		else if (four.size() == 12)
		{
			special = 4;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool CH_HeNan_ZhuanZhuan::CheckXiaoqidui(CardVector& handcard,Lint& special,Card* outCard, bool isGetCard, OperateState& gameInfo)
{
	special = 0;
	CardVector all = handcard;
	Lsize nLaiZiCount = 0;
	if (isGetCard)
	{
		if(outCard)
		{
			all.push_back(outCard);
		}
		for (Lsize i = 0; i < all.size(); )
		{
			if (gameInfo.m_pt_laizi&&all[i]->m_color == 4&&all[i]->m_number == 5)
			{
				gCM_HeNan_ZhuanZhuan->EraseCard(all,all[i]);
				++nLaiZiCount;
			}
			else
			{
				++i;
			}
		}
	}
	else
	{
		for (Lsize i = 0; i < all.size(); )
		{
			if (gameInfo.m_pt_laizi&&all[i]->m_color == 4&&all[i]->m_number == 5)
			{
				gCM_HeNan_ZhuanZhuan->EraseCard(all,all[i]);
				++nLaiZiCount;
			}
			else
			{
				++i;
			}
		}
		if(outCard)
		{
			all.push_back(outCard);
		}
	}
	gCM_HeNan_ZhuanZhuan->SortCard(all);
	if (nLaiZiCount > 0)
	{
		if (nLaiZiCount + all.size() != 14)
		{
			return false;
		}
		CardVector one,tow,three,four;
		GetSpecialOneTwoThreeFour(all,one,tow,three,four);
		if (one.size() + three.size()/3 <= nLaiZiCount)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (all.size() != 14)
		{
			return false;
		}
		CardVector one,tow,three,four;
		GetSpecialOneTwoThreeFour(all,one,tow,three,four);
		if(one.empty() && three.empty())
		{
			if(four.size()==4)
			{
				special = 1;
			}
			else if(four.size()>4)
			{
				special = 2;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
}

//检测清一色
bool CH_HeNan_ZhuanZhuan::CheckQingyise(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, std::vector<Lint>& vec)
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	if(tmp.empty())
	{
		return false;
	}

	Lint color = tmp.front()->m_color;
	for(Lsize i = 1 ; i < tmp.size(); ++i)
	{
		if(tmp[i]->m_color != color)
			return false;
	}

	if (xiaohu_CheckHu(handcard,outCard,false,false)|| vec.size() > 0)
	{
		return true;
	}
	return false;
}

//检测乱将胡
bool CH_HeNan_ZhuanZhuan::CheckLuanjianghu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard)
{
	CardVector tmp = handcard;
	if(outCard)
	{
		tmp.push_back(outCard);
	}
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(tmp.empty())
	{
		return false;
	}

	for(Lsize i = 0 ; i < tmp.size(); ++i)
	{
		if(!(tmp[i]->m_number == 2 || tmp[i]->m_number == 5 ||
			tmp[i]->m_number == 8))
		{
			return false;
		}
	}
	return true;
}

//检测碰碰胡
bool CH_HeNan_ZhuanZhuan::CheckPengpenghu(CardVector& handcard,CardVector& eatCard,Card* outCard, Card *WangBa1, Card *WangBa2)
{
	if(eatCard.size())
	{
		return false;
	}
	Lint WangBaNum = 0;
	CardVector check,one,tow,three,four;
	check = handcard;
	if(outCard)
	{
		check.push_back(outCard);
		SortCard(check);
	}
	if (WangBa1 != NULL || WangBa2 != NULL)
	{
		for (std::vector<Card *>::iterator it = check.begin(); it != check.end(); )
		{
			if (WangBa1 && gCM_HeNan_ZhuanZhuan->IsSame(*it, WangBa1))
			{
				it = check.erase(it);
				++WangBaNum;
			}
			else if (WangBa2 && gCM_HeNan_ZhuanZhuan->IsSame(*it, WangBa2))
			{
				it = check.erase(it);
				++WangBaNum;
			}
			else
			{
				++it;
			}
		}
	}

	GetSpecialOneTwoThreeFour(check,one,tow,three,four);

	if (WangBa1 == NULL && WangBa2 == NULL)
	{
		if (one.size() || four.size())
			return false;

		if (tow.size() != 2)
			return false;

		return true;
	}
	else
	{
		if (four.size())
		{
			Lint restWangBa = WangBaNum - 2 * one.size() - 2*four.size() / 4 + 1;
			if (restWangBa >= 0 && (restWangBa) % 3 == 0)
			{
				return true;
			}
			return false;
		}
		Lint restWangBa = WangBaNum - 2 * one.size() - tow.size() / 2 + 1;
		if (restWangBa >= 0 && (restWangBa)%3 == 0)
		{
			return true;
		}
		return false;
	}
}

//检测全球人2
bool CH_HeNan_ZhuanZhuan::CheckQuanqiuren2(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard)
{
	CardVector check;
	check = handcard;
	if (outCard)
	{
		check.push_back(outCard);
	}
	if (check.size() != 2)
	{
		return false;
	}
	return (check[0]->m_color == check[1]->m_color && check[0]->m_number == check[1]->m_number);
}

//检测全球人
bool CH_HeNan_ZhuanZhuan::CheckQuanqiuren(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard)
{
	CardVector check;
	check = handcard;
	if(outCard)
	{
		check.push_back(outCard);
	}
	if (check.size() != 2)
	{
		return false;
	}
	return ((CheckLuanjianghu(handcard,eatCard,pengCard,gangcard,mgangcard,outCard) || 
		(check[0]->m_color == check[1]->m_color && check[0]->m_number == check[1]->m_number)));
}

//杠上开花
bool CH_HeNan_ZhuanZhuan::CheckGangshangkaihua(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo)
{
	//判断是不是自已杠的
	if (gameInfo.m_MePos == gameInfo.m_playerPos && gameInfo.m_thinkGang)
	{
		return true;
	}

	return false;
}

//杠上炮
bool CH_HeNan_ZhuanZhuan::CheckGangshangpao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo)
{

	//杠上炮，判断是不是别人的杠
	if (gameInfo.m_MePos != gameInfo.m_playerPos && gameInfo.m_thinkGang)
	{
		return true;
	}

	return false;
}
//海底捞
bool CH_HeNan_ZhuanZhuan::CheckHaiDiLao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo)
{
	if (gameInfo.m_MePos == gameInfo.m_playerPos && gameInfo.m_deskState == DESK_PLAY_END_CARD)
	{
		return true;
	}
	return false;
}
//海底炮
bool CH_HeNan_ZhuanZhuan::CheckHaiDiPao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo)
{
	if (gameInfo.m_MePos != gameInfo.m_playerPos && gameInfo.m_deskState == DESK_PLAY_END_CARD)
	{
		return true;
	}
	return false;
}

bool CH_HeNan_ZhuanZhuan::CheckXiangTongSe(CardVector & handcard, Card * outCard)
{
	CardVector tmp = handcard;
	if (outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	Lint color = tmp.front()->m_color;
	for (Lsize i = 1; i < tmp.size(); ++i)
	{
		if (tmp[i]->m_color != color)
			return false;
	}
	return true;
}

bool CH_HeNan_ZhuanZhuan::BaseQingyise(CardVector & handcard, CardVector & eatCard, CardVector & pengCard, CardVector & gangcard, CardVector & mgangcard, Card * outCard, Card* Wangba1, Card* Wangba2)
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(), eatCard.begin(), eatCard.end());
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
	tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());
	if (outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	if (tmp.empty())
	{
		return false;
	}

	Lint color = 0;
	for (Lsize i = 0; i < tmp.size(); ++i)
	{
		if ((Wangba1 && Wangba1->m_color != 0 && Wangba1->m_color == tmp[i]->m_color && Wangba1->m_number == tmp[i]->m_number) ||
			(Wangba2 && Wangba2->m_color != 0 && Wangba2->m_color == tmp[i]->m_color && Wangba2->m_number == tmp[i]->m_number))
		{
			continue;
		}
		else if (0 == color)
		{
			color = tmp[i]->m_color;
		}
		else if (tmp[i]->m_color != color)
		{
			return false;
		}
	}
	return true;
}

bool CH_HeNan_ZhuanZhuan::CheckQingyise(CardVector & handcard, CardVector & eatCard, CardVector & pengCard, CardVector & gangcard, CardVector & mgangcard, Card * outCard)
{
	if (BaseQingyise(handcard,eatCard,pengCard,gangcard,mgangcard,outCard, NULL, NULL) && xiaohu_CheckHu(handcard, outCard, false,true))
	{
		return true;
	}
	return false;

}

bool CH_HeNan_ZhuanZhuan::CheckQingyiseDapengdui(CardVector & handcard, CardVector & eatCard, CardVector & pengCard, CardVector & gangcard, CardVector & mgangcard, Card * outCard)
{
	CardVector tmp = handcard;

	//peng  return false;
	if (!eatCard.empty())
	{
		return false;
	}

	if (tmp.size() > 1)
	{
		return false;
	}

	if (tmp[0]->m_color == outCard->m_color && tmp[0]->m_number == outCard->m_number)
	{
		return true;
	}
	else
		return false;
}

bool CH_HeNan_ZhuanZhuan::CheckMenQing(CardVector & handcard, CardVector & eatCard, CardVector & pengCard, CardVector & gangcard, CardVector & mgangcard, Card * outCard)
{
	//if(eatCard.size() || pengCard.size() ||gangcard.size() || mgangcard.size())
	if (eatCard.size() || pengCard.size() || mgangcard.size())
	{
		return false;
	}
	if (xiaohu_CheckHu(handcard, outCard, false,true))
	{
		return true;
	}
	return false;
}

bool CH_HeNan_ZhuanZhuan::CheckFengYiSe(CardVector & handcard, CardVector & eatCard, CardVector & pengCard, CardVector & gangcard, CardVector & mgangcard, Card * outCard)
{
	if (CheckPengpenghu(handcard, eatCard, outCard))
	{
		CardVector tmp = handcard;
		tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
		tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
		tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());
		for (Lsize i = 0; i < tmp.size(); ++i)
		{
			if (tmp[i]->m_color != 4)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

bool CH_HeNan_ZhuanZhuan::CheckShiSanYiao(CardVector & handcard, Card * outCard)
{
	CardVector tmp = handcard;
	if (outCard)
	{
		tmp.push_back(outCard);
	}
	if (tmp.size() > 13)
	{
		Card* mo = tmp.back();
		tmp.pop_back();
		if (!outCard)
		{
			outCard = mo;
		}
	}
	SortCard(tmp);
	EarseSameCard(tmp);
	if (tmp.size() < 13)
	{
		return false;		//手牌不够十三张，肯定不是十三
	}
	
	if (outCard && outCard->m_color < 4 && (outCard->m_number == 1 || outCard->m_number == 9))
	{
		return true;
	}
	else if (outCard && outCard->m_color == 4 && outCard->m_number > 0 && outCard->m_number <= 7)
	{
		return true;
	}
	return false;
}

bool CH_HeNan_ZhuanZhuan::CheckShiSanLan(CardVector & handcard, CardVector & eatCard, CardVector & pengCard, CardVector & gangcard, CardVector & mgangcard, Card * outCard, bool & isqifeng)
{
	// 有吃碰杠肯定不是烂牌
	CardVector* cards[4] = { &eatCard, &pengCard, &gangcard, &mgangcard };
	for (int i = 0; i < 4; ++i)
	{
		if (cards[i]->size() > 0)
		{
			return false;
		}
	}
	int cardnum[4][9] = { 0 };
	for (CardVector::iterator it = handcard.begin(); it != handcard.end(); ++it)
	{
		if (!(*it))
		{
			return false;
		}
		if ((*it)->m_color <= 0 || (*it)->m_color > 4)
		{
			return false;
		}
		if ((*it)->m_number <= 0 || (*it)->m_number > 9)
		{
			return false;
		}
		cardnum[(*it)->m_color - 1][(*it)->m_number - 1]++;
		// 只能有一张
		if (cardnum[(*it)->m_color - 1][(*it)->m_number - 1] > 1)
		{
			return false;
		}
	}
	if (outCard)
	{
		if (outCard->m_number <= 0 || outCard->m_number > 9)
		{
			return false;
		}
		cardnum[outCard->m_color - 1][outCard->m_number - 1]++;
		// 只能有一张
		if (cardnum[outCard->m_color - 1][outCard->m_number - 1] > 1)
		{
			return false;
		}
	}
	// 检查数字牌的间隔是否都隔开两种
	for (int i = 0; i < 3; ++i)
	{
		Lint last_number = -3;
		for (int k = 0; k < 9; ++k)
		{
			if (cardnum[i][k] > 0)
			{
				if (k - last_number <= 2)
				{
					return false;
				}
				last_number = k;
			}
		}
	}
	// 检查是否东南西北是否全部存在
	isqifeng = true;
	for (int k = 0; k < 7; ++k)
	{
		if (cardnum[3][k] == 0)
		{
			isqifeng = false;
			break;
		}
	}
	return true;
}

bool CH_HeNan_ZhuanZhuan::CheckCanChi(CardVector& handcard, Card* outCard, CardVector& vec, OperateState& gameInfo)
{
	LLOG_DEBUG("CheckCanChi");
	bool ret = false;
	if(handcard.size() < 2)
		return false;
	if(!outCard)
	{
		return false;
	}

	if (outCard->m_color == 4)
	{
		return false;
	}

	CardVector one = handcard;
	SortCard(one);
	EarseSameCard(one);
	EraseCard(one, outCard, 4);

	for (Luint i = 0; i + 1 < one.size(); ++i)
	{
		CardVector tmp;
		tmp.push_back(one[i]);
		tmp.push_back(one[i + 1]);
		tmp.push_back(outCard);
		SortCard(tmp);
		if (IsContinue(tmp))
		{
			vec.push_back(one[i]);
			vec.push_back(one[i + 1]);
			vec.push_back(outCard);
			ret = true;
		}
	}
	return ret;
}
bool CH_HeNan_ZhuanZhuan::CheckAnGang(CardVector& handcard,  CardVector& vec, OperateState& gameInfo)
{
	if(handcard.size() < 4)
		return false;

	for(Lsize i = 0 ;i+3 < handcard.size(); ++i)
	{
		Card* pcarda = handcard[i];
		Card* pcardb = handcard[i+3];
		if(gameInfo.m_pt_laizi)
		{
			if(pcarda->m_color == pcardb->m_color && 
				pcarda->m_number == pcardb->m_number&& !(pcarda->m_color == 4&&pcarda->m_number ==5))
			{
				vec.push_back(pcarda);
			}
		}
		else{
			if(pcarda->m_color == pcardb->m_color && pcarda->m_number == pcardb->m_number)
			{
				vec.push_back(pcarda);
			}
		}
	}
	return true;
}

bool CH_HeNan_ZhuanZhuan::CheckMGang(CardVector& handcard,CardVector& pengCard,  CardVector& vec, OperateState& gameInfo)
{
	for(Lsize i = 0 ; i < pengCard.size(); i+=3)
	{
		Card* pcard = pengCard[i];
		for(Lsize j = 0 ; j < handcard.size(); ++j)
		{
			if(gameInfo.m_pt_laizi)
			{
				if(pcard->m_number == handcard[j]->m_number &&
					pcard->m_color == handcard[j]->m_color&& !(pcard->m_color == 4&&pcard->m_number ==5))
				{
					vec.push_back(pcard);
					break;
				}
			}
			else{
				if(pcard->m_number == handcard[j]->m_number &&
					pcard->m_color == handcard[j]->m_color)
				{
					vec.push_back(pcard);
					break;
				}
			}
		}
	}
	return true;
}


bool CH_HeNan_ZhuanZhuan::IsContinue(CardVector& result)
{
	if(result.empty())
	{
		return false;
	}

	Lint number = result.front()->m_number;
	Lint color = result.front()->m_color;
	for(Lsize i = 1; i < result.size(); ++i)
	{
		if(result[i]->m_number != number+i || color != result[i]->m_color)
			return false;
	}

	return true;
}

bool CH_HeNan_ZhuanZhuan::IsSame(CardVector& result)
{
	if(result.empty())
	{
		return false;
	}

	Lint number = result.front()->m_number;
	Lint color = result.front()->m_color;
	for(Lsize i = 1; i < result.size(); ++i)
	{
		if(result[i]->m_number != number || color != result[i]->m_color)
			return false;
	}

	return true;
}
bool CH_HeNan_ZhuanZhuan::IsSame(Card* c1,Card* c2)
{
	return c1->m_color==c2->m_color&&c1->m_number==c2->m_number;
}

bool CH_HeNan_ZhuanZhuan::IsNineOne(Card* c)
{
	return c->m_number == 0 || c->m_number == 9;
}

bool CH_HeNan_ZhuanZhuan::HaveTheCard(CardVector & result,Card* thecard)
{
	if (!thecard)
	{
		return false;
	}

	for (auto x = result.begin(); x != result.end(); ++x)
	{
		if ((*x)->m_color == thecard->m_color && (*x)->m_number == thecard->m_number)
		{
			return true;
		}
	}
	return false;
}

Card * CH_HeNan_ZhuanZhuan::GetCard(Lint m_color, Lint m_number)
{
	for (auto x = m_3_feng.begin(); x != m_3_feng.end(); ++x)
	{
		if ((*x)->m_color == m_color && (*x)->m_number == m_number)
		{
			return *x;
		}
	}
	return nullptr;
}

bool CH_HeNan_ZhuanZhuan::GetSpecialOneTwoThreeFour(CardVector& src,CardVector& one,CardVector& two,CardVector& three,CardVector& four)
{
	if(src.empty())
	{
		return false;
	}

	Lsize i = 0 ;
	Lint number = 0,color = 0 ,length = 0 ;
	for(; i < src.size(); ++i)
	{
		Card* pCard = src[i];
		if(number == pCard->m_number && color == pCard->m_color)
		{
			length += 1;
		}
		else
		{
			if(length == 1)
			{
				one.push_back(src[i-1]);
			}
			else if(length == 2)
			{
				two.push_back(src[i-2]);
				two.push_back(src[i-1]);
			}
			else if(length == 3)
			{
				three.push_back(src[i-3]);
				three.push_back(src[i-2]);
				three.push_back(src[i-1]);
			}
			else if(length == 4)
			{
				four.push_back(src[i-4]);
				four.push_back(src[i-3]);
				four.push_back(src[i-2]);
				four.push_back(src[i-1]);
			}
			length = 1;
			number = pCard->m_number;
			color = pCard->m_color;
		}
	}

	if(length == 1)
	{
		one.push_back(src[i-1]);
	}
	else if(length == 2)
	{
		two.push_back(src[i-2]);
		two.push_back(src[i-1]);
	}
	else if(length == 3)
	{
		three.push_back(src[i-3]);
		three.push_back(src[i-2]);
		three.push_back(src[i-1]);
	}
	else if(length == 4)
	{
		four.push_back(src[i-4]);
		four.push_back(src[i-3]);
		four.push_back(src[i-2]);
		four.push_back(src[i-1]);
	}

	return true;
}



bool CH_HeNan_ZhuanZhuan::CheckQishouLaizi(CardVector& handcard)
{
	int lanzi_num=0;
	for(Luint x=0;x<handcard.size();++x)
	{
		if(handcard[x]!=NULL)
		{
			if(handcard[x]->m_color==4&&handcard[x]->m_number==5)
				++lanzi_num;
		}
	}
	if(lanzi_num==4)
	{
		return true;
	}
	else{
		return false;
	}
}

bool CH_HeNan_ZhuanZhuan::xiaohu_CheckHu(CardVector handcard,Card* curCard,bool needJiang,bool is_out_card,bool hava_laizi)
{
	if(curCard)
	{
		handcard.push_back(curCard);
	}

	std::vector<Lint> card_vector(40,0);
	Lint Lai_num=0;

	//第一步获取癞子,分数量统计
	for(Luint x=0;x<handcard.size();++x)
	{
		if(hava_laizi&&handcard[x]->m_color==4&&handcard[x]->m_number==5)
		{
			++Lai_num;
		}
		else
			card_vector[(handcard[x]->m_color-1)*10+(handcard[x]->m_number)*1]++;
	}

	if(curCard)
	{
		//玩家出了红中癞子
		if(hava_laizi&&is_out_card&&curCard->m_color==4&&curCard->m_number==5)
		{
			if(Lai_num>1)
			{
				std::vector<Lint> tmp_cards(card_vector);
				if (explorer_zheng_function(tmp_cards, 0, Lai_num - 2))
				{
					return true;
				}
				else {//癞子组成刻子
					if (Lai_num >= 3)
					{
						Lai_num -= 3;
					}
					else {
						return false;
					}
				}

			}
			else{
				return false;
			}
		}
	}
	int juge_lai_jiang = true;
	for(Luint x=0;x<card_vector.size();++x)
	{
		int index = x%10;
		if(x%10==0)
		{
			continue;
		}
		else if(x>37)
		{
			break;
		}

		if(needJiang&&(index!=2&&index!=5&&index!=8))
		{
			if(x==30)
				return false;
			continue;
		}

		if(card_vector[x]>1)
		{
			std::vector<Lint> tmp_cards(card_vector);
			tmp_cards[x]-=2;
			if(explorer_zheng_function(tmp_cards,0,Lai_num))
			{
				return true;
			}
		}
		else if(card_vector[x]==1&&Lai_num>0)
		{
			std::vector<Lint> tmp_cards(card_vector);
			tmp_cards[x]-=1;
			if(explorer_zheng_function(tmp_cards,0,Lai_num-1))
			{
				return true;
			}
		}
		else{
			if(Lai_num>1&&juge_lai_jiang)
			{
				juge_lai_jiang = false;
				if(explorer_zheng_function(card_vector,0,Lai_num-2))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool CH_HeNan_ZhuanZhuan::explorer_zheng_function(std::vector<Lint> cards,Lint index,Lint Lai_num, Lint *wangbashunzi, Card *Wangba)
{
	if(index<39)
	{
		if(index%10==0)
		{
			++index;
		}

		if(cards[index]>0)
		{
			std::vector<Lint> tmp_cards(cards);
			Lint ke_dif = Lai_num-judge_ke(tmp_cards,index);
			if(ke_dif>=0)
			{
				if (explorer_zheng_function(tmp_cards, index, ke_dif, wangbashunzi, Wangba))
				{
					return true;
				}
			}

			if(index%10<8&&index<30)
			{
				std::vector<Lint> tmp_cards2(cards);
				Lint shun_dif = Lai_num-judge_shun(tmp_cards2,index);
				if(shun_dif>=0)
				{
					if (explorer_zheng_function(tmp_cards2, index, shun_dif, wangbashunzi, Wangba))
					{
						if (shun_dif == Lai_num && wangbashunzi && Wangba && \
							Wangba->m_color == ((index/10)+1) && Wangba->m_number>=index%10 && Wangba->m_number <= ((index % 10)+3))
						{
							(*wangbashunzi)++;
						}
						return true;
					}
				}
			}
			return false;
		}
		else{
			if(Lai_num>0&&index%10<8&&index<30)
			{
				std::vector<Lint> tmp_cards(cards);
				Lint shun_dif = Lai_num-judge_shun(tmp_cards,index);
				if(shun_dif>=0)
				{
					if(explorer_zheng_function(tmp_cards,index,shun_dif, wangbashunzi, Wangba))
					{
						if (shun_dif == Lai_num && wangbashunzi && Wangba && \
							Wangba->m_color == ((index / 10) + 1) && Wangba->m_number >= index % 10 && Wangba->m_number <= ((index % 10) + 3))
						{
							(*wangbashunzi)++;
						}
						return true;
					}
				}
			}
			++index;
			if(explorer_zheng_function(cards,index,Lai_num, wangbashunzi, Wangba))
			{
				return true;
			}
			return false;
		}
	}
	else{
		return true;
	}
}

Lint CH_HeNan_ZhuanZhuan::judge_ke(std::vector<Lint>& targe_vector,Lint index)
{
	Lint Lai_num = 0;
	switch(targe_vector[index])
	{
	case 4:
	case 3:
		{
			targe_vector[index]-=3;
			Lai_num=0;
			break;
		}
	case 2:
		{
			targe_vector[index]-=2;
			Lai_num=1;
			break;
		}
	case 1:
		{
			targe_vector[index]-=1;
			Lai_num=2;
			break;
		}
	case 0:
		{
			Lai_num=3;
			break;
		}
	default:
		return 100;
		break;
	}
	return Lai_num;
}

Lint CH_HeNan_ZhuanZhuan::judge_shun(std::vector<Lint>& targe_vector,Lint index)
{
	Lint Lai_num =3;
	if(targe_vector[index]>0)
	{
		targe_vector[index]--;
		Lai_num--;
	}
	if(targe_vector[index+1]>0)
	{
		targe_vector[index+1]--;
		Lai_num--;
	}
	if(targe_vector[index+2]>0)
	{
		targe_vector[index+2]--;
		Lai_num--;
	}
	return Lai_num;
}

bool CH_HeNan_ZhuanZhuan::have_play_type(Lint playtype,std::vector<Lint> playtypes)
{
	for(auto x=playtypes.begin();x!=playtypes.end();++x)
	{
		if(*x == playtype)
			return true;
	}
	return false;
}

bool CH_HeNan_ZhuanZhuan::CheckTing(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,OperateState& gameInfo)
{
	//杠后不可能胡小七对 先删掉
	if(CheckLuanjianghu(handcard,eatCard,pengCard,agangCard,mgangCard,NULL))
	{
		return true;
	}
	if(handcard.size()==1)//听全球人
	{
		return true;
	}
	if(xiaohu_CheckTing(handcard,true))
	{
		return true;
	}	

	if(xiaohu_CheckTing(handcard,false))
	{
		//清一色
		bool qingyise=true;
		CardVector tmp(handcard);
		tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
		tmp.insert(tmp.end(),agangCard.begin(),agangCard.end());
		tmp.insert(tmp.end(),mgangCard.begin(),mgangCard.end());
		tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
		Lint color = 0;
		for(Luint x=0;x<tmp.size();++x)
		{
			if(x==0)
			{
				color=tmp[0]->m_color;
			}
			else{
				if(color!=tmp[x]->m_color)
				{
					qingyise=false;
					break;
				}
			}
		}
		if(qingyise)
		{
			return true;
		}

		//碰碰胡
		bool pengpenghu=false;

		tmp.clear();
		tmp.insert(tmp.end(),handcard.begin(),handcard.end());
		CardVector one,two,three,four;
		SortCard(tmp);
		GetSpecialOneTwoThreeFour(tmp,one,two,three,four);

		if(one.size()==1&&two.empty()&&four.empty())
		{
			pengpenghu=true;
		}
		else if(one.empty()&&two.size()==4&&four.empty())
		{
			pengpenghu=true;
		}

		if(!eatCard.empty())
		{
			pengpenghu=false;
		}

		if(pengpenghu)
		{
			return true;
		}
	}
	return false;
}

//目前只针对长沙
bool CH_HeNan_ZhuanZhuan::xiaohu_CheckTing(CardVector handcard,bool needJiang)
{
	std::vector<Lint> card_vector(40,0);
	Lint Lai_num=1;

	//第一步获取癞子,分数量统计
	for(Luint x=0;x<handcard.size();++x)
	{
		card_vector[(handcard[x]->m_color-1)*10+(handcard[x]->m_number)*1]++;
	}

	int juge_lai_jiang = true;
	for(Luint x=0;x<card_vector.size();++x)
	{
		int index = x%10;
		if(x%10==0)
		{
			continue;
		}
		else if(x>37)
		{
			break;
		}

		if(needJiang&&(index!=2&&index!=5&&index!=8))
		{
			if(x==30)
				return false;
			continue;
		}

		if(card_vector[x]>1)
		{
			std::vector<Lint> tmp_cards(card_vector);
			tmp_cards[x]-=2;
			if(explorer_zheng_function(tmp_cards,0,Lai_num))
			{
				return true;
			}
		}
		else if(card_vector[x]==1&&Lai_num>0)
		{
			std::vector<Lint> tmp_cards(card_vector);
			tmp_cards[x]-=1;
			if(explorer_zheng_function(tmp_cards,0,Lai_num-1))
			{
				return true;
			}
		}
		else{
			if(Lai_num>1&&juge_lai_jiang)
			{
				juge_lai_jiang = false;
				if(explorer_zheng_function(card_vector,0,Lai_num-2))
				{
					return true;
				}
			}
		}
	}
	return false;
}
