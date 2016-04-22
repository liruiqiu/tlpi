#ifndef _CONNECTOR_H
#define _CONNECTOR_H
#include "libev/ev.h"
#include "message.h"

#define IS_NOT_CONN(c) likely(c && c->fd == -1)
#define IS_ALREADY_CONN(c) likely(c && c->fd > -1)

typedef struct  connector
{
    int             fd;
    int             ip;
    int             port;
    ev_io           *prev_io;
    ev_io           *pwev_io;
    struct ring_buffer   *prbuff;
    struct ring_buffer   *psbuff;
    struct packet_head   recv_head;
}__packed connector;

int connector_create_socket();
int connector_accept(struct connector *pc);

bool connector_init(struct connector *pc,int fd); 
void connector_clear(struct connector *pc); 
bool connector_listen(struct connector *pc, const char *ip, int port);
bool connector_setsockopt(struct connector *pc);
bool connector_input(struct connector *pc);
bool connector_output(struct connector *pc);
bool connector_connect(struct connector *pc, const char *ip, int port);
bool connector_send_packet(struct connector *pc, const char *buff, int size);
#endif
