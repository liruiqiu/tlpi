#include "connector_pool.h"

void pool_destroy(pool_t * pool)
{
    int i;
    if( pool->apdata )
    {
        for(i=0; i<pool->max_count; i++ )
        {
            SAFE_FREE(pool->apdata[i]);
        }
    }

    SAFE_FREE(pool->apdata);

    pthread_mutex_destroy(&pool->lock);
}

bool pool_init(pool_t * pool, int max_count, int size)
{
    int i;
    pool->pos = 0;
    pool->max_count = max_count;
    pool->count = max_count;

    pool->apdata = malloc(sizeof(void *) * max_count);
    
    if (size == 0)
    {
        memset(pool->apdata,0,sizeof(void *) * max_count);
    }
    else
    {
        for(i = 0; i < max_count; i++)
        {
            pool->apdata[i] = malloc(size);
        }
    }

    pthread_mutex_init(&pool->lock, NULL);

    return true;
}

bool  pool_init_data(pool_t *pool,int idx,void* pdata)
{
    if(idx < 0 && idx >= pool->max_count)
        return false;
    if (pdata == NULL)
        return false;
    pool->apdata[idx] = pdata;

    return true;
}


void* pool_get(pool_t *pool, int idx)
{
    if(idx >= pool->max_count || idx < 0) 
        return NULL;
    return pool->apdata[idx];
}


void* pool_new(pool_t *pool)
{

    int iRet = 0;
    int i;
    pthread_mutex_lock(&pool->lock);

    for(i = 0; i < pool->max_count; i++)
    {
        //if(pool->apdata[pool->pos]->IsEmpty())
        {
            iRet = pool->pos;
            //if( bClear )
            //pool->apdata[iRet]->Clear();
            //pool->apdata[pool->pos]->SetEmpty(false);

            pool->pos++;
            if(pool->pos >= pool->max_count)
                pool->pos = 0;

            pool->count--;

            return pool->apdata[iRet];
        }

        pool->pos++;
        if(pool->pos >= pool->max_count) 
            pool->pos = 0;
    }
    pthread_mutex_unlock(&pool->lock);
    return NULL;
}


bool pool_del(pool_t *pool, int idx)
{

    //lock_guard<Fastlock>	autolock(pool->lock);

    if( idx<0 || idx>=pool->max_count )
    {
        pr_err("Error ! idx=%d", idx );
        return false;
    }

    //pool->apdata[idx]->Clear();

    pool->count++;

    return true; }
