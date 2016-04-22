#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "linux/base.h"
void t1()
{
    int i;
   char c[128] = {0}; 
   char cc[128] = {0};
   ring_buffer_t *prb;
   prb = ring_buffer_create(128);

  for(i=0; i<1500; i++) 
  {
    //c = i;
    memset(c,i%128,128);
    memset(cc,0,128);
   ring_buffer_put(prb, (char*)&c, i%128);
   ring_buffer_get(prb, (char *)&cc, 128);
   if (c[0] != cc[0])
   {
   pr_info("in=%d,out=%d", c[0],cc[0]);
   }

  }
}

int main(int argc, char **argv)
{
    unsigned int len, a,b;
    a = 0;
    b = 192;
    len = min(128, a - b);
    pr_info("min=%d,c=%d", len, a -b);
    pr_info("m=%d,a=%d",4%32,4&(32-1));
    pr_info("int=%d,p=%d,long=%d, float=%d, double=%d",sizeof(int),sizeof(int *),sizeof(long),sizeof(float),sizeof(double));
    t1();
    return 0;
}
