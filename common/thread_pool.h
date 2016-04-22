/**
 * thread_pool.h
 *
 * This file declares the functionality associated with
 * your implementation of a thread_pool.
 */

#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void * pthread_result_t;
#define PTHREAD_CALL
typedef pthread_result_t ( * pthread_func_t )( void * args );

// maximum number of threads allowed in a pool
#define MAXT_IN_POOL 256

// You must hide the internal details of the thread_pool
// structure from callers, thus declare thread_pool of type "void".
// In thread_pool.c, you will use type conversion to coerce
// variables of type "thread_pool" back and forth to a
// richer, internal type.  (See thread_pool.c for details.)

typedef void *thread_pool;
typedef void thread_pool_t;

// "dispatch_fn" declares a typed function pointer.  A
// variable of type "dispatch_fn" points to a function
// with the following signature:
// 
//     void dispatch_function(void *arg);

typedef void (*dispatch_fn)(void *);

/**
 * create_thread_pool creates a fixed-sized thread
 * pool.  If the function succeeds, it returns a (non-NULL)
 * "thread_pool", else it returns NULL.
 */
thread_pool create_thread_pool(int num_threads_in_pool);


/**
 * dispatch sends a thread off to do some work.  If
 * all threads in the pool are busy, dispatch will
 * block until a thread becomes free and is dispatched.
 * 
 * Once a thread is dispatched, this function returns
 * immediately.
 * 
 * The dispatched thread calls into the function
 * "dispatch_to_here" with argument "arg".
 */
int dispatch_thread_pool(thread_pool from_me, dispatch_fn dispatch_to_here,
	      void *arg);

void * get_thread_data(thread_pool from_me, int idx);

/**
 * destroy_thread_pool kills the thread_pool, causing
 * all threads in it to commit suicide, and then
 * frees all the memory associated with the thread_pool.
 */
void destroy_thread_pool(thread_pool destroyme);

#ifdef __cplusplus
}
#endif

#endif

