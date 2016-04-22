#include "ConnectorManager_Client.h"
#include "Connector.h"
#include "Message.h"
#include "Net.h"

#define ACCEPT_ONESTEP			50

ConnectorManager_Client g_ConnectorManager_Client;

ConnectorManager_Client::ConnectorManager_Client(void) {
	Clear();
}

ConnectorManager_Client::~ConnectorManager_Client(void) {
	SAFE_DELETE_ARRAY(events);
	close(m_Epollfd);
	Clear();
}

void ConnectorManager_Client::Clear(void) {

	m_uConnectorCount = 0;
	m_uMaxConnectorCount = 0;
	m_aConnecorID = NULL;
	m_pConnectorPool = NULL;

	m_Epollfd = 0;
	m_uEventsMaxSize = 0;
	events = NULL;
	m_nEvents = 0;
	m_uFDSize = 0;
}

bool ConnectorManager_Client::Acceptor(uint32_t port,const char *ip,  uint32_t backlog) {

	m_pServerSocket = new Socket();
	m_pServerSocket->listen_socket(port, ip, backlog);

	m_pServerSocket->setNonBlocking();

	m_SocketID = m_pServerSocket->getSOCKET();
	Check(m_SocketID != INVALID_SOCKET);

	struct epoll_event	ev;
	ev.data.u64 = ToUint64( (uint32_t)m_SocketID, INVALID_ID);
	ev.events = EPOLLIN;
	epoll_ctl(m_Epollfd, EPOLL_CTL_ADD, m_SocketID, &ev);	

	m_bServerMode = true;
	return true;
}

bool ConnectorManager_Client::Init(uint32_t uMaxSize) {

	events = new struct epoll_event[uMaxSize];
	Check(events);
	m_Epollfd = epoll_create(uMaxSize);
	Check(m_Epollfd != INVALID_ID);
	if(m_Epollfd == INVALID_ID) return false;
	m_uEventsMaxSize = uMaxSize;


	uint32_t uMaxCount = uMaxSize;

	m_uMaxConnectorCount = uMaxCount;
	m_aConnecorID = new ConnectorID_t[uMaxCount];
	Check(m_aConnecorID);

	for(uint32_t i=0; i<uMaxCount; i++ )
	{
		m_aConnecorID[i] = INVALID_ID;
	}

	m_pConnectorPool = new ConnectorPool();
	Check(m_pConnectorPool);

	m_pConnectorPool->Init(uMaxSize);

	for(uint32_t i=0; i<m_pConnectorPool->GetMaxCount(); i++ )
	{
		Connector* pConnector = new Connector;
		m_pConnectorPool->InitPoolData(i,pConnector);
	}


	m_Active = true;

	//pthread_t id;
	//pthread_create(&id, NULL, ConnectorManager_Client::ThreadRunTick, NULL);
	//pthread_create(&id, NULL, ThreadRunTick, NULL);

	return true;
}
bool ConnectorManager_Client::AddConnector(Connector *pConnector){
	if( pConnector == NULL )
	{
		WriteLog(KERN_ERR "ConnectorManager::AddConnector(): Error ! pConnector == NULL" );
		return false;
	}

	bool ret = AddConnectorSocket(pConnector->GetSocket()->getFD(), pConnector->GetConnectorID());

	if ( ret == false)
	{
		WriteLog(KERN_ERR "ConnectorManager::AddConnector(): Error ! AddConnectorSocket Failed!" );	
		goto EXCEPTION;
		return false;
	}

	if( m_uConnectorCount >= m_uMaxConnectorCount )
	{
		WriteLog(KERN_ERR
				"ConnectorManager::AddConnector: Error ! count(%d) > maxcount(%d)",  
				m_uConnectorCount,
				m_uMaxConnectorCount
				);
		goto EXCEPTION;
		return false;
	}

	if( m_aConnecorID[m_uConnectorCount] != INVALID_ID )
	{
		WriteLog(KERN_ERR 
				"ConnectorManager::AddConnector(): Error ! m_aConnecorID[%d]==%d", 
				m_uConnectorCount, 
				m_aConnecorID[m_uConnectorCount] 
				);
		goto EXCEPTION;
		return false;
	}

	m_aConnecorID[m_uConnectorCount] = pConnector->GetConnectorID();
	pConnector->SetManagerID(m_uConnectorCount);
	m_uConnectorCount++;

	return true;
EXCEPTION:
	DelConnectorSocket(pConnector->GetSocket()->getFD());
	return false;
}

Connector* ConnectorManager_Client::GetConnector(ConnectorID_t ConnectorID)
{
	return m_pConnectorPool->GetConnector(ConnectorID);
}

bool ConnectorManager_Client::DelConnector(Connector *pConnector){

	if( pConnector == NULL )
	{
		WriteLog(KERN_ERR "ConnectorManager::AddConnector(): Error ! pConnector == NULL" );
		return false;
	}

	int32_t fd = pConnector->GetSocket()->getFD();
	ConnectorID_t ConnectorID = pConnector->GetConnectorID();
	Connector* pLastConnector = NULL;
	ID_t ManagerID = INVALID_ID;

	bool bRet = false;

	bRet = DelConnectorSocket(pConnector->GetSocket()->getFD());

	if( !bRet )
	{
		WriteLog(KERN_ERR "ConnectorManager::DelConnector(): Error ! DelConnectorSocket Failed!" );
		goto EXCEPTION;
		return false;
	}

	if( m_uConnectorCount<=0 )
	{
		WriteLog(KERN_ERR "ConnectorManager::DelConnector(): Error ! count(%d)<=0", m_uConnectorCount );
		goto EXCEPTION;
		return false;
	}

	ManagerID = pConnector->GetManagerID();
	if(ManagerID < 0 || ManagerID >= m_uMaxConnectorCount)
	{
		WriteLog(KERN_ERR "ConnectorManager::DelConnector(): Error ! Invalid ManagerID %d second time", ManagerID );
		goto EXCEPTION;
		return false;
	}

	pLastConnector = GetConnector(m_aConnecorID[m_uConnectorCount - 1]);
	if( pLastConnector == NULL )
	{
		WriteLog(KERN_ERR
				"ConnectorManager::DelConnectorFromMgr(): Error ! pLastConnector==NULL,count=%d,id=%d",
				m_uConnectorCount,
				m_aConnecorID[m_uConnectorCount - 1]
				);
		goto EXCEPTION;
		return false;
	}

	if( pConnector != pLastConnector )
	{
		m_aConnecorID[ManagerID] = m_aConnecorID[m_uConnectorCount - 1];
		m_aConnecorID[m_uConnectorCount - 1] = INVALID_ID;
		pLastConnector->SetManagerID(ManagerID);
		pConnector->SetManagerID(INVALID_ID);
	}
	else
	{
		m_aConnecorID[ManagerID] = INVALID_ID;
		pConnector->SetManagerID(INVALID_ID);
	}

	m_uConnectorCount--;

	WriteLog(KERN_INFO "ConnectorManager::DelConnector(): Success ! fd=%d, pid=%d", fd, ConnectorID );

	return true;

EXCEPTION:
	DelConnectorSocket(pConnector->GetSocket()->getFD());
	return false;
}
Connector* ConnectorManager_Client::AcceptNewConnection()
{
	if(m_bServerMode == false) return NULL;

	int32_t		step = 0;
	bool		ret = false;
	bool 		bRemove = false;
	Connector*	pConnector = NULL;
	_NET_TRY
	{
		pConnector = m_pConnectorPool->NewConnector();

		if( pConnector == NULL )
		{
			WriteLog(KERN_ERR "ConnectorManager::AcceptNewConnection(): Fail ! New Connector Fail" );
			return NULL;
		}

		ret = m_pServerSocket->accept_socket(pConnector->GetSocket());

		if(!ret)
		{
			m_pConnectorPool->DelConnector(pConnector->GetConnectorID());
			return NULL;
		}
	}
	_NET_CATCH
	{
		step += 1000;
		goto EXCEPTION;
	}

	_NET_TRY
	{
		Socket*				pSocket = pConnector->GetSocket();	

		if(pSocket->getFD() == INVALID_SOCKET )
		{
			step = 2;
			goto EXCEPTION;
		}

		ret = pSocket->setNonBlocking();
		if(!ret)
		{
			step = 3;
			goto EXCEPTION;
		}

		ret = pSocket->setLinger(0);
		if(!ret)
		{
			step = 4;
			goto EXCEPTION;
		}
		step = 5;
		ret = AddConnector(pConnector);

		if(!ret)
		{
			goto EXCEPTION;
		}
		bRemove = true;

		NET::ConnecterState(pConnector->GetConnectorID(), NET::ConnecterConnect);
	}
	_NET_CATCH
	{
		step += 10000;
		goto EXCEPTION;
	}

	WriteLog(KERN_INFO
			"ServerConnectorManager::ConnectToServer(): Success ! sock=%d,pid=%d,ip=%s",
			pConnector->GetSocket()->getSOCKET(),
			pConnector->GetConnectorID(),
			pConnector->GetSocket()->getIP()
			);

	return pConnector;

EXCEPTION:
	WriteLog(KERN_ERR
			"ClientConnectorManager::ConnectToServer(): Fail ! IP:%s Port:%d,Step:%d",
			pConnector->GetSocket()->getSOCKET(),
			pConnector->GetConnectorID(),
			pConnector->GetSocket()->getIP()
			);

	if( pConnector != NULL )
	{
		if( bRemove )
			RemoveConnector(pConnector);
		else
			m_pConnectorPool->DelConnector(pConnector->GetConnectorID());
	}

	return NULL;
}


bool ConnectorManager_Client::ConnectToServer(const char* ip,
		uint16_t port, ConnectorID_t &ConnectorID) {
	int ret = 0;
	int step = 0;
	bool bRemove = false;
	Connector *pConnector = m_pConnectorPool->NewConnector();

	if (pConnector == NULL) return false;

	Socket*				pSocket = pConnector->GetSocket();	

	_NET_TRY
	{

		ret = pSocket->create_socket();
		if(!ret)
		{
			step = 1;
			goto EXCEPTION;
		}

		ret = pSocket->connect_socket(ip, port);
		if(!ret)
		{
			step = 2;
			goto EXCEPTION;
		}

		ret = pSocket->setNonBlocking();
		if(!ret)
		{
			step = 3;
			goto EXCEPTION;
		}

		ret = pSocket->setLinger(0);
		if(!ret)
		{
			step = 4;
			goto EXCEPTION;
		}

		ret = AddConnector(pConnector);

		if(!ret)
		{
			step = 6;
			goto EXCEPTION;
		}
		bRemove = true;

		NET::ConnecterState(pConnector->GetConnectorID(), NET::ConnecterConnect);
	}
	_NET_CATCH
	{
		step = 5;
		goto EXCEPTION;
	}

	WriteLog(KERN_INFO
			"ServerConnectorManager::ConnectToServer(): Success ! IP:%s Port:%d",
			ip,
			port
			);

	ConnectorID = pConnector->GetConnectorID();

	return true;

EXCEPTION:
	WriteLog(KERN_ERR
			"ClientConnectorManager::ConnectToServer(): Fail ! IP:%s Port:%d,Step:%d",
			ip,
			port,
			step
			);

	if( bRemove )
	{
		RemoveConnector(pConnector);
	}
	else
	{
		m_pConnectorPool->DelConnector(pConnector->GetConnectorID());
	}
	return false;
}

bool ConnectorManager_Client::AddConnectorSocket(int fd, ConnectorID_t ConnectorID) {

	if(fd == INVALID_SOCKET)
	{
		WriteLog(KERN_ERR "ConnectorManager_Epoll::AddConnectorSocket(): Error ! fd==INVALID" );
		return false;
	}

	if( m_uFDSize >= m_uEventsMaxSize )
	{
		WriteLog(KERN_ERR
				"ConnectorManager_Epoll::AddConnectorSocket(): Error ! m_uFDSize(%d)>m_uEventsMaxSize(%d)",
				m_uFDSize,
				m_uEventsMaxSize
				);
		return false;
	}

	struct epoll_event ev;
	ev.data.u64 = ToUint64( (uint32_t)fd, (uint32_t)ConnectorID );
	ev.events = EPOLLIN | EPOLLET;

	if( 0 != epoll_ctl(m_Epollfd, EPOLL_CTL_ADD, fd, &ev) )
	{
		print_errno();
		return false;
	}

	m_uFDSize++;

	return true;
}


bool ConnectorManager_Client::DelConnectorSocket(int fd) {

	if(fd == INVALID_SOCKET)
	{
		WriteLog(KERN_ERR "ConnectorManager_Epoll::DelConnectorSocket(): Error ! fd==INVALID" );
		return false;
	}

	if( m_uFDSize<=0 )
	{
		WriteLog(KERN_ERR "ConnectorManager_Epoll::DelConnectorSocket(): Error ! m_uFDSize(%d)<=0", m_uFDSize );
		return false;
	}

	struct epoll_event	ev;
	memset(&ev, 0, sizeof(epoll_event));
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLET;
	if( 0 != epoll_ctl(m_Epollfd, EPOLL_CTL_DEL, ev.data.fd, &ev) )
	{
		int err = errno;
		WriteLog(KERN_ERR "ConnectorManager_Epoll::DelConnectorSocket(): epoll_ctl Error ! m_uFDSize(%d)<=0", strerror(err) );
		return false;
	}

	m_uFDSize--;
	return true;
}
bool ConnectorManager_Client::RemoveConnector(Connector *pConnector){
	bool ret = false;

	ret = DelConnector(pConnector);
	if(ret && m_pConnectorPool)
	{
		m_pConnectorPool->DelConnector(pConnector->GetConnectorID());
		NET::ConnecterState(pConnector->GetConnectorID(), NET::ConnecterClose);
	}

	return ret;
}
void ConnectorManager_Client::RemoveAllConnector(){

	uint32_t uConnectorCount = GetConnectorCount();
	for(uint32_t i = 0; i < uConnectorCount; i++)
	{
		if(m_aConnecorID[i] == INVALID_ID)
		{
			WriteLog(KERN_ERR "ConnectorManager::RemoveAllConnector(): Error ! m_aConnecorID[%d] == INVALID_ID", i );
			continue;
		}

		Connector* pConnector = GetConnector(m_aConnecorID[i]);
		if(pConnector == NULL)
		{
			WriteLog(KERN_ERR "ConnectorManager::RemoveAllConnector(): Error ! pConnector==NULL, id=%d ", m_aConnecorID[i] );
			continue;
		}

		RemoveConnector(pConnector);
	}
}

bool	ConnectorManager_Client::Handle_Inputs()
{
	try
	{
		m_nEvents = epoll_wait(m_Epollfd, events, m_uEventsMaxSize, 0);
		if(m_nEvents > m_uEventsMaxSize || m_nEvents < 0)
		{
			WriteLog(KERN_ERR "epoll wait Error ! m_nEvents=%d", m_nEvents);
		}

	}
	catch(...)
	{
		WriteLog(KERN_ERR "ConnectorManager_Client::epoll_wait(): Error ! m_nEvents=%d", m_nEvents);
	}

	bool ret = false;

	for(int i = 0; i < m_nEvents; i++)
	{

		int32_t SockFD = GetHighSection(events[i].data.u64);
		ConnectorID_t idx = GetLowSection(events[i].data.u64);

		if(SockFD == m_SocketID)
		{
			if( m_bServerMode )
			{
				for(int32_t i = 0; i < ACCEPT_ONESTEP; i++)
				{
					if(AcceptNewConnection()==NULL) 
						break;
				}
			}
		}
		else if( events[i].events & EPOLLIN )
		{
			Connector* pConnector = GetConnector(idx);

			if( pConnector == NULL )
			{
				WriteLog(KERN_ERR "ConnectorManager_Epoll::ProcessInputs(): Error ! pConnector==NULL,id=%d", idx );
				continue;
			}

			if( pConnector->IsDisconnect() )
				continue;

			int fd = pConnector->GetSocket()->getFD();

			if(fd == INVALID_SOCKET)
			{
				WriteLog(KERN_ERR "ConnectorManager_Epoll::ProcessInputs(): Error ! socket==INVALID" );
				continue;
			}

			if(pConnector->GetSocket()->isSockError())
			{
				RemoveConnector(pConnector);
			}
			else
			{
				_NET_TRY
				{
					ret = pConnector->Handle_Input();
					if(!ret)
					{
						RemoveConnector(pConnector);
					}
				}
				_NET_CATCH
				{
					RemoveConnector(pConnector);
				}
			}
		}
	}
	return true;
}

bool ConnectorManager_Client::AddConnectorToMgr(ConnectorID_t ConnectorID) {
	lock_guard<FastLock> autolock(m_NewLock);
	m_aNewID.push_back(ConnectorID);
	return true;
}

bool ConnectorManager_Client::DelConnectorToMgr(ConnectorID_t ConnectorID) {
	lock_guard<FastLock> autolock(m_NewLock);
	m_aDelID.push_back(ConnectorID);
	return true;
}

void * ConnectorManager_Client::RunTick(void * arg)
{
	uint32_t tick_time = 30;//*((uint32_t *) arg);
	while(g_ConnectorManager_Client.IsActive())
	{
		usleep(tick_time);
		g_ConnectorManager_Client.Tick();
	}
	g_ConnectorManager_Client.RemoveAllConnector();
	return NULL;
}

bool ConnectorManager_Client::Tick() {

	bool ret = false;

	int i = 0;
	m_DelLock.Lock();
	for (i=0; i<m_aDelID.size();i++)
	{
		ret = RemoveConnector(GetConnector(m_aDelID[i]));
		if (ret)
		{
			m_aDelID.erase(m_aDelID.begin() + i);
		}
	}
	m_DelLock.Unlock();

	m_NewLock.Lock();
	for (i=0; i<m_aNewID.size();i++)
	{
		ret = AddConnector(GetConnector(m_aNewID[i]));
		m_aNewID.erase(m_aNewID.begin() + i);
	}
	m_NewLock.Unlock();

	CmStream *pPacket = NULL;

	m_PacketLock.Lock();
	for (i=0; i<m_apPacket.size();i++)
	{
		pPacket = m_apPacket[i];

		if (pPacket == NULL)
		{
			m_apPacket.erase(m_apPacket.begin() + i);
			WriteLog(KERN_ERR "que front NULL");
			continue;
		}

		Connector* pConnector = GetConnector(pPacket->ConnectorID);

		if (pConnector == NULL)
		{
			WriteLog(KERN_ERR "que connector NULL");
			m_apPacket.erase(m_apPacket.begin() + i);
			continue;
		}

		ret = pConnector->SendPacket(pPacket);
		if	(ret)
		{
			m_apPacket.erase(m_apPacket.begin() + i);
		}
		else
		{
			WriteLog(KERN_WARNING "que connector SendPacket false");
		}
	}
	m_PacketLock.Unlock();


	try
	{
		ret = Handle_Outputs();
		Check(ret);
	}
	catch (...) {
		Check(0);
	}


	try
	{
		ret = Handle_Inputs();
		Check(ret);
	}
	catch (...) {
		Check(0);
	}
	return ret;
}

bool	ConnectorManager_Client::Handle_Outputs()
{
	bool ret = false;
	uint32_t	uConnectorCount = GetConnectorCount();
	for(uint32_t i = 0; i < uConnectorCount; i++)
	{
		if(m_aConnecorID[i] == INVALID_ID)
		{
			WriteLog(KERN_ERR "ConnectorManager_Epoll::ProcessOutputs(): Error ! m_aConnecorID[%d] == INVALID_ID", i );
			continue;
		}

		Connector* pConnector = GetConnector(m_aConnecorID[i]);

		if( pConnector == NULL )
		{
			WriteLog(KERN_ERR "ConnectorManager_Epoll::ProcessOutputs(): Error ! pConnector==NULL,id=%d", i );
			continue;
		}
		if(!pConnector->IsOutput())
			continue;

		if( pConnector->IsDisconnect() )
			continue;

		int fd = pConnector->GetSocket()->getFD();

		if(fd == INVALID_SOCKET)
		{
			WriteLog(KERN_ERR "ConnectorManager_Epoll::ProcessInputs(): Error ! socket==INVALID" );
			continue;
		}

		if(pConnector->GetSocket()->isSockError())
		{
			RemoveConnector(pConnector);
		}
		else
		{
			_NET_TRY
			{
				ret = pConnector->Handle_Output();
				if(!ret)
				{
					RemoveConnector(pConnector);
				}
			}
			_NET_CATCH
			{
				RemoveConnector(pConnector);
			}
		}
	}
	return true;
}

bool ConnectorManager_Client::SendPacket(ConnectorID_t ConnectorID, const char* buf, uint32_t len) {

	CmStream *m_pSendStream = new CmStream();
	m_pSendStream->AllocBuffer(len);
	m_pSendStream->AddBuffer(buf, len);
	m_pSendStream->ConnectorID = ConnectorID;
	m_PacketLock.Lock();
	m_apPacket.push_back(m_pSendStream);
	m_PacketLock.Unlock();
	return true;
}
