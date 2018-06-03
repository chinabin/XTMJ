#include "LLog.h"
#include "NewUserVerifyManager.h"
#include "BaseCommonFunc.h"

//////////////////////////////////////////////////////////////////////////
bool NewUserVerifyManager::Init()
{
	return true;
}

bool NewUserVerifyManager::Final()
{

	return true;
}

void NewUserVerifyManager::Clear()
{

}


void NewUserVerifyManager::Push(const Lstring& strUUID, LMsg* pMsg)
{
	unsigned int uHashKey = caculateHashKey(strUUID.c_str(), strUUID.length());
	int iIndex = uHashKey % NEWUSERVERIFY_NUM;
	m_dbpool[iIndex].Push(pMsg);
}

void NewUserVerifyManager::Start()
{
	for(int i = 0; i < NEWUSERVERIFY_NUM; ++ i)
	{
		m_dbpool[i].Start();
		//_sleep(10);
	}
}

void NewUserVerifyManager::Join()
{
	for(int i = 0; i < NEWUSERVERIFY_NUM; ++i)
	{
		m_dbpool[i].Join();
	}
}

void NewUserVerifyManager::Stop()
{
	for(int i = 0; i < NEWUSERVERIFY_NUM; ++i)
	{
		m_dbpool[i].Stop();
	}
}
