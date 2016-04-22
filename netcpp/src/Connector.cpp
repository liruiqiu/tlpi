#include "Connector.h"
#include "Net.h"

#define CM_HEADER_SIZE					(0)
#define SOCKET_EWOULDBLOCK              (-10000)
            

CmStream::CmStream()
{
	Clear();
}

CmStream::~CmStream()
{
	SAFE_DELETE_ARRAY(m_pBuffer);
	Clear();
}

void CmStream::Clear()
{
	m_uHead		= CM_HEADER_SIZE;
	m_uTail		= CM_HEADER_SIZE;
	m_uMaxSize	= 0;
	m_pBuffer	= NULL;
}

bool CmStream::AllocBuffer(uint32_t uSize)
{
	m_pBuffer = new char[uSize];
	if( m_pBuffer == NULL )
		return false;
	m_uMaxSize = uSize;
	return true;
}

bool CmStream::FreeBuffer()
{
	m_uHead		= CM_HEADER_SIZE;
	m_uTail		= CM_HEADER_SIZE;
	m_uMaxSize	= 0;
	SAFE_DELETE(m_pBuffer);

	return true;
}

char* CmStream::GetBuffer()
{
	return m_pBuffer;
}

char* CmStream::GetHeadBuffer()
{
	return (m_pBuffer+m_uHead);
}

char* CmStream::GetTailBuffer()
{
	return (m_pBuffer+m_uTail);
}

uint32_t CmStream::GetSize()
{
	return m_uTail - m_uHead;
}

uint32_t CmStream::GetMaxSize()
{
	return m_uMaxSize;
}

uint32_t CmStream::GetFreeSize()
{
	return m_uMaxSize - m_uTail;
}

bool CmStream::PushBack(uint32_t uSize)
{
	m_uTail += uSize;
	if( m_uTail > m_uMaxSize )
	{
		m_uTail = m_uMaxSize;
		Check(0);
		return false;
	}
	return true;
}

bool CmStream::PopHead(uint32_t uSize)
{
	m_uHead += uSize;
	if( m_uHead > m_uTail )
	{
		m_uHead += m_uTail;
		Check(0);
		return false;
	}
	return true;
}

void CmStream::AddMsgHeader()
{
	uint16_t uSize = (uint16_t)GetSize();
	uSize |= 0x8000;

	*((uint16_t*)m_pBuffer) = uSize;

	m_uHead = 0;
}

bool CmStream::AddBuffer(const char* buf, uint32_t len) {

	if(m_uMaxSize - m_uTail < len)
	{
		Check(0);
		return false;
	}
	memcpy(m_pBuffer + m_uTail, buf, len);

	m_uTail += len;
}
void CmStream::ResetPos()
{
	m_uHead = m_uTail = CM_HEADER_SIZE;
}

Socket::Socket() {
	m_SocketID = INVALID_SOCKET ;
	memset( &m_SockAddr, 0, sizeof(sockaddr_in) ) ;
	memset( m_IP, 0, IP_SIZE ) ;
	m_Port = 0 ;
}

Socket::~Socket() {
	close_socket();
}

uint32_t	Socket::send_socket(const void *buf, uint32_t len, uint32_t flags)
{
	ssize_t isend = 0;
	try
	{
		isend = send(m_SocketID,buf,len,flags);

		if (isend == SOCKET_ERROR)
		{
			if(errno == EWOULDBLOCK)
				isend = SOCKET_EWOULDBLOCK;
			else
				print_errno();
		}
	}
	catch(...)
	{
		Check(0);	
	}

	return isend;
}
uint32_t 	Socket::receive_socket(void *buf, uint32_t len, uint32_t flags)
{
	ssize_t irecv = 0;
	try
	{
		irecv = recv(m_SocketID,buf,len,flags);

		if (irecv == SOCKET_ERROR)
		{
			if(errno == EWOULDBLOCK)
				irecv = SOCKET_EWOULDBLOCK;
			else
				print_errno();
		}
	}
	catch(...)
	{
		Check(0);	
	}

	return irecv;
}

bool Socket::listen_socket(uint32_t port,const char *ip,  int32_t backlog) {
	create_socket();

	int32_t opt = 1;
	if(setsockopt( m_SocketID , SOL_SOCKET , SO_REUSEADDR , &opt , sizeof(opt)) == SOCKET_ERROR)
	{
		print_errno();
		return false;
	}
	m_Port = port ;

	if(ip)
		m_SockAddr.sin_addr.s_addr = inet_addr(ip); 
	else
		m_SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	m_SockAddr.sin_port = htons(m_Port);

	int32_t result = bind(m_SocketID, (struct sockaddr *)&m_SockAddr, sizeof(m_SockAddr));

	if( result == SOCKET_ERROR) 
	{
		print_errno();	
		return false;
	}

	result = listen(m_SocketID, backlog);

	if( result == SOCKET_ERROR) 
	{
		print_errno();	
		return false;
	}

	return true;
}

bool Socket::accept_socket(Socket *socket){

	uint32_t addrlen = sizeof(sockaddr_in) ;
	socket->close_socket();

	//socket->m_SocketID = m_Socket->accept((struct sockaddr *)(&(socket->m_SockAddr)), &addrlen);
	int32_t fd  = accept( m_SocketID , (struct sockaddr *)(&(socket->m_SockAddr)), &addrlen);
	if( fd == INVALID_SOCKET )
		return false;

	socket->setFD(fd);
	socket->m_Port = ntohs( socket->m_SockAddr.sin_port);
	strncpy( socket->m_IP, inet_ntoa(socket->m_SockAddr.sin_addr), IP_SIZE-1);
	return true;
}

bool Socket::create_socket() {
	m_SocketID = socket( PF_INET, SOCK_STREAM, 0 );	
	memset( &m_SockAddr , 0 , sizeof(m_SockAddr) );

	m_SockAddr.sin_family = AF_INET;

	if( isValid() )
		return true;
	else 
		return false;
}

void Socket::close_socket() {
	if( isValid() && !isSockError() ) 
	{
		try
		{
			close(m_SocketID);
		} 
		catch(...)
		{
			Check(0);
		}
	}
	m_SocketID = INVALID_SOCKET ;
	memset( &m_SockAddr, 0, sizeof(sockaddr_in) ) ;
	m_Port = 0 ;
	memset( m_IP, 0, IP_SIZE ) ;
}

bool Socket::connect_socket(const char* host, int port) {

	strncpy( m_IP, host, IP_SIZE-1 ) ;
	m_Port = port ;

	m_SockAddr.sin_addr.s_addr = inet_addr( m_IP );
	m_SockAddr.sin_port = htons(m_Port);

	int result = connect(m_SocketID , (const struct sockaddr *)&m_SockAddr , sizeof(m_SockAddr));

	if( result == SOCKET_ERROR) 
	{
		print_errno();	
		return false;
	}

	return true;
}

bool Socket::setNonBlocking() {
	int old_option = fcntl( m_SocketID, F_GETFL, 0);
	int new_option = old_option | O_NONBLOCK;
	fcntl( m_SocketID, F_SETFL, new_option );
	//return old_option;
	return true;
}

bool Socket::setLinger(unsigned int lingertime) {
	struct linger ling;

	ling.l_onoff = lingertime > 0 ? 1 : 0;
	ling.l_linger = lingertime;

	if(setsockopt( m_SocketID , SOL_SOCKET , SO_LINGER , &ling , sizeof(ling)) == SOCKET_ERROR)
	{
		print_errno();
		return false;
	}
	return true;
}

bool Socket::isValid() const {
	return m_SocketID!=INVALID_SOCKET; 
}

bool Socket::isSockError() const {
	int32_t error;
	uint32_t len = sizeof(error);

	int32_t ret = getsockopt( m_SocketID , SOL_SOCKET , SO_ERROR , &error, &len );

	if( ret == 0 ) 
		return false;
	else 			  
		return true;

}

int32_t Socket::getFD() const {
	return m_SocketID; 
}

void Socket::setFD(int32_t fd) {
	m_SocketID = fd;
}




Connector::Connector(void) {
	m_ConnectorID		= INVALID_ID;
	m_ManagerID			= INVALID_ID;
	m_IsEmpty			= true;
	m_PacketIndex		= 0;
	m_LastPacketID		= 0;
	m_pSocket			= NULL;
	m_Status			= 0;
	m_bDisconnect		= false;
}

Connector::~Connector(void) {
	SAFE_DELETE(m_pSocket);
	SAFE_DELETE(m_pRecieveStream);
	SAFE_DELETE(m_pSendStream);
}

void Connector::Clear(void) {
	if( m_pSocket )
		m_pSocket->close_socket();

	m_ManagerID		= INVALID_ID;
	m_PacketIndex	= 0;
	m_LastPacketID	= 0;
	m_IsEmpty		= true;
	m_Status		= 0;
	m_bDisconnect	= false;

	NetHeadLen = 0;
	memset(&NetHead, 0, NET_MSG_HEAD_SIZE);
	SAFE_DELETE(m_pSendStream);
}

void Connector::InitSocket() {
	m_pSocket = new Socket;
	Check(m_pSocket);
	m_pRecieveStream = new CmStream();
	m_pSendStream = NULL;
}

bool Connector::Handle_Input() {
	if( !IsValid() ) return false;


	uint32_t body_len = 0;
	uint32_t irecv = 0;
	while(1)
	{
		if	(NET_MSG_HEAD_SIZE > NetHeadLen)
		{
			irecv = m_pSocket->receive_socket(&NetHead, NET_MSG_HEAD_SIZE - NetHeadLen);

            if (irecv == SOCKET_ERROR || irecv == 0) return false;

			if (irecv == SOCKET_EWOULDBLOCK) break;

			NetHeadLen += irecv;

			continue;
		}

		if(m_pRecieveStream->GetSize() == 0)
		{

			body_len = NetHead.body_len;//ntohl(NetHead.body_len);

			m_pRecieveStream->AllocBuffer(NET_MSG_HEAD_SIZE + body_len);

			m_pRecieveStream->AddBuffer((char *)&NetHead, NET_MSG_HEAD_SIZE);

			continue;
		}


		irecv = m_pSocket->receive_socket(m_pRecieveStream->GetTailBuffer(), m_pRecieveStream->GetFreeSize());

    	if (irecv == SOCKET_ERROR || irecv == 0) return false;

		if (irecv == SOCKET_EWOULDBLOCK) break;


		m_pRecieveStream->PushBack(irecv);

		if( m_pRecieveStream->GetFreeSize() == 0 )
		{
			NetHeadLen = 0;
			NET::ConnecterState(m_ConnectorID, NET::ConnecterRecievePacket, m_pRecieveStream->GetBuffer(), m_pRecieveStream->GetSize());
			m_pRecieveStream->FreeBuffer();
		}

	}

	return true;
}

bool Connector::Handle_Output() {

	if( !IsValid() ) return false;

	uint32_t isend = 0;

	while(m_pSendStream && m_pSendStream->GetSize())
	{
		isend = m_pSocket->send_socket(m_pSendStream->GetHeadBuffer(),m_pSendStream->GetSize());

		if (isend == SOCKET_ERROR) return false;

		if (isend == 0)  break;

		if (isend == SOCKET_EWOULDBLOCK) break;


		m_pSendStream->PopHead(isend);

		if (m_pSendStream->GetSize() == 0)
		{
			NET::ConnecterState(m_ConnectorID, NET::ConnecterSendPacket, m_pSendStream->GetBuffer(), m_pSendStream->GetMaxSize());
			m_pSendStream->FreeBuffer();
			SAFE_DELETE(m_pSendStream);
		}
	}

	return true;
}

bool Connector::IsValid() {
	if(m_pSocket == NULL) 
		return false;

	if(!m_pSocket->isValid()) 
		return false;

//	if(m_CreateTime + g_Config.NetOverTime * 60 > time(NULL))
//		return false;

	return true;
}

bool Connector::SendPacket(CmStream *pSendStream) {
	if(m_pSendStream == NULL)
	{
		m_pSendStream = pSendStream;
		return true;
	}
	return false;
}

void Connector::SetSocket(Socket* pSocket) {
	m_pSocket->m_SocketID	= pSocket->m_SocketID ;
	m_pSocket->m_SockAddr	= pSocket->m_SockAddr ;
	m_pSocket->m_Port		= pSocket->m_Port ;
	memcpy( m_pSocket->m_IP, pSocket->m_IP, IP_SIZE ) ;
}
