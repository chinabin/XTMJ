#include "LLog.h"
#include "Counter.h"
#include "CounterManager.h"

//////////////////////////////////////////////////////////////////////////

void CounterUnit::Finish()
{
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_finish = true;
	}
	m_condition.notify_one();
}

void CounterUnit::WaitFinish()
{
	boost::mutex::scoped_lock lock( m_mutex );
	if ( m_finish )
	{
		return;
	}
	m_condition.wait( lock );
}

void CounterTing::Calc()
{
	result.clear();
	bool checked[4][9] = {false};
	Lsize size = handcard->size();
	for ( Lsize i = 0; i < size; ++i )
	{
		if ( i%COUNTER_NUM != index )
		{
			continue;
		}

// 		char n[16];
// 		sprintf( n, "%d:%d\n", index, i );
// 		OutputDebugStringA(n);

		// 判断这张牌是否检查过
		if ( !(*handcard)[i] )
		{
			continue;
		}
		Lint color = (*handcard)[i]->m_color;
		Lint number = (*handcard)[i]->m_number;
		if ( (color <= 0 || color > 4) || (number <= 0 || number > 9) )
		{
			continue;
		}
		if ( checked[color-1][number-1] )
		{
			continue;
		}

		if(gameInfo->m_playtype.checkPlayType(PT_SQ_LIANG4DA1) && gameInfo->m_louhuCard.size() != 0)		// 亮四打一玩法 亮出的牌不能做打出测试
		{
			Lint handCardNum1 = 0;	//亮四打一时 测试的牌在亮出的牌中有几张
			for(int t=0; t<gameInfo->m_louhuCard.size(); ++t)
			{
				if(*(gameInfo->m_louhuCard[t]) == *((*handcard)[i]))
					++handCardNum1;
			}
			Lint handCardNum2 = 0;	//亮四打一时 测试的牌在手中的牌中有几张
			for(int t2=0; t2<handcard->size(); ++t2)
			{
				if(*((*handcard)[t2]) == *((*handcard)[i]))
					++handCardNum2;
			}
			if(handCardNum1 > 0 && handCardNum1 >= handCardNum2)		//亮出的牌<手中的牌 才能打出
			{
				continue;
			}
		}

		checked[color-1][number-1] = true;

		TingUnit unit;
		unit.m_card = *(*handcard)[i];	// 表示要打出去的牌

		// 组织手牌
		CardVector cards;
		cards.reserve( size-1 );
		for ( Lsize k = 0; k < size; ++k )
		{
			if ( k != i )
			{
				cards.push_back( (*handcard)[k] );
			}
		}

		cardmgr->CheckTing( cards, *pengCard, *agangCard, *mgangCard, *eatCard, *gameInfo, unit.m_tingcard );
		if ( unit.m_tingcard.size() > 0 )
		{
			result.push_back( unit );
		}
	}
}

void CounterTingWithHun::Calc()
{
	result.clear();
	bool checked[4][9] = {false};
	Lsize size = handcard->size();
	for ( Lsize i = 0; i < size; ++i )
	{
		if ( i%COUNTER_NUM != index )
		{
			continue;
		}

		// 判断这张牌是否检查过
		if ( !(*handcard)[i] )
		{
			continue;
		}
		// 判断要移除的牌是否是混
		bool ishun = false;
		for ( auto it = huntype.begin(); it != huntype.end(); ++it )
		{
			if ( (*handcard)[i]->m_color == it->m_color && (*handcard)[i]->m_number == it->m_number )
			{
				ishun = true;
				break;
			}
		}
		if ( ishun )
		{
			continue;
		}
		
		Lint color = (*handcard)[i]->m_color;
		Lint number = (*handcard)[i]->m_number;
		if ( (color <= 0 || color > 4) || (number <= 0 || number > 9) )
		{
			continue;
		}
		if ( checked[color-1][number-1] )
		{
			continue;
		}
		checked[color-1][number-1] = true;

		TingUnit unit;
		unit.m_card = *(*handcard)[i];	// 表示要打出去的牌

		// 组织手牌
		CardVector cards;
		cards.reserve( size-1 );
		for ( Lsize k = 0; k < size; ++k )
		{
			if ( k != i )
			{
				cards.push_back( (*handcard)[k] );
			}
		}

		cardmgr->CheckTing( cards, *pengCard, *agangCard, *mgangCard, *eatCard, *gameInfo, unit.m_tingcard );
		if ( unit.m_tingcard.size() > 0 )
		{
			result.push_back( unit );
		}
	}
}

Counter::Counter()
	: m_counter(NULL)
{
}

bool Counter::Init()
{
	return true;
}

bool Counter::Final()
{
	return true;
}


void Counter::Run()
{
	if(!this->Init())
	{
		LLOG_ERROR("DbServer init Failed!!!");
		return;
	}

	while(!GetStop())
	{
		boost::mutex::scoped_lock lock( m_mutex );
		if ( m_counter )
		{
			m_counter->Calc();
			m_counter->Finish();
			m_counter = NULL;
		}
		m_condition.wait( lock );

		if ( m_counter )
		{
			m_counter->Calc();
			m_counter->Finish();
			m_counter = NULL;
		}
	}
	
	this->Final();
}

void Counter::Clear()
{

}

void Counter::Notify( CounterUnit* unit )
{
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_counter = unit;
		if ( m_counter )
		{
			m_counter->m_finish = false;
		}
	}
	m_condition.notify_one();
};
