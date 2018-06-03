#ifndef _USER_MESSAGE_H_DB
#define _USER_MESSAGE_H_DB

#include "LBase.h"
#include "LRunnable.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"
#include "LDBSession.h"
#include "LTime.h"
#include "LRedisClient.h"

// #define  RUSER	"LDB_USER_10001"
// 
// Lstring record = "{"time":13456,"userid":"user1#user2#user3#user4","deskid":"12455"}";

class CUserMessage : public LRunnable
{
public:
	CUserMessage();
	virtual ~CUserMessage();
public:
	virtual bool		Init();
	virtual bool		Final();

	virtual void Clear();
	virtual void Run();

	virtual void Start();
	virtual	void Stop();
	virtual	void Join();
protected:
	void	HanderMsg(LMsg* msg);

	bool	Excute( const std::string& sql );

protected:
	void	HanderLogicReqVideo(LMsgL2LDBReqVideo* msg);

	void	HanderLogicReqVipLog(LMsgL2LBDReqVipLog* msg);

	void	HanderLogicReqCRELog(LMsgL2LBDReqCRELog* msg);

	void    HanderLogicDelCRELog(LMsgL2LDBDEL_GTU* msg);

	void    HanderLogicReqSaveLog(LMsgL2LDBSaveLogItem* msg);

	void    HanderLogicReqSaveVideo(LMsgL2LDBSaveVideo* msg);

	void	HanderLogicReqRoomLog(LMsgC2SRoomLog* msg);

	void    HanderLogicSaveCRELog(LMsgL2LDBSaveCRELog* msg);

	void    HanderUserLogin(LMsgLMG2LdbUserLogin* msg);

	// 活动相关
	void    HanderActivityPhone(LMsgC2SActivityPhone *msg);
	void    HanderActivityRequestLog(LMsgC2SActivityRequestLog *msg);
	void    HanderActivityRequestDrawOpen(LMsgC2SActivityRequestDrawOpen *msg);
	void    HanderActivityRequestDraw(LMsgC2SActivityRequestDraw *msg);

	//分享 相关
	void    HanderActivityRequestShare(LMsgC2SActivityRequestShare *msg);

	bool    _initRedisConnect();

	void	HanderUserShare(LMsgC2SShare* msg);

	void	HanderTaskInfo(LMsgL2LDBTaskInfo* msg);		// logicserver 消息

	void	HanderUserTaskInfo(LMsgC2STaskInfo *msg);	// user 消息
	void	HanderUserTaskReward(LMsgC2STaskReward *msg);
	
protected:
	void    UpdateUserInfo(const LUser& usert);
private:
	CMemeoryRecycle m_memoryRecycle;
	LTime		m_tickTimer;

	LDBSession* m_dbsession;

private:
	CRedisClient	m_RedisClient;
};

#endif
