#! /bin/sh

######################################################################################
#
#				Desc	: 清理拍卖行90天前的数据
#               Author  : lrq(ruiqiu@126.com)
#               Date    : 2010-04-29 11:33     
#
######################################################################################

export PATH="/data/mysql/bin:/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin"

BinDir=/data/mysql/bin
LogFile=/tmp/mysql_clear.log

buy_info="delete from buy_info where TO_DAYS(NOW()) - TO_DAYS(quto_time)>90"
sale_info="delete from sale_info where TO_DAYS(NOW()) - TO_DAYS(close_time)>90"

user="root"
pass="34ccpalm"
port="22334"

echo "-----------------------------------------------" >> $LogFile
echo "-----------------------------------------------" >> $LogFile
echo $(date +"%y-%m-%d %H:%M:%S") >> $LogFile
echo "-----------------------------------------------" >> $LogFile
for host in $@ 
do

	a_dbname=(`$BinDir/mysql -u$user -P$port -p$pass -h$host -e"show databases"|sed "1d"|grep "^tx"`)
	for dbname in ${a_dbname[@]}
do

	echo "start clear the $dbname  $(date +"%y-%m-%d %H:%M:%S")" >> $LogFile
	echo "-----------------------------------------------" >> $LogFile
	$BinDir/mysql -u$user -P$port -p$pass -h$host $dbname -e "$buy_info;$sale_info" 
	echo "the clear $dbname successfull! $(date +"%y-%m-%d %H:%M:%S")" >> $LogFile
	echo "-----------------------------------------------" >> $LogFile
done
echo "the code synchro data successfull! $(date +"%y-%m-%d %H:%M:%S")" >> $LogFile
echo "-----------------------------------------------" >> $LogFile
done

