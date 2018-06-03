#include "DeskManager.h"
#include "Work.h"
#include "LLog.h"
#include "UserManager.h"
#include "Config.h"

DeskInfos nullDesk;

bool DeskManager::Init()
{
	//初始所有的桌子号
	std::vector<Lint> m_deskVector;
	for(Lint iDeskId = 100000; iDeskId < 999999; ++iDeskId)
	{
		m_deskVector.push_back(iDeskId);
	}

	//生成随机队列
	Lint nSize = m_deskVector.size();
	while(nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		m_FreeDeskList.push(m_deskVector[seed1]);
		m_deskVector[seed1] = m_deskVector[nSize - 1];
		nSize --;
	}
	return true;
}

bool DeskManager::Final()
{
	return true;
}

Lint DeskManager::RecycleDeskId(Lint iDeskId)
{
	Lint iLogicServerId = INVALID_LOGICSERVERID;

	boost::mutex::scoped_lock l(m_mutexDeskId);

	auto itDesk2Logic = m_mapDeskId2LogicServerId.find(iDeskId);
	if(itDesk2Logic != m_mapDeskId2LogicServerId.end())
	{
		iLogicServerId = itDesk2Logic->second.m_logicServerId;
		m_mapDeskId2LogicServerId.erase(itDesk2Logic);
	}

	if(iLogicServerId != INVALID_LOGICSERVERID)	//ID放回队列
	{
		m_FreeDeskList.push(iDeskId);
	}
	
	return iLogicServerId;
}

Lint DeskManager::GetFreeDeskId(Lint uid, Lint nServerID, Lint nInCredits, Lint cardType, Lint gameType, Lstring ip)
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	Lint id = 0;
	Lint nDeskSize = m_FreeDeskList.size();

	LLOG_ERROR("DeskManager::GetFreeDeskId size=%d", nDeskSize);
	if (nDeskSize > 0)
	{
		id = m_FreeDeskList.front();
		m_FreeDeskList.pop();
		DeskInfos tDesk(uid, nServerID, nInCredits, cardType, gameType, ip);
		m_mapDeskId2LogicServerId[id] = tDesk;
	}

	return id;
}

Lint DeskManager::GetDeskCreditLimit( Lint iDeskId )
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	auto itDesk2Logic = m_mapDeskId2LogicServerId.find(iDeskId);
	if (itDesk2Logic != m_mapDeskId2LogicServerId.end())
	{
		return itDesk2Logic->second.m_creditsLimit;
	}

	return -1;
}

Lint DeskManager::GetLogicServerIDbyDeskID(Lint nDeskID)
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	auto itDesk2Logic = m_mapDeskId2LogicServerId.find(nDeskID);
	if (itDesk2Logic != m_mapDeskId2LogicServerId.end())
	{
		return itDesk2Logic->second.m_logicServerId;
	}

	return INVALID_LOGICSERVERID;
}

Lint DeskManager::GetDeskCardType(Lint iDeskId)
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	auto itDesk2Logic = m_mapDeskId2LogicServerId.find(iDeskId);
	if (itDesk2Logic != m_mapDeskId2LogicServerId.end())
	{
		return itDesk2Logic->second.m_cardType;
	}

	return 0;
}

Lstring DeskManager::GetDeskCreateIp(Lint iDeskId)
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	auto itDesk2Logic = m_mapDeskId2LogicServerId.find(iDeskId);
	if (itDesk2Logic != m_mapDeskId2LogicServerId.end())
	{
		return itDesk2Logic->second.m_createIp;
	}

	return "";
}

Lint DeskManager::GetDeskCreateUserId(Lint iDeskId)
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	auto itDesk2Logic = m_mapDeskId2LogicServerId.find(iDeskId);
	if (itDesk2Logic != m_mapDeskId2LogicServerId.end())
	{
		return itDesk2Logic->second.m_userId;
	}

	return 0;
}

DeskInfos DeskManager::GetDeskInfo(const Lint iDeskId)
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	auto itDesk2Logic = m_mapDeskId2LogicServerId.find(iDeskId);
	if (itDesk2Logic != m_mapDeskId2LogicServerId.end())
	{
		return itDesk2Logic->second;
	}

	return nullDesk;
}

void DeskManager::RecycleAllDeskIdOnLogicServer(Lint nLogicID)
{
	boost::mutex::scoped_lock l(m_mutexDeskId);

	for(auto itDesk2Logic = m_mapDeskId2LogicServerId.begin(); itDesk2Logic != m_mapDeskId2LogicServerId.end();)
	{
		if(itDesk2Logic->second.m_logicServerId == nLogicID)
		{
			m_mapDeskId2LogicServerId.erase(itDesk2Logic++);
		}
		else
		{
			++itDesk2Logic;
		}
	}
}

Lint DeskManager::GetCoinsDeskSize()
{
	boost::mutex::scoped_lock l(m_mutexDeskId);
	return m_coinsDeskID.size();
}

void DeskManager::GetFreeCoinDeskID( int count, std::vector<Lint>& deskid )
{
	boost::mutex::scoped_lock l(m_mutexDeskId);
	for ( int i = 0; i < count; ++i )
	{
		if ( m_FreeDeskList.size() > 0 )
		{
			deskid.push_back( m_FreeDeskList.front() );
			m_coinsDeskID.push_back( m_FreeDeskList.front() );
			m_FreeDeskList.pop();
		}
	}
}

void DeskManager::RecycleCoinsDeskId( const std::vector<int>& deskid )
{
	boost::mutex::scoped_lock l(m_mutexDeskId);
	for ( auto it = deskid.begin(); it != deskid.end(); ++it )
	{
		auto cit = std::find( m_coinsDeskID.begin(), m_coinsDeskID.end(), *it );
		if ( cit != m_coinsDeskID.end() )
		{
			m_FreeDeskList.push( *cit );
			m_coinsDeskID.erase( cit );
		}
	}
}

void DeskManager::RecycleCoinsDeskId()
{
	boost::mutex::scoped_lock l(m_mutexDeskId);
	for ( auto it = m_coinsDeskID.begin(); it != m_coinsDeskID.end(); ++it )
	{
		m_FreeDeskList.push( *it );
	}
	m_coinsDeskID.clear();
}
