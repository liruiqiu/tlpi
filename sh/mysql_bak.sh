#! /bin/sh
######################################################################################
#
#				Desc	: 以表备份当前项目使用的MYSQL
#               Author  : lrq(ruiqiu@126.com)
#               Date    : 2008-11-03 18:24     
#               Note	: 最后行是一个清理MYSQL的脚本所以放在这里面执行,以防万一....
#
######################################################################################

export PATH="/data/mysql/bin:/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin"

tday=`date +%d`
delday=`date +%d '--date=10 days ago'`
## 下面两个日期是拍卖行的备份目录
buyday=`date +%Y%m%d '--date=10 days ago'`
delday_60=`date +%Y%m%d '--date=60 days ago'`

BaseDir=/data/rsync/mysql
LogFile=/tmp/mysql_bak.log
BinDir=/usr/local/mysql/bin
OldDir=$BaseDir/$delday
user="root"
pass="34ccpalm"
port="22334"
pass="root"
port="3306"
for host in $@ 
do
	a_dbname=(`$BinDir/mysql -u$user -P$port -p$pass -h$host -e"show databases"|sed "1d"|grep -vE "information_schema|mysql|test"`)
	for dbname in ${a_dbname[@]} 
do
	echo $dbname
	#$BinDir/mysqldump -d --default-character-set=utf8 -u$user -P$port -p$pass -h$host --extended-insert=false $dbname > 252/$dbname.sql
	$BinDir/mysqldump -d --default-character-set=utf8 -u$user -P$port -p$pass -h$host --extended-insert=false $dbname > 221/$dbname.sql
done
done

