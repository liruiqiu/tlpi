/**
 * thread_pool.c
 *
 * This file will contain your implementation of a thread_pool.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "thread_pool.h"
//#include "spthread.h"

typedef struct _thread_st {
	pthread_t id;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	dispatch_fn fn;
	void *arg;
	thread_pool parent;
} _thread;

// _thread_pool is the internal thread_pool structure that is
// cast to type "thread_pool" before it given out to callers
typedef struct _thread_pool_st {
	// you should fill in this structure with whatever you need
	pthread_mutex_t tp_mutex;
	pthread_cond_t tp_idle;
	pthread_cond_t tp_full;
	pthread_cond_t tp_empty;
	_thread ** tp_total_list;
	_thread ** tp_list;
	int tp_index;
	int tp_max_index;
	int tp_stop;

	int tp_total;
} _thread_pool;

thread_pool create_thread_pool(int num_threads_in_pool)
{
	_thread_pool *pool;

	// sanity check the argument
	if ((num_threads_in_pool <= 0) || (num_threads_in_pool > MAXT_IN_POOL))
		return NULL;

	pool = (_thread_pool *) malloc(sizeof(_thread_pool));
	if (pool == NULL) {
		fprintf(stderr, "Out of memory creating a new thread_pool!\n");
		return NULL;
	}

	// add your code here to initialize the newly created thread_pool
	pthread_mutex_init( &pool->tp_mutex, NULL );
	pthread_cond_init( &pool->tp_idle, NULL );
	pthread_cond_init( &pool->tp_full, NULL );
	pthread_cond_init( &pool->tp_empty, NULL );
	pool->tp_max_index = num_threads_in_pool;
	pool->tp_index = 0;
	pool->tp_stop = 0;
	pool->tp_total = 0;
	pool->tp_list = ( _thread ** )malloc( sizeof( void * ) * MAXT_IN_POOL );
	memset( pool->tp_list, 0, sizeof( void * ) * MAXT_IN_POOL );
    pool->tp_total_list = ( _thread ** )malloc( sizeof( void * ) * MAXT_IN_POOL );
	memset( pool->tp_total_list, 0, sizeof( void * ) * MAXT_IN_POOL );

	return (thread_pool) pool;
}

int save_thread( _thread_pool * pool, _thread * thread )
{
    int ret = -1;

    pthread_mutex_lock( &pool->tp_mutex );

    if( pool->tp_index < pool->tp_max_index ) {
        pool->tp_list[ pool->tp_index ] = thread;
        pool->tp_index++;
        ret = 0;

        pthread_cond_signal( &pool->tp_idle );

        if( pool->tp_index >= pool->tp_total ) {
            pthread_cond_signal( &pool->tp_full );
        }
    }

    pthread_mutex_unlock( &pool->tp_mutex );

    return ret;
}

pthread_result_t PTHREAD_CALL wrapper_fn( void * arg )
{
    _thread * thread = (_thread*)arg;
    _thread_pool * pool = (_thread_pool*)thread->parent;

    for( ; 0 == ((_thread_pool*)thread->parent)->tp_stop; ) {
        thread->fn( thread->arg );

        if( 0 != ((_thread_pool*)thread->parent)->tp_stop ) break;

        pthread_mutex_lock( &thread->mutex );
        if( 0 == save_thread( thread->parent, thread ) ) {
            pthread_cond_wait( &thread->cond, &thread->mutex );
            pthread_mutex_unlock( &thread->mutex );
        } else {
            pthread_mutex_unlock( &thread->mutex );
            pthread_cond_destroy( &thread->cond );
            pthread_mutex_destroy( &thread->mutex );

            free( thread );
            break;
        }
    }

    pthread_mutex_lock( &pool->tp_mutex );
    pool->tp_total--;
    if( pool->tp_total <= 0 ) pthread_cond_signal( &pool->tp_empty );
    pthread_mutex_unlock( &pool->tp_mutex );

    return 0;
}


int dispatch_thread_pool(thread_pool from_me, dispatch_fn dispatch_to_here, void *arg)
{
    int ret = 0;

    _thread_pool *pool = (_thread_pool *) from_me;
    pthread_attr_t attr;
    _thread * thread = NULL;

    // add your code here to dispatch a thread
    pthread_mutex_lock( &pool->tp_mutex );

    while( pool->tp_index <= 0 && pool->tp_total >= pool->tp_max_index ) {
        pthread_cond_wait( &pool->tp_idle, &pool->tp_mutex );
    }

    if( pool->tp_index <= 0 ) {
        _thread * thread = ( _thread * )malloc( sizeof( _thread ) );
        memset( &( thread->id ), 0, sizeof( thread->id ) );
        pthread_mutex_init( &thread->mutex, NULL );
        pthread_cond_init( &thread->cond, NULL );
        thread->fn = dispatch_to_here;
        thread->arg = arg;
        thread->parent = pool;

        pthread_attr_init( &attr );
        pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED);

        if( 0 == pthread_create( &thread->id, &attr, wrapper_fn, thread ) ) {
            pool->tp_total_list[pool->tp_total]= thread;
            pool->tp_total++;
            printf( "create thread#%ld\n", thread->id );
        } else {
            ret = -1;
            printf( "cannot create thread\n" );
            pthread_mutex_destroy( &thread->mutex );
            pthread_cond_destroy( &thread->cond );
            free( thread );
        }
    } else {
        pool->tp_index--;
        thread = pool->tp_list[ pool->tp_index ];
        pool->tp_list[ pool->tp_index ] = NULL;

        thread->fn = dispatch_to_here;
        thread->arg = arg;
        thread->parent = pool;

        pthread_mutex_lock( &thread->mutex );
        pthread_cond_signal( &thread->cond ) ;
        pthread_mutex_unlock ( &thread->mutex );
    }

    pthread_mutex_unlock( &pool->tp_mutex );

    return ret;
}

void * get_thread_data(thread_pool from_me, int idx)
{
    _thread_pool *pool = (_thread_pool *) from_me;
    if ((idx < 0) || (idx >= MAXT_IN_POOL))
		return NULL;
    return pool->tp_total_list[idx]->arg;
}

void destroy_thread_pool(thread_pool destroyme)
{
    _thread_pool *pool = (_thread_pool *) destroyme;

    // add your code here to kill a thread_pool
    int i = 0;

    pthread_mutex_lock( &pool->tp_mutex );

    if( pool->tp_index < pool->tp_total ) {
        printf( "waiting for %d thread(s) to finish\n", pool->tp_total - pool->tp_index );
        pthread_cond_wait( &pool->tp_full, &pool->tp_mutex );
    }

    pool->tp_stop = 1;

    for( i = 0; i < pool->tp_index; i++ ) {
        _thread * thread = pool->tp_list[ i ];

        pthread_mutex_lock( &thread->mutex );
        pthread_cond_signal( &thread->cond ) ;
        pthread_mutex_unlock ( &thread->mutex );
    }

    if( pool->tp_total > 0 ) {
        printf( "waiting for %d thread(s) to exit\n", pool->tp_total );
        pthread_cond_wait( &pool->tp_empty, &pool->tp_mutex );
    }

    for( i = 0; i < pool->tp_index; i++ ) {
        free( pool->tp_list[ i ] );
        pool->tp_list[ i ] = NULL;
    }

    pthread_mutex_unlock( &pool->tp_mutex );

    pool->tp_index = 0;

    pthread_mutex_destroy( &pool->tp_mutex );
    pthread_cond_destroy( &pool->tp_idle );
    pthread_cond_destroy( &pool->tp_full );
    pthread_cond_destroy( &pool->tp_empty );

    free( pool->tp_list );
    free( pool );
}
