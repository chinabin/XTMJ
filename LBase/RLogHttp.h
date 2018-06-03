/*************************************************************************
	> File Name: RLogHttp.h
	> Author: 
	> Mail: 
	> Created Time: Mon 24 Oct 2016 02:00:03 PM PDT
 ************************************************************************/

#ifndef _RLOGHTTP_H
#define _RLOGHTTP_H

#include <curl/curl.h>
#include <sstream>
#include "LBase.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LRunnable.h"

// 最后加上一个\n 换行
#define RLOG(type, content)  do { \
	std::basic_ostringstream<char> bo; \
	bo << type; \
	bo << "|"; \
	bo << content; \
	bo << '\n'; \
	gRLT.Push( new Lstring(bo.str()) ); \
} while(0)

struct RLogConfig
{
	bool bopenrlog;
	std::string m_url;
	size_t m_max_cached_size;
	size_t m_max_cached_num;

	bool operator == ( const RLogConfig& r )
	{
		return r.bopenrlog == bopenrlog && r.m_url == m_url && r.m_max_cached_size == m_max_cached_size && r.m_max_cached_num == m_max_cached_num;
	}
};

class RLogHttp : public LSingleton<RLogHttp>, public LRunnable
{
public:
	RLogHttp();
	virtual ~RLogHttp();

	virtual bool Init();
	virtual bool Final();

	bool Init(bool bopen, const std::string& url, size_t max_cached_log_size, size_t max_cached_log_num);

	// 支持多线程 返回值表示配置发生了变化
	bool SetConfig( bool bopen, const std::string& url, size_t max_cached_log_size, size_t max_cached_log_num );

	RLogConfig GetCoinfig();

private:
	virtual void Clear() {};

	virtual void Run(void);

	void Tick(LTime& cur);

	void checkConfig();

	int rlog(const Lstring& log);

	// 发送数据
	void sendLog();
	int sendToServer();

	CURL* curl;
	curl_slist* list;

	Lstring m_configname;
	RLogConfig m_setconfig;
	RLogConfig m_curconfig;
	boost::mutex m_configMutex;	// 对m_setconfig的锁

	LTime		m_tickTimer;
	Llong		m_1SceTick;//1秒循环一次
	Llong		m_minSendSceTick;//300秒循环一次

	// 缓存数据
	char* m_buff;
	// 记录当前缓存的长度和条数
	size_t m_cached_size;
	size_t m_cached_log_num;
};

#define gRLT RLogHttp::Instance()

#endif
