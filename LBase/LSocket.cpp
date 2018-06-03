#include "LSocket.h"
#include "LNet.h"
#include "LMsg.h"
#include "LLog.h"

#define RECV_BUFFER_SIZE	(8 * 1024)

LSocket::LSocket(LNet& ls)
	:m_ns(ls),
	m_socket(ls.GetIoservice()),
	m_strand(ls.GetIoservice()),
	m_connectStatus(SOCKET_CONNECT_STATUS_DISCONNECTED),
	m_start(false),
	m_sending(false),
	m_remotePort(0),
	m_iLocalPort(0),
	m_active(false)
{
	m_pRecvBuff = new char[RECV_BUFFER_SIZE];
	memset(m_pRecvBuff, 0, RECV_BUFFER_SIZE);

	m_iCurrentSendSize	= 0;
}

LSocket::~LSocket()
{
	if(m_pRecvBuff)
	{
		delete [] m_pRecvBuff;
	}
}

bool LSocket::Connect(boost::asio::ip::tcp::endpoint endpoint)
{
	if(m_connectStatus != SOCKET_CONNECT_STATUS_DISCONNECTED)
	{
		LLOG_ERROR("LSocket::Connect -- Need to stop firtly");
		return false;
	}

	try
	{
		m_connectStatus = SOCKET_CONNECT_STATUS_CONNECTING;
		m_socket.connect(endpoint);
		Start();
	}
	catch(...)
	{
		m_connectStatus = SOCKET_CONNECT_STATUS_DISCONNECTED;
		LLOG_ERROR("LSocket::Connect -- Excetion on connnect endpoint");
	}

	return m_connectStatus == SOCKET_CONNECT_STATUS_CONNECTED ? true : false;
}

bool LSocket::Connect(Lstring ip,Lshort port)
{
	bool bRet = false;
	try
	{
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(ip), port);
		bRet = Connect(endpoint);
	}
	catch(...)
	{
		LLOG_ERROR("LSocket::Connect -- Excetion on connnect ip and port");
	}	

	return bRet;
}

void LSocket::AsyncConnect(boost::asio::ip::tcp::endpoint endpoint)
{
	if(m_connectStatus != SOCKET_CONNECT_STATUS_DISCONNECTED)
	{
		LLOG_ERROR("LSocket::AsyncConnect -- Need to stop firtly");
		return;
	}

	try
	{
		m_connectStatus = SOCKET_CONNECT_STATUS_CONNECTING;
		m_socket.async_connect(endpoint,
			boost::bind(&LSocket::HandleAsyncConnect,
			shared_from_this(),
			boost::asio::placeholders::error));
	}
	catch(...)
	{
		LLOG_ERROR("LSocket::AsyncConnect -- Exception on anyncconnect with endpoint");
		m_connectStatus = SOCKET_CONNECT_STATUS_DISCONNECTED;
		_sendKickMsg();
	}
}

void LSocket::AsyncConnect(Lstring ip,Lshort port)
{
	try
	{
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(ip), port);
		AsyncConnect(endpoint);
	}
	catch(...)
	{
		LLOG_ERROR("LSocket::AsyncConnect -- Exception on anyncconnect with ip and port");
		_sendKickMsg();
	}
	
}

bool LSocket::HandleAsyncConnect(const boost::system::error_code& e)
{
	if(!e)
	{
		Start();
	}
	else
	{
		LLOG_ERROR("LSocket::HandleAsyncConnect error = %s", e.message().c_str() );
		m_connectStatus = SOCKET_CONNECT_STATUS_DISCONNECTED;
		_sendKickMsg();
	}

	return !e;
}

void LSocket::Start()
{	
	if(m_start)
	{
		LLOG_ERROR("LSocket::Start -- Start had been invoked");
		return;
	}

	m_connectStatus = SOCKET_CONNECT_STATUS_CONNECTED;

	try
	{
		m_curretSendBuffer.reset();
		m_currentReadBuffer.reset(new LBuff);
		m_iCurrentSendSize	= 0;

		boost::asio::ip::tcp::socket::endpoint_type remoteEndPoint = m_socket.remote_endpoint();
		m_strRemoteIp = remoteEndPoint.address().to_v4().to_string();
		m_remotePort  = remoteEndPoint.port();

		LLOG_DEBUG("client login ip=%s,port=%d", m_strRemoteIp.c_str(), m_remotePort);

		boost::asio::ip::tcp::socket::endpoint_type localEndPoint = m_socket.local_endpoint();
		m_strLocalIp = localEndPoint.address().to_v4().to_string();
		m_iLocalPort = localEndPoint.port();
	}
	catch(...)
	{
		LLOG_WARN("LSocket::Start -- Exception in get endpoint");					// 客户端连上后立刻断开会导致remote_endpoint（）异常
		Stop(true);
		return;
	}

	m_start = true;

	//通知client in
	LBuffPtr clientIn(new LBuff());
	clientIn->Write((unsigned char)0);	//需要多写一位用于区分是否是packet包
	clientIn->Write((int)MSG_CLIENT_IN);
	OnRecvOneMsgPack(clientIn, true);

	try
	{
		m_socket.async_read_some(boost::asio::buffer(m_pRecvBuff, RECV_BUFFER_SIZE),
			boost::bind(&LSocket::HandleRead,
			shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	catch(...)
	{
		LLOG_ERROR("LSocket::Start -- Exception on read");
		Stop(true);
		return;
	}
}

void LSocket::Stop(bool bKick)
{
	boost::mutex::scoped_lock l(m_mutexSocketForStop);

	if(m_connectStatus == SOCKET_CONNECT_STATUS_DISCONNECTED || m_connectStatus == SOCKET_CONNECT_STATUS_DISCONNECTING)
	{
		LLOG_ERROR("Socket is operating. Status = %d", m_connectStatus);
		return;
	}

	try
	{
		m_connectStatus = SOCKET_CONNECT_STATUS_DISCONNECTING;
		m_start = false;
		m_strand.post(boost::bind(&LSocket::_stop, shared_from_this(), bKick ? true : false));
	}
	catch(...)
	{
		LLOG_ERROR("LSocket::Stop -- Exception on stop");
	}
}

int LSocket::getSendPoolSize()
{
	boost::mutex::scoped_lock l(m_mutexSocketForWrite);
	if(!m_start)
	{
		return 0;
	}

	return m_sendPool.size();
}

void LSocket::OnRecvOneMsgPack(LBuffPtr buff, bool bIsFromInternal)
{
	m_ns.RecvMsgPack(buff,shared_from_this(), bIsFromInternal);
}

void LSocket::HandleRead(const boost::system::error_code& e,std::size_t readSize)
{
	boost::mutex::scoped_lock l(m_mutexSocketForRead);
	if(!m_start)
	{
		return;
	}

	if(!e)
	{ 
		if(readSize > 0)
		{
			m_currentReadBuffer->Write(m_pRecvBuff, readSize);

			//LLOG_ERROR("read size = %d", m_iCurrentReadSize);
			if(OnRecv())
			{
				try
				{
					m_socket.async_read_some(boost::asio::buffer(m_pRecvBuff, RECV_BUFFER_SIZE),
						boost::bind(&LSocket::HandleRead,
						shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
				}
				catch(...)
				{
					LLOG_ERROR("LSocket::HandleRead -- Read excaption on read");
				}
			}
			else
			{
				Kick();
			}
			
		}
		else //网络被关闭
		{
			Kick();
		}
	}
	else
	{
		LLOG_DEBUG("LSocket::HandleRead error = %s  ip = %s", e.message().c_str(), m_strRemoteIp.c_str() );
		Kick();
	}
}

void LSocket::HandleWrite(const boost::system::error_code& e, std::size_t writeSize)
{
	boost::mutex::scoped_lock l(m_mutexSocketForWrite);
	if(!m_start)
	{
		return;
	}
	//LLOG_ERROR("send111 size = %d", writeSize);
	if(!e)
	{
		if(m_curretSendBuffer.get() != NULL)
		{
			m_iCurrentSendSize += writeSize;

			int iRemainSize = m_curretSendBuffer->Size() - m_iCurrentSendSize;
			if(iRemainSize == 0)
			{
				m_iCurrentSendSize = 0;
				m_curretSendBuffer.reset();

				m_sending = false;
				m_sendPool.pop_front();
				Check();
			}
			else
			{
				LLOG_ERROR("send size = %d   %d", m_iCurrentSendSize, iRemainSize);

				try
				{
					m_socket.async_write_some(boost::asio::buffer(m_curretSendBuffer->Data() + m_iCurrentSendSize, iRemainSize),
						boost::bind(&LSocket::HandleWrite,
						shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
				}
				catch(...)
				{
					LLOG_ERROR("LSocket::HandleWrite -- Excaption on write");
				}
			}
		}
		else
		{
			Kick();
		}
	}	
	else
	{
		LLOG_DEBUG("LSocket::HandleWrite error = %s  ip = %s", e.message().c_str(), m_strRemoteIp.c_str() );
		Kick();
	}
}

bool LSocket::Check()
{
	if(!m_sending && m_sendPool.size() != 0)
	{
		m_sending = true;

		m_curretSendBuffer = m_sendPool.front();

		try
		{
			m_socket.async_write_some(boost::asio::buffer(m_curretSendBuffer->Data(), m_curretSendBuffer->Size()),
				boost::bind(&LSocket::HandleWrite,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		catch(...)
		{
			LLOG_ERROR("LSocket::Check -- Exception on Check");
		}
	}

	return true;
}

void LSocket::Send(LBuffPtr pBuff)
{
	boost::mutex::scoped_lock l(m_mutexSocketForWrite);
	if(m_start)
	{
		m_sendPool.push_back(pBuff);
		Check();
	}  
}

boost::asio::ip::tcp::socket& LSocket::Socket()
{
	return m_socket;
}

SOCKET_CONNECT_STATUS LSocket::getSocketConnectStatus()
{
	return m_connectStatus;
}

void LSocket::Kick()
{
	Stop(true);
}

bool LSocket::OnRecv()
{
	if(m_ns.GetIsHttp())
	{
		return onRecvHttp();
	}
	else
	{
		return onRecvBinary();
	}
}

Lstring LSocket::getLocalIp()
{
	return m_strLocalIp;
}

int	LSocket::getLocalPort()
{
	return m_iLocalPort;
}

Lstring LSocket::GetRemoteIp()
{
	return m_strRemoteIp;
}

int LSocket::GetRemotePort()
{
	return m_remotePort;
}

bool LSocket::GetActive()
{
	return m_active;
}

// |--2 bytes(body len)--|--2 bytes(checksum)--|-- 4 bytes(time)--|--4 bytes(message id)--|--1 byte(pack)--|--N bytes(data)--|
// |-----------------Head------------------------------------------------------------|--------------Body---------------------|
bool LSocket::onRecvBinary()
{
	Lint offsetData = 0;
	Lint iDataPacketSize = m_currentReadBuffer->Size();

	Lint iHeadLen = 12;
	bool bRet = true;
	do 
	{
		if(iDataPacketSize < iHeadLen)
		{
			break;
		}

		unsigned short uDataLen = 0;

		m_currentReadBuffer->SetOffset(offsetData);
		m_currentReadBuffer->Read(uDataLen);

		if(uDataLen > (0xFFFF - iHeadLen))
		{
			LLOG_ERROR("LSocket::onRecvBinary -- The data len more than max value");
			bRet = false;
			break;
		}

		Lint iTotalSize = uDataLen + iHeadLen; 

		if(iDataPacketSize < iTotalSize) //对比总的字节数
		{
			break;
		}

		LBuffPtr data(new LBuff());
		data->Write(m_currentReadBuffer->Data() + offsetData + iHeadLen, uDataLen);
		OnRecvOneMsgPack(data);

		offsetData += iTotalSize;
		iDataPacketSize -= iTotalSize;

		//接收到一个完整的消息，这个链接就算是激活了
		m_active = true;

	}while(true);

	if(offsetData > 0)
	{
		_completeReadData(offsetData);
	}
	
	return bRet;
}

bool LSocket::onRecvHttp()
{
	Lint offsetData = 0;
	Lint iDataPacketSize = m_currentReadBuffer->Size();
	do 
	{
		if(iDataPacketSize == 0)
		{
			break;
		}

		std::string head,body;
		std::string httpStr(m_currentReadBuffer->Data() + offsetData, iDataPacketSize);

		std::string::size_type posHeadEnd = httpStr.find("\r\n\r\n", 0);
		if(posHeadEnd == std::string::npos)
		{
			break;
		}

		//解析正文长度
		head = httpStr.substr(0, posHeadEnd+4);

		std::string::size_type posContentLenBegin = httpStr.find("Content-Length:",0);
		if(posContentLenBegin == std::string::npos)
		{
			LBuffPtr buff(new LBuff());
			buff->Write((unsigned char)0); //需要多写一位用于区分是否是packet包
			buff->Write(int(MSG_HTTP));
			buff->Write(head);
			buff->Write(body);
			OnRecvOneMsgPack(buff);

			offsetData += posHeadEnd + 4;
			iDataPacketSize -= posHeadEnd + 4; 
			continue;
		}

		std::string::size_type posContentEnd = httpStr.find("\r\n", posContentLenBegin);
		if(posContentEnd == std::string::npos)
		{
			break;
		}

		std::string strContentLength = httpStr.substr(posContentLenBegin+15, posContentEnd-posContentLenBegin-15);
		int dataLen = atoi(strContentLength.c_str());
		if((httpStr.size() - posHeadEnd - 4) < (std::size_t)dataLen )
		{
			break;
		}

		head = httpStr.substr(0,posHeadEnd+4);
		body = httpStr.substr(posHeadEnd+4,dataLen);
		LBuffPtr buff(new LBuff());
		buff->Write((unsigned char)0); //需要多写一位用于区分是否是packet包
		buff->Write(int(MSG_HTTP));
		buff->Write(head);
		buff->Write(body);
		OnRecvOneMsgPack(buff);

		offsetData += posHeadEnd + dataLen + 4;
		iDataPacketSize -= posHeadEnd + dataLen + 4;

	}while(true);

	if(offsetData > 0)
	{
		_completeReadData(offsetData);
	}
	return true;
}

void LSocket::_completeReadData(Lint iCompleteLen)
{
	if(m_currentReadBuffer.get() == NULL || iCompleteLen <= 0)
	{
		return;
	}

	LBuff* pReplace = new LBuff;
	if(iCompleteLen < m_currentReadBuffer->Size())
	{
		Lint iRemain = m_currentReadBuffer->Size() - iCompleteLen;
		pReplace->Write(m_currentReadBuffer->Data() + iCompleteLen, iRemain);
	}

	m_currentReadBuffer.reset(pReplace);
}

void LSocket::_stop(bool bKick)
{
	try
	{
		//销毁之前先拿到读和写的权限
		boost::mutex::scoped_lock lr(m_mutexSocketForRead);
		boost::mutex::scoped_lock lw(m_mutexSocketForWrite);

		boost::system::error_code error;
		m_socket.shutdown(boost::asio::socket_base::shutdown_both, error);
		if(error)
		{
			LLOG_WARN("LSocket::Stop -- Fail to shut down. Error = %s", error.message().c_str());
		}

		m_socket.close(error);
		if(error)
		{
			LLOG_ERROR("LSocket::Stop -- Fail to close socket. Error = %s", error.message().c_str());
		}
	}
	catch(...)
	{
		LLOG_ERROR("LSocket::_closeSocket -- Have exception on close socket");
	}

	m_connectStatus = SOCKET_CONNECT_STATUS_DISCONNECTED;

	if(bKick)
	{
		_sendKickMsg();
	}
}

void LSocket::_sendKickMsg()
{
	LBuffPtr kick(new LBuff());
	kick->Write((unsigned char)0); //需要多写一位用于区分是否是packet包
	kick->Write((int)MSG_CLIENT_KICK);
	OnRecvOneMsgPack(kick, true);
}

//////////////////////////////////////////////////////////////////////////
