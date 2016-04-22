#include "message.h"
#include "connector.h"
#include "net.h"

#define SOCKET_EWOULDBLOCK              (-EWOULDBLOCK)
            

static uint32_t	send_socket(int fd,const void *buf, uint32_t len)
{
    ssize_t isend = 0;
    int flags =0;
    isend = send(fd,buf,len,flags);

    if (isend == SOCKET_ERROR)
    {
        if(errno == EWOULDBLOCK)
            isend = SOCKET_EWOULDBLOCK;
        else
            print_errno();
    }

    return isend;
}
static uint32_t 	receive_socket(int fd,void *buf, uint32_t len)
{
    ssize_t irecv = 0;
    int flags =0;
    irecv = recv(fd,buf,len,flags);

    if (irecv == SOCKET_ERROR)
    {
        if(errno == EWOULDBLOCK)
            irecv = SOCKET_EWOULDBLOCK;
        else
            print_errno();
    }

    return irecv;
}

static bool listen_socket(int fd, const char *ip, int32_t port, int32_t backlog) {

    int32_t opt = 1;
    struct sockaddr_in m_SockAddr;
    int32_t result;
    if(setsockopt(fd, SOL_SOCKET , SO_REUSEADDR , &opt , sizeof(opt)) == SOCKET_ERROR)
    {
        print_errno();
        return false;
    }

    m_SockAddr.sin_family = AF_INET;

    if(ip)
        m_SockAddr.sin_addr.s_addr = inet_addr(ip); 
    else
        m_SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    m_SockAddr.sin_port = htons(port);

    result = bind(fd, (struct sockaddr *)&m_SockAddr, sizeof(m_SockAddr));

    if( result == SOCKET_ERROR) 
    {
        print_errno();	
        return false;
    }

    result = listen(fd, backlog);

    if( result == SOCKET_ERROR) 
    {
        print_errno();	
        return false;
    }

    return true;
}

static int accept_socket(int fd) {
    struct sockaddr_in sin;
    uint32_t addrlen = sizeof(struct sockaddr_in) ;
    return accept(fd , (struct sockaddr *)(&(sin)), &addrlen);
}

static int create_socket() {
    int fd = socket( PF_INET, SOCK_STREAM, 0 );	
    if (fd == INVALID_ID)
        print_errno();	
    return fd;
}

void close_socket(int fd) {
    if(fd == INVALID_ID) 
        return;
    //&& !isSockError() ) 
    shutdown(fd,SHUT_RDWR);
    close(fd);
}

bool connect_socket(int fd, const char* ip, int port) {


    struct sockaddr_in server_address;
    bzero( &server_address, sizeof( server_address ) );
    server_address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &server_address.sin_addr );
    server_address.sin_port = htons( port );

    int result = connect(fd , (struct sockaddr *)&server_address , sizeof(server_address));

    if( result == SOCKET_ERROR) 
    {
        print_errno();	
        return false;
    }

    return true;
}

bool set_socket_nonblock(int fd) {
    int old_option = fcntl( fd, F_GETFL, 0);
    int new_option = old_option | O_NONBLOCK;
    if (fcntl( fd, F_SETFL, new_option) == SOCKET_ERROR)
        return false;
    return true;
}

bool set_socket_linger(int fd, unsigned int lingertime) {
    struct linger ling;

    ling.l_onoff = lingertime > 0 ? 1 : 0;
    ling.l_linger = lingertime;

    if (setsockopt( fd , SOL_SOCKET , SO_LINGER , &ling , sizeof(ling)) == SOCKET_ERROR)
        return false;
    return true;
}

bool is_sock_err(int fd) {

    if (fd == INVALID_ID) return false;

    int32_t error;
    uint32_t len = sizeof(error);

    int32_t ret = getsockopt( fd , SOL_SOCKET , SO_ERROR , &error, &len );

    if( ret == 0 ) 
        return false;
    else 			  
        return true;

}

bool connector_input(connector *pc)
{
    int len;
    int step = 0;
    int fd = -1;

    int body_len = 0;
    char *buff;

    if(!IS_ALREADY_CONN(pc)) 
    {
        step = 1;
        goto err; 
    }

    fd = pc->fd;

    if (is_sock_err(pc->fd)) 
    {
        step = 2;
        goto err; 
    }

    len = ring_buffer_recv_socket(pc->fd, pc->prbuff, 1024*1024*1); 

    if (len == -EWOULDBLOCK) return true;
    else if (len == 0) 
    {
        step = 3;
        goto out;
    }
    else if (len == -1) 
    {
        step = 4;
        goto err;
    }

    while(5)
    {
        if(pc->recv_head.idx == 0) 
        {
            if(ring_buffer_len(pc->prbuff) >= PACKET_HEAD_SIZE)
            {
                ring_buffer_get(pc->prbuff, (char *)&pc->recv_head, PACKET_HEAD_SIZE);
        	pr_info("idx=%d,len=%d",pc->recv_head.idx,pc->recv_head.len);
	    	print_hex_dump_bytes("data: ", DUMP_PREFIX_OFFSET, &pc->recv_head, PACKET_HEAD_SIZE);
            }
            else
                break;
        }

        if(ring_buffer_len(pc->prbuff) >= pc->recv_head.len)
        {
            body_len = pc->recv_head.len;
            char *buff = malloc(body_len); 
            memset(buff, 0, body_len);
            ring_buffer_get(pc->prbuff,buff,body_len);  
            //pr_info("idx=%d,len=%d,body=%s",pc->recv_head.idx,pc->recv_head.len, buff);
	    print_hex_dump_bytes("data: ", DUMP_PREFIX_OFFSET, buff, pc->recv_head.len);
            SAFE_FREE(buff);
            pc->recv_head.idx = 0; 
            pc->recv_head.len = 0; 
        }
        else
            break;
    }

    return true;
out:
    pr_info("fd=%d, step=%d", fd, step);
    return false;
err:
    pr_err("fd=%d, step=%d", fd, step);
    return false;
}

bool connector_output(connector *pc)
{
    int len;
    int step = 0;

    if(!IS_ALREADY_CONN(pc)) goto err; step++;

    if (is_sock_err(pc->fd)) goto err; step++;
        
    if (ring_buffer_len(pc->psbuff) == 0) return true;

    len = ring_buffer_send_socket(pc->fd, pc->psbuff, 1024*1024*1); 

    if (len == -EWOULDBLOCK) return true;
    else if (len == 0) goto out;
    else if (len == -1) goto err;

    return true;
out:
    return false;
err:
    return false;
}

bool connector_send_packet(struct connector *pc, const char *buff, int size)
{
	int len;
    len = ring_buffer_put(pc->psbuff, buff, size);
	pr_info("len=%d", len);
    connector_output(pc);
    return true;
}

bool connector_init(struct connector *pc, int fd) {

    if (fd == -1 || fd == 0)
        fd = create_socket();

    if (fd == INVALID_ID)
        return false;

    pc->prev_io = malloc(sizeof(ev_io));
    pc->pwev_io = malloc(sizeof(ev_io));
    pc->fd = fd; 
    pc->ip = 0;
    pc->port = 0;
    pc->recv_head.idx = 0;
    pc->recv_head.len = 0;

    //pc->prbuff = ring_buffer_create(1024*1024*1);
    //pc->psbuff = ring_buffer_create(1024*1024*1);

    pc->prbuff = ring_buffer_create(255);
    pc->psbuff = ring_buffer_create(255);

    return true;
}

void connector_clear(struct connector *pc) 
{

    close_socket(pc->fd);
    SAFE_FREE(pc->prev_io);
    SAFE_FREE(pc->pwev_io);
    ring_buffer_destroy(pc->prbuff);
    ring_buffer_destroy(pc->psbuff);
    pc->fd = -1;
    pc->ip = 0;
    pc->port = 0;

}

bool connector_listen(struct connector *pc, const char *ip, int port)
{
    int32_t     step = 0;
    int fd;

    fd = pc->fd;
    if (fd == INVALID_ID)
    {
        step = 2;
        goto err;
    }

    if(!listen_socket(fd, ip, port, 5))
    {
        step = 3;
        goto err;
    }

    if(!set_socket_nonblock(fd))
    {
        step = 4;
        goto err;
    }

    pr_info("fd=%d", fd);
    return true;
err:
    pr_err("fd=%d,step=%d", fd, step);
    return false;
}

int connector_accept(struct connector *pc)
{
    return accept_socket(pc->fd);
}
int connector_create_socket()
{
    return create_socket();
}
bool connector_setsockopt(struct connector *pc)
{
    int32_t     step = 0;
    int fd = pc->fd;

    if(fd == INVALID_ID )
    {
        step = 1; 
        goto err;
    }

    if(!set_socket_nonblock(fd))
    {
        step = 2;
        goto err;
    }

    if(!set_socket_linger(fd, 0))
    {
        step = 3;
        goto err;
    }
    pr_info("fd=%d", fd);
    return true;
err:
    pr_err("fd=%d,step=%d", fd, step);
    return false;
}


bool connector_connect(struct connector *pc, const char *ip, int port)
{
    int32_t     step = 0;
    int fd = pc->fd;

    if(!connect_socket(fd,ip,port)) goto err; step++;

    if(!set_socket_nonblock(fd)) goto err; step++;

    if(!set_socket_linger(fd, 0)) goto err; step++;


    pr_info("fd=%d", fd);
    return true;
err:
    pr_err("fd=%d,step=%d", fd, step);
    return false;

}
