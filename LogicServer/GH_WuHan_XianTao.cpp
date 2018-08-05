#include "GH_WuHan_XianTao.h"
#include "Config.h"
#include "Work.h"
#include "LLog.h"
#include "RoomVip.h"


bool GH_WuHan_XianTao::startup(Desk *desk)
{
	if (desk)
	{
		shutdown();
		m_desk = desk;
	}
	return true;
}

void GH_WuHan_XianTao::shutdown(void)
{
	for (Lint i = 0; i< DESK_USER_COUNT; ++i)
	{
		m_louHuCard[i] = NULL;
		m_startHuRet[i] = 0;
		m_thinkInfo[i].Reset();
		m_thinkRet[i].Clear();
		m_startHu[i].clear();
		m_handCard[i].clear();
		m_outCard[i].clear();
		m_pengCard[i].clear();
		m_gangCard[i].clear();
		m_abombCard[i].clear();
		m_eatCard[i].clear();
		m_pengInfo[i].clear();
		m_gangInfo[i].clear();
		m_pengGangInfo[i].clear();
		m_mingGangInfo[i].clear();
		m_angang[i] = 0;
		m_minggang[i] = 0;
		m_diangang[i] = 0;
		m_adiangang[i] = 0;
		m_gangPos[i] = 0;
		m_playerHuInfo[i].Clear();
		m_guoShouHu[i] = false;		// 过手胡限制
		m_gsCard[i].clear();
		m_isPiao[i] = 0;
		m_isFly[i] = false;
		m_tingCard[i].clear();
		m_tingPos[i] = 0;
		m_paoGangScore[i] = 0;
		m_paoBeforeScore[i] = 0;
		m_paoAfterScore[i] = 0;
		m_paoTieGang[i] = 0;
		m_zhuangHuGang[i] = 0;
		m_pt_zhuangDouble[i] = 1;
		m_resultAnGangScore[i] = 0;
		m_resultMingGangScore[i] = 0;
		m_resultADianGangScore[i] = 0;
		m_scoreInfo[i].clear();
		b_userPlayCard[i] = false;
		m_baseScore[i] = 1;
		m_beforeOpScore[i] = 0;
		m_totalScore[i] = 0;
	}
	m_desk = NULL;
	m_curOutCard = NULL;
	m_curGetCard = NULL;
	m_beforePos = INVAILD_POS;
	m_beforeType = THINK_OPERATOR_NULL;
	m_zhuangpos = L_Rand(0, 2);//0;	// 庄开始为0			// 改为随机庄
	m_curPos = INVAILD_POS;
	m_endStartPos = 0;
	m_endStartCurPos = 0;
	m_needGetCard = false;
	m_deskCard.clear();
	m_qiangganghu = 0;
	m_ghostCardReal.clear();
	m_video.Clear();	
	m_gameInfo.Clear();
	m_ghostCardFlop.m_color = 0;
	m_ghostCardFlop.m_number = 0;
	m_pt_ting = false;
	m_pt_hun = false;
	m_pt_feng = false;
	m_pt_pao = false;
	m_paoZhuangPos = m_zhuangpos;
	m_zhuangChangeCount = 0;
	m_playGhostCard = false;

	m_cardPos1 = 0;
	m_cardPos2 = 2;
	m_cardPos3 = 1;
	m_changeZhuang = true;
}

void GH_WuHan_XianTao::SetDeskPlay()
{
	if (!m_desk || !m_desk->m_vip)
	{
		return;
	}

	for (Lint i = 0; i< DESK_USER_COUNT; ++i)
	{
		m_louHuCard[i] = NULL;
		m_startHuRet[i] = 0;
		m_thinkInfo[i].Reset();
		m_thinkRet[i].Clear();
		m_startHu[i].clear();
		m_handCard[i].clear();
		m_outCard[i].clear();
		m_pengCard[i].clear();
		m_gangCard[i].clear();
		m_abombCard[i].clear();
		m_eatCard[i].clear();
		m_pengInfo[i].clear();
		m_gangInfo[i].clear();
		m_pengGangInfo[i].clear();
		m_mingGangInfo[i].clear();
		m_angang[i] = 0;
		m_minggang[i] = 0;
		m_diangang[i] = 0;
		m_adiangang[i] = 0;
		m_gangPos[i] = 0;
		m_playerHuInfo[i].Clear();
		m_guoShouHu[i] = false;		// 过手胡限制
		m_gsCard[i].clear();
		m_isPiao[i] = 0;
		//m_isFly[i] = false;
		m_tingCard[i].clear();
		m_tingPos[i] = 0;
		m_zhuangHuGang[i] = 0;
		m_pt_zhuangDouble[i] = 1;
		m_resultAnGangScore[i] = 0;
		m_resultMingGangScore[i] = 0;
		m_resultADianGangScore[i] = 0;
		m_scoreInfo[i].clear();
		b_userPlayCard[i] = false;
		m_baseScore[i] = 1;
		m_beforeOpScore[i] = 0;
		m_totalScore[i] = 0;
	}
	if(!m_pt_pao || m_changeZhuang)
	{
		for (Lint i = 0; i< DESK_USER_COUNT; ++i)
			m_isFly[i] = false;
	}
	m_curOutCard = NULL;
	m_curGetCard = NULL;
	m_beforePos = INVAILD_POS;
	m_beforeType = THINK_OPERATOR_NULL;
	m_curPos = m_zhuangpos;
	m_endStartPos = 0;
	m_endStartCurPos = 0;
	m_needGetCard = false;
	m_deskCard.clear();
	m_video.Clear();
	m_qiangganghu = 0;
	m_ghostCardReal.clear();
	m_gameInfo.Clear();
	m_ghostCardFlop.m_color = 0;
	m_ghostCardFlop.m_number = 0;
	m_playGhostCard = false;

	m_desk->m_vip->SendInfo();

	m_desk->setDeskState(DESK_PLAY);

	// 设置玩法
	m_playtype.setAllTypes( m_desk->getPlayType() );

	m_pt_pao = m_playtype.hasPour();

	if(m_pt_pao)	// 带跑玩法的默认玩法
	{
		m_pt_ting = true;		// 带跑玩法必须听牌才能胡
		m_pt_hun = false;
		m_pt_feng = true;
	}
	else
	{
		if(m_playtype.checkPlayType(PT_ZK_TING))
			m_pt_ting = true;
		else
			m_pt_ting = false;

		if(m_playtype.checkPlayType(PT_ZK_GHOST))
			m_pt_hun = true;
		else
			m_pt_hun = false;

		if(m_playtype.checkPlayType(PT_ZK_DAIFENG))
			m_pt_feng = true;
		else
			m_pt_feng = false;
	}
	m_pt_hun = true;
	m_pt_feng = false;
	m_pt_ting = false;

	// 设置gameinfo
	m_gameInfo.m_playtype = m_playtype;
	m_gameInfo.m_pt_laizi = m_pt_hun;					// m_gameInfo.m_pt_laizi不管其他地方怎么用 在这里作为判断混牌依据

	/*if(!m_pt_pao || m_changeZhuang)
	{
		BroadcastBottomPour();   //下跑通知
		m_desk->setDeskPlayState(DESK_PLAY_BOTTOM_POUR);

		m_changeZhuang = false;
	}
	else*/
	{
		DeakCard();

		CheckStartPlayCard();
	}

}

void GH_WuHan_XianTao::HanderUserChange( User* pUser, LMsgC2SBottomPour* msg )
{
	if (m_desk == NULL || pUser == NULL || msg == NULL)
	{
		LLOG_ERROR("HanderUserChange NULL ERROR ");
		return;
	}

	Lint pos = m_desk->GetUserPos(pUser);
	if(pos == INVAILD_POS)// || !m_pt_pao)
	{
		LLOG_ERROR("HanderUserChange pos error %s", pUser->m_userData.m_nike.c_str());
		return;
	}

	if (m_isFly[pos])
	{
		LLOG_ERROR("HanderUserChange pos error %s", pUser->m_userData.m_nike.c_str());
		return;
	}

	m_isFly[pos] = true;

	if(!m_pt_pao)
	{
		m_isPiao[pos] = msg->m_nPour;

		//录像
		std::vector<CardValue> cards;
		CardValue card;
		card.m_color = msg->m_nPour;
		card.m_number = 0;
		cards.push_back(card);
		m_video.AddOper(VIDEO_OPER_ZK_XIAFEN, pos, cards);
	}
	else
	{
		m_paoGangScore[pos] = msg->m_nGangPour;
		m_paoBeforeScore[pos] = msg->m_nQianPour;
		m_paoAfterScore[pos] = msg->m_nHouPour;
		m_paoTieGang[pos] = msg->m_nTiePour;

		//录像
		std::vector<CardValue> cards;
		CardValue card;
		card.m_color = msg->m_nGangPour;
		card.m_number = msg->m_nQianPour;
		cards.push_back(card);
		card.m_color = msg->m_nHouPour;
		card.m_number = msg->m_nTiePour;
		cards.push_back(card);

		m_video.AddOper(VIDEO_OPER_ZK_XIAPAO, pos, cards);
	}

	LMsgS2CUserBottomPourInfo info;
	info.m_nPos = pos;
	info.m_nPour = msg->m_nPour;
	info.m_nGangPour = msg->m_nGangPour;
	info.m_nQianPour = msg->m_nQianPour;
	info.m_nHouPour = msg->m_nHouPour;
	info.m_nTiePour = msg->m_nTiePour;

	m_desk->BroadcastWithoutUser(info, pos);

	if (IsPouredOver())
	{
		DeakCard();

		CheckStartPlayCard();
	}
}

void GH_WuHan_XianTao::ProcessRobot(Lint pos, User * pUser)
{
	if (pos < 0 || pos > 3)
	{
		return;
	}

	switch(m_desk->getDeskPlayState())
	{
	case DESK_PLAY_START_HU:
		{
			if (m_startHu[pos].size() > 0)
			{
				LMsgC2SUserStartHuSelect msg;
				msg.m_type = START_HU_NULL;
				m_desk->HanderUserStartHu(pUser, &msg);
				return;
			}
		}
		break;
	case DESK_PLAY_GET_CARD:
		{
			//打出牌去
			if (m_desk->getDeskPlayState() == DESK_PLAY_GET_CARD && m_curPos == pos)
			{
				LMsgC2SUserPlay msg;
				msg.m_thinkInfo.m_type = THINK_OPERATOR_OUT;
				CardValue card;
				card.m_color = m_handCard[pos][0]->m_color;
				card.m_number = m_handCard[pos][0]->m_number;
				msg.m_thinkInfo.m_card.push_back(card);
				m_desk->HanderUserPlayCard(pUser, &msg);
			}
		}
		break;
	case DESK_PLAY_THINK_CARD:
		{
			if (m_thinkInfo[pos].NeedThink())
			{
				for (Lsize i = 0; i < m_thinkInfo[pos].m_thinkData.size(); ++i)
				{
					if (m_thinkInfo[pos].m_thinkData[i].m_type == THINK_OPERATOR_BOMB)
					{
						LMsgC2SUserOper msg;
						msg.m_think.m_type = THINK_OPERATOR_NULL;		//
						CardVector& mCard = m_thinkInfo[pos].m_thinkData[i].m_card;
						for (Lsize j = 0; j <  mCard.size(); ++j)
						{
							CardValue card;
							card.m_color = mCard[j]->m_color;
							card.m_number =  mCard[j]->m_number;
							msg.m_think.m_card.push_back(card) ;
						}
						m_desk->HanderUserOperCard(pUser, &msg);
						return;
					}
					else
					{
						LMsgC2SUserOper msg;
						msg.m_think.m_type = m_thinkInfo[pos].m_thinkData[i].m_type;
						CardVector& mCard = m_thinkInfo[pos].m_thinkData[i].m_card;
						for (Lsize j = 0; j <  mCard.size(); ++j)
						{
							CardValue card;
							card.m_color = mCard[j]->m_color;
							card.m_number =  mCard[j]->m_number;
							msg.m_think.m_card.push_back(card) ;
						}
						m_desk->HanderUserOperCard(pUser, &msg);
					}
				}
			}
		}
		break;
	case DESK_PLAY_END_CARD:
		{
			if (m_desk->getDeskPlayState() == DESK_PLAY_END_CARD && m_curPos == pos)
			{
				LMsgC2SUserEndCardSelect msg;
				msg.m_flag = 0;
				m_desk->HanderUserEndSelect(pUser, &msg);
				return;
			}
		}
		break;
	case DESK_PLAY_BOTTOM_POUR:
		{
			// TODO: 下跑
			LMsgC2SBottomPour msg;
			msg.m_nPour = 2;	
			m_desk->HanderUserChange(pUser, &msg);
		}
		break;
	default:
		{

		} break;
	}
}

void GH_WuHan_XianTao::HanderUserSpeak( User* pUser, LMsgC2SUserSpeak* msg )
{
	if(gConfig.GetDebugModel())
	{
		std::vector<Lstring> des;
		L_ParseString(msg->m_msg, des, " ");
		if (des.size() == 3)
		{
			if (des[0] == "#give")
			{
				for (auto itr=m_deskCard.begin(); itr!=m_deskCard.end(); ++itr)
				{
					if ((*itr)->m_number == atoi(des[1].c_str()) && (*itr)->m_color == atoi(des[2].c_str()))
					{
						std::swap((*itr),m_deskCard.back());
						break;
					}
				}
			}
		}
	}
}

void GH_WuHan_XianTao::HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg)
{
	if (m_desk == NULL || pUser == NULL || msg == NULL)
	{
		LLOG_DEBUG("HanderUserPlayCard NULL ERROR ");
		return;
	}
	LMsgS2CUserPlay sendMsg;
	sendMsg.m_errorCode = 0;
	sendMsg.m_pos = m_curPos;
	sendMsg.m_card = msg->m_thinkInfo;

	Lint pos = m_desk->GetUserPos(pUser);
	if(pos == INVAILD_POS)
	{
		//pUser->Send(sendMsg);
		LLOG_DEBUG("HanderUserPlayCard pos error %s", pUser->m_userData.m_nike.c_str());
		return;
	}

	if(pos != m_curPos)
	{
		sendMsg.m_errorCode = 4;
		pUser->Send(sendMsg);
		LLOG_ERROR("HanderUserPlayCard not my pos %d:%d", pos, m_curPos);
		return;
	}
	LLOG_DEBUG("WUHAN_XIANTAO::HanderUserPlayCard, beforePos=%d, curPos=%d, thinkType=%d.", m_beforePos, m_curPos, msg->m_thinkInfo.m_type);
	LLOG_DEBUG("WUHAN_XIANTAO::HanderUserPlayCard id:%d pos:%d think_type:%d", pUser->m_userData.m_id, pos, msg->m_thinkInfo.m_type);
	
	if(msg->m_thinkInfo.m_type == THINK_OPERATOR_OUT)
	{
		if(msg->m_thinkInfo.m_card.size())
		{
			for (Lsize i = 0; i < m_handCard[pos].size(); ++i)
			{
				if (*m_handCard[pos][i] == msg->m_thinkInfo.m_card[0])
				{
					m_curOutCard = m_handCard[pos][i];
					gCB_WuHan_XianTao->EraseCard(m_handCard[pos], m_curOutCard);

					if (m_curOutCard->m_color == m_ghostCardReal[0].m_color && m_curOutCard->m_number == m_ghostCardReal[0].m_number)
					{
						m_playGhostCard = true;
						
						int score = 0;
						for (int x = 0; x < m_desk->GetPlayerCapacity(); x++)
						{
							m_beforeOpScore[x] = 0;

							if (m_curPos == x)
							{
								continue;
							}
							int tmpScore = m_desk->m_baseScore * m_baseScore[x] * m_baseScore[m_curPos];
							sendMsg.m_addScore[x] = -tmpScore;
							m_totalScore[x] += -tmpScore;
							score += tmpScore;
						}

						sendMsg.m_addScore[m_curPos] = score;
						m_totalScore[m_curPos] += score;

						// 飘癞子倍数翻倍
						m_baseScore[m_curPos] = m_baseScore[m_curPos] * 2;
					}

					m_desk->BoadCast(sendMsg);
					m_beforePos = pos;
					
					b_userPlayCard[pos] = true;
					LLOG_DEBUG("broadcast message, curPos=%d.", pos);
					//录像
					std::vector<CardValue> cards;
					CardValue card;
					card.m_color = m_curOutCard->m_color;
					card.m_number = m_curOutCard->m_number;
					cards.push_back(card);
					m_video.AddOper(VIDEO_OPER_OUT_CARD, pos, cards);

					//这里玩家思考
					SetThinkIng();
					// 这里设置了之后会导致当前用户杠了之后，打的那张牌不能强制胡
					//m_beforeType = THINK_OPERATOR_OUT;
					break;
				}
			}
			
		}
		return;
	}

	ThinkUnit* unit = NULL;
	for(Lsize i = 0 ; i < m_thinkInfo[pos].m_thinkData.size(); ++i)
	{
		LLOG_DEBUG("msg thinkType=%d，data type=%d，i=%d.", msg->m_thinkInfo.m_type, m_thinkInfo[pos].m_thinkData[i].m_type, i);
		if(msg->m_thinkInfo.m_type == m_thinkInfo[pos].m_thinkData[i].m_type)
		{
			if(msg->m_thinkInfo.m_card.size() == m_thinkInfo[pos].m_thinkData[i].m_card.size())
			{
				bool find = true;
				for(Lsize j = 0 ; j < msg->m_thinkInfo.m_card.size() ; ++j)
				{
					LLOG_DEBUG("msg card=%d.%d, data card=%d.%d.", msg->m_thinkInfo.m_card[j].m_color, msg->m_thinkInfo.m_card[j].m_number, m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_color, m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_number);
					if(msg->m_thinkInfo.m_card[j].m_color != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_color ||
						msg->m_thinkInfo.m_card[j].m_number != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_number)
					{
						LLOG_DEBUG("find=false, i=%d.", i);
						find = false;
						break;
					}
				}

				if(find)
				{
					unit = &m_thinkInfo[pos].m_thinkData[i];
					unit->toString();
					break;
				}
			}
		}
	}

	if(unit)
	{
		if(unit->m_type == THINK_OPERATOR_BOMB)
		{
			//录相;
			VideoDoing(unit->m_type,pos,0,0);

			bool hasLaizi = false;
			if (m_curGetCard)
			{
				LLOG_DEBUG("zimo check: %d,%d;%d,%d.", m_curGetCard->m_color, m_curGetCard->m_number, m_ghostCardReal[0].m_color, m_ghostCardReal[0].m_number);
				gCB_WuHan_XianTao->EraseCard(m_handCard[pos], m_curGetCard);
				sendMsg.m_huCard.m_color = m_curGetCard->m_color;
				sendMsg.m_huCard.m_number = m_curGetCard->m_number;

				if (m_curGetCard->m_color == m_ghostCardReal[0].m_color && m_curGetCard->m_number == m_ghostCardReal[0].m_number)
				{
					hasLaizi = true;
				}
			}
			sendMsg.m_hu = unit->m_hu;
			sendMsg.m_cardCount = m_handCard[pos].size();
			for (Lint i = 0; i < sendMsg.m_cardCount; ++i)
			{
				CardValue mCard;
				mCard.m_color = m_handCard[pos][i]->m_color;
				mCard.m_number = m_handCard[pos][i]->m_number;
				sendMsg.m_cardValue.push_back(mCard);

				LLOG_DEBUG("zimo check: %d,%d;%d,%d.", mCard.m_color, mCard.m_number, m_ghostCardReal[0].m_color, m_ghostCardReal[0].m_number);
				if (mCard.m_color == m_ghostCardReal[0].m_color && mCard.m_number == m_ghostCardReal[0].m_number)
				{
					hasLaizi = true;
				}
			}

			// 手上没癞子自摸，翻4倍
			// 手上有癞子，但是癞子作为本身使用，翻4倍
			int doubleSize = 2;
			LLOG_DEBUG("hasLaizi=%d.", hasLaizi);
			if (hasLaizi)
			{
				if (gCB_WuHan_XianTao->CheckHoo(m_handCard[pos], m_curGetCard))
				{
					LLOG_DEBUG("true");
					doubleSize = 4;
				}
				else
				{
					LLOG_DEBUG("false");
				}
			}
			else
			{
				doubleSize = 4;
			}

			if (hasFourLaizi(m_curPos))
			{
				doubleSize = 2 * doubleSize;
			}

			int score = 0;
			for (int i = 0; i < m_desk->GetPlayerCapacity(); ++i)
			{
				LLOG_DEBUG("pos=%d,baseScore=%d.", i, m_baseScore[i]);
				if (i == m_curPos)
				{
					continue;
				}
				int tmpScore = doubleSize * m_desk->m_baseScore * m_baseScore[i] * m_baseScore[m_curPos];
				sendMsg.m_addScore[i] = -tmpScore;
				m_totalScore[i] += -tmpScore;
				score += tmpScore;
			}

			sendMsg.m_addScore[m_curPos] = score;
			m_totalScore[m_curPos] += score;
			m_desk->BoadCast(sendMsg);

			m_thinkRet[m_curPos] = m_thinkInfo[m_curPos].m_thinkData[0];
			m_playerHuInfo[m_curPos].type = CheckIfBigHu(m_thinkRet[m_curPos].m_hu)? WIN_SUB_DZIMO : WIN_SUB_ZIMO;

			//录像
			if ( m_curGetCard )
			{
				CardValue curGetCard;
				curGetCard.m_color = m_curGetCard->m_color;
				curGetCard.m_number = m_curGetCard->m_number;
				m_playerHuInfo[m_curPos].wincards.push_back(curGetCard);
			}
			m_video.AddOper( VIDEO_OPER_ZIMO, m_curPos, m_playerHuInfo[m_curPos].wincards );

			OnGameOver(WIN_ZIMO, INVAILD_POS);
		}
		else if(unit->m_type == THINK_OPERATOR_AGANG)
		{
			int score = 0;
			for (int i = 0; i < m_desk->GetPlayerCapacity(); ++i)
			{
				if (i == m_curPos)
				{
					continue;
				}

				int tmpScore = 2 * m_desk->m_baseScore * m_baseScore[i] * m_baseScore[m_curPos];
				sendMsg.m_addScore[i] = -tmpScore;
				m_totalScore[i] += -tmpScore;
				m_beforeOpScore[i] = m_beforeOpScore[i] - tmpScore;
				score += tmpScore;
			}

			sendMsg.m_addScore[m_curPos] = score;
			m_totalScore[m_curPos] += score;
			m_beforeOpScore[m_curPos] = m_beforeOpScore[m_curPos] + score;

			//录相;
			VideoDoing(unit->m_type,pos,unit->m_card[0]->m_color,unit->m_card[0]->m_number);
			//
			gCB_WuHan_XianTao->EraseCard(m_handCard[pos], unit->m_card[0],4);	
			m_desk->BoadCast(sendMsg);
			m_angang[pos] += 1;

			//录像
			std::vector<CardValue> cards(4, unit->m_card[0]->ToCardValue());
			m_video.AddOper(VIDEO_OPER_AN_GANG, pos, cards);

			m_abombCard[pos].insert(m_abombCard[pos].end(), 4, unit->m_card[0]);

			m_beforeType = THINK_OPERATOR_AGANG;
			if (unit->m_card[0]->m_color == m_ghostCardFlop.m_color && unit->m_card[0]->m_number == m_ghostCardFlop.m_number)
			{
				SetPlayIng(pos, false, true, true, true);
			}
			else
			{
				SetPlayIng(pos, true, true, true, true);
			}
		}
		else if(unit->m_type == THINK_OPERATOR_MGANG)
		{
			int score = m_desk->m_baseScore * m_baseScore[m_curPos] * m_baseScore[m_beforePos];
			sendMsg.m_addScore[m_curPos] = score;
			sendMsg.m_addScore[m_beforePos] = -score;

			//录相;
			VideoDoing(unit->m_type,pos,unit->m_card[0]->m_color,unit->m_card[0]->m_number);

			LLOG_DEBUG("m_curOutCard=%d,%d.", m_curOutCard->m_color, m_curOutCard->m_number);
			m_curOutCard = unit->m_card[0];
			LLOG_DEBUG("m_curOutCard=%d,%d.", m_curOutCard->m_color, m_curOutCard->m_number);
			gCB_WuHan_XianTao->EraseCard(m_handCard[pos], unit->m_card[0],3);	
			m_desk->BoadCast(sendMsg);
			m_minggang[pos] += 1;

			//这里玩家思考
			//m_beforePos = m_curPos;
			m_beforeType = THINK_OPERATOR_MGANG;
			//SetThinkIng();
			SetPlayIng(pos, true, true, true, true);
		}
		else if(unit->m_type == THINK_OPERATOR_MBU)
		{
			int score = 0;
			for (int i = 0; i < DESK_USER_COUNT; ++i)
			{
				if (i == m_curPos)
				{
					continue;
				}

				int tmpScore = m_desk->m_baseScore * m_baseScore[i] * m_baseScore[m_curPos];
				sendMsg.m_addScore[i] = -tmpScore;
				m_beforeOpScore[i] = m_beforeOpScore[i] - tmpScore;
				score += tmpScore;
			}

			sendMsg.m_addScore[m_curPos] = score;
			m_beforeOpScore[m_curPos] = m_beforeOpScore[m_curPos] + score;

			//录相;
			VideoDoing(unit->m_type,pos,unit->m_card[0]->m_color,unit->m_card[0]->m_number);
			//
			m_curOutCard = unit->m_card[0];
			gCB_WuHan_XianTao->EraseCard(m_handCard[pos], unit->m_card[0],1);
			m_desk->BoadCast(sendMsg);	
			m_minggang[pos] += 1;

			//这里玩家思考
			m_beforeType = THINK_OPERATOR_MBU;
			SetPlayIng(pos, true, true, true, true);
			
		}
		//我们的thinkinfo中不会有THINK_OPERATOR_NULL，所以不会有unit，不会走到这一步
		// 			else if(unit->m_type == THINK_OPERATOR_NULL)
		// 			{
		// 				//录相;
		// 				VideoDoing(unit->m_type,pos,0,0);
		// 				//
		// 			}
	}
	else
	{
		if(msg->m_thinkInfo.m_type == THINK_OPERATOR_NULL)
		{
			// 检查玩家是否能胡 能胡添加上过手胡限制
			for(Lsize i = 0 ; i < m_thinkInfo[pos].m_thinkData.size(); ++i)
			{
				if(m_thinkInfo[pos].m_thinkData[i].m_type == THINK_OPERATOR_BOMB)
				{
					m_guoShouHu[pos] = true;
				}
			}
			//录相;
			VideoDoing(msg->m_thinkInfo.m_type,pos,0,0);
		}
		LLOG_DEBUG("Desk::HanderUserPlayCard %s,%d", pUser->m_userData.m_nike.c_str(), msg->m_thinkInfo.m_type);
	}
}

void GH_WuHan_XianTao::HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg)
{
	LMsgS2CUserOper sendMsg;
	sendMsg.m_think = msg->m_think;
	sendMsg.m_pos = m_curPos;
	Lint pos = m_desk->GetUserPos(pUser);
	if (pos == INVAILD_POS || !m_thinkInfo[pos].NeedThink())
	{
		sendMsg.m_errorCode = 1;
		pUser->Send(sendMsg);
		LLOG_DEBUG("HanderUserOperCard pos error pos: %d     curPos:%d", pos, m_endStartCurPos);
		return;
	}

	// 操作结果是取消时，不调整curPos的位置，避免发牌错位
	if (msg->m_think.m_type != THINK_OPERATOR_NULL)
	{
		m_curPos = pos;
	}
	sendMsg.m_pos = m_curPos;
	LLOG_DEBUG("HanderUserOperCard, beforePos=%d, curPos=%d, opType=%d.", m_beforePos, m_curPos, msg->m_think.m_type);
	LLOG_DEBUG("gCB_WuHan_XianTao::HanderUserOperCard id:%d pos:%d think_type:%d", pUser->m_userData.m_id, pos, msg->m_think.m_type);

	printThinkInfo(m_thinkInfo[pos]);
	bool find = false;
	for(Lsize i = 0 ; i < m_thinkInfo[pos].m_thinkData.size(); ++i)
	{
		if(m_thinkInfo[pos].m_thinkData[i].m_type == msg->m_think.m_type)
		{
			bool check = true;
			if(m_thinkInfo[pos].m_thinkData[i].m_card.size() == msg->m_think.m_card.size())
			{
				for(Lsize j = 0 ; j < msg->m_think.m_card.size(); ++j)
				{
					if(msg->m_think.m_card[j].m_color != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_color || 
						msg->m_think.m_card[j].m_number != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_number)
					{
						check = false;
						break;
					}
				}

				if(check)
				{
					m_thinkRet[pos] = m_thinkInfo[pos].m_thinkData[i];
					find = true;
					break;
				}
			}
		}
	}

	if (!find)
	{
		m_thinkRet[pos].m_type = THINK_OPERATOR_NULL;
		msg->m_think.m_type = THINK_OPERATOR_NULL;
	}

	//录相;
	VideoDoing(msg->m_think.m_type,pos,(msg->m_think.m_card.size()>0)?msg->m_think.m_card[0].m_color:0,(msg->m_think.m_card.size()>0)?msg->m_think.m_card[0].m_number:0);

	if (msg->m_think.m_type == THINK_OPERATOR_BOMB)
	{
		/*
		LMsgS2CUserOper send;
		send.m_pos = pos;
		send.m_errorCode = 0;
		send.m_think = msg->m_think;
		send.m_card.m_color = (m_curOutCard == NULL)? 1 : m_curOutCard->m_color;		//临时的 有错误
		send.m_card.m_number =(m_curOutCard == NULL)? 1 : m_curOutCard->m_number;		//临时的 有错误
		send.m_hu = m_thinkRet[pos].m_hu;
		send.m_cardCount = m_handCard[pos].size();
		for (Lint i = 0; i < send.m_cardCount; ++i)
		{
			CardValue mCard;
			mCard.m_color = m_handCard[pos][i]->m_color;
			mCard.m_number = m_handCard[pos][i]->m_number;
			send.m_cardValue.push_back(mCard);
		}
		m_desk->BoadCast(send);
		*/
		// 清除其他玩家的碰杠数据 其他玩家只能胡和过
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			if (i != pos)
			{
				for (auto itThinkData = m_thinkInfo[i].m_thinkData.begin(); itThinkData != m_thinkInfo[i].m_thinkData.end();)
				{
					if (itThinkData->m_type != THINK_OPERATOR_BOMB)
					{
						itThinkData = m_thinkInfo[i].m_thinkData.erase(itThinkData);
					}
					else
					{
						itThinkData++;
					}
				}
			}
		}
	}

	// 过手胡
	if (msg->m_think.m_type == THINK_OPERATOR_NULL)
	{
		for (Lsize i = 0; i < m_thinkInfo[pos].m_thinkData.size(); ++i)
		{
			if (m_thinkInfo[pos].m_thinkData[i].m_type == THINK_OPERATOR_BOMB)
			{
				m_guoShouHu[pos] = true;
			}
		}
	}
	else if (msg->m_think.m_type == THINK_OPERATOR_MBU ||
		msg->m_think.m_type == THINK_OPERATOR_MGANG)
	{
		m_beforeType = THINK_OPERATOR_MGANG;
		if (msg->m_think.m_type == THINK_OPERATOR_MBU)
		{
			m_beforeType = THINK_OPERATOR_MBU;
		}
		m_guoShouHu[pos] = false;
		bool isQiangGangHu = false;
		for (Lint m = 0; m < DESK_USER_COUNT; m++)
		{
			LLOG_DEBUG("==================");
			// 抢杠胡的逻辑需要剔除当前用户，避免当前用户杠的时候直接胡牌
			if (m == pos)
			{
				continue;
			}
			if (m_thinkInfo[m].HasHu() || m_guoShouHu[m])
			{
				m_thinkRet[m].m_type = THINK_OPERATOR_BOMB;
				isQiangGangHu = true;
			}
		}

		if (isQiangGangHu)
		{
			ThinkEnd();
			return;
		}
	}
	else if (msg->m_think.m_type == THINK_OPERATOR_PENG)
	{
		m_beforeType = THINK_OPERATOR_PENG;
	}

	//设置以及思考过了
	m_thinkInfo[pos].Reset();

	CheckThink();
}

void GH_WuHan_XianTao::OnUserReconnect(User* pUser)
{
	if (pUser == NULL || m_desk == NULL)
	{
		return;
	}
	Lint pos = m_desk->GetUserPos(pUser);
	if (pos == INVAILD_POS)
	{
		LLOG_ERROR("Desk::OnUserReconnect pos error %d", pUser->GetUserDataId());
		return;
	}
	Lint nCurPos = m_curPos;
	Lint nDeskPlayType = m_desk->getDeskPlayState();

	LMsgS2CAllUserBottomPourInfo BottomPourInfo;
	memcpy(&BottomPourInfo.m_bPoured, m_isFly, sizeof(m_isFly));
	memcpy(&BottomPourInfo.m_nPour, m_isPiao, sizeof(m_isPiao));
	memcpy(&BottomPourInfo.m_nGangPour, m_paoGangScore, sizeof(m_paoGangScore));
	memcpy(&BottomPourInfo.m_nQianPour, m_paoBeforeScore, sizeof(m_paoBeforeScore));
	memcpy(&BottomPourInfo.m_nHouPour, m_paoAfterScore, sizeof(m_paoAfterScore));
	memcpy(&BottomPourInfo.m_nTiePour, m_paoTieGang, sizeof(m_paoTieGang));

	for (Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		if (m_desk->m_user[i] == NULL)
		{
			continue;
		}
		m_desk->m_user[i]->Send(BottomPourInfo);
	}

	if(nDeskPlayType == DESK_PLAY_BOTTOM_POUR)
		return;

	LMsgS2CDeskState reconn;
	reconn.m_state = m_desk->getDeskState();
	reconn.m_pos = nCurPos;
	reconn.m_time = 15;
	reconn.m_zhuang = m_zhuangpos;
	reconn.m_myPos = pos;
	reconn.m_cardPos1 = m_cardPos1;
	reconn.m_cardPos2 = m_cardPos2;
	reconn.m_cardPos3 = m_cardPos3;

	reconn.m_ghostCard.m_color = m_ghostCardFlop.m_color;
	reconn.m_ghostCard.m_number = m_ghostCardFlop.m_number;

	if (m_pt_ting)
	{
		if(m_tingCard[pos].size() > 0)
		{
			reconn.m_tingCard.reserve(m_tingCard[pos].size());
			for(auto it=m_tingCard[pos].begin(); it != m_tingCard[pos].end(); ++it)
			{
				TingData::TingCard tc;
				tc.m_card.m_color = it->m_card.m_color;
				tc.m_card.m_number = it->m_card.m_number;
				tc.m_hufan = it->m_hufan;
				reconn.m_tingCard.push_back(tc);
			}
		}
	}
	else
	{
		std::vector<TingCard> winCards;
		int oldPlayPos = m_gameInfo.m_playerPos;				// 赖子测试的时候会判断outCard是否是自己的 所以这里临时改为自己
		m_gameInfo.m_playerPos = m_gameInfo.m_MePos;

		gCB_WuHan_XianTao->CheckTing(m_handCard[pos], m_pengCard[pos], m_abombCard[pos], m_gangCard[pos], m_eatCard[pos], m_gameInfo, winCards, nullptr);

		m_gameInfo.m_playerPos = oldPlayPos;

		if(winCards.size() > 0)
		{
			reconn.m_tingCard.reserve(winCards.size());
			for(auto it=winCards.begin(); it != winCards.end(); ++it)
			{
				TingData::TingCard tc;
				tc.m_card.m_color = it->m_card.m_color;
				tc.m_card.m_number = it->m_card.m_number;
				tc.m_hufan = it->m_hufan;
				reconn.m_tingCard.push_back(tc);
			}
		}
	}

	if(nCurPos != pos)
	{
		reconn.m_flag = 0;
	}
	else
	{
		reconn.m_flag = 1;			//不知道对不对
	}
	reconn.m_dCount = m_deskCard.size();
	for(Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		reconn.m_cardCount[i] = m_handCard[i].size();
		reconn.m_oCount[i] = m_outCard[i].size();
		reconn.m_aCount[i] = m_abombCard[i].size();
		reconn.m_mCount[i] = m_gangCard[i].size();
		reconn.m_pCount[i] = m_pengCard[i].size();
		reconn.m_eCount[i] = m_eatCard[i].size();
		reconn.m_ting[i] = m_tingCard[i].size()>0?1:0;
		reconn.m_tingPos[i] = m_tingPos[i];

		reconn.m_gangCount += (reconn.m_aCount[i] + reconn.m_mCount[i])/4;

		reconn.m_pengInfo[i] = m_pengInfo[i];
		reconn.m_gangInfo[i] = m_gangInfo[i];

		for(Lsize j = 0 ; j < m_outCard[i].size(); ++j)
		{
			reconn.m_oCard[i][j].m_color = m_outCard[i][j]->m_color;
			reconn.m_oCard[i][j].m_number = m_outCard[i][j]->m_number;
		}

		for(Lsize j = 0 ; j < m_abombCard[i].size(); ++j)
		{
			reconn.m_aCard[i][j].m_color = m_abombCard[i][j]->m_color;
			reconn.m_aCard[i][j].m_number = m_abombCard[i][j]->m_number;
		}

		for(Lsize j = 0 ; j < m_gangCard[i].size(); ++j)
		{
			reconn.m_mCard[i][j].m_color = m_gangCard[i][j]->m_color;
			reconn.m_mCard[i][j].m_number = m_gangCard[i][j]->m_number;
		}

		for(Lsize j = 0 ; j < m_pengCard[i].size(); ++j)
		{
			reconn.m_pCard[i][j].m_color = m_pengCard[i][j]->m_color;
			reconn.m_pCard[i][j].m_number = m_pengCard[i][j]->m_number;
		}

		for(Lsize j = 0 ; j < m_eatCard[i].size(); ++j)
		{
			reconn.m_eCard[i][j].m_color = m_eatCard[i][j]->m_color;
			reconn.m_eCard[i][j].m_number = m_eatCard[i][j]->m_number;
		}
	}

	//我的牌,客户的重连，之前莫得牌的重新拿出来发给他
	if(nDeskPlayType == DESK_PLAY_GET_CARD && m_needGetCard && pos == nCurPos)
	{
		CardVector tmp = m_handCard[pos];
		if (m_curGetCard)
		{
			reconn.m_cardCount[pos] -= 1;
			gCB_WuHan_XianTao->EraseCard(tmp,m_curGetCard);
		}
		for(Lsize j = 0 ;j < tmp.size(); ++j)
		{
			reconn.m_cardValue[j].m_color = tmp[j]->m_color;
			reconn.m_cardValue[j].m_number = tmp[j]->m_number;
		}
	}
	else
	{
		for(Lsize j = 0 ;j < m_handCard[pos].size(); ++j)
		{
			reconn.m_cardValue[j].m_color = m_handCard[pos][j]->m_color;
			reconn.m_cardValue[j].m_number = m_handCard[pos][j]->m_number;
		}
	}

	//该出牌的玩家，多发一张牌，用于打出去。
	if(m_needGetCard && nDeskPlayType == DESK_PLAY_THINK_CARD)
	{
		if (m_curOutCard && pos != nCurPos)
		{
			reconn.m_cardCount[nCurPos] ++;
		}
	}
	pUser->Send(reconn);

	//我思考
	if(nDeskPlayType == DESK_PLAY_THINK_CARD )
	{
		if (m_thinkInfo[pos].NeedThink())
		{
			LMsgS2CThink think;
			think.m_time = 15;
			think.m_flag = 1;
			think.m_card.m_color = (m_curOutCard == NULL)? 1 : m_curOutCard->m_color;		//临时的 有错误 m_curOutCard->m_color;
			think.m_card.m_number = (m_curOutCard == NULL)? 1 : m_curOutCard->m_number;		//临时的 有错误m_curOutCard->m_number;
			for (Lsize j = 0; j < m_thinkInfo[pos].m_thinkData.size(); ++j)
			{		
				ThinkData info;
				info.m_type = m_thinkInfo[pos].m_thinkData[j].m_type;
				for(Lsize n = 0 ; n < m_thinkInfo[pos].m_thinkData[j].m_card.size(); ++n)
				{
					CardValue v;
					v.m_color = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_color;
					v.m_number = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_number;
					info.m_card.push_back(v);
				}
				think.m_think.push_back(info);
			}
			pUser->Send(think);
		}
	}

	///海底牌
	if(nDeskPlayType == DESK_PLAY_END_CARD && pos == m_endStartCurPos)
	{
		LMsgS2CUserEndCardSelect msg;
		msg.m_pos = pos;
		msg.m_time = 15;
		pUser->Send(msg);
	}

	//我出牌
	if(nDeskPlayType == DESK_PLAY_GET_CARD && m_needGetCard && pos == nCurPos)
	{
		LMsgS2COutCard msg;
		msg.m_time = 15;
		msg.m_pos = pos;
		msg.m_deskCard = (Lint)m_deskCard.size();
		msg.m_flag = (m_curGetCard&&m_needGetCard)?0:1;
		msg.m_gang = m_gangPos[pos];
		msg.m_end = m_deskCard.size()==1?1:0;
		if(m_needGetCard && m_curGetCard)
		{
			msg.m_curCard.m_color = m_curGetCard->m_color;
			msg.m_curCard.m_number = m_curGetCard->m_number;
		}
		for (Lsize j = 0; j < m_thinkInfo[pos].m_thinkData.size(); ++j)
		{		
			ThinkData info;
			info.m_type = m_thinkInfo[pos].m_thinkData[j].m_type;
			for(Lsize n = 0 ; n < m_thinkInfo[pos].m_thinkData[j].m_card.size(); ++n)
			{
				CardValue v;
				v.m_color = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_color;
				v.m_number = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_number;
				info.m_card.push_back(v);
			}
			msg.m_think.push_back(info);
		}

		//听牌 参照漯河
		if(m_pt_ting &&  m_thinkInfo[pos].m_tingData.size() > 0)
		{
			ThinkData info;
			info.m_type = THINK_OPERATOR_TINGCARD;
			msg.m_think.push_back(info);
		}
		for (Lsize j = 0; j < m_thinkInfo[pos].m_tingData.size(); ++j)
		{
			TingData info;
			info.m_card.m_color = m_thinkInfo[pos].m_tingData[j].m_card.m_color;
			info.m_card.m_number = m_thinkInfo[pos].m_tingData[j].m_card.m_number;

			//有赖子时 加上赖子客户端显示
			if (m_pt_hun)
			{
				bool findqidui = m_thinkInfo[pos].m_tingData[j].FindQiDui();
				bool findghost = m_thinkInfo[pos].m_tingData[j].FindGhost(m_ghostCardFlop);

				if (!findqidui && !findghost) // 七对不显示
				{
					TingData::TingCard c;
					c.m_card = m_ghostCardFlop;
					info.m_tingcard.push_back(c);
				}
			}

			for(Lsize n = 0 ; n < m_thinkInfo[pos].m_tingData[j].m_tingcard.size(); ++n)
			{
				TingData::TingCard c;
				c.m_card.m_color = m_thinkInfo[pos].m_tingData[j].m_tingcard[n].m_card.m_color;
				c.m_card.m_number = m_thinkInfo[pos].m_tingData[j].m_tingcard[n].m_card.m_number;
				c.m_hufan = m_thinkInfo[pos].m_tingData[j].m_tingcard[n].m_hufan;
				info.m_tingcard.push_back( c );
			}
			msg.m_ting.push_back(info);
		}

		pUser->Send(msg);
	}

	//桌面上的牌重新发给玩家的桌牌
	if(m_needGetCard && nDeskPlayType == DESK_PLAY_THINK_CARD)
	{
		if (m_curOutCard)
		{
			LMsgS2CUserPlay sendMsg;
			sendMsg.m_errorCode = 0;
			sendMsg.m_pos = nCurPos;
			sendMsg.m_card.m_type = THINK_OPERATOR_REOUT;
			CardValue card;
			card.m_color = m_curOutCard->m_color;
			card.m_number = m_curOutCard->m_number;
			sendMsg.m_card.m_card.push_back(card);

			if(m_qiangganghu)
			{
				//sendMsg.m_qiangGangflag = 1;
			}

			pUser->Send(sendMsg);
		}
	}
}

void GH_WuHan_XianTao::OnGameOver(Lint result,Lint bombpos)
{
	if (m_desk == NULL || m_desk->m_vip == NULL)
	{
		LLOG_DEBUG("OnGameOver NULL ERROR ");
		return;
	}
	//计算输赢结果
	Lint gold[4]={0};
	Lint bombCount = 0;
	Lint winPos[4] = {0};
	Lint mgang[4] = {0};
	//计算分数
	
	for (Lint i = 0; i < m_desk->GetPlayerCapacity(); i++)
	{
		gold[i] = m_totalScore[i];
	}
	LLOG_DEBUG("game over, score=%d,%d,%d,%d.", gold[0], gold[1], gold[2], gold[3]);
	
	Lint zhuangPos = m_zhuangpos;
	//计算庄
	if (result == WIN_ZIMO)
	{
		m_zhuangpos = m_curPos;

		for(Lint i = 0 ; i < DESK_USER_COUNT; ++i)
		{
			if(winPos[i] == 1)
			{
				m_zhuangpos = i;	
				break;
			}
		}
	}
	else if (result == WIN_BOMB)
	{
		for(Lint i = 0 ; i < DESK_USER_COUNT; ++i)
		{
			if(winPos[i] == 1)
			{
				m_zhuangpos = i;
				break;
			}
		}
	}
	else
	{
		//m_zhuangpos = m_curPos;
		//录像
		std::vector<CardValue> cards;
		if ( result == WIN_NONE && m_video.m_time > 0 )
		{
			m_video.AddOper(VIDEO_OPER_HUANGZHUANG, m_curPos, cards);
		}
		else if (result == WIN_DISS && m_video.m_time > 0)
		{
			m_video.AddOper(VIDEO_OPER_DISS, m_curPos, cards);
		}
	}

	//保存录像
	LMsgL2LDBSaveVideo video;
	m_video.m_Id = gVipLogMgr.GetVideoId();
	if (m_video.m_time > 0)
	{
		m_video.m_playType = m_desk->getPlayType();

		video.m_userId = m_desk->m_user[0]->m_userData.m_id;
		video.m_type = 0;
		video.m_sql = m_video.GetInsertSql();
		gWork.SendMsgToDb(video);
	}
	
	//广播结果
	LMsgS2CGameOver over;
	over.m_result = result;
	for ( int i = 0; i < DESK_USER_COUNT; ++i )
	{
		over.m_win[i] = m_playerHuInfo[i].type;
		over.m_hucards[i] = m_playerHuInfo[i].wincards;	
		over.m_scoreInfo[i] = m_scoreInfo[i];
	}
	// 点炮
	for ( int i = 0; i < DESK_USER_COUNT; ++i )
	{
		if ( m_playerHuInfo[i].type == WIN_SUB_BOMB && m_playerHuInfo[i].abombpos != INVAILD_POS )
		{
			over.m_win[m_playerHuInfo[i].abombpos] = WIN_SUB_ABOMB;
		}

		over.m_fanShu[i] = m_pengGangInfo[i].size();//碰杠

		std::vector<Lint> pengPosVect = m_pengGangInfo[i];
		for (size_t k = 0; k < pengPosVect.size(); ++k)		
		{
			Lint pengPos = pengPosVect[k];
			if ((pengPos >= 0) && (pengPos < DESK_USER_COUNT))
			{
				m_adiangang[pengPos] += 1;//加上被碰杠的数量
			}
		}

	}

	memcpy(over.m_score, gold, sizeof(over.m_score));
	//memcpy(over.m_agang, m_angang, sizeof(over.m_agang));//暗杠
	//memcpy(over.m_mgang, m_diangang, sizeof(over.m_mgang));//明杠
	//memcpy(over.m_dgang, m_adiangang, sizeof(over.m_dgang));//点杠
	//memset(over.m_gsgang,0,sizeof(over.m_gsgang));
	//memcpy(over.m_mbgang, m_minggang, sizeof(over.m_mbgang));
	memcpy(over.m_startHu, m_startHuRet, sizeof(over.m_startHu));
	memcpy(over.m_agang, m_resultAnGangScore, sizeof(over.m_agang));//暗杠
	memcpy(over.m_mgang, m_resultMingGangScore, sizeof(over.m_mgang));//明杠
	memcpy(over.m_dgang, m_resultADianGangScore, sizeof(over.m_dgang));//点杠

	// 计算过手杠
// 	for(int i = 0; i < 4; ++i)
// 	{
// 		Lint count = 0;
// 		for(Lsize j = 0; j < m_gangCard[i].size(); j+=4)
// 		{
// 			if(isGuoHandGang(m_gangCard[i][j],i))
// 			{
// 				++count;
// 			}
// 		}
// 	//	over.m_gsgang[i] = count;
// 	//	over.m_mbgang[i] -= count;
// 	}

	/*for(Lsize i = 0; i < m_desk->m_vip->m_playtype.size(); ++i)
	{
	over.m_playType.push_back(m_desk->m_vip->m_playtype[i]);
	}*/

	for (Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		over.m_count[i] = m_handCard[i].size();

		//手牌
		for (Lint j = 0; j < over.m_count[i]; ++j)
		{
			over.m_card[i][j].m_color = m_handCard[i][j]->m_color;
			over.m_card[i][j].m_number = m_handCard[i][j]->m_number;
		}

		//胡牌类型
		if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
		{
			for (Lsize k = 0; k < m_thinkRet[i].m_hu.size(); ++k)
			{
				over.m_hu[i].push_back(m_thinkRet[i].m_hu[k]);
			}
		}
	}

	CardValue CardFlop;
	CardFlop.m_color = m_ghostCardReal[0].m_color;
	CardFlop.m_number = m_ghostCardReal[0].m_number;
	over.m_ghostCard.push_back(CardFlop);

	m_desk->SetDeskWait();

	//保存结果
	Lint m_checkTing[DESK_USER_COUNT] = {0};
	m_desk->m_vip->AddLog(m_desk->m_user, gold, m_playerHuInfo, zhuangPos, m_angang, mgang, m_video.m_Id, m_checkTing, m_pt_pao?m_zhuangChangeCount==4 : true);

	//if(m_zhuangChangeCount == 4)
	//	m_zhuangChangeCount = 0;

	//是否最后一局
	over.m_end = m_desk->m_vip->isEnd()?1:0;
	CardValue* lastCard = m_deskCard.front();
	CardValue tmpLastCard;
	tmpLastCard.m_color = lastCard->m_color;
	tmpLastCard.m_number = lastCard->m_number;
	over.m_lastCard = tmpLastCard;
	m_desk->BoadCast(over);

	if(result != WIN_DISS)
	{
		LMsgL2LDBTaskInfo task;
		task.m_zhuang = m_desk->m_user[zhuangPos]->m_userData.m_id;
		task.m_create = m_desk->m_vip->m_round == 1 ? 1 : 0;  // 上面 m_desk->m_vip->AddLog 之后导致 m_round增加 所以判断是否为1 来确定算开房次数

		for(int i=0; i<4; ++i)
		{
			if (m_desk->m_user[i] != NULL)
			{
				task.m_userId[i] = m_desk->m_user[i]->m_userData.m_id;
				task.m_hu[i] = over.m_win[i];
			}
		}

		gWork.SendMsgToDb(task);
	}

	m_desk->HanderGameOver(result);
	
	//////////////////////////////////////////////////////////////////////////
}

Lint GH_WuHan_XianTao::getCurPos()
{
	return m_curPos;
}

void GH_WuHan_XianTao::DeakCard()
{		
	if (!m_desk || !m_desk->m_vip)
	{
		return;
	}

	//发牌   
// 	if(gConfig.GetDebugModel() && (m_desk->m_specialCard[0].m_color > 0 && m_desk->m_specialCard[0].m_number > 0))   //玩家指定发牌 牌局
// 	{
// 		gCB_WuHan_XianTao->DealCard2(m_handCard[0],m_handCard[1],m_handCard[2],m_handCard[3],m_deskCard,m_desk->m_specialCard, m_playtype.gameNeedWind());
// 	}
// 	else                //正常随机发牌 牌局
// 	{
// 		gCB_WuHan_XianTao->DealCard(m_handCard[0],m_handCard[1],m_handCard[2],m_handCard[3],m_deskCard, m_playtype.gameNeedWind());
// 	}
	
	Card* special = gConfig.GetDebugModel() && m_desk->m_specialCard[0].isValid() ? m_desk->m_specialCard : nullptr;
	if (DESK_USER_COUNT == m_desk->GetPlayerCapacity())
	{
		gCB_WuHan_XianTao->DealCard(m_handCard[0], m_handCard[1], m_handCard[2], m_handCard[3], m_deskCard, special, m_pt_feng);
	}
	else
	{
		gCB_WuHan_XianTao->DealCardThree(m_handCard[0], m_handCard[1], m_handCard[2], m_handCard[3], m_deskCard, special, m_pt_feng);
	}

	std::stringstream str;
	for(Lint i=0; i<DESK_USER_COUNT; ++i)
	{
		for(Lint j=0; j<m_handCard[i].size(); ++j)
		{
			str<<m_handCard[i][j]->GetNCIndex()<<" ";
		}
		str<<"\n";
	}
	for(Lint i=0; i<m_deskCard.size(); ++i)
	{
		str<<m_deskCard[i]->GetNCIndex()<<" ";
	}
	LLOG_DEBUG("DeakCard deskid:%d\n%s", m_desk->GetDeskId(), str.str().c_str());

	//翻混牌
	if(m_pt_hun)
		InitHunCard();

	//庄家多发一张牌
	//Card* newCard = m_deskCard.back();
	//m_handCard[m_curPos].push_back(newCard);
	//m_deskCard.pop_back();
	gCB_WuHan_XianTao->SortCard(m_handCard[m_curPos]);
	if (m_desk->GetPlayerCapacity() != DESK_USER_COUNT)
	{
		m_cardPos1 = L_Rand(0, 2);
	}
	else
	{
		m_cardPos1 = L_Rand(0, 3);
	}
	m_cardPos2 = L_Rand(1, 6);
	m_cardPos3 = L_Rand(1, 6);

	//发送消息给客户端
	for(Lint i = 0 ; i < m_desk->GetPlayerCapacity(); ++i)
	{
		if(m_desk->m_user[i] != NULL)
		{
			LMsgS2CPlayStart msg;
			msg.m_zhuang = m_curPos;
			msg.m_pos = i;
			msg.m_dCount = (Lint)m_deskCard.size();
			msg.m_cardPos1 = m_cardPos1;
			msg.m_cardPos2 = m_cardPos2;
			msg.m_cardPos3 = m_cardPos3;

			for(Lsize j = 0 ; j < m_handCard[i].size(); ++j)
			{
				msg.m_cardValue[j].m_number = m_handCard[i][j]->m_number;
				msg.m_cardValue[j].m_color = m_handCard[i][j]->m_color;
			}

			for(Lint j = 0 ; j < DESK_USER_COUNT; ++j)
			{
				msg.m_cardCount[j] = m_handCard[j].size();
			}

			if (i != m_curPos)
			{
				std::vector<TingCard> winCards;
				int oldPlayPos = m_gameInfo.m_playerPos;				// 赖子测试的时候会判断outCard是否是自己的 所以这里临时改为自己
				m_gameInfo.m_playerPos = m_gameInfo.m_MePos;

				gCB_WuHan_XianTao->CheckTing(m_handCard[i], m_pengCard[i], m_abombCard[i], m_gangCard[i], m_eatCard[i], m_gameInfo, winCards, nullptr);

				m_gameInfo.m_playerPos = oldPlayPos;

				if(winCards.size() > 0)
				{
					msg.m_tingCard.reserve(winCards.size());
					for(auto it=winCards.begin(); it != winCards.end(); ++it)
					{
						TingData::TingCard tc;
						tc.m_card.m_color = it->m_card.m_color;
						tc.m_card.m_number = it->m_card.m_number;
						tc.m_hufan = it->m_hufan;
						msg.m_tingCard.push_back(tc);
					}
				}
			}

			m_desk->m_user[i]->Send(msg);
		}
	}

	//SetPlaying();
	//录像功能
	Lint id[4];
	Lint score[4];
	std::vector<CardValue> vec[4];
	for (Lint i = 0; i < m_desk->GetPlayerCapacity(); ++i)
	{
		if (m_desk->m_user[i] == NULL)
		{
			LLOG_ERROR("user is null, i = %d.", i);
			continue;
		}
		id[i] = m_desk->m_user[i]->GetUserDataId();
		score[i] = m_desk->m_vip->GetUserScore(m_desk->m_user[i]);
		for (Lsize j = 0; j < m_handCard[i].size(); ++j)
		{
			CardValue v;
			v.m_color = m_handCard[i][j]->m_color;
			v.m_number = m_handCard[i][j]->m_number;
			vec[i].push_back(v);
		}
	}
	m_video.DealCard(id, vec,gWork.GetCurTime().Secs(),m_zhuangpos,score,m_desk->GetDeskId(),m_desk->m_vip->m_curCircle,m_desk->m_vip->m_maxCircle,m_desk->getGameType(), m_desk->getPlayType());
}

void GH_WuHan_XianTao::CheckStartPlayCard()
{
	m_desk->setDeskPlayState(DESK_PLAY_GET_CARD);
	SetPlayIng(m_curPos,true, false, true, true);
	m_curGetCard = m_handCard[m_curPos].back();
	m_needGetCard = true;
}
//摸牌
void GH_WuHan_XianTao::SetPlayIng(Lint pos,bool needGetCard,bool gang, bool needThink, bool canhu)
{
	if (m_desk == NULL )
	{
		LLOG_ERROR("SetPlayIng NULL ERROR ");
		return;
	}
	if (pos < 0 || pos >= INVAILD_POS)
	{
		LLOG_ERROR("SetPlayIng Pos Error ! %d ", pos);
		return;
	}
	//穿庄
	if(needGetCard && isDeskCardEmpty())//(needGetCard && m_deskCard.empty())
	{
		LLOG_INFO("Desk::SetPlayIng huangzhuang game over");
		OnGameOver(WIN_NONE, INVAILD_POS);
		
		return;
	}
	if (m_curPos != pos)
	{
		m_beforePos = m_curPos;
	}

	m_curPos = pos;

	// 取消过手胡限制D
	m_guoShouHu[pos] = false;

	//我摸牌思考信息
	m_thinkInfo[pos].m_thinkData.clear();
	m_desk->setDeskPlayState(DESK_PLAY_GET_CARD);
	m_needGetCard = false;
	if (needGetCard)
	{
		m_needGetCard = true;
		m_curGetCard = m_deskCard.back();
		m_deskCard.pop_back();
		//录像
		std::vector<CardValue> cards;
		CardValue card;
		card.m_color = m_curGetCard->m_color;
		card.m_number = m_curGetCard->m_number;
		cards.push_back(card);
		m_video.AddOper(VIDEO_OPER_GET_CARD, pos, cards);
	}
	LLOG_DEBUG("needGetCard:%d, needThink:%d.", needGetCard, needThink);
	if (needThink)
	{
		m_gameInfo.m_pGscard = NULL;
		m_gameInfo.b_canHu = canhu;		// 是否可以胡
		m_gameInfo.b_onlyHu = false;
		if (m_deskCard.size() <= DESK_USER_COUNT)
		{
			m_gameInfo.b_onlyHu = true;
		}
		m_gameInfo.m_thinkGang = gang;	// 单独处理是不是杠的牌
		m_gameInfo.m_deskState = m_desk->getDeskPlayState();	// 当前局牌状态
		m_gameInfo.m_playerPos = m_curPos;	// 当前一个出牌位置
		m_gameInfo.m_cardState = THINK_OPERATOR_NULL;	// 当前一个出牌状态
		m_gameInfo.m_endStartPos = m_endStartPos;	// 结束位置，即海底位置
		m_gameInfo.m_MePos = pos;		// 玩家的位置
		m_gameInfo.bNoCard = isDeskCardEmpty();//m_deskCard.empty();
		//m_gameInfo.m_playtype = m_playtype;
		m_gameInfo.m_tingCard = &m_tingCard[pos];
		m_gameInfo.b_userPlayCard = b_userPlayCard[pos];

		m_thinkInfo[pos].m_thinkData = gCB_WuHan_XianTao->CheckGetCardOperator(m_handCard[pos],m_pengCard[pos],m_abombCard[pos],m_gangCard[pos],m_eatCard[pos],m_curGetCard,m_gameInfo);
		// 记录过手杠
		if(m_gameInfo.m_pGscard)
		{
			m_gsCard[pos].push_back(m_gameInfo.m_pGscard);
		}
		VideoThink(pos);
	}

	{
		m_thinkInfo[pos].m_tingData.clear();
		if (m_tingCard[pos].size() == 0)
		{
			m_thinkInfo[pos].m_tingData = gCB_WuHan_XianTao->CheckGetCardTing(m_handCard[pos], m_pengCard[pos], m_abombCard[pos], m_gangCard[pos], m_eatCard[pos], needGetCard?m_curGetCard:NULL, m_gameInfo);
		}
	}
	if (m_needGetCard)
	{
		m_handCard[pos].push_back(m_curGetCard);
		gCB_WuHan_XianTao->SortCard(m_handCard[pos]);
	}

	for(Lint i = 0 ; i < DESK_USER_COUNT; ++i)
	{
		if(m_desk->m_user[i] != NULL)
		{
			LMsgS2COutCard msg;
			msg.m_time = 15;
			msg.m_pos = pos;
			msg.m_deskCard = (Lint)m_deskCard.size();
			msg.m_gang = m_gangPos[pos];
			msg.m_end = m_desk->getDeskPlayState()==DESK_PLAY_END_CARD?1:0;
			msg.m_flag = 1;
			if(m_needGetCard)
			{
				msg.m_flag = 0;
			}

			if (pos == i)
			{
				if (m_needGetCard)
				{
					msg.m_curCard.m_number = m_curGetCard->m_number;
					msg.m_curCard.m_color = m_curGetCard->m_color;
				}

				for (Lsize j = 0; j < m_thinkInfo[pos].m_thinkData.size(); ++j)
				{
					ThinkData info;
					info.m_type = m_thinkInfo[pos].m_thinkData[j].m_type;
					for (Lsize n = 0; n < m_thinkInfo[pos].m_thinkData[j].m_card.size(); ++n)
					{
						CardValue v;
						v.m_color = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_color;
						v.m_number = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_number;
						info.m_card.push_back(v);
					}
					msg.m_think.push_back(info);
				}

				//添加听牌思考 参照漯河
				if(m_pt_ting && m_thinkInfo[pos].m_tingData.size() > 0)
				{
					ThinkData info;
					info.m_type = THINK_OPERATOR_TINGCARD;
					msg.m_think.push_back(info);
				}

				for (Lsize j = 0; j < m_thinkInfo[pos].m_tingData.size(); ++j)
				{
					TingData info;
					info.m_card.m_color = m_thinkInfo[pos].m_tingData[j].m_card.m_color;
					info.m_card.m_number = m_thinkInfo[pos].m_tingData[j].m_card.m_number;

					for(Lsize n = 0 ; n < m_thinkInfo[pos].m_tingData[j].m_tingcard.size(); ++n)
					{
						TingData::TingCard c;
						c.m_card.m_color = m_thinkInfo[pos].m_tingData[j].m_tingcard[n].m_card.m_color;
						c.m_card.m_number = m_thinkInfo[pos].m_tingData[j].m_tingcard[n].m_card.m_number;
						c.m_hufan = m_thinkInfo[pos].m_tingData[j].m_tingcard[n].m_hufan;
						info.m_tingcard.push_back( c );
					}
					msg.m_ting.push_back(info);
				}
			}

			m_desk->m_user[i]->Send(msg);
		}
	}
}
// 计算玩家得分
void GH_WuHan_XianTao::calcUserScore(Lint result, Lint gold[], Lint& bombCount, Lint winPos[])
{
	//计算杠牌得分
	GetGangSocore(gold);

	GetPaoSocore(result, gold, bombCount, winPos);
}

bool GH_WuHan_XianTao::checkOtherPlayCanHu(int pos)
{
	LLOG_ERROR("m_playGhostCard=%d, ghostCard=%d,%d.", m_playGhostCard, m_ghostCardReal[0].m_color, m_ghostCardReal[0].m_number);
	if (m_playGhostCard)
	{
		return false;
	}

	std::vector<Card *> handcard = m_handCard[pos];
	for (auto it = handcard.begin(); it != handcard.end(); ++it)
	{
		Card* card = *it;
		if (card->m_color == m_ghostCardReal[0].m_color && card->m_number == m_ghostCardReal[0].m_number)
		{
			return false;
		}
	}
	return true;
}

void GH_WuHan_XianTao::SetThinkIng()
{
	if (!m_desk)
	{
		LLOG_ERROR("SetThinkIng m_desk == NULL");
		return;
	}
	bool think = false;
	Lint canHuNumber = 0;

	for(Lint i = 0 ; i < DESK_USER_COUNT;++i)
	{
		m_thinkRet[i].Clear();
		m_thinkInfo[i].Reset();
		if(i != m_curPos)
		{
			m_gameInfo.b_canHu = checkOtherPlayCanHu(i); // 是否可以胡
			m_gameInfo.b_onlyHu = false;
			if (m_deskCard.size() <= m_desk->GetPlayerCapacity())
			{
				m_gameInfo.b_onlyHu = true;
			}
			m_gameInfo.m_thinkGang = false;	// 单独处理是不是杠的牌
			m_gameInfo.m_deskState = m_desk->getDeskPlayState();	// 当前局牌状态
			m_gameInfo.m_playerPos = m_curPos;	// 当前一个出牌位置
			m_gameInfo.m_cardState = m_beforeType;	// 当前一个出牌状态
			m_gameInfo.m_endStartPos = m_endStartPos;	// 结束位置，即海底位置
			m_gameInfo.m_MePos = i;		// 玩家的位置
			m_gameInfo.bNoCard = isDeskCardEmpty();//m_deskCard.empty();
			m_gameInfo.m_tingCard = &m_tingCard[i];
			m_gameInfo.b_userPlayCard = b_userPlayCard[i];

			m_thinkInfo[i].m_thinkData = gCB_WuHan_XianTao->CheckOutCardOperator(m_handCard[i],m_pengCard[i],m_abombCard[i],m_gangCard[i],m_eatCard[i],m_curOutCard,m_gameInfo);
			if (m_thinkInfo[i].NeedThink())
			{
				think = true;
				VideoThink(i);
				
				printThinkInfo(m_thinkInfo[i]);
				ThinkVec thinkVec = m_thinkInfo[i].m_thinkData;
				for (ThinkUnit thinkUnit : thinkVec)
				{
					if (thinkUnit.m_type == THINK_OPERATOR_BOMB)
					{
						m_thinkRet[i].m_type = THINK_OPERATOR_BOMB;
						canHuNumber++;
					}
				}
			}
		}
	}

	// 当canHuNumber=1时，且上一次的结果为明杠和补杠时，直接胡牌
	LLOG_DEBUG("canHuNumber=%d, m_beforeType=%d, m_beforePos=%d.", canHuNumber, m_beforeType, m_beforePos);
	if (canHuNumber == 1 && (m_beforeType == THINK_OPERATOR_AGANG || m_beforeType == THINK_OPERATOR_MGANG || m_beforeType == THINK_OPERATOR_MBU))
	{
		ThinkEnd();
		return;
	}

	if (canHuNumber < 2)
	{
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			m_thinkRet[i].Clear();
		}
	}

	if (think && canHuNumber < 2)
	{
		m_beforePos = m_curPos;
		m_desk->setDeskPlayState(DESK_PLAY_THINK_CARD);
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			LMsgS2CThink think;
			think.m_time = 15;
			think.m_card.m_color = m_curOutCard->m_color;
			think.m_card.m_number = m_curOutCard->m_number;
			if (m_thinkInfo[i].NeedThink())
			{
				think.m_flag = 1;
				for (Lsize j = 0; j < m_thinkInfo[i].m_thinkData.size(); ++j)
				{		
					ThinkData info;
					info.m_type = m_thinkInfo[i].m_thinkData[j].m_type;
					for(Lsize n = 0 ; n < m_thinkInfo[i].m_thinkData[j].m_card.size(); ++n)
					{
						CardValue v;
						v.m_color = m_thinkInfo[i].m_thinkData[j].m_card[n]->m_color;
						v.m_number = m_thinkInfo[i].m_thinkData[j].m_card[n]->m_number;
						info.m_card.push_back(v);
					}
					think.m_think.push_back(info);
				}
				if(m_beforeType == THINK_OPERATOR_MBU)
				{
					m_qiangganghu = 1;
				}
			}
			else
			{
				think.m_flag = 0;
			}
			if (m_desk->m_user[i] != NULL)
			{
				m_desk->m_user[i]->Send(think);
			}
		}
	}
	else
	{
		ThinkEnd();
	}
}

void GH_WuHan_XianTao::CheckThink()
{
	bool hu		= false;
	bool Peng	= false;
	bool Chi	= false;
	bool Gang	= false;
	bool anGang = false;
	bool Bu		= false;
	bool hu_New		= false;
	bool Peng_New	= false;
	bool Gang_New	= false;
	bool anGang_New = false;
	bool Bu_New		= false;
	for (Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		//
		if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)			hu		= true;
		else if (m_thinkRet[i].m_type == THINK_OPERATOR_PENG)		Peng	= true;
		else if (m_thinkRet[i].m_type == THINK_OPERATOR_MGANG)		Gang	= true;
		else if (m_thinkRet[i].m_type == THINK_OPERATOR_AGANG)		anGang  = true;
		else if (m_thinkRet[i].m_type == THINK_OPERATOR_MBU)		Bu		= true;

		//
		if(m_thinkInfo[i].NeedThink())
		{
			if (m_thinkInfo[i].HasHu())				hu_New		= true;
			else if (m_thinkInfo[i].HasPeng())		Peng_New	= true;
			else if (m_thinkInfo[i].HasMGang())		Gang_New	= true;
			else if (m_thinkInfo[i].HasAnGang())	anGang_New  = true;
			else if (m_thinkInfo[i].HasMBu())		Bu_New		= true;
		}
	}

	LLOG_DEBUG("hu=%d,peng=%d,gang=%d,angang=%d,bu=%d.", hu, Peng, Gang, anGang, Bu);
	LLOG_DEBUG("hu_new=%d,peng_new=%d,gang_new=%d,angang_new=%d,bu_new=%d.", hu_New, Peng_New, Gang_New, anGang_New, Bu_New);
	bool think = false;

	if (hu_New)	
		think = true;
	else
	{
		if (!hu)
		{
			if (Peng_New || Gang_New || Bu_New || anGang_New)
				think = true;
		}
	}

	if (!think)
		ThinkEnd();
}

void GH_WuHan_XianTao::ThinkEnd()
{
	for(int i = 0; i < DESK_USER_COUNT; ++i)
	{
		m_thinkInfo[i].Reset();
		m_guoShouHu[i] = false;
	}
	Lint huCount = 0;

	Lint pengPos = INVAILD_POS;
	Lint gangPos = INVAILD_POS;
	Lint anGangPos = INVAILD_POS;
	Lint buPos = INVAILD_POS;

	for (Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
		{
			m_playerHuInfo[i].type = WIN_SUB_BOMB;
			m_playerHuInfo[i].abombpos = m_beforePos;
			huCount++;
		}

		if (m_thinkRet[i].m_type == THINK_OPERATOR_MGANG)
			gangPos = i;

		if (m_thinkRet[i].m_type == THINK_OPERATOR_AGANG)
			anGangPos = i;

		if (m_thinkRet[i].m_type == THINK_OPERATOR_MBU)
			buPos = i;

		if (m_thinkRet[i].m_type == THINK_OPERATOR_PENG)
			pengPos = i;
	}

	m_qiangganghu = 0;
	LLOG_DEBUG("m_curPos=%d, m_beforePos=%d.", m_curPos, m_beforePos);
	LLOG_DEBUG("ThinkEnd, huCount=%d,gangPos=%d,angangPos=%d,buPos=%d,pengPs=%d.", huCount, gangPos, anGangPos, buPos, pengPos);
	if (huCount != 0)
	{
		//这里有人胡牌
		if (m_beforeType == THINK_OPERATOR_AGANG || m_beforeType == THINK_OPERATOR_MGANG || m_beforeType == THINK_OPERATOR_MBU)
		{
			// 这里可以先把所有胡牌的人筛选出来，然后统一计算分数，发送消息给前台

			std::vector<Lint> huVec;
			//录像
			for (int i = 0; i < DESK_USER_COUNT; ++i)
			{
				if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
				{
					huVec.push_back(i);
					for(Lsize n=0;n<m_thinkRet[i].m_card.size();++n)
					{
						CardValue curOutCard;
						curOutCard.m_color = m_thinkRet[i].m_card[n]->m_color;
						curOutCard.m_number = m_thinkRet[i].m_card[n]->m_number;
						m_playerHuInfo[i].wincards.push_back(curOutCard);
					}
					LLOG_DEBUG("Begin to send msg.");
					LMsgS2CUserOper send;
					send.m_pos = i;
					send.m_errorCode = 0;
					send.m_beforePos = m_beforePos;
					ThinkData tmpThinkData;
					tmpThinkData.m_type = THINK_OPERATOR_BOMB;
					std::vector<CardValue> tmpCardVector;
					CardValue curOutCard;
					curOutCard.m_color = m_curOutCard->m_color;
					curOutCard.m_number = m_curOutCard->m_number;
					tmpCardVector.push_back(curOutCard);
					tmpThinkData.m_card = tmpCardVector;
					send.m_think = tmpThinkData;
					send.m_card.m_color = (m_curOutCard == NULL) ? 1 : m_curOutCard->m_color;		//临时的 有错误
					send.m_card.m_number = (m_curOutCard == NULL) ? 1 : m_curOutCard->m_number;		//临时的 有错误
					//send.m_hu = m_thinkRet[i].m_hu;
					send.m_cardCount = m_handCard[i].size();
					for (Lint j = 0; j < send.m_cardCount; ++j)
					{
						CardValue mCard;
						mCard.m_color = m_handCard[i][j]->m_color;
						mCard.m_number = m_handCard[i][j]->m_number;
						send.m_cardValue.push_back(mCard);
					}

					// 此处不计算分，后面统一发算分消息
					// int score = 2 * m_desk->m_baseScore * m_baseScore[m_curPos] * m_baseScore[m_beforePos];
					// send.m_addScore[m_curPos] = score;
					// send.m_addScore[m_beforePos] = -score;

					m_desk->BoadCast(send);
					
					m_video.AddOper(VIDEO_OPER_SHOUPAO, i, m_playerHuInfo[i].wincards);
				}
			}

			LLOG_DEBUG("beforeScore=%d,%d,%d,%d.", m_beforeOpScore[0], m_beforeOpScore[1], m_beforeOpScore[2], m_beforeOpScore[3]);

			int huScore[4] = { 0 };
			// 遍历所有人，如果此人胡牌，则按照正常扣分
			// 如果此人不胡牌，则直接返还积分
			// 如果此人是上一个出牌人，则扣除双倍分数
			for (int m = 0; m < DESK_USER_COUNT; m++)
			{
				bool hasFound = false;
				for (int n = 0; n < huVec.size(); n++)
				{
					int tmpCurPos = huVec[n];
					if (tmpCurPos == m)
					{
						hasFound = true;
						int tmpScore = 0;
						if (hasFourLaizi(tmpCurPos))
						{
							tmpScore = 4 * m_desk->m_baseScore * m_baseScore[tmpCurPos] * m_baseScore[m_beforePos];
						}
						else
						{
							tmpScore = 2 * m_desk->m_baseScore * m_baseScore[tmpCurPos] * m_baseScore[m_beforePos];
						}
						huScore[tmpCurPos] = m_beforeOpScore[m_beforePos] + tmpScore - m_beforeOpScore[tmpCurPos];
						huScore[m_beforePos] = huScore[m_beforePos] - tmpScore - m_beforeOpScore[m_beforePos];
					}
				}

				if (!hasFound)
				{
					if (m == m_beforePos)
					{
						huScore[m] = huScore[m] - m_beforeOpScore[m];
					}
					else
					{
						huScore[m] = -m_beforeOpScore[m];
					}
				}
			}
			LLOG_DEBUG("score=%d,%d,%d,%d.", huScore[0], huScore[1], huScore[2], huScore[3]);

			for (int i = 0; i < DESK_USER_COUNT; ++i)
			{
				LMsgS2CUserOper send;
				send.m_pos = i;
				send.m_errorCode = 0;
				send.m_beforePos = m_beforePos;
				ThinkData tmpThinkData;
				tmpThinkData.m_type = THINK_OPERATOR_QIANGANGH;
				std::vector<CardValue> tmpCardVector;
				CardValue curOutCard;
				curOutCard.m_color = m_curOutCard->m_color;
				curOutCard.m_number = m_curOutCard->m_number;
				tmpCardVector.push_back(curOutCard);
				tmpThinkData.m_card = tmpCardVector;
				send.m_think = tmpThinkData;
				send.m_card.m_color = (m_curOutCard == NULL) ? 1 : m_curOutCard->m_color;		//临时的 有错误
				send.m_card.m_number = (m_curOutCard == NULL) ? 1 : m_curOutCard->m_number;
				
				for (int j = 0; j < DESK_USER_COUNT; j++)
				{
					send.m_addScore[j] = huScore[j];
				}

				m_desk->BroadcastWithoutUser(send, i);
			}

			for (int j = 0; j < m_desk->GetPlayerCapacity(); j++)
			{
				m_totalScore[j] += huScore[j];
			}

			OnGameOver(WIN_BOMB, m_beforePos);
		}
		else
		{
			for (int i = 0; i < m_desk->GetPlayerCapacity(); ++i)
			{
				if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
				{
					std::vector<CardValue> cards;
					CardValue curOutCard;
					curOutCard.m_color = m_curOutCard->m_color;
					curOutCard.m_number = m_curOutCard->m_number;
					m_playerHuInfo[i].wincards.push_back(curOutCard);

					LLOG_DEBUG("Begin to send msg.");
					LMsgS2CUserOper send;
					send.m_pos = i;
					send.m_errorCode = 0;
					send.m_beforePos = m_beforePos;
					ThinkData tmpThinkData;
					tmpThinkData.m_type = THINK_OPERATOR_BOMB;
					std::vector<CardValue> tmpCardVector;
					tmpCardVector.push_back(curOutCard);
					tmpThinkData.m_card = tmpCardVector;
					send.m_think = tmpThinkData;
					send.m_card.m_color = (m_curOutCard == NULL) ? 1 : m_curOutCard->m_color;		//临时的 有错误
					send.m_card.m_number = (m_curOutCard == NULL) ? 1 : m_curOutCard->m_number;		//临时的 有错误
																									//send.m_hu = m_thinkRet[i].m_hu;
					send.m_cardCount = m_handCard[i].size();
					for (Lint j = 0; j < send.m_cardCount; ++j)
					{
						CardValue mCard;
						mCard.m_color = m_handCard[i][j]->m_color;
						mCard.m_number = m_handCard[i][j]->m_number;
						send.m_cardValue.push_back(mCard);
					}

					int score = 0;
					if (hasFourLaizi(i))
					{
						score = 4 * m_desk->m_baseScore * m_baseScore[i] * m_baseScore[m_beforePos];
					}
					else
					{
						score = 2 * m_desk->m_baseScore * m_baseScore[i] * m_baseScore[m_beforePos];
					}
					send.m_addScore[i] = score;
					send.m_addScore[m_beforePos] = -score;

					m_totalScore[i] += score;
					m_totalScore[m_beforePos] += -score;

					m_desk->BoadCast(send);

					m_video.AddOper(VIDEO_OPER_SHOUPAO, i, m_playerHuInfo[i].wincards);
				}
			}
			OnGameOver(WIN_BOMB, m_beforePos);
		}
		return;
	}

	//杠
	if (gangPos != INVAILD_POS)
	{
		LMsgS2CUserOper send;
		send.m_pos = gangPos;
		send.m_errorCode = 0;
		send.m_beforePos = m_beforePos;

		//明杠底分不翻倍，只是分数翻倍
		int score = m_desk->m_baseScore*m_baseScore[m_curPos] * m_baseScore[m_beforePos];
		send.m_addScore[m_curPos] = score;
		send.m_addScore[m_beforePos] = -score;
		
		m_totalScore[m_curPos] += score;
		m_totalScore[m_beforePos] += -score;

		// 设置当前操作分数，考虑到多次杠的场景，此处使用
		m_beforeOpScore[m_curPos] = m_beforeOpScore[m_curPos] + score; 
		m_beforeOpScore[m_beforePos] = m_beforeOpScore[m_beforePos] - score;

		send.m_think.m_type = m_thinkRet[gangPos].m_type;
		for(Lsize i = 0 ; i < m_thinkRet[gangPos].m_card.size(); ++i)
		{
			CardValue v;
			v.m_color = m_thinkRet[gangPos].m_card[i]->m_color;
			v.m_number = m_thinkRet[gangPos].m_card[i]->m_number;
			send.m_think.m_card.push_back(v);
		}
		send.m_card.m_color = m_curOutCard->m_color;
		send.m_card.m_number = m_curOutCard->m_number;
		m_desk->BoadCast(send);
		gCB_WuHan_XianTao->EraseCard(m_handCard[gangPos], m_thinkRet[gangPos].m_card[0],3);

		//录像
		std::vector<CardValue> cards;
		for (int i = 0; i < 4; ++i)
		{
			CardValue card;
			card.m_color = m_curOutCard->m_color;
			card.m_number = m_curOutCard->m_number;
			cards.push_back(card);
			m_gangCard[gangPos].push_back(m_curOutCard);
		}
		m_video.AddOper(VIDEO_OPER_GANG, gangPos, cards);

		m_diangang[gangPos] += 1;
		m_adiangang[m_beforePos] += 1;
		m_mingGangInfo[gangPos].push_back(m_beforePos);

		PengInfo pengInfo;
		pengInfo.card.m_color = m_curOutCard->m_color;
		pengInfo.card.m_number = m_curOutCard->m_number;
		pengInfo.pos = m_beforePos;
		m_gangInfo[gangPos].push_back(pengInfo);

		for (int i = 0; i < DESK_USER_COUNT; ++i)
		{
			m_thinkRet[i].Clear();
		}
		//给玩家摸一张牌
		SetPlayIng(gangPos, true, true, true, true);
		return;
	}

	//杠
	if (anGangPos != INVAILD_POS)
	{
		LMsgS2CUserOper send;
		send.m_pos = anGangPos;
		send.m_errorCode = 0;
		send.m_beforePos = m_beforePos;

		send.m_think.m_type = m_thinkRet[anGangPos].m_type;
		for (Lsize i = 0; i < m_thinkRet[anGangPos].m_card.size(); ++i)
		{
			CardValue v;
			v.m_color = m_thinkRet[anGangPos].m_card[i]->m_color;
			v.m_number = m_thinkRet[anGangPos].m_card[i]->m_number;
			send.m_think.m_card.push_back(v);
		}
		send.m_card.m_color = m_curOutCard->m_color;
		send.m_card.m_number = m_curOutCard->m_number;
		
		//只有打癞子倍数翻倍，其他最多扣分翻倍
		int score = 0;
		for (int i = 0; i < m_desk->GetPlayerCapacity(); ++i)
		{
			if (i == m_curPos)
			{
				continue;
			}
			//暗杠时只是分数翻倍，低分不翻倍
			int tmpScore = 2*m_desk->m_baseScore*m_baseScore[i]*m_baseScore[m_curPos];
			send.m_addScore[i] = -tmpScore;
			m_totalScore[i] += -tmpScore;
			
			m_beforeOpScore[i] = m_beforeOpScore[i] - tmpScore;
			score += tmpScore;
		}

		send.m_addScore[m_curPos] = score;
		m_totalScore[m_curPos] += score;
		
		m_beforeOpScore[m_curPos] = m_beforeOpScore[m_curPos] + score;

		m_desk->BoadCast(send);
		gCB_WuHan_XianTao->EraseCard(m_handCard[anGangPos], m_thinkRet[anGangPos].m_card[0], 4);

		//录像
		std::vector<CardValue> cards;
		for (int i = 0; i < 4; ++i)
		{
			CardValue card;
			card.m_color = m_curOutCard->m_color;
			card.m_number = m_curOutCard->m_number;
			cards.push_back(card);
			m_gangCard[anGangPos].push_back(m_curOutCard);
		}

		m_video.AddOper(VIDEO_OPER_GANG, anGangPos, cards);

		m_diangang[anGangPos] += 1;
		m_adiangang[m_beforePos] += 1;
		m_mingGangInfo[anGangPos].push_back(m_beforePos);

		PengInfo pengInfo;
		pengInfo.card.m_color = m_curOutCard->m_color;
		pengInfo.card.m_number = m_curOutCard->m_number;
		pengInfo.pos = m_beforePos;
		m_gangInfo[anGangPos].push_back(pengInfo);

		for (int i = 0; i < DESK_USER_COUNT; ++i)
		{
			m_thinkRet[i].Clear();
		}
		//给玩家摸一张牌
		SetPlayIng(anGangPos, true, true, true, true);
		return;
	}

	//补
	if (buPos != INVAILD_POS)
	{
		LMsgS2CUserOper send;
		send.m_pos = buPos;
		send.m_beforePos = m_beforePos;
		send.m_errorCode = 0;
		send.m_think.m_type = m_thinkRet[buPos].m_type;
		for(Lsize i = 0 ; i < m_thinkRet[buPos].m_card.size(); ++i)
		{
			CardValue v;
			v.m_color = m_thinkRet[buPos].m_card[i]->m_color;
			v.m_number = m_thinkRet[buPos].m_card[i]->m_number;
			send.m_think.m_card.push_back(v);
		}
		send.m_card.m_color = m_curOutCard->m_color;
		send.m_card.m_number = m_curOutCard->m_number;

		int score = 0;
		for (int i = 0; i < m_desk->GetPlayerCapacity(); ++i)
		{
			if (i == m_curPos)
			{
				continue;
			}
			//补杠所有人都扣分，但是不翻倍
			int tmpScore = m_desk->m_baseScore*m_baseScore[i] * m_baseScore[m_curPos];
			send.m_addScore[i] = -tmpScore;
			m_totalScore[i] += -tmpScore;
			
			m_beforeOpScore[i] = m_beforeOpScore[i] - tmpScore;
			score += tmpScore;
		}

		send.m_addScore[m_curPos] = score;
		m_totalScore[m_curPos] += score;
		
		m_beforeOpScore[m_curPos] = m_beforeOpScore[m_curPos] + score;

		m_desk->BoadCast(send);
		gCB_WuHan_XianTao->EraseCard(m_handCard[buPos], m_thinkRet[buPos].m_card[0],3);

		//录像
		std::vector<CardValue> cards;
		for (int i = 0; i < 4; ++i)
		{
			CardValue card;
			card.m_color = m_curOutCard->m_color;
			card.m_number = m_curOutCard->m_number;
			cards.push_back(card);
			m_gangCard[buPos].push_back(m_curOutCard);
		}
		m_video.AddOper(VIDEO_OPER_OTHER_BU, buPos, cards);

		m_diangang[buPos] += 1;
		m_adiangang[m_beforePos] += 1;
		m_mingGangInfo[buPos].push_back(m_beforePos);

		PengInfo pengInfo;
		pengInfo.card.m_color = m_curOutCard->m_color;
		pengInfo.card.m_number = m_curOutCard->m_number;
		pengInfo.pos = m_beforePos;
		m_gangInfo[buPos].push_back(pengInfo);

		for (int i = 0; i < DESK_USER_COUNT; ++i)
		{
			m_thinkRet[i].Clear();
		}
		//给玩家摸一张牌
		SetPlayIng(buPos, true, false, true, true);
		return;
	}

	if (pengPos != INVAILD_POS)
	{
		LMsgS2CUserOper send;
		send.m_pos = pengPos;
		send.m_beforePos = m_beforePos;
		send.m_errorCode = 0;
		send.m_think.m_type = m_thinkRet[pengPos].m_type;
		for(Lsize i = 0 ; i < m_thinkRet[pengPos].m_card.size(); ++i)
		{
			CardValue v;
			v.m_color = m_thinkRet[pengPos].m_card[i]->m_color;
			v.m_number = m_thinkRet[pengPos].m_card[i]->m_number;
			send.m_think.m_card.push_back(v);
		}
		send.m_card.m_color = m_curOutCard->m_color;
		send.m_card.m_number = m_curOutCard->m_number;

		if (m_curOutCard->m_color == m_ghostCardFlop.m_color && m_curOutCard->m_number == m_ghostCardFlop.m_number)
		{
			// 小朝天和明杠一样处理
			int score = m_desk->m_baseScore*m_baseScore[m_curPos] * m_baseScore[m_beforePos];
			send.m_addScore[m_curPos] = score;
			send.m_addScore[m_beforePos] = -score;

			m_totalScore[m_curPos] += score;
			m_totalScore[m_beforePos] += -score;
		}
		m_desk->BoadCast(send);
		gCB_WuHan_XianTao->EraseCard(m_handCard[pengPos], m_curOutCard,2);

		{
			PengInfo pengInfo;
			pengInfo.card.m_color = m_curOutCard->m_color;
			pengInfo.card.m_number = m_curOutCard->m_number;
			pengInfo.pos = m_beforePos;
			m_pengInfo[pengPos].push_back(pengInfo);
		}
		

		//录像
		std::vector<CardValue> cards;
		for (int i = 0; i < 3; ++i)
		{
			CardValue card;
			card.m_color = m_curOutCard->m_color;
			card.m_number = m_curOutCard->m_number;
			cards.push_back(card);
			m_pengCard[pengPos].push_back(m_curOutCard);
		}
		m_video.AddOper(VIDEO_OPER_PENG_CARD, pengPos, cards);


		for (int i = 0; i < DESK_USER_COUNT; ++i)
		{
			m_thinkRet[i].Clear();
		}
		//碰完打一张牌
		m_curGetCard = NULL;
		SetPlayIng(pengPos, false, false, true, false);
		m_needGetCard = true;
		return;
	}
	LLOG_DEBUG("m_beforePos=%d,m_curPos=%d,m_beforeType=%d.", m_beforePos, m_curPos, m_beforeType);
	
	//这里没有人操作 自己明杠 其他玩家没有抢杠胡的 就会走到这儿
	if (m_curOutCard != NULL)
	{
		m_outCard[m_curPos].push_back(m_curOutCard);
	}
	LLOG_DEBUG("m_curOutCard:%d,%d;m_ghostCard=%d,%d.", m_curOutCard->m_color, m_curOutCard->m_number, m_ghostCardReal[0].m_color, m_ghostCardReal[0].m_number);
	if (m_curOutCard->m_color == m_ghostCardReal[0].m_color && m_curOutCard->m_number == m_ghostCardReal[0].m_number)
	{
		SetPlayIng(m_curPos, true, false, true, true);
	}
	else
	{
		m_beforeType = THINK_OPERATOR_OUT;
		for (int i = 0; i < DESK_USER_COUNT; i++)
		{
			m_beforeOpScore[i] = 0;
		}
		SetPlayIng(m_desk->GetNextPos(m_curPos), true, false, true, true);
	}
}

void GH_WuHan_XianTao::VideoThink(Lint pos)
{
	if(m_thinkInfo[pos].m_thinkData.size() >0)
	{
		std::vector<CardValue> cards;
		for (auto itr=m_thinkInfo[pos].m_thinkData.begin(); itr!=m_thinkInfo[pos].m_thinkData.end(); ++itr)
		{
			CardValue card;
			card.m_number = itr->m_type;
			if (itr->m_card.size()>0)
				card.m_color  = itr->m_card[0]->m_color*10+itr->m_card[0]->m_number;
			if (itr->m_card.size()>1)
				card.m_color  = card.m_color*1000+itr->m_card[1]->m_color*10+itr->m_card[1]->m_number;
			cards.push_back(card);
		}
		m_video.AddOper(VIDEO_OPEN_THINK, pos, cards);
	}
}

void GH_WuHan_XianTao::VideoDoing(Lint op, Lint pos, Lint card_color, Lint card_number)
{
	std::vector<CardValue> cards;
	CardValue card;
	card.m_number = op;
	card.m_color  = card_color*10+card_number;
	cards.push_back(card);
	m_video.AddOper(VIDEO_OPEN_DOING, pos, cards);

}

bool GH_WuHan_XianTao::isGuoHandGang(Card* card,Lint pos)
{
	if(!card || pos < 0 || pos >= 4)
	{
		//printf("error...\n");
		return false;
	}

	for(size_t i = 0; i < m_gsCard[pos].size(); ++i)
	{
		if(*m_gsCard[pos][i] == *card)
		{
			return true;
		}
	}
	return false;
}

void GH_WuHan_XianTao::InitHunCard()
{
	if(m_gameInfo.m_pt_laizi)//(m_playtype.GametypeGetGhost())
	{
		Card* card = m_deskCard.back();
		m_deskCard.pop_back();
		if (gConfig.GetDebugModel() && m_desk->m_specialCard[0].isValid()){
			LLOG_DEBUG("Set special card from clinet.");
			card = m_desk->m_specialCard;
		}

		if (card)
		{
			m_ghostCardFlop = *card;

			CardValue ghost1;
			ghost1.m_color = card->m_color;
			if (card->m_number == 9)
			{
				ghost1.m_number = 1;
			}
			else
			{
				ghost1.m_number = card->m_number + 1;
			}

			m_ghostCardReal.clear();
			m_ghostCardReal.push_back(ghost1);

			m_gameInfo.m_hCard.clear();
			m_gameInfo.m_hCard.push_back(ghost1);

			//录像
			std::vector<CardValue> cards;
			CardValue cardValue;
			cardValue.m_color = ghost1.m_color;
			cardValue.m_number = ghost1.m_number;
			cards.push_back(cardValue);
			m_video.AddOper(VIDEO_OPER_FANJIN, 0, cards);//需要修改
		}
		
	}
	LLOG_DEBUG("m_ghostCardFlop=%d,%d;m_ghostCardReal=%d,%d;m_hCard=%d,%d.", m_ghostCardFlop.m_color, m_ghostCardFlop.m_color, m_ghostCardReal[0].m_color, m_ghostCardReal[0].m_number, m_gameInfo.m_hCard[0].m_color, m_gameInfo.m_hCard[0].m_number);

	LMsgS2CGhostCard ghoshCard;
	ghoshCard.m_ghostCard.m_color = m_ghostCardFlop.m_color;
	ghoshCard.m_ghostCard.m_number = m_ghostCardFlop.m_number;
	m_desk->BoadCast(ghoshCard);
}

void GH_WuHan_XianTao::BroadcastBottomPour()
{
	LMsgS2CStartBottomPour bottompour;
	bottompour.m_need_bottom = m_pt_pao;
	m_desk->BoadCast(bottompour);
}

bool GH_WuHan_XianTao::IsPouredOver()
{
	int iDingQueCount = 0;
	for(int i = 0; i < DESK_USER_COUNT; ++i)
	{
		if(m_isFly[i] == 0)
			break;
		iDingQueCount++;
	}

	return iDingQueCount == DESK_USER_COUNT;
}

bool GH_WuHan_XianTao::CheckIfBigHu(std::vector<Lint>& vec)
{
	bool isBigHu = false;
	for (Lint i = 0; i < vec.size(); ++i)
	{
		if (vec[i] == HU_QIDUI || vec[i] == HU_FOURGHOST)
		{
			isBigHu = true;
			break;
		}
	}
	return isBigHu;
}

Lint GH_WuHan_XianTao::GetHuBaseSocore(HuInfo* pHuInfo)
{
	Lint base = 0;
	if(pHuInfo == NULL)
		return base;

	for(int i = 0; i < DESK_USER_COUNT; ++i)
	{
		if((pHuInfo+i)->type == WIN_SUB_ZIMO)
		{
			base = 1;
			break;
		}
		else if((pHuInfo+i)->type == WIN_SUB_BOMB)
		{
			base = 1;
			break;
		}
	}

	return base;
}

void GH_WuHan_XianTao::GetGangSocore(Lint gold[])
{
	ScoreInfo info;

	for (Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		//gold[i] += 3 * (2 + m_zhuangHuGang[i]) * m_angang[i] * m_pt_zhuangDouble[i];  				// 暗杠 向每人收取2
		//gold[i] += (1 + m_zhuangHuGang[i]) * m_pengGangInfo[i].size() * m_pt_zhuangDouble[i];  		// 碰杠数量
		//gold[i] += (1 + m_zhuangHuGang[i]) * m_diangang[i] * m_pt_zhuangDouble[i];					// 点杠 向点杠的人收取X

		//gold[i] += 3 * m_angang[i] * m_paoGangScore[i];				// 加上自己的杠跑分
		//gold[i] += m_pengGangInfo[i].size() * m_paoGangScore[i];
		//gold[i] += m_diangang[i] * m_paoGangScore[i];

		//暗杠
		for (Lint j = 0; j < DESK_USER_COUNT; ++j)
		{
			if (j != i)
			{
				int score1 = (2 + m_zhuangHuGang[i] + m_zhuangHuGang[j]) * m_angang[i] * (m_pt_zhuangDouble[i] + m_pt_zhuangDouble[j] - 1);		// 最后庄翻倍 庄倍率2+自己倍率1-原有倍率1
				int score2 = m_angang[i] * (m_paoGangScore[i] + m_paoGangScore[j]);				// 带跑玩法杠跑分
				int score = score1 + score2;

				gold[i] += score;
				gold[j] -= score;

				m_resultAnGangScore[i] += score;
				m_resultAnGangScore[j] -= score;

				if(score > 0)
				{
					int avg = score/m_angang[i];

					info.m_pos = j;
					info.m_type = 5;
					info.m_score = avg;
					m_scoreInfo[i].insert(m_scoreInfo[i].end(), m_angang[i], info);

					info.m_pos = i;
					info.m_type = 6;
					info.m_score = -avg;
					m_scoreInfo[j].insert(m_scoreInfo[j].end(), m_angang[i], info);
				}
			}
		}

		//碰杠
		std::vector<Lint> pengPosVect = m_pengGangInfo[i];
		for (size_t k = 0; k < pengPosVect.size(); ++k)		//扣除碰杠的数量
		{
			Lint pengPos = pengPosVect[k];
			if ((pengPos >= 0) && (pengPos < DESK_USER_COUNT))
			{
				int score1 = (1 + m_zhuangHuGang[i] + m_zhuangHuGang[pengPos]) * (m_pt_zhuangDouble[i] + m_pt_zhuangDouble[pengPos] - 1);
				int score2 = m_paoGangScore[i] + m_paoGangScore[pengPos];
				int score = score1 + score2;

				gold[i] += score;
				gold[pengPos] -= score;

				m_resultMingGangScore[i] += score;
				m_resultADianGangScore[pengPos] -= score;

				if(score > 0)
				{
					info.m_pos = pengPos;
					info.m_type = 7;
					info.m_score = score;
					m_scoreInfo[i].push_back(info);

					info.m_pos = i;
					info.m_type = 8;
					info.m_score = -score;
					m_scoreInfo[pengPos].push_back(info);
				}
			}
		}

		//暗刻转杠
		std::vector<Lint> PosVect = m_mingGangInfo[i];
		for (size_t k = 0; k < PosVect.size(); ++k)		//扣除碰杠的数量
		{
			Lint pengPos = PosVect[k];
			if ((pengPos >= 0) && (pengPos < DESK_USER_COUNT))
			{
				int score1 = (1 + m_zhuangHuGang[i] + m_zhuangHuGang[pengPos]) * (m_pt_zhuangDouble[i] + m_pt_zhuangDouble[pengPos] - 1);
				int score2 = m_paoGangScore[i] + m_paoGangScore[pengPos];
				int score = score1 + score2;

				gold[i] += score;
				gold[pengPos] -= score;

				m_resultMingGangScore[i] += score;
				m_resultADianGangScore[pengPos] -= score;

				if(score > 0)
				{
					info.m_pos = pengPos;
					info.m_type = 7;
					info.m_score = score;
					m_scoreInfo[i].push_back(info);

					info.m_pos = i;
					info.m_type = 8;
					info.m_score = -score;
					m_scoreInfo[pengPos].push_back(info);
				}
			}
		}

		//这里是给别人点杠
		//gold[i] -= (1 + m_zhuangHuGang[i]) * m_adiangang[i] * m_pt_zhuangDouble[i];

		//gold[i] -= m_adiangang[i] * m_paoGangScore[i];
	}

}

void GH_WuHan_XianTao::GetPaoSocore(Lint result,Lint gold[], Lint& bombCount, Lint winPos[])
{
	ScoreInfo info;

	int x = GetHuBaseSocore(m_playerHuInfo);

	for (Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		if (result == WIN_ZIMO)
		{
			// 向每人收取Y
			if (m_playerHuInfo[i].type == WIN_SUB_ZIMO || m_playerHuInfo[i].type == WIN_SUB_DZIMO)
			{
				winPos[i] = 1;
				for (Lint j = 0; j < DESK_USER_COUNT; ++j)
				{
					if (j != i)
					{
						Lint score1 = (m_isPiao[j] + m_isPiao[i] + m_zhuangHuGang[j] + m_zhuangHuGang[i] + x) * (m_pt_zhuangDouble[i] + m_pt_zhuangDouble[j] - 1);//x为胡牌分数，跑数加上胡牌分数
						Lint score2 = m_paoBeforeScore[i] + m_paoBeforeScore[j] + m_paoAfterScore[i] + m_paoAfterScore[j];
						Lint score = score1 + score2;

						gold[j] -= score;
						gold[i] += score;

						if(score > 0)
						{
							info.m_pos = j;
							info.m_type = 1;
							info.m_score = score;
							m_scoreInfo[i].push_back(info);

							info.m_pos = i;
							info.m_type = 2;
							info.m_score = -score;
							m_scoreInfo[j].push_back(info);
						}
					}
				}
			}
		}
		else if (result == WIN_BOMB)
		{
			if (m_playerHuInfo[i].type == WIN_SUB_BOMB || m_playerHuInfo[i].type == WIN_SUB_DBOMB)
			{
				winPos[i] = 1;
				bombCount++;
				for (Lint j = 0; j < DESK_USER_COUNT; ++j)
				{
					if (j == m_playerHuInfo[i].abombpos)
					{
						Lint score1 = (m_isPiao[j] + m_isPiao[i] + m_zhuangHuGang[j] + m_zhuangHuGang[i] + x) * (m_pt_zhuangDouble[i] + m_pt_zhuangDouble[j] - 1);//x为胡牌分数，跑数加上胡牌分数
						Lint score2 = m_paoBeforeScore[i] + m_paoBeforeScore[j];
						Lint score = score1 + score2;

						gold[j] -= score;
						gold[i] += score;

						if(score > 0)
						{
							info.m_pos = j;
							info.m_type = 3;
							info.m_score = score;
							m_scoreInfo[i].push_back(info);

							info.m_pos = i;
							info.m_type = 4;
							info.m_score = -score;
							m_scoreInfo[j].push_back(info);
						}
					}
				}
			}
		}
	}
}

Lint GH_WuHan_XianTao::FindPengGangPos(Lint curPos, CardValue card) 
{
	if ((curPos < 0) || (curPos >= DESK_USER_COUNT))
	{
		return INVAILD_POS;
	}

	auto it = m_pengInfo[curPos].begin();
	for ( ; it != m_pengInfo[curPos].end(); ++it)
	{
		if (it->card == card)
		{
			return it->pos;
		}
	}
	return INVAILD_POS;
}

bool GH_WuHan_XianTao::isDeskCardEmpty()
{
	if (m_deskCard.size() == 0)
	{
		return true;
	}
	return false;
}

bool GH_WuHan_XianTao::hasFourLaizi(Lint pos)
{
	int laiziCount = 0;
	for (Lint i = 0; i < m_handCard[pos].size(); i++)
	{
		Card *tmpHandCard = m_handCard[pos][i];
		if (tmpHandCard->m_color == m_ghostCardReal[0].m_color && tmpHandCard->m_number == m_ghostCardReal[0].m_number)
		{
			laiziCount++;
		}
	}

	for (Lint i = 0; i < m_outCard[pos].size(); i++)
	{
		Card *tmpOutCard = m_outCard[pos][i];
		if (tmpOutCard->m_color == m_ghostCardReal[0].m_color && tmpOutCard->m_number == m_ghostCardReal[0].m_number)
		{
			laiziCount++;
		}
	}
	return (laiziCount == 4);
}
