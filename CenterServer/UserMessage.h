#ifndef _USER_MESSAGE_H_
#define _USER_MESSAGE_H_

#include "LBase.h"
#include "LRunnable.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"
#include "DUser.h"
#include "LDBSession.h"
#include "LTime.h"
#include "LRedisClient.h"

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
	void	HanderModifyUserNew(LMsgL2CeModifyUserNew* msg);
	void	HanderLogicUserLogin(LMsgL2CeUserServerLogin* msg);
	void	HanderLogicUserLogout(LMsgL2CeUserServerLogout* msg);
	void	HanderModifyCard(LMsgL2LMGModifyCard* msg);
	void	HanderAddUserPlayCount(LMsgL2LMGAddUserPlayCount* msg);
	void	HanderModifyUserCoinsNum(LMsgCN2LMGModifyUserCoins* msg);

	void    HanderModifyUserCreditValue(LMsgC2C_ADD_CRE* msg);
	void    HanderExchangeCard(LMsgL2LMGExchCard* msg);

	void    HanderAddAgent(LMsgLMG2CEAddAgent* msg);
	
protected:
	void	HanderFromLoginServer(LMsgFromLoginServer* msg);
// 	void	HanderUserLogin(LMsgC2SMsg* msg, LSocketPtr gameloginSP);
// 	void	HanderUserLocalLogin(LMsgC2SMsg* msg, LSocketPtr gameloginSP);
// 	void	HanderUserWechatLogin(LMsgC2SMsg* msg, LSocketPtr gameloginSP);
	// 需要记录IP 所以改为传递LMsgFromLoginServer
	void	HanderUserLogin(LMsgFromLoginServer* msg, LSocketPtr gameloginSP);
	void	HanderUserLocalLogin(LMsgFromLoginServer* msg, LSocketPtr gameloginSP);
	void	HanderUserWechatLogin(LMsgFromLoginServer* msg, LSocketPtr gameloginSP);
	void	HanderNewUserVerifyReply(LMsgNewUserVerifyReply* msg);
	void	SendUserLogin( const Lstring& uuid, LSocketPtr gameloginSP, Lint severID, Lint errorCode, const Lstring& errorMsg );
private:
	void    UpdateUserInfo(const LUser& usert);
private:
	CMemeoryRecycle m_memoryRecycle;
	LTime		m_tickTimer;

private:
	CRedisClient	m_RedisClient;
};

#endif