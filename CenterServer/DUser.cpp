#include "DUser.h"
#include "LLog.h"
#include "LCharge.h"
#include "UserManager.h"


DUser::DUser()
{
	m_lastReqTime = 0;
	m_logicID = 0;
}

DUser::~DUser()
{

}

void DUser::AddCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin, const Lstring& remark)
{
	LLOG_INFO("DUser::AddCardCount userid=%d, curcount=%d, type=%d, count=%d, operType=%d", m_usert.m_id, m_usert.m_numOfCard2s, cardType, count, operType);

	m_usert.m_numOfCard2s += count;
	m_usert.m_totalbuynum += count;

	// 保存数据
	gUserManager.SaveUser(this);
	gUserManager.SaveCardInfo( this, cardType, count, operType, admin, remark);
}

void DUser::DelCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin)
{
	LLOG_INFO("DUser::DelCardCount userid=%d, curcount=%d, type=%d, count=%d, operType=%d", m_usert.m_id, m_usert.m_numOfCard2s, cardType, count, operType);

	if (m_usert.m_numOfCard2s >= count)
		m_usert.m_numOfCard2s -= count;
	else
		m_usert.m_numOfCard2s = 0;

	// 保存数据
	gUserManager.SaveUser(this);
	gUserManager.SaveCardInfo( this, cardType, /*delCount*/count, operType, admin );
}

void DUser::AddCoinsCount(Lint count, Lint operType)
{
	m_usert.m_coins += count;
	gUserManager.SaveUser(this);
}

void DUser::DelCoinsCount(Lint count, Lint operType)
{
	if ( m_usert.m_coins < count )
	{
		m_usert.m_coins = 0;
	}
	else
	{
		m_usert.m_coins -= count;
	}
	gUserManager.SaveUser(this);
}

void DUser::ExchangeCardCoin(Lint add,Lint del,Lint operType,Lint cardType,const Lstring& admin)
{
	m_usert.m_numOfCard2s += add;
	m_usert.m_coins -= del;
	if(m_usert.m_coins < 0)
		m_usert.m_coins = 0;
	gUserManager.SaveUser(this);
	gUserManager.SaveCardInfo( this, cardType, add, operType, admin );
}

void DUser::AddCreditValue()
{
	m_usert.m_creditValue ++;
	gUserManager.SaveUser(this);
}

void DUser::AddPlayCount()
{
	LLOG_INFO("DUser::AddPlayCount userid=%d", m_usert.m_id);
	m_usert.m_totalplaynum++;

	// 保存数据
	gUserManager.SaveUser(this);
}

void DUser::ModifyNew( Lint value )
{
	LLOG_INFO("DUser::ModifyNew userid=%dc value=%d", m_usert.m_id, value);
	m_usert.m_new = value;

	// 保存数据
	gUserManager.SaveUser(this);
}

void DUser::Save()
{
	gUserManager.SaveUser(this);
}

/////////////////////////////////////////////////////////////
boost::recursive_mutex CSafeUser::m_mutexLockUser;
CSafeUser::CSafeUser(boost::shared_ptr<DUser> user)
{
	m_bLocked  = false;
	m_User	   = user;

	_lockUser();
}

CSafeUser::~CSafeUser()
{
	_unlockUser();
}

boost::shared_ptr<DUser>& CSafeUser::getUser()
{
	return m_User;
}

bool CSafeUser::isValid()
{
	if(!m_bLocked || m_User.get() == NULL)
	{
		return false;
	}

	return true;
}

void CSafeUser::_lockUser()
{
	if(m_User.get() == NULL || m_bLocked)
	{
		return;
	}

	int iLockTime = 0;
	while(true)
	{
		boost::recursive_mutex::scoped_lock l(m_mutexLockUser);

		boost::thread::id thisThredId = boost::this_thread::get_id();

		if(m_User->m_uLockedSum == 0) //第一次锁定
		{
			m_User->m_lockedThreadId = thisThredId;
			m_User->m_uLockedSum++;
			break;
		}

		if(m_User->m_uLockedSum > 0 && m_User->m_lockedThreadId == thisThredId) //本线程锁定
		{
			m_User->m_uLockedSum++;
			break;
		}

		//此玩家正在被其它线程锁定，等待1毫秒。继续请求
		l.unlock();
		boost::this_thread::sleep(boost::posix_time::millisec(1));

		++iLockTime;
		if(iLockTime >= 5000)
		{
			LLOG_ERROR("Lock time more than 5 seconds. %s", m_User->m_usert.m_unioid.c_str());
			iLockTime = 0;
		}
	}

	m_bLocked = true;
}

void CSafeUser::_unlockUser()
{
	if(!m_bLocked)
	{
		return;
	}

	boost::recursive_mutex::scoped_lock l(m_mutexLockUser);
	if(m_User->m_uLockedSum > 0)
	{
		m_User->m_uLockedSum--;
	}

	m_bLocked = false;
}