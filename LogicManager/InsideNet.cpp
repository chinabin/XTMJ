#include "InsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "Work.h"
#include "LLog.h"
#include "BaseCommonFunc.h"

bool InsideNet::Init()
{
	LNet::Init(gConfig.GetInsideIp(), gConfig.GetInsidePort(), 16, true, false);
	return true;
}

bool InsideNet::Final()
{
	return true;
}

void InsideNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s, bool bIsFromInternal)
{
	LMsg* pMsg = parserMsgPack(recv, s);
	if(pMsg)
	{
		LLOG_DEBUG("LogicManager get an message, message id=%d.", pMsg->m_msgId);
		gWork.Push(pMsg);
	}
}