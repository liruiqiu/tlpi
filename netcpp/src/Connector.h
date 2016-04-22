#ifndef _CONNECTOR_H
#define _CONNECTOR_H
#include "Util.h"
#include "Message.h"
#include <netinet/in.h>

class CmStream
{
	public:
		CmStream();
		~CmStream();
		void	Clear();

	public:
		bool	AllocBuffer(uint32_t uSize);
		bool	FreeBuffer();
		char*	GetBuffer();
		char*	GetHeadBuffer();
		char*	GetTailBuffer();
		bool	AddBuffer(const char* buf, uint32_t len);

		uint32_t	GetSize();
		uint32_t	GetMaxSize();
		uint32_t	GetFreeSize();

		bool	PushBack(uint32_t uSize);
		bool	PopHead(uint32_t uSize);
		void	SetHead(uint32_t h) {m_uHead=h;}
		void	SetTail(uint32_t t) {m_uTail=t;}
		void	AddMsgHeader();
		void	ResetPos();
		ConnectorID_t ConnectorID;
	private:
		char*	m_pBuffer;
		uint32_t	m_uHead;
		uint32_t	m_uTail;
		uint32_t	m_uMaxSize;
};

class Socket 
{
	friend class Connector;
	public:
	Socket();
	~Socket();

	uint32_t	send_socket(const void *buf, uint32_t len, uint32_t flags = 0);
	uint32_t 	receive_socket(void *buf, uint32_t len, uint32_t flags = 0);
	bool listen_socket(uint32_t port, const char *ip = NULL,  int32_t backlog = 5);
	bool accept_socket(Socket *socket);

	bool	create_socket();
	void	close_socket();
	bool	connect_socket(const char *host, int port);

	bool	setNonBlocking();
	bool	setLinger(unsigned int lingertime);
	bool	isValid() const;
	bool	isSockError() const;
	int32_t	getFD() const;
	void 	setFD(int32_t fd);
	int32_t 	getSOCKET() const { return m_SocketID;}
	const char 	*getIP() const { return m_IP;}
	private:
	int32_t			m_SocketID;
	sockaddr_in m_SockAddr;
	char			m_IP[IP_SIZE];
	in_port_t	m_Port;
};

class Connector
{
	friend class ConnectorManager_Client;
	public:
	Connector(void);
	~Connector(void);

	// 初始化Connector时调用。
	// 初始化Socket。
	void	InitSocket();

	// 清理操作。
	void	Clear();

	// 读取数据
	bool	Handle_Input();

	// 发送数据
	bool	IsOutput() { return m_pSendStream != NULL; }
	bool	Handle_Output();

	bool SendPacket(CmStream *pSendStream);

	public:
	bool			IsValid();

	Socket*			GetSocket() const				{ return m_pSocket; }
	void			SetSocket(Socket* pSocket);

	ConnectorID_t	GetConnectorID() const			{ return m_ConnectorID; }
	void			SetConnectorID(ConnectorID_t id){ m_ConnectorID = id; }

	ID_t			GetManagerID() const 			{ return m_ManagerID; }
	void			SetManagerID(ID_t id)			{ m_ManagerID = id; }

	bool			IsEmpty() const					{ return m_IsEmpty; }
	void			SetEmpty(bool b = true)			{ m_IsEmpty = b; }

	uint32_t		GetStatus() const				{ return m_Status; }
	void			SetStatus(uint32_t status)		{ m_Status = status; }

	bool			IsDisconnect()					{ return m_bDisconnect; }
	void			SetDisconnect(bool b)			{ m_bDisconnect = b; }

	void			SetCreateTime() 				{m_CreateTime = time(NULL);}

	private: // 线程安全
	time_t  			m_CreateTime;		/* 有效生命周期*/
	ConnectorID_t		m_ConnectorID;			// Connector连接ID
	uint32_t			m_Status;			// Connector状态
	bool				m_bDisconnect;			// 是否断开连接
	bool				m_IsEmpty;				// 在管理器中，标识Connector是否被使用
	ID_t				m_ManagerID;			// Connector在管理器中的ID
	uint8_t				m_PacketIndex;			// 发送包序列号
	MsgID_t				m_LastPacketID;		// 上一个收到包的ID
	Socket				*m_pSocket;			// Socket指针
	CmStream			*m_pRecieveStream;		// 接收流缓存
	CmStream			*m_pSendStream;			// 发送流缓存
	NET_MSG_HEAD 	NetHead;
	uint32_t 		NetHeadLen;
};

#endif
