#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "linux/kfifo.h"
#include "linux/list.h"
#include "linux/printk.h"
//#include "linux/compiler.h"
# define kmalloc(x, f) malloc(x)

 #define KERN_EMERG  "<0>"   /* system is unusable           */
 #define KERN_ALERT  "<1>"   /* action must be taken immediately */
 #define KERN_CRIT   "<2>"   /* critical conditions          */
 #define KERN_ERR    "<3>"   /* error conditions         */
 #define KERN_WARNING    "<4>"   /* warning conditions           */
 #define KERN_NOTICE "<5>"   /* normal but significant condition */
 #define KERN_INFO   "<6>"   /* informational            */
 #define KERN_DEBUG  "<7>"   /* debug-level messages         */
 
 /* Use the default kernel loglevel */
 #define KERN_DEFAULT    "<d>"
 /*
  *   * Annotation for a "continued" line of log printout (only done after a
  *     * line that had no enclosing \n). Only to be used by core/arch code
  *       * during early bootup (a continued line is not SMP-safe otherwise).
  *         */
 #define KERN_CONT   "<c>"
 
#define printk printf
//


struct __kfifo g_ff;
typedef struct Data
{
    int id;
    char data[128];
}Data;

typedef struct DataNode
{
    Data d;
    struct list_head ptr;
}DataNode;

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

struct list_head g_head;
void test_list()
{
    int i=0;
    INIT_LIST_HEAD(&g_head);
    for(i=0;i<100;++i)
    {
        DataNode *n=(DataNode*)malloc(sizeof(DataNode));
        n->d.id=100-i;
        list_add(&n->ptr,&g_head);
    }
    DataNode *n1=list_first_entry(&g_head,DataNode,ptr);
    printf("%d\n", n1->d.id);
    struct list_head *p;
    DataNode *dn, *dnext;

    list_for_each(p, &g_head) {
        dn = list_entry(p, DataNode, ptr);
        printf("%d,", dn->d.id);
    }
    printf("\n");

    list_for_each_entry_safe(dn, dnext, &g_head, ptr) {
        printf("%d,", dn->d.id);
        free(dn);
    }

    printf("\n");
    return ;
}

struct student
{
    int id;
    char* name;
    struct list_head list;
};

void print_student(struct student*);

static int testlist_init(void)
{
    struct student *stu1, *stu2, *stu3, *stu4;
    struct student *stu;

    // init a list head
    LIST_HEAD(stu_head);

    // init four list nodes
    stu1 = kmalloc(sizeof(*stu1), GFP_KERNEL);
    stu1->id = 1;
    stu1->name = "wyb";
    INIT_LIST_HEAD(&stu1->list);

    stu2 = kmalloc(sizeof(*stu2), GFP_KERNEL);
    stu2->id = 2;
    stu2->name = "wyb2";
    INIT_LIST_HEAD(&stu2->list);

    stu3 = kmalloc(sizeof(*stu3), GFP_KERNEL);
    stu3->id = 3;
    stu3->name = "wyb3";
    INIT_LIST_HEAD(&stu3->list);

    stu4 = kmalloc(sizeof(*stu4), GFP_KERNEL);
    stu4->id = 4;
    stu4->name = "wyb4";
    INIT_LIST_HEAD(&stu4->list);

    // add the four nodes to head
    list_add (&stu1->list, &stu_head);
    list_add (&stu2->list, &stu_head);
    list_add (&stu3->list, &stu_head);
    list_add (&stu4->list, &stu_head);

    // print each student from 4 to 1
    list_for_each_entry(stu, &stu_head, list)
    {
        print_student(stu);
    }
    // print each student from 1 to 4
    list_for_each_entry_reverse(stu, &stu_head, list)
    {
        print_student(stu);
    }

    // delete a entry stu2
    list_del(&stu2->list);
    list_for_each_entry(stu, &stu_head, list)
    {
        print_student(stu);
    }

    // replace stu3 with stu2
    list_replace(&stu3->list, &stu2->list);
    list_for_each_entry(stu, &stu_head, list)
    {
        print_student(stu);
    }

    return 0;
}

static void testlist_exit(void)
{
    printk(KERN_ALERT "*************************\n");
    printk(KERN_ALERT "testlist is exited!\n");
    printk(KERN_ALERT "*************************\n");
}

void print_student(struct student *stu)
{
    printk (KERN_ALERT "======================\n");
    printk (KERN_ALERT "id  =%d\n", stu->id);
    printk (KERN_ALERT "name=%s\n", stu->name);
    printk (KERN_ALERT "======================\n");
}

typedef struct Node
{
    int m;
    struct list_head ptr;
}Node;

struct list_head *g_h;
int main()
{
    pr_info("-----------------------test list------------------------------------------\n");

    Node *n;

    size_t ss;
    int x=offsetof(Node,m);
    printf("x=%d\n", x);
    //printk (KERN_ALERT "id  =%d\n", x);
    n=list_entry(g_h,Node,ptr);

    testlist_init();
    testlist_exit();
    test_list();

    printf("-----------------------test fifo------------------------------------------\n");
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
