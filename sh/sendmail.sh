#!/bin/sh

#export PATH=$PATH:/usr/local/bin:/usr/bin:/usr/local/sbin
export PATH="$PATH:/usr/sbin:/sbin:/usr/local/bin:/usr/local/sbin:/usr/X11R6/bin"
#echo $PATH
localip=`ifconfig | awk '/inet/{print $2}' | awk -F: '{print $2}' | grep 61`

#echo $localip,"MO-ProcessSpeed-Stat"
mail -s $localip",ÌìÆøÔ¤±¨push" lidingkun@kongzhong.com < /usr/local/smsproduct/tools/temp/wmt.txt
#mail -s $localip",MO-ProcessSpeed-Stat"  heyizhou@kongzhong.com < /usr/local/smsproduct/tools/temp/mospeedstat.txt 
