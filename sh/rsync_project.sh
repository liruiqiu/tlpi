#! /bin/sh

####################################################################
#
#               Desc   : 同步脚本
#               Author : lrq(ruiqiu@126.com)
#               Date   : 2010-04-23 11:39
#		        Note   : 以小时为单位做同步倍份
#
#####################################################################

export PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

LogFile=/tmp/rsync.log
echo "-----------------------------------------------" >> $LogFile
echo "-----------------------------------------------" >> $LogFile
echo $(date +"%y-%m-%d %H:%M:%S") >> $LogFile
echo "-----------------------------------------------" >> $LogFile
##tx synchro data
echo "start synchro data the tx $(date +"%y-%m-%d %H:%M:%S")" >> $LogFile  
while read line
do
	if [ ${line:0:1} != "#" ]
	then
		tx_arr=(`echo $line|tr ':' ' '`)
		area=${tx_arr[0]}            # 游戏区
		front=${tx_arr[1]}           # 前台 
		after=${tx_arr[2]}           # 后台
		port=${tx_arr[3]}            # 端口
		dns=${tx_arr[4]}            # 域名
		name_cn=${tx_arr[5]}            # 中文名
		join_area=${tx_arr[6]}
		front_ip="192.168.2."$front  # 前台IP
		after_ip="192.168.2."$after  # 后台IP
		rsync -zrtopg --delete rsync://$after_ip/$area  /data/rsync/$area
	fi
done < /home/script/conf/tx.conf
echo "the tx synchro data successfull! $(date +"%y-%m-%d %H:%M:%S")" >> $LogFile    

echo "start synchro data the qn $(date +"%y-%m-%d %H:%M:%S")" >> $LogFile  
while read line
do
	if [ ${line:0:1} != "#" ]
	then
		tx_arr=(`echo $line|tr ':' ' '`)
		area=${tx_arr[0]}            # 游戏区
		front=${tx_arr[1]}           # 前台 
		after=${tx_arr[2]}           # 后台
		port=${tx_arr[3]}            # 端口
		dns=${tx_arr[4]}            # 域名
		name_cn=${tx_arr[5]}            # 中文名
		join_area=${tx_arr[6]}
		front_ip="192.168.2."$front  # 前台IP
		after_ip="192.168.2."$after  # 后台IP
		rsync -zrtopg --delete rsync://$after_ip/$area  /data/rsync/$area
	fi
done < /home/script/conf/qn.conf
echo "the qn synchro data successfull! $(date +"%y-%m-%d %H:%M:%S")" >> $LogFile    
if [  `date -d yesterday "+%H"` == 02 ]
then
	echo "start synchro data the html $(date +"%y-%m-%d %H:%M:%S")" >> $LogFile  
	for i in 97 98 99 100 101 102 103 104 105 106 107 108 109 
	do
		rsync -zrtopg --delete rsync://192.168.2.$i/html  /data/rsync/html/$i                                   
		rsync -zrtopg --include=access.lo* --exclude=* --delete rsync://192.168.2.$i/resin_logs /data/rsync/resin_logs/$i
	done
	echo "start synchro data the html $(date +"%y-%m-%d %H:%M:%S")" >> $LogFile  
	echo "start synchro data the xd $(date +"%y-%m-%d %H:%M:%S")" >> $LogFile  
	for i in 98 163 97
	do
		rsync -zrtopg --delete rsync://192.168.2.$i/games  /data/rsync/games/$i
	done
	echo "start synchro data the xd $(date +"%y-%m-%d %H:%M:%S")" >> $LogFile  
fi

	#rsync -zrtopg --delete rsync://115.238.28.50/txd8  /data/rsync/txd8
	#rsync -zrtopg --delete rsync://218.241.156.225/txe1  /data/rsync/txe1
