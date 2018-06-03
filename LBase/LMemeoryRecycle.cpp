#include "LMemoryRecycle.h"
#include "LTime.h"

CMemeoryRecycle::CMemeoryRecycle()
{

}

CMemeoryRecycle::~CMemeoryRecycle()
{

}

void CMemeoryRecycle::Run()
{
	LTime msgTime;
	int iMsgCount = 0;
	while(!GetStop())
	{
		LMsg* pRecycleMsg = (LMsg*)Pop();
		if(pRecycleMsg == NULL)
		{
			Sleep(1);
			continue;
		}

		delete pRecycleMsg;
		pRecycleMsg = NULL;

		++iMsgCount;
		if(iMsgCount == 10000)
		{
			LTime now;
			LLOG_ERROR("10000 package recyle need %ld", now.MSecs() - msgTime.MSecs());
			iMsgCount = 0;
		}
		else if(iMsgCount == 1)
		{
			msgTime.Now();
		}
	}
}

void CMemeoryRecycle::Clear()
{

}

void CMemeoryRecycle::recycleMsg(LMsg* pMsg)
{
	Push(pMsg);
}