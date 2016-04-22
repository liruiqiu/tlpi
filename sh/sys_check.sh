#! /bin/sh      
####################################################################
#
#               Desc   : 监控脚本
#               Author : lrq(ruiqiu@126.com)
#               Date   : 2008-09-15 11:24
#                 
#               修改记录:2010-05-01
#                        evan 将告警的方式由原来的短信修改为邮件
#####################################################################
export PATH="/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin" 
desc_info=`df -h | grep -E "8[5-9]%|9[0-9]%" | awk '{print $4$5$6}'| awk -F%/ '{print $1$2}'`
inode_info=`df -hi | grep -E "8[5-9]%|9[0-9]%"| awk '{print $4$5$6}'| awk -F%/ '{print $1}'`
cpuuse=`vmstat 1 3 |sed 1,4d|awk '{print $13}'`
load=`cat /proc/loadavg |awk '$1>5||$2>5||$3>5 {print $1" "$2" "$3}'`
host=`ifconfig |grep "inet addr:"|awk  -F: '{print $2}'|awk '/^192.168./ {print $1}'`
cd /opt
if [ -n "${desc_info}" ] ; then
msg_info="desk space is almost full ${desc_info}; " 
fi

if [ -n "$inode_info" ] ; then
msg_info=${msg_info}"desc inode is almost full ${inode_info}; " 
fi

if [ "$cpuuse" -gt 60 ] ; then
msg_info=${msg_info}"cpu High :${cpuuse}; " 
fi

if [ -n "${load}" ] ; then
msg_info=${msg_info}"loadaverage:${load} " 
fi

if [ -n "${msg_info}" ] ; then
	for addr in 13810348337@139.com 13910170668@139.com 13911353057@139.com 13511016795@139.com 13466788532@139.com 13552851967@139.com 15810270057@139.com 13910227644@139.com 13811112791@139.com
do
	wget http://manager.dog3g.com/api/send_email.jsp?addr=$addr\&msgs="${msg_info};IP:$host" 
done
fi
