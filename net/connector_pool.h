#ifndef _POOL_H
#define _POOL_H

#include "message.h"

typedef struct	pool_s
{
    void            **apdata;
    int				count;
    int				max_count;
    int				pos;
    pthread_mutex_t         lock;
} pool_t;

void                pool_destroy(pool_t *pool);

bool				pool_init(pool_t *pool, int max_count, int size);
bool				pool_init_data(pool_t *pool,int idx,void* pdata);

void*		        pool_new(pool_t *pool);
void*		        pool_get(pool_t *pool, int idx);
bool			    pool_del(pool_t *pool, int idx);

#endif
