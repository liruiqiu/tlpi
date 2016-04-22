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
BinDir=/data/mysql/bin
OldDir=$BaseDir/$delday
user="root"
pass="34ccpalm"
port="22334"
for host in $@ 
do
	echo "-----------------------------------------------" >> $LogFile
	echo "-----------------------------------------------" >> $LogFile
	echo $(date +"%y-%m-%d %H:%M:%S") >> $LogFile
	echo "-----------------------------------------------" >> $LogFile
	if [ -d $OldDir ]
	then
		rm -rf $OldDir
		echo "delete the 10 days ago backupfile is ok!" >>$LogFile
	else
		echo "No 10 days ago bakcupfile!" >> $LogFile
	fi


	if [ -d $OldDir ]
	then
		rm -rf $OldDir
		echo "delete the 10 days ago backupfile is ok!" >>$LogFile
	else
		echo "No 10 days ago bakcupfile!" >> $LogFile
	fi


	a_dbname=(`$BinDir/mysql -u$user -P$port -p$pass -h$host -e"show databases"|sed "1d"|grep -vE "information_schema|mysql|test"`)
	for dbname in ${a_dbname[@]} 
do
	if [ -d $BaseDir/$tday/$dbname ]
	then
		echo 'the dir is ok!'>> $LogFile
	else
		echo "the dir is not,please create it" >> $LogFile
		mkdir -p /$BaseDir/$tday/$dbname
	fi
	echo "start backup the $dbname $(date +"%y-%m-%d %H:%M:%S")" >> $LogFile
	echo "-----------------------------------------------" >> $LogFile
	a_tbname=(`$BinDir/mysql -u$user -P$port -p$pass -h$host $dbname -e"show tables"|sed "1d"|grep -v tx_consumes`)
	for tbname in ${a_tbname[@]}
do
	$BinDir/mysqldump --default-character-set=utf8 -u$user -P$port -p$pass -h$host --extended-insert=false $dbname $tbname > $BaseDir/$tday/$dbname/$dbname.$tbname.sql
	echo "$tbname table backup successfull !">> $LogFile
done
cd $BaseDir/$tday
tar czf $dbname.tar.gz $dbname
rm -rf $dbname
echo "the $dbname database backup successfull! $(date +"%y-%m-%d %H:%M:%S")" >> $LogFile
echo "-----------------------------------------------" >> $LogFile
done
done

