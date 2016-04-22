#! /bin/sh      
####################################################################
#
# =>           Desc   : 日志清理脚本
# =>           Author : lrq(ruiqiu@126.com)
# =>           Date   : 2008-09-17 11:24
# =>           Note   : 
#
#####################################################################

export PATH="/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/sbin:/usr/local/bin"

month=`date +%m`
year=`date +%Y`
year_month=`date +%Y-%m`
today=`date +%Y-%m-%d`
last_month=`date -d "last month" +%Y-%m`
last_year=`date -d "1 year ago" +%Y-%m`

area_dir=""
tx_dir="/home/hezuo"
xd_dir="/usr/local/games"

#####################clear resin/log start#########################
if [ -d "/usr/local/resin/log" ]; then
	cd /usr/local/resin/log
	find ./ -mtime +7 -type f |grep ".20"|grep -v ".gz"|xargs -i gzip -9 {}
	find ./ -type f -size +1000000k |grep -v ".gz"|xargs -i gzip -9 -S -$today.big.gz {}
	for i in $year_month $last_month
	do
		if [ ! -d "$i" ]
		then
			mkdir $i
		fi
		gz_file=`ls -l |grep ^-|awk '{print $NF}'|grep ".gz"|grep "$i"`
		if [ -n "$gz_file" ]; then
			mv $gz_file $i

		fi
	done
	rm -rf $last_year
	find ./ -mtime +180 -type f |xargs -i rm -f {}                                                            
fi

if [ -d "/usr/local/resin/logs" ]; then
	cd /usr/local/resin/logs
	find ./ -mtime +3 -type f |grep ".20"|grep -v ".gz"|xargs -i gzip -9 {}
	find ./ -type f -size +1000000k |grep -v ".gz"|xargs -i gzip -9 -S -$today.big.gz {}
	find /usr/local/resin/logs/ -mtime +30 -type f | xargs -i rm {}
fi 
#####################clear tx/log start #########################

for area in $@ 
do
	if [ ${area:0:2} == "tx" ]
	then
		area_dir=$tx_dir
	elif [ ${area:0:2} == "xd" ]
	then
		area_dir=$xd_dir
	elif [ ${area:0:2} == "xi" ]
	then
		area_dir=$xd_dir
	elif [ ${area:0:2} == "po" ]
	then
		area_dir=$xd_dir
	else
		area_dir=""
	fi
	db_log_dir="$area_dir/db_log"
	if [ -d "$db_log_dir" ]; then
		cd $db_log_dir
		find ./ -mtime +7 -type f|grep -vE ".gz|.svn"|xargs -i gzip -9 {}
		find ./ -type f -size +500000k |grep -vE ".gz|.svn"|xargs -i gzip -9 -S -$today.big.gz {}
		find ./ -mtime +365 -type f |grep -v ".svn"|xargs -i rm -f {}   
	fi
	log_dir="$area_dir/log"
	if [ -d "$log_dir" ]; then 
		cd $log_dir
		find ./ -mtime +7 -type f |grep "\-20"|grep -vE ".gz|.svn"|xargs -i gzip -9 {}
		find ./ -mtime +7 -type f|grep -E "cmd_record.log|stderr|error"|xargs -i rm -f {}
		find ./ -type f -size +200000k |grep -vE ".gz|.svn"|xargs -i gzip -9 -S -$today.big.gz {}

		if [ ! -d "$year_month" ]; then
			mkdir $year_month
		fi
		pike_month=`echo $month |tr -d '0'`
		pike_year_month=$year"-"$pike_month
		gz_file=`ls -l |grep ^-|awk '{print $NF}'|grep ".gz"|grep -E "$year_month|$pike_year_month"`
		if [ -n "$gz_file" ]; then
			mv $gz_file $year_month
		fi
		rm -rf $last_year
		find ./ -mtime +365 -type f |grep -v ".svn"|xargs -i rm -f {}
	fi
done
