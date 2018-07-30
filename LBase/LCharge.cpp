#include "LCharge.h"
#include "GameDefine.h"

Lint CARD_TYPE_NEED_COUNT[CARD_TYPE_MAX]={
	2,		// CARD_TYPE_NONE
	2,		// CARD_TYPE_4
	4,		// CARD_TYPE_8
	8,		// CARD_TYPE_10
	5,		// CARD_TYPE_16
	2,		// CARD_TYPE_ZK_1
	3,		// CARD_TYPE_ZK_2
	1,		// CARD_TYPE_ZK_4
	2,		// CARD_TYPE_ZK_PH_4
	1,		// CARD_TYPE_ZK_PH_8
	2,		// CARD_TYPE_ZK_PH_16
};

Lint CARD_TYPE_CIRCLE[CARD_TYPE_MAX]={
	4,		// CARD_TYPE_NONE
	5,		// CARD_TYPE_4
	10,		// CARD_TYPE_8
	20,		// CARD_TYPE_10
	16,		// CARD_TYPE_16
	1,		// CARD_TYPE_ZK_1
	2,		// CARD_TYPE_ZK_2
	4,		// CARD_TYPE_ZK_4
	4,		// CARD_TYPE_ZK_PH_4
	8,		// CARD_TYPE_ZK_PH_8
	16		// CARD_TYPE_ZK_PH_16
};

// Lint GetCardTypeByCircle(int n)
// {
// 	Lint cardType = CARD_TYPE_DEFAULT;
// 	for(int i=CARD_TYPE_NONE+1; i<CARD_TYPE_MAX; ++i)
// 	{
// 		if(n != CARD_TYPE_CIRCLE[i])
// 			continue;
//
// 		cardType = i;
// 		break;
// 	}
// 	return cardType;
// }

bool NeedCardCount(const Lint cardType, Lint& count)
{
	count = CARD_TYPE_NEED_COUNT[cardType];

	switch(cardType)
	{
		case CARD_TYPE_ZK_4:
		case CARD_TYPE_ZK_PH_8:
		case CARD_TYPE_ZK_PH_16:
			return true;
		default:
			return false;
	}
}

Lint GetCircle(const Lint gameType, const Lint cardType)
{
	return CARD_TYPE_CIRCLE[cardType];
}

LCharge::LCharge()
{
	m_time = 0;
	m_type = 0;
	m_cardType = 0;
	m_cardNum = 0;
	m_userId = 0;
}

LCharge::~LCharge()
{

}

void LCharge::Write(LBuff& buff)
{
	buff.Write(m_time);
	buff.Write(m_type);
	buff.Write(m_cardType);
	buff.Write(m_cardNum);
	buff.Write(m_userId);
	buff.Write(m_admin);
}

void LCharge::Read(LBuff& buff)
{
	buff.Read(m_time);
	buff.Read(m_type);
	buff.Read(m_cardType);
	buff.Read(m_cardNum);
	buff.Read(m_userId);
	buff.Read(m_admin);
}