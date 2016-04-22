#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "linux/kfifo.h"
#include "linux/list.h"
#include "linux/printk.h"

struct __kfifo g_ff;
typedef struct Data
{
    int id;
    char data[128];
}Data;

void test_1()
{
    struct __kfifo ff;
    int rt,i;
    rt=__kfifo_alloc(&ff,1024,sizeof(Data));
    for(i=0;i<100;++i)
    {
        Data d;
        d.id=i;
        sprintf(d.data,"data %d",i);
        __kfifo_in(&ff,&d,1);
    }


    for(i=0;i<100;++i)
    {
        Data d;
        __kfifo_out(&ff,&d,1);
        printf("%d %s,",d.id,d.data);
    }

    __kfifo_free(&ff);
}
pthread_mutex_t lock;
void* register_data(void *arg)
{
    int i;
    Data d;
    while(1)
    {
        pthread_mutex_lock(&lock);
        if(__kfifo_out(&g_ff,&d,1)>0)
        {
            printf("%d\t",d.id);
            fflush(stdout);
        }
        //		else 
        //			sleep(1);
        pthread_mutex_unlock(&lock);
    }

    return 0;
}
void add_data()
{
    int i=0;
    srand(time(NULL));
    for(i=1;i<100;++i)
    {
        //		sleep(rand()%3);
        Data *d=(Data*)malloc(sizeof(Data));
        d->id=i;
        sprintf(d->data,"data %d",i);
        //		pthread_mutex_lock(&lock);
        __kfifo_in(&g_ff,d,1);
        //		pthread_mutex_unlock(&lock);
    }
}

typedef struct sf
{
    DECLARE_KFIFO(fifo, Data, 4);
    spinlock_t lock;

};

void test_2()
{
    struct sf f;
    struct sf *me = &f;

    pthread_spin_init(&me->lock, 0);
    INIT_KFIFO(me->fifo);

    int rt,i;
    rt= kfifo_alloc(&me->fifo,4);
    for(i=0;i<4;++i)
    {
        Data d;
        d.id=i;
        sprintf(d.data,"data %d",i);
        kfifo_in_locked(&me->fifo,&d,1,&me->lock);
    }


    for(i=0;i<4;++i)
    {
        Data d;
        kfifo_out_locked(&me->fifo,&d,1,&me->lock);
        printf("%d %s,",d.id,d.data);
    }

    kfifo_free(&me->fifo);

}

typedef struct sf_int
{
    DECLARE_KFIFO(fifo, int, 4);
    //struct kfifo fifo;
    spinlock_t lock;
};

int test3()
{
   DECLARE_KFIFO(fifo, Data, 4);
    spinlock_t lock;


    struct sf_int f;
    struct sf_int *me = &f;

    pthread_spin_init(&me->lock, 0);
    INIT_KFIFO(me->fifo);

    int rt,i;
    rt= kfifo_alloc(&me->fifo,4);
    for(i=0;i<5;++i)
    {
        //sprintf(d.data,"data %d",i);
        int d = i * 10000 + i*2;
        rt = kfifo_in_locked(&me->fifo,&d,1,&me->lock);
        pr_info("%d,0x%X, %d", d, d, rt);
    }

    
    int id;
    //for(id=0;id<5;++id)
    int d;
    while(kfifo_out_locked(&me->fifo,&d,1,&me->lock))
    {
        //rt=kfifo_out_locked(&me->fifo,&d,1,&me->lock);
        pr_info("%d,%d,0x%X",id, d, d);
    }

    kfifo_free(&me->fifo);
    return 0;

}
int test4()
{
    int a;
    pr_info("--------1----------");
out:
    pr_info("--------2----------");
    pr_info("--------2----------");
    pr_info("--------2----------");
    pr_info("--------2----------");

    return 1;
}
int main()
{
    printf("-----------------------test fifo------------------------------------------\n");
    return test3();
    return 0;
    test_1();
    pthread_t id;
    pthread_mutex_init(&lock,NULL);

    __kfifo_alloc(&g_ff,1024,sizeof(Data));
    pthread_create(&id,0,register_data,0);
    pthread_create(&id,0,register_data,0);
    pthread_create(&id,0,register_data,0);
    printf("------------------\n");
    pr_info("------------------\n");
    //  add_data();
    //
    //	sleep(100);

    return 0;
}
