/*
Bjtu GPS Yang Zi
2014-07-05
版本：	V1.0
	
	多线程形式的Socket
*/

#include "MTSock.h"

#ifdef WIN32

#pragma comment(lib,"ws2_32.lib")

#define close	closesocket
typedef int socklen_t;

#else

#endif

///////////////////////////////////////////////////////////////////
//通用服务类
//thread function:
static void *rx_fun(void *p)
{
	volatile int *volatile exit_flag=new int;//标志是否已经调用过close了
	*exit_flag=0;
	CServer *pserv = (CServer *)p;
	pserv->exit_flag=exit_flag;
	while (1)
	{
		int r=pserv->pro();
		if (r)
		{
			if (*exit_flag==0)//如果只是出错了
			{
				pserv->Close();
				pserv->exit_flag=0;
			}//若为1，则说明主动close，对象内容可能已经删除
			break;
		}
	}
	delete exit_flag;
	return NULL;
}
int CServer::Create(void)
{
	//(3)create the receive thread
	//int err = ethread_ini(rx_fun, this);
	//ethread_ini(rx_fun, this);
	//if (err != 0) //if failed to create thread
	//{
		//close(sock);
		//return 1;
	//}
	thread th1(rx_fun,this);
	th1.detach(); //避免析构,若线程已经退出，则会出错。所以可以加try
	return 0;
}
static void void_close_cb(CServer &)
{
	return;
}
CServer::CServer()
{
	close_cb=void_close_cb;
	//ntid = 0;
	exit_flag=0;
	memset(&toAddr, 0, sizeof(toAddr));
	toAddr.sin_family = AF_INET;
	memset(&sendAddr, 0, sizeof(sendAddr));
	sendAddr.sin_family = AF_INET;
}
void CServer::Close(void)
{
	//printf("sock:%d,exit_flag:%d\r\n",sock,*exit_flag);
	if (sock)//从来不关闭线程，等线程自己return
	{
		if(exit_flag)
		{
			*exit_flag=1;//必须先赋值
		}
		close(sock);
		sock=0;
	}
	close_cb(*this);
}
CServer::~CServer()
{
	Close();
}
void MTSock_void_cb(u8 *,int ,CServer &)
{

}
///////////////////////////////////////////////////////////////
//UDP部分

int CUdp::ini(const char *ip,int n, RX_CB rx)
{
	//(1)Create the socket
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0)
	{
		//failed to create socket
		return ERR_CREATE;
	}
	int value = 1024000;  
	setsockopt(sock, SOL_SOCKET, SO_RCVBUF,(char *)&value, sizeof(value));
	socklen_t valSize = sizeof(value);  
	getsockopt(sock, SOL_SOCKET, SO_RCVBUF,(char *)&value, &valSize);  
	printf("socket size: %d\n",value); 
	//(2)bind the port
	if (n)
	{
		struct sockaddr_in fromAddr;
		memset(&fromAddr, 0, sizeof(fromAddr));
		fromAddr.sin_family = AF_INET;
		fromAddr.sin_addr.s_addr = inet_addr(ip);
		fromAddr.sin_port = htons(n);
		if (::bind(sock, (struct sockaddr *) &fromAddr, sizeof(fromAddr)) < 0)
		{
			close(sock);
			sock = 0;
			return ERR_BIND;
		}
	}
	//(3)初始化线程
	if(Create())
	{
		return UDP_ERR_THREAD;
	}
	//(4)set the value of pointers
	cb = rx;
	return 0;
}

void CUdp::SetAdd(const char *ip,int n)
{
	sendAddr.sin_addr.s_addr = inet_addr(ip);
	sendAddr.sin_port = htons(n);
}

void CUdp::SetAdd(int ip,int n)
{
	sendAddr.sin_addr.s_addr = ip;
	sendAddr.sin_port = htons(n);
}

int CUdp::Send(u8 *p, int n)
{
	return sendto(sock,(const char*)p,n,0,(struct sockaddr *)&sendAddr,sizeof(sendAddr));
}
int CUdp::pro(void)//接收处理函数
{
	int len;
	int addrLen;
	char buf[65536];//接收的缓冲
	addrLen = sizeof(toAddr);
	len = recvfrom(sock, buf, sizeof(buf), 0,
		(struct sockaddr *) &toAddr, (socklen_t*)&addrLen);
	if(len > 0) //空包不发
	{
		cb((u8 *)buf,len,*this);
	}
	else if (len<0)//错误或终止
	{
		return -1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////
//TCP客户端

int MTSock_void_accept_cb(CTcpClient &ser)//accept的回调函数
{
	return 0;
}

int	CTcpClient::ini(const char *ip,int n, RX_CB rx)
{
	isConnect=0;
	if (sock)//若非零，则关闭重开
	{
		Close();
	}
	//首先建立端口
	sock=::socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		//failed to create socket
		return ERR_CREATE;
	}
	//(2)bind the port
	struct sockaddr_in fromAddr;
	memset(&fromAddr, 0, sizeof(fromAddr));
	fromAddr.sin_family = AF_INET;
	fromAddr.sin_addr.s_addr = inet_addr(ip);
	fromAddr.sin_port = htons(n);
	if (::bind(sock, (struct sockaddr *) &fromAddr, sizeof(fromAddr)) < 0)
	{
		close(sock);
		return ERR_BIND;
	}
	cb=rx;
	return 0;
}
void CTcpClient::Close(void)//重载,加入在服务器中维护list
{
	isConnect=0;
	CServer::Close();
	if (server)
	{
		((CTcpServer*)server)->del(this); //这以后很可能已经析构，不能再有任何操作
	}
}
int CTcpClient::Connect(const char *ip,int n)
{
	return Connect(inet_addr(ip), n);
}
int	CTcpClient::Connect(int ip,int n)//主动连接外部服务器
{
	isConnect=0;
	toAddr.sin_addr.s_addr=ip;
	toAddr.sin_port = htons(n);
	int t= ::connect(sock, (struct sockaddr *)&toAddr, sizeof(struct sockaddr));
	if(t) return t;
	isConnect=1;
	t+=Create();//开线程
	return t;
}
int	CTcpClient::Send(u8 *p, int n)//发送数据
{
	return ::send(sock, (const char*)p, n, 0);
}

int CTcpClient::pro(void)//接收处理函数
{
	int len;
	char buf[1024];
	len=recv(sock,buf,sizeof(buf),0); 
	if(len>0)
	{
		cb((u8*)buf,len,*this);
	}
	else if (len<=0)
	{
		return -1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//TCP服务器
int	CTcpServer::ini(const char *ip,int n,ACCEPT_CB cb)
{
	if (sock)//若非零，则关闭重开
	{
		Close();
	}
	//首先建立端口
	sock=::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0)
	{
		//failed to create socket
		return ERR_CREATE;
	}
	//(2)bind the port
	struct sockaddr_in fromAddr;
	memset(&fromAddr, 0, sizeof(fromAddr));
	fromAddr.sin_family = AF_INET;
	fromAddr.sin_addr.s_addr = inet_addr(ip);
	fromAddr.sin_port = htons(n);
	if (::bind(sock, (struct sockaddr *) &fromAddr, sizeof(fromAddr)) < 0)
	{
		close(sock);
		return ERR_BIND;
	}
	if(listen(sock,5)<0)
	{
		return -1;
	}
	accept_cb=cb;
	return Create();//创建线程开始accept
}
int CTcpServer::pro(void)//接收处理函数
{
	int len = sizeof(struct sockaddr_in);
	int nfd;
	// accept
	if((nfd = accept(sock, (struct sockaddr*)&toAddr, (socklen_t*)&len)) == -1)
	{
		return -1;
	}

	shared_ptr<CTcpClient> clt(new CTcpClient());
	clt->sock=nfd;
	clt->toAddr=toAddr;
	if(accept_cb(*clt))//若不接受，则断开
	{
		return 0;//析构里close
	}
	//开始接收
	if (clt->Create())//若开接收线程失败
	{
		return -1;
	}
	//加入列表
	clt->server=this;
	TYPE_u32u8 t;
	t.dw=*((u32*)&(toAddr.sin_addr));
	u16 tport=htons(toAddr.sin_port);
	char sbuf[100];
	sprintf(sbuf,"%d.%d.%d.%d:%d",t.b[0],t.b[1],t.b[2],t.b[3],tport);
	string s=sbuf;
	{
		while(sock_list_lock);
		sock_list_lock=1;
		sock_list[s]=clt;
		sock_list_lock=0;
	}
	return 0;
}

