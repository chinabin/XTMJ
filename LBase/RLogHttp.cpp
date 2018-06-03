/*************************************************************************
> File Name: RLogHttp.cpp
> Author: 
> Mail: 
> Created Time: Mon 24 Oct 2016 02:00:28 PM PDT
************************************************************************/

#include "LLog.h"
#include "RLogHttp.h"

#define MAX_LOG_SIZE   1024
#define DEFAULT_MAX_CACHED_LOG_SIZE   1024

RLogHttp::RLogHttp()
{ 
	curl = NULL;
	list = NULL;

	list = curl_slist_append(list, "Content-Type: text/plain; charset=UTF-8");
	list = curl_slist_append(list, "Expect:");

	m_buff = NULL;

	m_cached_size = 0;
	m_cached_log_num = 0;
}

RLogHttp::~RLogHttp()
{
	if(list != NULL)
	{
		curl_slist_free_all(list);
	}

	if(curl != NULL)
	{
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();

	if(m_buff != NULL)
	{
		free(m_buff);
		m_buff = NULL;
	}
}

bool RLogHttp::Init()
{
	LTime cur;
	m_tickTimer = cur;
	m_1SceTick = cur.MSecs();
	m_minSendSceTick = cur.MSecs();
	return true;
}

bool RLogHttp::Final()
{
	return true;
}

bool RLogHttp::Init(bool bopen, const std::string& url, size_t max_cached_size, size_t max_cached_num)
{
	if (curl)
	{
		LLOG_DEBUG("[rlog] You have inited RLogHttp");
		return false;
	}

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (!curl)
	{
		LLOG_ERROR("[rlog] Libcurl init failed");
		return false;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10); // 超时时间10秒
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);        //屏蔽其它信号
	curl_easy_setopt(curl, CURLOPT_POST, 1); //设置post

	SetConfig( bopen, url, max_cached_size, max_cached_num );
	m_curconfig = m_setconfig;

	m_buff = (char *)calloc(m_curconfig.m_max_cached_size, 1);
	memset(m_buff, 0, m_curconfig.m_max_cached_size);
	m_cached_size = 0;
	m_cached_log_num = 0;
	
	return true;
}

bool RLogHttp::SetConfig( bool bopen, const std::string& url, size_t max_cached_size, size_t max_cached_num )
{
	boost::mutex::scoped_lock l(m_msgMutex);

	if(max_cached_size < DEFAULT_MAX_CACHED_LOG_SIZE)
	{
		LLOG_ERROR("[rlog] max_cached_size : %d < %d, set to %d", max_cached_size, DEFAULT_MAX_CACHED_LOG_SIZE, DEFAULT_MAX_CACHED_LOG_SIZE); 
		max_cached_size = DEFAULT_MAX_CACHED_LOG_SIZE;
	}
	else if ( max_cached_size > DEFAULT_MAX_CACHED_LOG_SIZE*1024 )
	{
		LLOG_ERROR("[rlog] max_cached_size : %d > %d, set to %d", max_cached_size, DEFAULT_MAX_CACHED_LOG_SIZE*1024, DEFAULT_MAX_CACHED_LOG_SIZE*1024); 
		max_cached_size = DEFAULT_MAX_CACHED_LOG_SIZE*1024;
	}

	if(max_cached_num < 10)
	{
		LLOG_ERROR("[rlog] max_cached_num : %d < 10, set to 10", max_cached_num); 
		max_cached_num = 10;
	}

	bool bchange = false;
	if ( m_setconfig.bopenrlog != bopen )
	{
		bchange = true;
		m_setconfig.bopenrlog = bopen;
	}
	if ( m_setconfig.m_url != url )
	{
		bchange = true;
		m_setconfig.m_url = url;
	}
	if ( m_setconfig.m_max_cached_size != max_cached_size )
	{
		bchange = true;
		m_setconfig.m_max_cached_size = max_cached_size;
	}
	if ( m_setconfig.m_max_cached_num != max_cached_num )
	{
		bchange = true;
		m_setconfig.m_max_cached_num = max_cached_num;
	}
	return bchange;
}

RLogConfig RLogHttp::GetCoinfig()
{
	boost::mutex::scoped_lock l(m_msgMutex);
	return m_setconfig;
}

void RLogHttp::Run()
{
	Init();
	while(true)
	{
		LTime cur;
		if(cur.MSecs() - m_tickTimer.MSecs() > 1)
		{
			m_tickTimer = cur;
			Tick(cur);	
		}

		Lstring* str = (Lstring*)Pop();
		if(str == NULL)
		{
			Sleep(1);
			continue;
		}
		else
		{
			rlog(*str);
			delete str;
		}
	}
	Final();
}

void RLogHttp::Tick(LTime& cur)
{
	//1秒循环一次
	if (cur.MSecs() - m_1SceTick > 1000)
	{
		m_1SceTick = cur.MSecs();
		
		// 检查配置是否变化了
		checkConfig();
	}
	if (cur.MSecs() - m_minSendSceTick > 300*1000)
	{
		m_minSendSceTick = cur.MSecs();
		sendLog();
	}
}

void RLogHttp::checkConfig()
{
	boost::mutex::scoped_lock l(m_msgMutex);
	if ( !(m_setconfig == m_curconfig) )
	{
		// 配置发生了变化 先把现有的数据发送了
		sendLog();

		m_curconfig.bopenrlog = m_setconfig.bopenrlog;
		if ( m_setconfig.m_url != m_curconfig.m_url )
		{
			m_curconfig.m_url = m_setconfig.m_url;
			curl_easy_setopt(curl, CURLOPT_URL, m_curconfig.m_url.c_str());
		}
		if ( m_setconfig.m_max_cached_size != m_curconfig.m_max_cached_size )
		{
			if(m_buff != NULL)
			{
				free(m_buff);
			}
			m_curconfig.m_max_cached_size = m_setconfig.m_max_cached_size;
			m_buff = (char *)calloc(m_curconfig.m_max_cached_size, 1);
			memset(m_buff, 0, m_curconfig.m_max_cached_size);
		}
		m_curconfig.m_max_cached_num = m_setconfig.m_max_cached_num;
	}
}

int RLogHttp::rlog(const Lstring& log)
{
	if ( !m_curconfig.bopenrlog )
	{
		return -1;
	}
	size_t log_size = log.size();
	if(log_size > MAX_LOG_SIZE - 2)
	{
		LLOG_ERROR("[rlog] this log is too large, log size: %d, limit max log size: %d", log_size, MAX_LOG_SIZE - 1);
		return -1;
	}

	if( m_buff == NULL )
	{
		LLOG_ERROR("[rlog] _buff is null!");
		return -1;
	}

	// 满足条件发送数据
	if ((m_cached_size + log_size) > m_curconfig.m_max_cached_size - 2 || m_cached_log_num > m_curconfig.m_max_cached_num)
	{
		sendLog();
	}

	memcpy(m_buff+m_cached_size, log.data(), log_size);
	++m_cached_log_num;
	m_cached_size += log_size;
	return 0;
}

void RLogHttp::sendLog()
{
	if ( m_cached_size == 0 )
	{
		// 没有数据发送
		return;
	}
	LTime cur;
	m_minSendSceTick = cur.MSecs();

	int ret = sendToServer();
	if (ret < 0)
	{
		// 发送失败也删掉
	}
	else
	{
		// 发送成功
		LTime cur2;
		LLOG_ERROR("[rlog] send rlog data, m_cached_size: %d, m_cached_log_num: %d, time: %lld", m_cached_size, m_cached_log_num, cur2.MSecs() - cur.MSecs());
	}
	//	清楚数据
	memset(m_buff, 0, m_curconfig.m_max_cached_size);
	m_cached_size = 0;
	m_cached_log_num = 0;
}

int RLogHttp::sendToServer()
{
	if(curl == NULL)
	{
		LLOG_ERROR("[rlog] sendLog curl is null!");
		return -1;
	}

	if( m_buff == NULL )
	{
		LLOG_ERROR("[rlog] sendLog _buff is null!");
		return -1;
	}

	CURLcode res = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, m_buff);
	if(res != CURLE_OK)
	{
		LLOG_ERROR("RLogHttp::sendLog curl_easy_setopt() failed: %s", curl_easy_strerror(res));
		return -1;
	}
	res = curl_easy_perform(curl);
	if(res != CURLE_OK)
	{
		LLOG_ERROR("RLogHttp::sendLog curl_easy_perform() failed: %s", curl_easy_strerror(res));
		return -1;
	}
	return 0;
}


