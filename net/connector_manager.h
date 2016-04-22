#ifndef _CONNECTOR_MANAGER_H
#define _CONNECTOR_MANAGER_H
#include "message.h"
#include "connector.h"
#include "connector_pool.h"

#define THREAD_FD_FIFO_SIZE 1024
#define MAXLEN 1023

typedef struct {
    pthread_t idx;         /* unique ID of this thread */
    struct ev_loop *loop;     /* libev loop this thread uses */
    struct ev_async async_watcher;   /* async watcher for new connect */
    DECLARE_KFIFO(fifo, int, THREAD_FD_FIFO_SIZE);
    spinlock_t lock;
} WORK_THREAD;

typedef struct accept_thread {
    pthread_t idx;         /* unique ID of this thread */
    struct ev_loop *loop;     /* libev loop this thread uses */
    struct ev_io accept_watcher;   /* accept watcher for new connect */
} accept_thread;

typedef struct connector_manager{
    struct pool_s           cp;
    struct accept_thread    at;
    thread_pool_t           *ptp;
    struct connector		sc;  /*server connector*/
    bool	                active;
    int                     count;                    
    bool					mode;
}connector_manager;

bool connector_manager_init(uint32_t uMaxSize);
void connector_manager_clear(void);
bool connector_manager_listen(const char *ip,  int port);
bool connector_manager_tick();

//void * RunTick(void * arg);
// 增加connector
bool	add_connector(connector *pc);
// 取得connector指针
struct connector*		get_connector(int idx);
// 删除connector(包括管理器，SOCKET)
bool	remove_connector(connector *pc);
// 连接Server
struct connector    *connect_server(const char* ip,  int  port);
// 彻底删除connector(包括管理器，SOCKET，EPOOL 事件队列中)
// 删除管理器中的所有connector
void		remove_all_connector();
// 取得当前管理器中connector数量

void    connector_manager_output(EV_P_ ev_io *w, int revents);
void    connector_manager_input(EV_P_ ev_io *w, int revents);

bool 	connector_manager_send_packet(int idx, const char *buf, uint32_t len);

extern struct connector_manager g_cm;
#endif
