#include "BaseCommonFunc.h"
#include "LLog.h"
#include <openssl/md5.h>

#if _LINUX
#include "IPFilter.h"
#endif

#ifdef WIN32
#include <tchar.h>
#include <Windows.h>

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	TCHAR* pCurrentTile = (TCHAR*)lParam;
	if(pCurrentTile == NULL)
	{
		return FALSE;
	}

	TCHAR szTitle[MAX_PATH] = {0};
	::GetWindowText(hWnd, szTitle, MAX_PATH);
	if(_tcscmp(szTitle, pCurrentTile) == 0)
	{
		HMENU hSystemMenu = ::GetSystemMenu(hWnd, FALSE);
		if(hSystemMenu)
		{
			::RemoveMenu(hSystemMenu, SC_CLOSE, MF_BYCOMMAND);
		}
	}

	return TRUE;
}

void disableConsoleCloseButton()
{
#if _DEBUG
	return;
#endif

	TCHAR szTitle[MAX_PATH] = {0};
	::GetConsoleTitle(szTitle, MAX_PATH);

	::EnumWindows(EnumWindowsProc, (LPARAM)szTitle);
}

#endif

Lstring caculateMd5ToHex(const void* pData, unsigned int uDataLen)
{
	if(pData == NULL || uDataLen == 0)
	{
		return "";
	}

	unsigned char sign[128] = {0};
	MD5((const unsigned char*)pData, uDataLen, sign);

	//转换成16进制
	char signHex[128] = {0};
	for(int i = 0 ; i < 16; ++i)
	{
		sprintf(signHex + i*2, "%02x", sign[i]);
	}

	return std::string(signHex);
}


LMsg* _unpackMessage(char* pData, unsigned short uDataLen)
{
	LMsg* pMessage = NULL;
	try
	{
		int iMsgId = -1;
		msgpack::unpacked  unpack;
		msgpack::unpack(&unpack, pData, uDataLen);
		msgpack::object obj = unpack.get();		
		if(obj.is_nil())
		{
			LLOG_ERROR("parserMsgPack -- Fail to unpack data");
			return NULL;
		}

		ReadMapData(obj, "m_msgId", iMsgId);

		pMessage = LMsgFactory::Instance().CreateMsg(iMsgId);
		if(pMessage != NULL)
		{
			pMessage->Read(obj);
		}
		else
		{
			LLOG_ERROR("parserMsgPack -- unpacked error msgId = %d", iMsgId);
		}
	}
	catch(...)
	{
		LLOG_ERROR("parserMsgPack -- Exception on unpack");
		if(pMessage)
		{
			delete pMessage;
			pMessage = NULL;
		}
	}

	return pMessage;
}

/*
|--2 bytes(checksum)--4 bytes(时间戳)--|--2 bytes(验证头长度)--|--N bytes(验证头)--|--2 bytes(消息实体长度)--|--1 byte(是否经过封包)--|--N bytes(消息实体)--|
*/
bool parserClientMsgPack(LBuffPtr recv, 
						 boost::shared_ptr<LMsgC2SVerifyHead>& msgVerifyHead, 
						 boost::shared_ptr<LMsg>& msgEntity,
						 boost::shared_ptr<LBuff>& msgOriginData)
{
	msgVerifyHead.reset();
	msgEntity.reset();

	bool bRet = false;
	int iDataRemainSize = recv->Size();
	do 
	{
		unsigned short uHeadLen = 0;
		if(iDataRemainSize < sizeof(uHeadLen))
		{
			LLOG_ERROR("Don't get any message head size");
			break;
		}

		recv->Read(uHeadLen);
		iDataRemainSize -= sizeof(uHeadLen);
		if(uHeadLen > iDataRemainSize)
		{
			LLOG_ERROR("Head's size is bigger than data len");
			break;
		}

		LMsg* pMessage = _unpackMessage(recv->Data() + recv->GetOffset(), uHeadLen);
		if(pMessage == NULL)
		{
			LLOG_ERROR("Fail to parser message head");
			break;
		}
		if(pMessage->m_msgId != MSG_C_2_S_VERIFY_HEAD)
		{
			LLOG_ERROR("The package isn't head package");
			delete pMessage;
			break;
		}
		LMsgC2SVerifyHead* pMsgHead = dynamic_cast<LMsgC2SVerifyHead*>(pMessage);
		msgVerifyHead.reset(pMsgHead);
		iDataRemainSize -= uHeadLen;

		int iEntityOffetSet = uHeadLen + sizeof(uHeadLen);
		recv->SetOffset(iEntityOffetSet);

		unsigned short uEntityLen = 0;
		if(iDataRemainSize < sizeof(uEntityLen))
		{
			LLOG_ERROR("Entiy's len is enough");
			break;;
		}
		recv->Read(uEntityLen);
		iDataRemainSize -= sizeof(uEntityLen);
		if(uEntityLen > iDataRemainSize)
		{
			LLOG_ERROR("Entity's size is bigger than data len");
			break;
		}

		LBuffPtr bufMsg(new LBuff);
		bufMsg->Write(recv->Data() + recv->GetOffset() + 1, uEntityLen - 1); //跳过封包字节，来自客户端肯定是被封包过的

		if(msgVerifyHead->m_iMd5Begin <= 0 || msgVerifyHead->m_iMd5End <= 0 || msgVerifyHead->m_iMd5End < msgVerifyHead->m_iMd5Begin || msgVerifyHead->m_iMd5End > bufMsg->Size())
		{
			LLOG_ERROR("Begin or end is wrong");
			break;
		}

		//根据md5值判断包的合法性
		std::string strMd5Data;
		for(int i = msgVerifyHead->m_iMd5Begin - 1; i < msgVerifyHead->m_iMd5End; ++i)
		{
			unsigned char tmp = *((char*)(bufMsg->Data() + i));
			char convert[10] = {0};
			sprintf(convert, "%02X", tmp);
			strMd5Data += convert;
		}
		strMd5Data += "Uj4!LE8z6l*O4j,Y";
		std::string strMd5 = caculateMd5ToHex(strMd5Data.c_str(), strMd5Data.size());
		if(strMd5.compare(msgVerifyHead->m_strMd5) != 0)
		{
			LLOG_ERROR("Md5 is error! s = %s  --  c = %s", strMd5.c_str(), msgVerifyHead->m_strMd5.c_str());
			break;
		}

		pMessage = _unpackMessage(bufMsg->Data(), bufMsg->Size());
		if(pMessage == NULL)
		{
			LLOG_ERROR("Fail to parser message entity");
			break;
		}
		msgEntity.reset(pMessage);
		msgOriginData = bufMsg;
		msgOriginData->SetOffset(0);

		bRet = true;
	}while(false);

	return bRet;
}

LMsg* parserMsgPack(LBuffPtr recv, LSocketPtr s)
{
	if(recv->Size() < 1)
	{
		LLOG_ERROR("InsideNet -- The package size is zero");
		return NULL;
	}

	//先去第一个字节看是否为封包过
	unsigned char isPacket = 0;
	recv->Read(isPacket);

	int iMsgId = -1;
	LMsg* pRetMsg = NULL;
	if(isPacket)
	{

		pRetMsg = _unpackMessage(recv->Data() + recv->GetOffset(), recv->Size() - recv->GetOffset());
		if(pRetMsg)
		{
			pRetMsg->m_sp = s;
		}
	}
	else
	{
		recv->Read(iMsgId);

		pRetMsg = LMsgFactory::Instance().CreateMsg(iMsgId);
		if(pRetMsg != NULL)
		{
			pRetMsg->Read(*recv.get());
			pRetMsg->m_sp = s;
		}
		else
		{
			LLOG_ERROR("parserMsgPack -- read error msgId = %d", iMsgId);
		}
	}

	return pRetMsg;
}

void disableIP(std::string strIP)
{
#if _LINUX
	static bool bCteate = false;
	static boost::mutex mutexCreate;
	if(!bCteate)
	{
		boost::mutex::scoped_lock l(mutexCreate);
		if(!bCteate)
		{
			if(App_IPFilter::instance()->CreateIPFilterSocket() == 0)
			{
				bCteate = true;
				App_IPFilter::instance()->SetIPFilterServer("127.0.0.1", 30000);
			}
		}
	}

	if(bCteate)
	{
		App_IPFilter::instance()->SendFilterIPPacket(strIP);
	}

#endif
}

//BKDR Hash
unsigned int caculateHashKey(const char* pchValue, int iDataLen)
{
	if(pchValue == NULL)
	{
		return 0;
	}

	unsigned int uSeed = 131 ;// 31 131 1313 13131 131313 etc..  
	unsigned int uHash = 0; 

	while(*pchValue && iDataLen > 0) 
	{ 
		uHash = uHash*uSeed + (*pchValue++);
		iDataLen--;
	} 

	return uHash;
}

void updateRandomFactor()
{
	static Lstring strSeed = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*(){}[];,.?|";
	Lstring strKey;
	for(int i = 0; i < 128; ++i)
	{
		int iRand = L_Rand(0, strSeed.length() - 1);
		strKey += strSeed.at(iRand);
	}

	unsigned int iHash = caculateHashKey(strKey.c_str(), strKey.length());
	srand(iHash);
}

std::string convertInt2String(const int iValue)
{
	char chValue[100] = {0};
	sprintf(chValue, "%d", iValue);

	return std::string(chValue);
}

std::string convertUInt2String(const unsigned int uValue)
{
	char chValue[100] = {0};
	sprintf(chValue, "%u", uValue);

	return std::string(chValue);
}