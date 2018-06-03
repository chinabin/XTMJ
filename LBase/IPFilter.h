//
//  IPFilter.hpp
//  CardServer
//
//  Created by chen andycl on 16/8/1.
//  Copyright © 2016年 CardServer. All rights reserved.
//

#ifndef IPFilter_hpp
#define IPFilter_hpp

#include <stdio.h>
#include <iostream>
#include <memory>
#include <sys/types.h>
#include <time.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#if _LINUX
#	include <sys/time.h>
#	include <sys/socket.h>
#	include <netinet/if_ether.h>
#	include <net/if_arp.h>
#	include <netinet/in.h>
#	include <unistd.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <sys/ioctl.h>
#else
#	include <winsock2.h>
	typedef unsigned int socklen_t;
	//以下这段来自linux <bits/socket.h>
	enum
	{
		MSG_DONTWAIT = 0x40, /* Nonblocking IO.  */
	};
	#define MSG_DONTWAIT	MSG_DONTWAIT
#endif


using namespace std;

template <class T>

class  Singleton
{
public:
    static inline T* instance();
    
    
    int init()
    {
        return 0;
    }
    
    
protected:
    Singleton(void)
    {
    }
    virtual ~Singleton(void)
    {
    }
    Singleton(const Singleton<T>&){}
    
    
    Singleton & operator= (const Singleton<T> &){ return *this;}
    
    static auto_ptr<T> _instance;
    
};

template <class T>
auto_ptr<T> Singleton<T>::_instance;



template <class T>

inline T* Singleton<T>::instance()
{
    if( 0== _instance.get())
    {
        _instance.reset ( new T);
    }
    
    return _instance.get();
}


#define DECLARE_SINGLETON_CLASS( type ) \
friend class auto_ptr< type >;\
friend class Singleton< type >;


typedef struct _ip_packet
{
    unsigned int addr;
    unsigned int level;
    
    _ip_packet()
    {
        Clear();
    }
    
    void Clear()
    {
        addr = 0;
        level = 0;
    }
    
}IPPACKET;

class IPFilter
{
private:
    int nSockfd;
    std::string strServerHost;
    unsigned int nServerPort;
    
public:
	IPFilter()
	{ 
		nSockfd = -1; 
		nServerPort = 0;
	};
    ~IPFilter(){};
    
    int CreateIPFilterSocket();
    void SetIPFilterServer(std::string strIP, unsigned short port);
    void SendFilterIPPacket(unsigned int nAddr);
    void SendFilterIPPacket(std::string strIP);
};

typedef Singleton<IPFilter> App_IPFilter;


#endif /* IPFilter_hpp */
