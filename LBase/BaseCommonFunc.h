#ifndef _BASE_COMMON_FUNC_
#define _BASE_COMMON_FUNC_

#include "LBuff.h"
#include "LSocket.h"
#include "LRunnable.h"
#include "LMsg.h"

#ifdef WIN32

void disableConsoleCloseButton();

#endif

Lstring caculateMd5ToHex(const void* pData, unsigned int uDataLen);

/*
解析客户端发过来的消息

消息格式：
|--2 bytes(验证头长度)--|--N bytes(验证头)--|--2 bytes(消息实体长度)--|--1 byte(是否经过封包)--|--N bytes(消息实体)--|
*/
bool parserClientMsgPack(LBuffPtr recv, 
						 boost::shared_ptr<LMsgC2SVerifyHead>& msgVerifyHead, 
						 boost::shared_ptr<LMsg>& msgEntity,
						 boost::shared_ptr<LBuff>& msgOriginData);

//Format
// |--1 byte(isPacket)--|--N bytes(data)--|
LMsg* parserMsgPack(LBuffPtr recv, LSocketPtr s);

void disableIP(std::string strIP);

unsigned int caculateHashKey(const char* pchData, int iDataLen);

//更新随机因子
void updateRandomFactor();	

///////////////////////Function///////////////////////////////////////////////////

std::string convertInt2String(const int iValue);
std::string convertUInt2String(const unsigned int uValue);

#endif