#include <stdlib.h>
#include <stdio.h>
//#include "linux/base.h"

void test1()
{
    //char a[4]={0x16,0x16,0x16,0x16};
    char a[5]={0};
    a[0]=1;
    a[1]=1;
    a[2]=2;
    a[3]=3;
    //memcpy(a,0,0);
    printf("------11---------%s,%s,%s,\n",a,a,a);
    //exit(1);
    printf("------12----------\n");
}
void test2()
{
    int len = 0;
    while(1)
    {

        switch(len)
        {
            case 0:
                break;
                break;
            case -1:
                break;
                break;
        } 
        sleep(1);
        printf("------21----------\n");
    }

    printf("------22----------\n");

}

int main()
{
    test1();
#ifdef __cplusplus
    printf("------1----------\n");
#endif

#ifdef linux
    printf("------2----------\n");
#endif
    //   pr_info();
    //#pragma messages("-----3----------")
    //#error "-----4----------"
#warning "----5--------";

    printf("------end----------\n");
    //#include "asfjasljdsf"
    return 1;
}
