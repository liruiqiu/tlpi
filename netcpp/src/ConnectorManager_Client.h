#ifndef _CONNECTOR_MANAGER_CLIENT_H
#define _CONNECTOR_MANAGER_CLIENT_H
#include "ConnectorPool.h"
#include <vector>

class Connector;
struct epoll_event;

class ConnectorManager_Client
{
	public:
		ConnectorManager_Client(void);
		~ConnectorManager_Client(void);
		void Clear(void);
		bool Acceptor(uint32_t port,const char *ip = NULL,  uint32_t backlog = 5);
		bool Init(uint32_t uMaxSize);
		bool Tick();
		static void * RunTick(void * arg);
		// 接受新的连接
		Connector*	AcceptNewConnection();
		// 增加Connector
		bool	AddConnector(Connector *pConnector);
		// 取得Connector指针
		Connector*		GetConnector(ConnectorID_t ConnectorID);
		// 删除Connector(包括管理器，SOCKET)
		bool	DelConnector(Connector *pConnector);
		// 连接Server
		bool 		ConnectToServer(const char* ip,  uint16_t port, ConnectorID_t &ConnectorID);
		// 增加Connector的Socket
		bool		AddConnectorSocket(int32_t fd, ConnectorID_t ConnectorID);
		// 删除Connector的Socket
		bool 		DelConnectorSocket(int32_t fd);
		// 彻底删除Connector(包括管理器，SOCKET，EPOOL 事件队列中)
		bool 		RemoveConnector(Connector *pConnector);
		// 删除管理器中的所有Connector
		void		RemoveAllConnector();
		// 取得当前管理器中Connector数量
		uint32_t	GetConnectorCount() { return m_uConnectorCount; };

		// 增加Connector到管理器中
		bool	AddConnectorToMgr(ConnectorID_t ConnectorID);
		//从管理器  删除Connector
		bool	DelConnectorToMgr(ConnectorID_t ConnectorID);

		bool		Handle_Inputs();
		bool		Handle_Outputs();
		bool 		SendPacket(ConnectorID_t ConnectorID, const char *buf, uint32_t len);
		bool IsActive(){return m_Active;};
		void stop(){ m_Active = false;};
	private:
		bool	m_Active;
		FastLock						m_NewLock;
		std::vector<int32_t>		m_aNewID;
		FastLock						m_DelLock;
		std::vector<int32_t>		m_aDelID;
		FastLock						m_PacketLock;
		std::vector<CmStream *>  m_apPacket;
 	private:
		ConnectorID_t*		m_aConnecorID;				// ConnectorID数组
		uint32_t			m_uMaxConnectorCount;		// Connector最大数量
		uint32_t			m_uConnectorCount;			// Connector当前数量
		ConnectorPool*		m_pConnectorPool;			// ConnectorPool指针
	private:
		bool					m_bServerMode;
		Socket					*m_pServerSocket;
		int32_t					m_SocketID;				// 服务器SocketID
		int32_t					m_Epollfd;
		uint32_t				m_uEventsMaxSize;
		struct epoll_event*		events;
		int32_t					m_nEvents;
		uint32_t				m_uFDSize;				// 实际的网络连接数量
};
extern ConnectorManager_Client g_ConnectorManager_Client;
#endif
