/*
Bjtu GPS Yang Zi
2014-07-05
版本：	V1.0
	
多线程形式的Socket，模块共两个文件，.cpp和.h
	
	使用linux标准函数编写，在windows下使用宏定义的方式兼容
	基本结构分为4个类：
	CServer为主类，包括公用的函数，负责接收线程的开启和通用线程函数
		负责定义本机的sock，和远程端的地址
		具有断开连接的回调函数
		负责回收清理
	CUdp类继承CServer，重载接收处理函数，经过初始化后即可使用
	CTcpClient类继承CServer，与CUdp类似，初始化后，经过Connect后，与UDP一样
		可注册断开回调函数
	CTcpServer类继承CServer，他的线程函数用作accept，类中包含CTcpClient的vector
		用户可通过CTcpClient的vector访问所有客户
		每次accept具有回调函数，用户根据需要选择是否接受（不接受则断开连接，或其他动作）

	对于线程的退出，只使用线程自己退出。当线程接收出错时，会自动解除阻塞，
	并根据一个在堆中的变量（通过对象将指针传出）标志对象内容是否可用，是否需要调用close

	TCP服务器关闭流程：
	远程关闭：
		线程检测到错误->close->server_del->析构close->不调用server_del->对象不可用
	用户主动关闭：
		close->server_del->析构close->不调用server_del->对象不可用
			线程检测到错误->不调用close直接退出
*/

#ifndef MTSOCK_H
#define MTSOCK_H

#include "main.h"

#ifdef WIN32
//#include <windows.h>
#include <WinSock2.h>
//在mingw下需要使用-lwsock32

//需要用户自行调用初始化函数和终止函数
//WSADATA wsaData;
//int Ret;
//if ((Ret = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
//{
//	printf("WSAStartup failed with error %d\n", Ret);
//	return;
//}

//if (WSACleanup() == SOCKET_ERROR)
//{
//	printf("WSACleanup failed with error %d\n", WSAGetLastError());
//}
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#ifndef __GNUC__
#define pthread_t HANDLE
#else
#include <pthread.h>
#endif

typedef union
{
	u32 dw;
	u8 b[4];
} TYPE_u32u8;
class CServer//UPD和TCP通用的服务类
{
public:
	CServer();
	~CServer();
	int sock; //本机的sock
	void *para;//参数
	pthread_t ntid;
	volatile int *volatile exit_flag;//标志是否已经调用过close了,为1则对象内容不可用
	struct sockaddr_in toAddr;	//接收时对方的地址
	struct sockaddr_in sendAddr;	//要发送到的地址

	int Create(void);
	virtual void Close(void);
	virtual int pro(void){return 0;}
	//可在此添加断开连接时的回调函数
	void (*close_cb)(CServer &ser);
};

typedef void (*RX_CB)(u8 *d,int n,CServer &ser);
void MTSock_void_cb(u8 *d,int n,CServer &ser);

#define ERR_CREATE	1
#define ERR_BIND	2
#define UDP_ERR_THREAD	3

class CUdp : public CServer
{
public:
	CUdp()
	{
		cb=MTSock_void_cb;
	}
	~CUdp(){}
	//port num, ip address, and the receive call back function
	int	ini(const char *ip,int n, RX_CB rx);
	void SetAdd(const char *ip,int n);
	void SetAdd(int ip,int n);
	int	Send(u8 *p, int n);

	virtual int pro(void);//接收处理函数

	RX_CB cb;//接收到数据后的回调函数
	
};

////////////////////////////////////////////////////////////////////////

class CTcpClient : public CServer//TCP客户端也要接收，所以也和UDP一样继承server类
{
public:
	CTcpClient(){cb=MTSock_void_cb;server=0;}
	~CTcpClient(){}
	int isConnect;
	int	ini(const char *ip,int n, RX_CB rx);
	int	Connect(int ip,int n);
	int	Connect(const char *ip,int n);//主动连接外部服务器
	int	Send(u8 *p, int n);//发送数据
	virtual void Close(void);
	virtual int pro(void);//接收处理函数

	RX_CB cb;//接收到数据后的回调函数

	void *server;//如果是服务器建立的socket，则指向服务器
};

typedef int (*ACCEPT_CB)(CTcpClient &ser);//收到连接请求时的回调函数,返回0为接受
int MTSock_void_accept_cb(CTcpClient &ser);//accept的回调函数

class CTcpServer : public CServer//多线程模型的TCP服务器
{
public:
	CTcpServer()
	{
		accept_cb=MTSock_void_accept_cb;
		sock_list_lock=0;
	}
	~CTcpServer(){}

	map<string,shared_ptr<CTcpClient> > sock_list;//客户
	int sock_list_lock; //访问公共资源的锁
	void del(CTcpClient* clt)//从列表中移除客户对象
	{
		clt->server=0;//防止递归
		while(sock_list_lock);
		sock_list_lock=1;
		auto result = find_if(sock_list.begin(),sock_list.end(),
			[clt](pair<string,shared_ptr<CTcpClient> > pa)
		{
			return pa.second.get()==clt;
		}); //查找
		if (result != sock_list.end()) //找到
		{
			sock_list.erase(result);
		}
		sock_list_lock=0;
	}
	int	ini(const char *ip,int n,ACCEPT_CB cb);
	virtual int pro(void);//接收处理函数

	ACCEPT_CB accept_cb;//accept的回调函数
};

#endif
