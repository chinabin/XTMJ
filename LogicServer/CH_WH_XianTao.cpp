#include "CH_WH_XianTao.h"
#include "GH_WuHan_XianTao.h"

CH_WuHan_XianTao* CH_WuHan_XianTao::mInstance = NULL;

CH_WuHan_XianTao::CH_WuHan_XianTao()
{
}

ThinkVec CH_WuHan_XianTao::CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo)
{
	ThinkVec result;
	ThinkUnit unit;
	gameInfo.bChaTing = false;
	LLOG_DEBUG("b_canhu=%d, outCard=%d:%d.", gameInfo.b_canHu, outCard->m_color, outCard->m_number);
	std::stringstream str;
	for (Lint j = 0; j< handcard.size(); ++j)
	{
		str << handcard[j]->GetNCIndex() << " ";
	}
	str << ".";
	LLOG_DEBUG("handcard=%s.", str.str().c_str());

	if(gameInfo.b_canHu && CheckCanHu(handcard,pengCard,agangCard,mgangCard,eatCard, outCard,gameInfo,unit.m_hu))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		unit.m_card.push_back(outCard);
		result.push_back(unit);
	}

	if (!gameInfo.b_onlyHu)
	{
		if(gameInfo.m_pt_laizi && gameInfo.m_hCard.size() > 0 && outCard)
		{
			//if(*outCard == gameInfo.m_hCard[0])//混牌打出不能吃碰杠
			if (outCard->m_color == gameInfo.m_hCard[0].m_color && outCard->m_number == gameInfo.m_hCard[0].m_number)//混牌打出不能吃碰杠
				return result;
		}

		if(!gameInfo.bNoCard && CheckCanGang(handcard,outCard))
		{
			unit.Clear();
			unit.m_type = THINK_OPERATOR_MGANG;
			unit.m_card.push_back(outCard);
			result.push_back(unit);
		}

		if(CheckCanPeng(handcard,outCard))
		{
			/*if (outCard->m_color == gameInfo.m_hCard[0].m_color)
			{
				// 判断是否是小朝天
				Lint cardNumber = gameInfo.m_hCard[0].m_number - 1;
				if (cardNumber <= 0)
				{
					cardNumber = 9;
				}

				if (outCard->m_number == cardNumber)
				{
					unit.Clear();
					unit.m_type = THINK_OPERATOR_MGANG;
					unit.m_card.push_back(outCard);
					result.push_back(unit);
				}
			}
			else*/
			{
				unit.Clear();
				unit.m_type = THINK_OPERATOR_PENG;
				unit.m_card.push_back(outCard);
				result.push_back(unit);
			}
		}
	}

	return result;
}

ThinkVec CH_WuHan_XianTao::CheckGetCardOperator(CardVector& handcard, CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,Card* getCard,OperateState& gameInfo)
{
	ThinkVec result;
	ThinkUnit unit;
	gameInfo.bChaTing = false;
	if(gameInfo.b_canHu && CheckCanHu(handcard, pengCard,agangCard,mgangCard,eatCard,getCard,gameInfo,unit.m_hu))
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
		if(!gameInfo.bNoCard && CheckAnGang(tmp,vec))
		{
			for(Lsize i = 0 ; i < vec.size(); ++i)
			{
				if(gameInfo.m_pt_laizi && gameInfo.m_hCard.size() > 0 && vec[i]->m_color == gameInfo.m_hCard[0].m_color && vec[i]->m_number == gameInfo.m_hCard[0].m_number)//鬼牌不能杠碰
					continue;

				LLOG_DEBUG("userPlayCard=%d,card=%d,%d.", gameInfo.b_userPlayCard, vec[i]->m_color, vec[i]->m_number);
				
				// 如果用户没有打过牌，则直接可以暗杠
				if (!gameInfo.b_userPlayCard)
				{
					LLOG_DEBUG("angang");
					unit.Clear();
					unit.m_type = THINK_OPERATOR_AGANG;
					unit.m_card.push_back(vec[i]);
					result.push_back(unit);
					continue;
				}

				// 如果已经打过牌，则暗杠的牌只能是当前摸得牌，此时不打就不能再杠了
				if (getCard && (vec[i]->m_color == getCard->m_color && vec[i]->m_number == getCard->m_number))
				{
					LLOG_DEBUG("angang");
					unit.Clear();
					unit.m_type = THINK_OPERATOR_AGANG;
					unit.m_card.push_back(vec[i]);
					result.push_back(unit);
				}
			}
		}

		vec.clear();
		if (!gameInfo.bNoCard)
		{
			Lint color = gameInfo.m_hCard[0].m_color;
			Lint number = gameInfo.m_hCard[0].m_number - 1;
			if (number == 0)
			{
				number = 9;
			}
			if (getCard)
			{
				LLOG_DEBUG("check dachaotian, ghostFlop=%d,%d;getcard:%d,%d.", color, number, getCard->m_color, getCard->m_number);
			}
			
			if (getCard && getCard->m_color == color && getCard->m_number == number)
			{
				if (CheckCanPeng(handcard, getCard))
				{
					LLOG_DEBUG("dachaotian");
					unit.Clear();
					unit.m_type = THINK_OPERATOR_AGANG;
					unit.m_card.push_back(getCard);
					result.push_back(unit);
				}
			}
			else if (!gameInfo.b_userPlayCard)
			{
				if (GetCardNumber(handcard, color, number) >= 3)
				{
					LLOG_DEBUG("dachaotian");
					unit.Clear();
					unit.m_type = THINK_OPERATOR_AGANG;

					unit.m_card.push_back(GetCardPointer(handcard, color, number));
					result.push_back(unit);
				}
			}
		}

		vec.clear();
		if(!gameInfo.bNoCard && CheckMGang(tmp,pengCard,vec))
		{
			for(Lsize i = 0 ; i < vec.size(); ++i)
			{
				if(gameInfo.m_pt_laizi && gameInfo.m_hCard.size() > 0 && vec[i]->m_color == gameInfo.m_hCard[0].m_color && vec[i]->m_number == gameInfo.m_hCard[0].m_number)//鬼牌不能杠碰
					continue;

				// 明杠只能是当前拿的牌，错过后就不能明杠了
				if (getCard && vec.size() > 0 && (vec[i]->m_color == getCard->m_color && vec[i]->m_number == getCard->m_number))
				{
					bool isBuGang = false;
					for (Card* card : pengCard)
					{
						if (card->m_color == vec[i]->m_color && card->m_number == vec[i]->m_number)
						{
							isBuGang = true;
							break;
						}
					}

					unit.Clear();
					if (isBuGang)
					{
						unit.m_type = THINK_OPERATOR_MBU;
					}
					else
					{
						unit.m_type = THINK_OPERATOR_MGANG;
					}
					
					unit.m_card.push_back(vec[i]);
					result.push_back(unit);
				}
				
			}

			if(getCard && vec.size() > 0 && (vec.at(0)->m_color != getCard->m_color || vec.at(0)->m_number != getCard->m_number))
			{
				gameInfo.m_pGscard = vec.at(0);
			}
		}
	}
	return result;
}

bool CH_WuHan_XianTao::CheckCanHu(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec)
{ 
	vec.clear();

	// 是否带混的玩法
	if (gameInfo.m_pt_laizi)
	{
		CheckCanHuHun( handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec );
	}
	else
	{
		CheckCanHuNormal( handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec );
	}
	/*LLOG_DEBUG("check can hu size: %d.", vec.size());
	for (int ret : vec)
	{
		LLOG_DEBUG("ret=%d.", ret);
	}
	LLOG_DEBUG("======");*/
	return vec.size()!=0;
}

Lint CH_WuHan_XianTao::CheckTing(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, OperateState& gameInfo, std::vector<TingCard>& winCards, const Card* ignore)
{
	Lint ret = -1;
	int retfan = -1;

	// 花猪未听
	//if (IsHuaZhu(handcard, gameInfo.m_dingQue))
	//	return -1;

	for (Lint i = 0; i < 4; ++i)
	{
		if ( (i+1) == gameInfo.m_dingQue )
		{
			continue;
		}
		for (Lint j = 0; j < 9; ++j)
		{
			Card card;
			card.m_color = i+1;
			card.m_number = j+1;
			if ( ignore && card.m_color == ignore->m_color && card.m_number == ignore->m_number )
			{
				continue;
			}
			//if ( gameInfo.m_playtype.wzGametypeYaoJiDai() )
			//{
			//	// 带混玩法 如果是混 跳过去
			//	if ( card.m_color == CARD_COLOR_SUO && card.m_number == 1 )
			//	{
			//		continue;
			//	}
			//}

			if(!card.isValid())
				continue;

			TingCard tingcard;
			tingcard.m_card = card;

			std::vector<Lint> temp;
			gameInfo.bChaTing = true;
			bool canHu = CheckCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, &card, gameInfo, tingcard.m_hu);
			if (canHu)
			{
				// 计算番数
				for (Lsize k = 0; k < tingcard.m_hu.size(); ++k)
				{
					if ( gameInfo.m_hufan && gameInfo.m_hufan[tingcard.m_hu[k]] > 0 )
					{
						tingcard.m_hufan += gameInfo.m_hufan[tingcard.m_hu[k]];
					}
				}

				// 不查根
				// Lint gen = CheckGen(handcard, eatCard, pengCard, agangCard, mgangCard, &card);
				Lint gen = 0;
				tingcard.m_hufan += gen;
				tingcard.m_gen = gen;

				//Lint maxFan = gameInfo.m_playtype.gametypeGetFan();
				//if(tingcard.m_hufan > maxFan)
				//{
				//	tingcard.m_hufan = maxFan;
				//}

				winCards.push_back( tingcard );

				if(tingcard.m_hufan > retfan)
				{
					ret = winCards.size() - 1;
					retfan = tingcard.m_hufan;
				}
			}
		}
	}
	return ret;
}

//检测抢杠胡
bool CH_WuHan_XianTao::CheckQiangganghu(OperateState& gameInfo)
{
	if (!gameInfo.m_thinkGang && (gameInfo.m_cardState == THINK_OPERATOR_MGANG/*THINK_OPERATOR_MBU*/ && gameInfo.m_deskState != DESK_PLAY_END_CARD))
	{
		return true;
	}
	return false;
}

bool CH_WuHan_XianTao::CheckFourGhost(CardVector& handcard, Card* outCard, bool is_get_card, std::vector<Card>& ghostCard) 
{
	int ghostNum = 0;
	CardVector tempCard = handcard;
	if (outCard && is_get_card)
	{
		tempCard.push_back(outCard);
	}
	for (Lint i = 0; i < tempCard.size(); ++i)
	{
		std::vector<Card>::iterator cit = ghostCard.begin();
		for ( ; cit != ghostCard.end(); ++cit)
		{
			if (tempCard[i]->m_color == cit->m_color && tempCard[i]->m_number == cit->m_number)
			{
				++ghostNum;
			}
		}
	}
	return (ghostNum >= 4 ? true:false);
}

void CH_WuHan_XianTao::CheckCanHuNormal(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec)
{
	if(gameInfo.m_playtype.gametypeQiXiaoDui()) //默认可以胡七对
	{
		Lint special;
		if ( CheckXiaoqidui(handcard, eatCard, pengCard, agangCard, mgangCard, outCard, special) )
		{
			vec.push_back(HU_QIDUI);
			return ;
		}
	}
	
	if ( CheckHoo(handcard,outCard) )	// 33332
	{
		if(CheckQiangganghu(gameInfo))
		{
			if(gameInfo.m_playtype.GametypeQiangKang())
			{
				vec.push_back(HU_QIANGGANGHU);
				return ;
			}
		}
		else
		{
			vec.push_back(HU_XIAOHU);
			return ;
		}
	}
}

void CH_WuHan_XianTao::CheckCanHuHun(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec)
{
	// 临时手牌 不包括outCard
	CardVector tmpcards;
	int cardnum[4][9];
	memset(cardnum,0,sizeof(cardnum));

	int hunnum = 0;
	for ( auto it = handcard.begin(); it != handcard.end(); ++it )
	{
		if ( (*it)->m_color == gameInfo.m_hCard[0].m_color && (*it)->m_number == gameInfo.m_hCard[0].m_number )
		{
			++hunnum;
		}
		else
		{
			tmpcards.push_back( *it );
			const int color = (*it)->m_color;
			const int number = (*it)->m_number;
			if(color > 0 && color < 5 && number > 0 && number < 10)
			{
				cardnum[color-1][number-1]++;
			}
		}
	}

	if (outCard)
	{
		// 需要判断outCard是自己抓取的还是别人的打的牌 别人打的牌就按照幺鸡处理 否则按照混处理
		if ( gameInfo.m_MePos == gameInfo.m_playerPos && outCard->m_color == gameInfo.m_hCard[0].m_color && outCard->m_number == gameInfo.m_hCard[0].m_number )
		{
			++hunnum;
			//outCard = NULL;  //这儿不能置空 会导致后面七对判断错误
		}
		else
		{
			const int color = outCard->m_color;
			const int number = outCard->m_number;
			if(color > 0 && color < 5 && number > 0 && number < 10)
			{
				cardnum[color-1][number-1]++;
			}
		}
	}

	if ( CheckHooWithHun(cardnum, hunnum) )	// 33332
	{
		if(CheckQiangganghu(gameInfo))
		{
			vec.push_back(HU_QIANGGANGHU);
		}
		else
		{
			vec.push_back(HU_XIAOHU);
		}
	}

	//癞子红中数量
	//CardVector tmphandcard;
	//Lint laiZiCount = 0;
	//laiZiCount = checkLaiZi(handcard,outCard,(gameInfo.m_MePos == gameInfo.m_playerPos),gameInfo.m_hCard,tmphandcard);

	////牌数据排序
	//gCB_WuHan_XianTao->SortCard(tmphandcard);

	//if (matchHu_ShunKe(tmphandcard,laiZiCount,false,NULL))
	//{
	//	if(CheckQiangganghu(gameInfo))
	//	{
	//		if(gameInfo.m_playtype.GametypeQiangKang())
	//			vec.push_back(HU_QIANGGANGHU);
	//	}
	//	else
	//	{
	//		vec.push_back(HU_XIAOHU);
	//	}
	//}
}

bool CH_WuHan_XianTao::matchHu_ShunKe(const CardVector& cards, Lint laiZiCount, bool isneed258, std::vector<Card>* pMatch) const
{
	if( cards.empty() && (2 == laiZiCount || 5 == laiZiCount) )
	{
		//手里牌为空直接胡吧 因为手里只剩下赖子牌做将了
		return true;
	}

	bool bRet = false;

	Luchar szCards[52] = {0};	// 万00桶00条00东00南00西00北00中00发00白

	//重新组织数据
	for (CardVector::size_type index = 0; index < cards.size(); ++index)
	{
		if (cards[index]->m_color < CARD_COLOR_ZI)
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
	Lint cardCount = sizeof(szCards) / sizeof(szCards[0]);

	//检测是否满足胡牌
	if ((cards.size() + laiZiCount) % 3 == 2)	//考虑将牌
	{
		for (Lint index = 0; index < cardCount && (!bRet || NULL != pTingMatch); ++index)
		{
			//单胡将牌的情况
			if (0 < szCards[index] && 0 < laiZiCount)
			{
				switch(index)
				{
				case 1:
				case 4:
				case 7:
				case 12:
				case 15:
				case 18:
				case 23:
				case 26:
				case 29:
					{
						szCards[index] -= 1;
						laiZiCount -= 1;
						if (matchLaiZi(szCards, cardCount, laiZiCount, pTingMatch))
						{
							bRet = true;
							if (NULL != pTingMatch)
							{
								pTingMatch->insert(szCards + index);
							}
						}
						szCards[index] += 1;
						laiZiCount += 1;
						break;
					}
				default:
					{
						if (!isneed258)
						{
							szCards[index] -= 1;
							laiZiCount -= 1;
							if (matchLaiZi(szCards, cardCount, laiZiCount, pTingMatch))
							{
								bRet = true;
								if (NULL != pTingMatch)
								{
									pTingMatch->insert(szCards + index);
								}
							}
							szCards[index] += 1;
							laiZiCount += 1;
						}
						break;
					}
				}

			}
			//已经存在将牌的情况
			if ((!bRet || NULL != pMatch) && 2 <= szCards[index])
			{
				switch(index)
				{
				case 1:
				case 4:
				case 7:
				case 12:
				case 15:
				case 18:
				case 23:
				case 26:
				case 29:
					{
						szCards[index] -= 2;
						bRet = matchLaiZi(szCards, cardCount, laiZiCount, pTingMatch) || bRet;
						szCards[index] += 2;
						break;
					}
				default:
					{
						if (!isneed258)
						{
							szCards[index] -= 2;
							bRet = matchLaiZi(szCards, cardCount, laiZiCount, pTingMatch) || bRet;
							szCards[index] += 2;
						}
						break;
					}
				}
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
		Lint jianPaiStart = (CARD_COLOR_ZI - 1) * 11;
		std::set<Luchar*>::iterator iterTing = tingCards.begin();
		for (; iterTing != tingCards.end(); ++iterTing)
		{
			Lint cardIndex = *iterTing - szCards;
			if (jianPaiStart <= cardIndex)
			{
				pMatch->push_back(Card((cardIndex - jianPaiStart) / 3 + 1,CARD_COLOR_ZI));
			}
			else
			{
				cardColor = cardIndex / 11 + 1;
				cardNumber = (cardIndex - (cardColor - 1) * 11) + 1;
				if (1 <= cardNumber && cardNumber <= 9)
				{
					pMatch->push_back(Card(cardNumber,cardColor));
				}
			}
		}
	}

	return bRet;
}

Lint CH_WuHan_XianTao::checkLaiZi(CardVector& handcard, Card* outCard, bool is_get_card, std::vector<Card>& ghostCard, CardVector& tempCard)
{
	tempCard.clear();
	tempCard = handcard;
	if (outCard)
	{
		tempCard.push_back(outCard);
	}

	std::vector<int> intVector;
	std::vector<Card>::iterator cit = ghostCard.begin();
	for ( ; cit != ghostCard.end(); ++cit)//统计每种赖子的数量
	{
		int ghostNum = 0;
		for (Lint i = 0; i < tempCard.size(); ++i)
		{
			if (*tempCard[i] == *cit)
			{
				++ghostNum;
			}
		}
		intVector.push_back(ghostNum);
	}

	Lint hunnum = 0;
	Lint index = 0;
	cit = ghostCard.begin();
	for ( ; cit != ghostCard.end(); ++cit)//从手牌中删除每种赖子
	{
		Card card;
		card.m_color = cit->m_color;
		card.m_number = cit->m_number;
        EraseCard(tempCard,&card,intVector[index]);
		hunnum += intVector[index];
		++index;
	}

	cit = ghostCard.begin();
	for ( ; cit != ghostCard.end(); ++cit)//判断别人打的牌或者自己摸得牌是否是赖子来决定插入手牌中
	{
		if (outCard && *outCard == *cit)
		{
			if(!is_get_card)//别人打的不算赖子
			{
				hunnum--;
				tempCard.push_back(outCard);
			}

			break;
		}	
	}

	return hunnum;
}