#include "LMsg.h"
#include "LLog.h"
#include "LMsgS2S.h"

LMsg::LMsg(int id)
:m_msgId(id), m_isMsgPack(false)
{

}

LMsg::~LMsg()
{

}

// |--2 bytes(body len)--|--2 bytes(checksum)--|-- 4 bytes(time)--|--4 bytes(message id)--|--1 byte(pack)--|--N bytes(data)--|
// |-----------------Head------------------------------------------------------------|--------------Body---------------------|
LBuffPtr LMsg::GetSendBuff()
{
	if(!m_send)
	{
		m_send.reset(new LBuff());
		if(preAllocSendMemorySize() > BUFF_SIZE)
		{
			m_send->Alloc(preAllocSendMemorySize());
		}

		if (m_isMsgPack)
		{
			msgpack::sbuffer buffer;
			msgpack::packer<msgpack::sbuffer> pac(&buffer);
			Write(pac); 

			//fill head
			m_send->Write((unsigned short)(buffer.size() + 1));
			m_send->Write((unsigned short)0);
			m_send->Write((int)0);
			m_send->Write(m_msgId);

			//fill body
			m_send->Write((unsigned char)1);
			m_send->Write(buffer.data(), buffer.size());
		}
		else
		{
			LBuff tmpBuff;
			WriteMsgId(tmpBuff);
			Write(tmpBuff);

			//fill head
			m_send->Write((unsigned short)(tmpBuff.Size() + 1));
			m_send->Write((unsigned short)0);
			m_send->Write((int)0);
			m_send->Write(m_msgId);

			//fill body
			m_send->Write((unsigned char)0);
			m_send->Write(tmpBuff.Data(), tmpBuff.Size());
		}
	}
	return m_send;
}

void LMsg::WriteMsgId(LBuff& buff)
{
	buff.Write(m_msgId);
}

void LMsg::ReadMsgId(LBuff& buff)
{
	buff.Read(m_msgId);
}

bool LMsg::Read(LBuff& buff)
{
	return true;
}

bool LMsg::Read(msgpack::object& pack)
{
	return true;
}

bool LMsg::Write(LBuff& buff)
{
	return true;
}

bool LMsg::Write(msgpack::packer<msgpack::sbuffer>& pack)
{
	return true;
}

LMsg* LMsg::Clone()
{
	return NULL;
}

Lint LMsg::preAllocSendMemorySize()
{
	return BUFF_SIZE;
}

//////////////////////////////////////////////////////////////////////////
bool LMsgFactory::Init()
{
	Final();

	RegestMsg(new LMsgIn());
	RegestMsg(new LMsgKick());
	RegestMsg(new LMsgHttp());

	RegestMsg(new LMsgC2SMsg());
	RegestMsg(new LMsgS2CMsg());

	RegestMsg(new LMsgC2SHeart());
	RegestMsg(new LMsgS2CHeart());

	RegestMsg(new LMsgC2SLoginGate());
	RegestMsg(new LMsgS2CLoginGate());

	RegestMsg(new LMsgC2SNotice());
	RegestMsg(new LMsgS2CNotice());

	RegestMsg(new LMsgC2SLogin());
	RegestMsg(new LMsgS2CLogin());
	RegestMsg(new LMsgL2CeUserServerLogin());
	RegestMsg(new LMsgL2CeUserServerLogout());

	RegestMsg(new LMsgS2CItemInfo());
	RegestMsg(new LMsgS2CHorseInfo());

	RegestMsg(new LMsgS2CIntoDesk());
	RegestMsg(new LMsgS2CDeskAddUser());
	RegestMsg(new LMsgS2CDeskDelUser());
	RegestMsg(new LMsgC2SSelectResetDesk());
	RegestMsg(new LMsgS2CResetDesk());
	RegestMsg(new LMsgC2SResetDesk());

	RegestMsg(new LMsgS2CDeskState());

	RegestMsg(new LMsgC2SUserReady());
	RegestMsg(new LMsgS2CUserReady());

	RegestMsg(new LMsgS2CPlayStart());
	RegestMsg(new LMsgS2COutCard());
	RegestMsg(new LMsgC2SUserPlay());
	RegestMsg(new LMsgS2CUserPlay());
	RegestMsg(new LMsgS2CThink());
	RegestMsg(new LMsgC2SUserOper());
	RegestMsg(new LMsgS2CUserOper());
	RegestMsg(new LMsgS2CGameOver());
	RegestMsg(new LMsgC2SUserAIOper());
	RegestMsg(new LMsgS2CUserAIOper());

	RegestMsg(new LMsgS2CUserStartHu());
	RegestMsg(new LMsgC2SUserStartHuSelect());
	RegestMsg(new LMsgS2CUserStartHuSelect());

	RegestMsg(new LMsgS2CUserGangCard());
	RegestMsg(new LMsgS2CUserEndCardSelect());
	RegestMsg(new LMsgC2SUserEndCardSelect());
	RegestMsg(new LMsgS2CUserEndCardGet());

	RegestMsg(new LMsgC2SUserOutRoom());
	RegestMsg(new LMsgS2CUserOutRoom());

	RegestMsg(new LMsgC2SLeaveDesk());
	RegestMsg(new LMsgS2CLeaveDesk());

	RegestMsg(new LMsgC2SUserSpeak());
	RegestMsg(new LMsgS2CUserSpeak());

	RegestMsg(new LMsgC2SCreateDesk());
	RegestMsg(new LMsgS2CCreateDeskRet());

	RegestMsg(new LMsgC2SAddDesk());
	RegestMsg(new LMsgS2CAddDeskRet());

	RegestMsg(new LMsgC2SEnterCoinsDesk());
	RegestMsg(new LMsgS2CEnterCoinsDeskRet());

	
	RegestMsg(new LMsgS2CVipEnd());
	RegestMsg(new LMsgC2SVipLog());
	RegestMsg(new LMsgS2CVipLog());
	RegestMsg(new LMsgC2SVideo());
	RegestMsg(new LMsgS2CVideo());
	RegestMsg(new LMsgS2CRemoveQiangGangCard());

	RegestMsg(new LMsgC2SCRELog());
	RegestMsg(new LMsgS2CCRELogRet());
	RegestMsg(new LMsgC2SREQ_GTU());
	RegestMsg(new LMsgS2CREQ_GTU_RET());

	RegestMsg(new LMsgC2SREQ_Exch());
	RegestMsg(new LMsgS2CREQ_EXCH_RET());

	RegestMsg(new LMsgS2CActivityInfo());
	RegestMsg(new LMsgC2SActivityPhone());
	RegestMsg(new LMsgS2CActivityPhone());
	RegestMsg(new LMsgC2SActivityRequestLog());
	RegestMsg(new LMsgS2CActivityReplyLog());
	RegestMsg(new LMsgC2SActivityRequestDrawOpen());
	RegestMsg(new LMsgS2CActivityReplyDrawOpen());
	RegestMsg(new LMsgC2SActivityRequestDraw());
	RegestMsg(new LMsgS2CActivityReplyDraw());

	RegestMsg(new LMsgC2SActivityRequestShare());
	RegestMsg(new LMsgS2CActivityFinishShare());

	RegestMsg(new LMsgC2SRoomLog());
	RegestMsg(new LMsgLBD2LReqRoomLog());
	RegestMsg(new LMsgS2CRoomLog());

	RegestMsg(new LMsgS2CInvitingInfo());
	RegestMsg(new LMsgC2SBindInviter());
	RegestMsg(new LMsgS2CBindInviter());
	RegestMsg(new LMsgC2SGetUserInfo());
	RegestMsg(new LMsgC2SGetInvitingInfo());
	

	RegestMsg(new LMsgL2CeLogin());
	RegestMsg(new LMsgL2CeGateInfo());
	RegestMsg(new LMsgCe2LUserLogin());
	RegestMsg(new LMsgCe2LUserIdInfo());

	RegestMsg(new LMsgG2LLogin());
	RegestMsg(new LMsgG2LUserMsg());
	RegestMsg(new LMsgL2GUserMsg());
	RegestMsg(new LMsgL2CeModifyUserNew());
	RegestMsg(new LMsgL2CeSaveLogItem());
	RegestMsg(new LMsgCe2LGMCharge());
	RegestMsg(new LMsgCe2LGMHorse());
	RegestMsg(new LMsgCe2LGMBuyInfo());
	RegestMsg(new LMsgCe2LGMCoins());
	RegestMsg(new LMsgCe2LGMHide());

	RegestMsg(new LMsgG2LUserOutMsg());
	RegestMsg(new LMsgL2GUserOutMsg());

	RegestMsg(new LMsgC2SVerifyHead());
	
	RegestMsg(new LMsgL2LdbLogin());
	RegestMsg(new LMsgL2LDBSaveLogItem());
	RegestMsg(new LMsgL2LDBSaveVideo());
	RegestMsg(new LMsgL2LBDReqVipLog());
	RegestMsg(new LMsgLBD2LReqVipLog());
	RegestMsg(new LMsgL2LDBReqVideo());
	RegestMsg(new LMsgLDB2LReqVideo());

	RegestMsg(new LMsgL2LBDReqCRELog());
	RegestMsg(new LMsgL2LDBSaveCRELog());
	RegestMsg(new LMsgLDB2LM_RecordCRELog());
	RegestMsg(new LMsgL2LDBDEL_GTU());

	RegestMsg(new LMsgLM2CEN_ADD_CRE());
		
	RegestMsg(new LMsgLDB2LLogInfo());

	RegestMsg(new LMsgS2CUserStartDingQue());
	RegestMsg(new LMsgC2SUserDingQue());
	RegestMsg(new LMsgS2CUserCompleteDingQue());

	RegestMsg(new LMsgS2CStartBottomPour());
	RegestMsg(new LMsgC2SBottomPour());
	RegestMsg(new LMsgS2CUserBottomPourInfo());
	RegestMsg(new LMsgS2CAllUserBottomPourInfo());

	RegestMsg(new LMsgC2STangCard());
	RegestMsg(new LMsgS2CTangCardRet());


	RegestMsg(new LMsgLMG2GateLogicInfo());
	RegestMsg(new LMsgL2LMGLogin());
	RegestMsg(new LMsgLMG2LRLogInfo());
	RegestMsg(new LMsgLMG2GHorseInfo());
	RegestMsg(new LMsgLMG2LLogin());
	RegestMsg(new LMsgL2LMGModifyUserState());
	RegestMsg(new LMsgL2GModifyUserState());
	RegestMsg(new LMsgLMG2LCreateDesk());
	RegestMsg(new LMsgLMG2LAddToDesk());
	RegestMsg(new LMsgL2LMGModifyCard());
	RegestMsg(new LMsgL2LMGAddUserPlayCount());
	RegestMsg(new LMsgLMG2LPXActive());
	RegestMsg(new LMsgG2LMGLogin());
	RegestMsg(new LMsgL2LMGRecyleDeskID());
	RegestMsg(new LMsgLMG2LdbUserLogin());
	RegestMsg(new LMsgLDB2LMGUserMsg());
	RegestMsg(new LMsgLMG2LdbLogin());
	RegestMsg(new LMsgCE2LSetGameFree());
	RegestMsg(new LMsgCE2LSetPXActive());
	RegestMsg(new LMsgCE2LSetOUGCActive());
	RegestMsg(new LMsgCE2LSetExchActive());
	RegestMsg(new LMsgCE2LSetActivity());

	RegestMsg(new LMsgLM_2_LDBBindInviter());
	RegestMsg(new LMsgLDB2LMGBindingRelationships());
	RegestMsg(new LMsgLM_2_LDB_ReqInfo());
	RegestMsg(new LMsgLDB_2_LM_RetInfo());

	RegestMsg(new LMsgFromLoginServer());
	RegestMsg(new LMsgToLoginServer());

	RegestMsg(new LMsgConvertClientMsg());
	RegestMsg(new LMsgHeartBeatRequestMsg());
	RegestMsg(new LMsgHeartBeatReplyMsg());

	RegestMsg(new LMsgNewUserVerifyRequest());

	RegestMsg(new LMsgCN2LMGLogin());
	RegestMsg(new LMsgLMG2CNLogin());
	RegestMsg(new LMsgCN2LMGFreeDeskReq());
	RegestMsg(new LMsgLMG2CNFreeDeskReply());
	RegestMsg(new LMsgLMG2CNUserLogin());
	RegestMsg(new LMsgLMG2CNEnterCoinDesk());
	RegestMsg(new LMsgCN2LMGModifyUserCoins());
	RegestMsg(new LMsgL2LMGExchCard());
	RegestMsg(new LMsgLMG2CNGetCoins());
	RegestMsg(new LMsgLMG2GLCoinsServerInfo());
	RegestMsg(new LMsgG2CNLogin());
	RegestMsg(new LMsgL2CNLogin());
	RegestMsg(new LMsgCN2LCreateCoinDesk());
	RegestMsg(new LMsgL2CNCreateCoinDeskRet());
	RegestMsg(new LMsgL2CNEndCoinDesk());
	RegestMsg(new LMsgC2SGoOnCoinsDesk());
	RegestMsg(new LMsgS2CGoOnCoinsDeskRet());
	RegestMsg(new LMsgS2CKickCoinsDesk());
	RegestMsg(new LMsgS2CGiveCoins());
	RegestMsg(new LMsgC2SGetCoins());
	RegestMsg(new LMsgS2CGetCoins());
	RegestMsg(new LMsgS2COtherCoins());

	// Add HuNanZhuanZhuan by hanlu 02242017 Begin
	RegestMsg(new LMsgS2CZhaBirdShow());
	RegestMsg(new LMsgS2CUserGangThinkCard());

	RegestMsg(new LMsgLMG2CEAddAgent());
	RegestMsg(new LMsgCE2LMGAddAgent());

	RegestMsg(new LMsgLMG2CERank());
	RegestMsg(new LMsgCE2LMGRank());
	RegestMsg(new LMsgLMG2CEGetRankReward());

	RegestMsg(new LMsgC2SUpdateRank());
	RegestMsg(new LMsgS2CUpdateRank());
	RegestMsg(new LMsgC2SGetRankReward());
	RegestMsg(new LMsgS2CGetRankReward());

	RegestMsg(new LMsgC2SLocation());
	RegestMsg(new LMsgS2CLocation());

	RegestMsg(new LMsgC2SShare());
	RegestMsg(new LMsgS2CShare());

	RegestMsg(new LMsgL2LDBTaskInfo());

	RegestMsg(new LMsgC2STaskInfo());
	RegestMsg(new LMsgS2CTaskInfo());
	RegestMsg(new LMsgC2STaskReward());
	RegestMsg(new LMsgS2CTaskReward());

	return true;
}

bool LMsgFactory::Final()
{
	std::map<int,LMsg*>::iterator it = m_msgMap.begin();
	for(;it != m_msgMap.end();++it)
	{
		delete it->second;
	}
	return true;
}

void LMsgFactory::RegestMsg(LMsg* pMsg)
{
	std::map<int,LMsg*>::iterator it = m_msgMap.find(pMsg->m_msgId);
	if(it != m_msgMap.end())
	{
		LLOG_INFO("LMsgFactory::RegestMsg msg exiset msgId:%d",pMsg->m_msgId);
	}
	else
	{
		m_msgMap[pMsg->m_msgId] = pMsg;
	}
}

LMsg* LMsgFactory::CreateMsg(int msgId)
{
	std::map<int,LMsg*>::iterator it = m_msgMap.find(msgId);
	if(it == m_msgMap.end())
	{
		//LLOG_INFO("LMsgFactory::CreateMsg msg not exiset msgId:%d",msgId);
		return NULL;
	}
	else
	{
		return it->second->Clone();
	}
}
