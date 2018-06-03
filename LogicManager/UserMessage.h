#ifndef _USER_MESSAGE_H_
#define _USER_MESSAGE_H_

#include "LBase.h"
#include "LRunnable.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"
#include "LTime.h"

class CUserMessage : public LRunnable
{
public:
	CUserMessage();
	virtual ~CUserMessage();
public:
	
	virtual void Clear();
	virtual void Run();

	virtual void Start();
	virtual	void Stop();
	virtual	void Join();
protected:
	void	HanderMsg(LMsg* msg);
protected:
	//From CenterServer
	void	HanderCenterUserLogin(LMsgCe2LUserLogin* msg);
	void	HanderCenterGMCharge(LMsgCe2LGMCharge* msg);
	void	HanderCenterGMCoins(LMsgCe2LGMCoins* msg);
	void	HanderCenterAddAgent(LMsgCE2LMGAddAgent* msg);

	//From LogicServer
	void	HanderModifyUserState(LMsgL2LMGModifyUserState* msg);
	void	HanderModifyUserCardNum(LMsgL2LMGModifyCard* msg);
	void	HanderAddUserPlayCount(LMsgL2LMGAddUserPlayCount* msg);

	//From GateSever
	void	HanderUserMsg(LMsgG2LUserMsg* msg);
	void	HanderUserOutMsg(LMsgG2LUserOutMsg* msg);

	//From DB
	void	HanderDBReqVipLog(LMsgLBD2LReqVipLog* msg);
	void	HanderDBReqRoomLog(LMsgLBD2LReqRoomLog* msg);
	void	HanderDBReqVideo(LMsgLDB2LReqVideo* msg);
	void    HanderDBRetBindingRelation(LMsgLDB2LMGBindingRelationships* msg);
	void    HanderDBRetInfo(LMsgLDB_2_LM_RetInfo* msg);
	void	HanderDBUserMsg(LMsgLDB2LMGUserMsg* msg);

	void    HanderDBReqCRELogHis(LMsgLDB2LM_RecordCRELog *msg);

	//From CoinsServer
	void	HanderModifyUserCoinsNum(LMsgCN2LMGModifyUserCoins* msg);
protected:
	//处理玩家登陆的消息 
	void	HanderUserLogin(Lint id, Lint seed, Lint gateId, Lstring& md5, Lstring& ip);
protected:
	LTime&	GetCurTime();
private:
	CMemeoryRecycle m_memoryRecycle;
	LTime		m_tickTimer;
};

#endif
