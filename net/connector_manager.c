#include "connector_manager.h"
#include "connector.h"
#include "message.h"
#include "net.h"

#define ACCEPT_ONESTEP			50


struct connector_manager g_cm;

bool        is_active(){return g_cm.active;};
void        stop(){ g_cm.active = false;};
uint32_t	get_connector_count() { return g_cm.count; };


void accept_callback(struct ev_loop *loop, ev_io *w, int revents)
{
    pr_info();
    int fd;
    fd = connector_accept(&g_cm.sc);
    //WORK_THREAD *me =  (WORK_THREAD*)(w->data);
    WORK_THREAD *me = get_thread_data(g_cm.ptp,0);
    kfifo_in_locked(&me->fifo, &fd, 1,&me->lock);
    ev_async_send(me->loop, &me->async_watcher); 
}

static void async_cb (EV_P_ ev_async *w, int revents)
{
    pr_info();
    int fd;
    WORK_THREAD *me =  (WORK_THREAD*)(w->data);
    kfifo_out_locked(&me->fifo,&fd,1,&me->lock);

    struct connector *pc = pool_get(&g_cm.cp, fd);
    if (pc)
    {
        connector_init(pc, fd);

        ev_io_init(pc->pwev_io,connector_manager_output,fd, EV_WRITE);
        //ev_io_init(pc->pwev_io,write_callback,fd, EV_WRITE);
        ev_io_start(me->loop,pc->pwev_io);

        //ev_io_init(pc->prev_io,recv_callback,fd, EV_READ);
        ev_io_init(pc->prev_io,connector_manager_input,fd, EV_READ);
        ev_io_start(me->loop,pc->prev_io);
    }
}

void dispatch_thread_pool_fn(void *arg) {
    WORK_THREAD *me = (WORK_THREAD *)arg;
    me->idx = pthread_self();
    //ev_loop(me->loop, 0);
    for(;;)
    {
        ev_loop(me->loop, EVRUN_NOWAIT);
        sleep(1);
    }

}

void connector_manager_clear(void) {
    remove_all_connector();
    destroy_thread_pool(g_cm.ptp);
    ev_loop_destroy(g_cm.at.loop);
}

static void *connector_manager_listen_loop(void *arg)
{
    pr_info();
    ev_loop(g_cm.at.loop,0);
}

bool connector_manager_listen(const char *ip,  int port)
{

    connector_init(&g_cm.sc, -1);
    connector_listen(&g_cm.sc, "127.0.0.1", 3333);

    int listen = g_cm.sc.fd;
    g_cm.at.loop = ev_default_loop(0);
    g_cm.at.idx = pthread_self();
    ev_io_init(&(g_cm.at.accept_watcher), accept_callback,listen, EV_READ);
    ev_io_start(g_cm.at.loop,&(g_cm.at.accept_watcher)); 

    pthread_t       thread;
    pthread_attr_t  attr;
    int             ret;
    pthread_attr_init(&attr);

    if ((ret = pthread_create(&thread, &attr, connector_manager_listen_loop, &g_cm.at)) != 0) {
        fprintf(stderr, "Can't create thread: %s\n", strerror(ret));
        exit(1);
    }
    return true;
}

bool connector_manager_init(uint32_t uMaxSize) {

    int i;
    pool_init(&g_cm.cp, 1024, sizeof(struct connector));

    g_cm.ptp = create_thread_pool(3);

    for (i=0; i<3; i++)
    {
        WORK_THREAD *me =  malloc(sizeof(WORK_THREAD));

        me->loop = ev_loop_new(0);
        if (! me->loop) {
            fprintf(stderr, "Can't allocate event base\n");
            exit(1);
        }

        pthread_spin_init(&me->lock, 0);
        INIT_KFIFO(me->fifo);

        me->async_watcher.data = me;

        /* Listen for notifications from other threads */
        ev_async_init(&me->async_watcher, async_cb);
        ev_async_start(me->loop, &me->async_watcher);

        dispatch_thread_pool(g_cm.ptp, dispatch_thread_pool_fn, (void *) me);
    }

    g_cm.active = true;

    return true;
}
bool connector_manager_add_connector(connector *pc){
    if( pc == NULL )
    {
        //WriteLog(KERN_ERR "connector_manager_::add_connector(): Error ! pc == NULL" );
        return false;
    }

    g_cm.count++;

    return true;
EXCEPTION:
    //remove_connectorSocket(pc->get_Socket()->getFD());
    return false;
}

struct connector* connector_manager_get_connector(int idx)
{
    return (struct connector *)pool_get(&g_cm.cp, idx);
}

bool remove_connector(connector *pc){

    int fd;
    if( pc == NULL )
    {
        pr_err("pc == NULL" );
        return false;
    }
    fd = pc->fd;

    WORK_THREAD *me = (WORK_THREAD *)get_thread_data(g_cm.ptp, 0);
    ev_io_stop(me->loop, pc->prev_io);
    ev_io_stop(me->loop, pc->pwev_io);


    connector_clear(pc);

    pr_info("success ! fd=%d", fd);

    return true;

EXCEPTION:
    //remove_connectorSocket(pc->get_Socket()->getFD());
    return false;
}

struct connector    *connect_server(const char* ip,  int  port)
{
    int ret = 0;
    int step = 0;
    bool bremove_ = false;
    int fd = connector_create_socket();
    struct connector *pc = pool_get(&g_cm.cp, fd);
    if (pc)
    {
        if (connector_init(pc, fd) == false)
            return NULL;
        if (connector_connect(pc, ip, port)==false) 
            return NULL;
    }
    return pc;
}

void remove_all_connector(){
    /*
       uint32_t uconnectorCount = get_connector_count();
       for(uint32_t i = 0; i < uconnectorCount; i++)
       {
       if(m_aConnecorID[i] == INVALID_ID)
       {
       WriteLog(KERN_ERR "connector_manager_::remove_Allconnector(): Error ! m_aConnecorID[%d] == INVALID_ID", i );
       continue;
       }

       connector* pc = get_connector(m_aConnecorID[i]);
       if(pc == NULL)
       {
       WriteLog(KERN_ERR "connector_manager_::remove_Allconnector(): Error ! pc==NULL, id=%d ", m_aConnecorID[i] );
       continue;
       }

       remove_connector(pc);
       }
       */
}

bool connector_manager_tick() {

    bool ret = false;

    int i = 0;
    uint32_t tick_time = 30;//*((uint32_t *) arg);
    while(is_active())
    {
        usleep(tick_time);
    }
    //remove_Allconnector();


    return ret;
}

void connector_manager_output(EV_P_ ev_io *w, int revents)
{
    pr_info();
    struct connector *pc = pool_get(&g_cm.cp, w->fd);

    if(!connector_output(pc))
    {
        remove_connector(pc);
    }
}

void    connector_manager_input(EV_P_ ev_io *w, int revents)
{

    pr_info();
    struct connector *pc = pool_get(&g_cm.cp, w->fd);

    if(!connector_input(pc))
    {
        remove_connector(pc);
    }

}

bool connector_manager_send_packet(int idx, const char* buf, uint32_t len) {
    pr_info();
    struct connector *pc = pool_get(&g_cm.cp, idx);
    connector_send_packet(pc, buf, len);
    
    /*
       CmStream *m_pSendStream = new CmStream();
       m_pSendStream->AllocBuffer(len);
       m_pSendStream->add_Buffer(buf, len);
       m_pSendStream->connectorID = connectorID;
       m_PacketLock.Lock();
       m_apPacket.push_back(m_pSendStream);
       m_PacketLock.Unlock();
       */
    return true;
}













/*
void recv_callback(struct ev_loop *loop, ev_io *w, int revents)
{
    pr_info();
 char buffer[1024]={0};
 int ret =0;
 //ev_io write_event;
loop:
 ret=recv(w->fd,buffer,MAXLEN,0);
 if(ret > 0)
 {
  printf("thread[%lu] recv message :%s  \n",pthread_self(),buffer);
  
 }
 else if(ret ==0)
 {
  printf("thread[%lu] remote socket closed!socket fd: %d\n",pthread_self(),w->fd);
  close(w->fd);
  ev_io_stop(loop, w);
  free(w);
  return;
 }
 else
 {
  if(errno == EAGAIN ||errno == EWOULDBLOCK)
  {
    goto loop;
  }
  else
  {
   printf("thread[%lu] ret :%d ,close socket fd : %d\n",pthread_self(),ret,w->fd);
   close(w->fd);
   ev_io_stop(loop, w);
   free(w);
   return;
  }
 }
 int fd=w->fd;
 //ev_io_stop(loop,  w);
 //ev_io_init(w,write_callback,fd,EV_WRITE);
 //ev_io_start(loop,w);
 printf("thread[%lu] socket fd : %d, turn read 2 write loop! ",pthread_self(),fd);
 
}


void write_callback(struct ev_loop *loop, ev_io *w, int revents)
{
    pr_info();
 char buffer[1024]={0};
 //ev_io read_event;
 snprintf(buffer,1023,"this is a test!");
 write(w->fd,buffer,strlen(buffer));
 int fd=w->fd;
 ev_io_stop(loop,  w);
 //ev_io_init(w,recv_callback,fd,EV_READ);
 //ev_io_start(loop,w);
} 
*/

