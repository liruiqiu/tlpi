#!/bin/bash
# 防爬虫脚本 by Sky Bin 2009.12
# description: Disable Spider

#一个简单的防爬虫脚本
#因为公司有时候受爬虫的影响，有时候应用压力很高，所以决定封掉大部分爬虫。
#1.检测user-agent
#2.设定一个阀值，如果超过这个访问阀值，就进入灰名单，某个时间段联系两次进入灰名单，就干掉这个ip
#3.检测开发提供的特殊连点，查过阀值并访问特殊连点，也限制它。
#4.判断reffer，如果为空的链接记录数大于整体访问的某个阀值，也限制该IP



PATH="/var/PROGRAM/MANAGEMENT/modules/xbash:/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:"
LANG=zh_CN
export PATH LANG
PATTERN_MIN=30
PORT=8080 #暂时不需要
MROOT="/data/resin-pro-3.1.6"
ACCESS_LOG=$MROOT"/logs/access.log"
IPTABLE="/tmp/.iptablestore"
LIMITIPLOG=$MROOT"/logs/limitIP.log"
CRITICAL=30 #系统并发数的阀值 当系统的连接数超过该值才运行脚本
PY_ACCESS="/var/PROGRAM/MANAGEMENT/modules/xbash/access.py"
SORT_D="/tmp/.sortd" #临时放置排序的选出来的段
SORT_S="/tmp/.sorts" #临时放置排序的选出来的IP

cd $MROOT

now_time_hour=$(date +%H)
now_time_min=$(date +%M)

case $now_time_hour in
	01|02|03|04|05|06|07|08|09)
	now_time_hour=`echo $now_time_hour |  awk ' BEGIN { FS="0"} {print $2}'`
	;;
	00)
	now_time_min=0
	;;
esac

case $now_time_min in
	01|02|03|04|05|06|07|08|09)
	now_time_min=`echo $now_time_min |  awk ' BEGIN { FS="0"} {print $2}'`
	;;
	00)
	now_time_min=0
	;;
esac

now_year=$(date +%Y)
now_month=$(date +%m)
now_day=$(date +%d)


if [[ $now_time_min -ge 0 && $now_time_min -le 20 && $now_time_hour -eq 8 && -e $IPTABLE ]] ; then


	mv $MROOT/logs/limitIP.log $MROOT/logs/limitIP.log.$now_year$now_month$now_day
	if [[ "$now_day" == "15" ]] ; then

		tar -cvf $MROOT/logs/limitIP.log.tar $MROOT/logs/limitIP.log.[0-9][0-9]*
		rm  -f $MROOT/logs/limitIP.log.[0-9][0-9]*
	fi
	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >> $LIMITIPLOG
	echo "复制iptables" >> $LIMITIPLOG
	iptables-restore < $IPTABLE
	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >> $LIMITIPLOG
	rm -f $IPTABLE
	exit
fi


if [[ $now_time_min -ge 0  && $now_time_min -le 50 && $now_time_hour -eq 2 ]] ; then
	iptables-save > $IPTABLE
	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >> $LIMITIPLOG
	echo "已备份iptables" >> $LIMITIPLOG
	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >> $LIMITIPLOG
	iptables -F
	exit
fi
connect=$(netstat -st | awk '/connections established/{print $1}')

[[ $connect -lt $CRITICAL ]] && exit  #如果连接数少于阀值就退出

BLACK=/tmp/.black #存储符合黑名单的IP和IP段
GRAY=/tmp/.gray  #灰名单
LIST=/tmp/.list  #存储符合相应条件的日志信息
LISTT=/tmp/.listt #临时存储变量，存储不是ip段的IP
LOGPID=/tmp/.sclogpid #存储灰名单的版本
BFB=100 #百分比的分母
FZ=2    #百分比的分子
TOTALLINE=15000 #选取的总数
#临时存放灰名单的IP
patternGrayIP=""


pattern_year=$(date +%Y)
pattern_hour=$(date +%k)
now_patter_hour=0
min=`echo $PATTERN_MIN | awk ' BEGIN { FS="0"} {print $1}'`
i=0
head_min=`date +%M | awk '{print substr($1,1,1)}'`
real_min=$head_min
pattern_tap=0
while [ $i -lt $min ]
do
	if [[ real_min -eq 0 ]] ; then

		if [[ "$pattern_hour" == " 0" ]] ; then

			pattern_hour=24
		fi
		pattern_hour=$(($pattern_hour-1))
		head_min=6
		pattern_tap=1
	fi

	if [[ $pattern_tap -eq 0 ]] ; then

		real_min=$(($head_min-$i-1))
	else
		real_min=$(($head_min+$i-$min))   
	fi   

	case $pattern_hour in
		" 0"|" 1"|" 2"|" 3"|" 4"|" 5"|" 6"|" 7"|" 8"|" 9")
		now_pattern_hour=`echo $pattern_hour | awk '{print substr($1,1,1)}'`
		now_pattern_hour="0$now_pattern_hour"
		;;
		0|1|2|3|4|5|6|7|8|9)
		now_pattern_hour="0$pattern_hour"
		;;
		*)
		now_pattern_hour="$pattern_hour"
		;;
	esac  
	if [[ $i -lt $(($min-1)) ]] ; then 
		pattern_min="$pattern_min$pattern_year:$now_pattern_hour:$real_min|"
	else
		pattern_min="$pattern_min$pattern_year:$now_pattern_hour:$real_min"
	fi
	i=$(($i+1))  
done

time_pattern="$pattern_min"


############
# 主机信息 #
############
INTF=$(netstat -rn | tail -1 | awk '{print $NF}')
IP=$(ifconfig $INTF | awk '/inet addr/{ split($2,tmp,":") ; print tmp[2] }')
HOST_NAME=$(hostname --short)
###########
##########
# 邮件环境#
##########
#CHARTSET="zh_CN."
MAIL_CLIENT=""
MAIL_SENDER=""
MAIL_SERVER=""
case $IP in
	192.168.230.*|192.168.1[0-1].*|192.168.238.*|202.*)
	MAIL_SERVER=""
	;;
	*)
	MAIL_SERVER=""
	;;
esac
##########
LINECOUNT=0

rm -f $LIST
rm -f $LISTT

testline=`wc -l $ACCESS_LOG | awk '{print $1}'`

#### 以下是有时间匹配的样本数据的获取
#tail -$TOTALLINE $ACCESS_LOG | grep -E "$time_pattern" > $LIST
grep -E "$time_pattern" $ACCESS_LOG > $LIST
TOTALLINE=`wc -l $LIST | awk '{print $1}'`
if [[ -e $LIST ]]  ; then
	patternLINE=`wc -l $LIST | awk '{print $1}'`
	BFB=$(($patternLINE/$BFB))
	FZ=$(($FZ*$BFB))
	echo "##########################################################" >> $LIMITIPLOG
	date >> $LIMITIPLOG
	echo "匹配的时间模式是:"$time_pattern >> $LIMITIPLOG
	echo "Access_LOG的数据总数是:"$testline >> $LIMITIPLOG
	echo "获取的数据的总数是:"$TOTALLINE >> $LIMITIPLOG
	echo "阀值是:"$FZ >> $LIMITIPLOG
else
	echo "##########################################################" >> $LIMITIPLOG
	date >> $LIMITIPLOG
	echo "-----------------------------------------------------------" >> $LIMITIPLOG
	echo "没有获取到样本数据" >> $LIMITIPLOG
	exit
fi




# 把agent中含有爬虫类代表的字符的先清除到黑名单中
cat $LIST | \
grep -iEv "googlebot|baiduspider" |grep -E "spider|bot|Yahoo|archiver|yodaoice" | awk '{print $1}' | sort | uniq  > $BLACK


#缓存通过第一个条件的log数据
cat $LIST | \
#选取不包含特殊字符的数据
grep  -Eiv "googlebot|archiver|spider|bot|Yahoo" | \
#选取例外IP之外的数据
grep -Ev "192.168.*.*|xx|127.0.0.1" > $LISTT
cat $LISTT > $LIST
rm -f $LISTT



if [[ -e $BLACK ]] ; then
	spiderNum=`wc -l $BLACK | awk '{print $1}'`

	if [[ $spiderNum -gt 0 ]] ; then
		echo "-------------以下IP包含Spider等特殊字符--------------" >> $LIMITIPLOG
		cat $BLACK >> $LIMITIPLOG 
		limitIP_num=`wc -l $BLACK | awk '{print($1)}'`

		cat $BLACK | sort | uniq -c > /tmp/.tmp_black

		if [[ -e /tmp/.tmp_black ]] ; then
			#########发送邮件############
			env MAILRC=/dev/null charset="$CHARTSET" from="$MAIL_SENDER" smtp="$MAIL_SERVER" \
			nail -n -s "$HOST_NAME($IP)含有爬虫等关键字名单的IP" $MAIL_CLIENT < /tmp/.tmp_black
		fi

	fi

fi


cat $LIST | \
$PY_ACCESS G -gh | \
awk '{if( NR>2 && $1~/[0-9]+/&&"-"!~$2 && $3!="" )  print $1,$3}' | \
while read num ip
do
	if [[ $num -gt $FZ ]]
	then
		echo "$ip"  >> $GRAY
	else
		continue
	fi

done









if [[ -e $GRAY ]] ; then
	tmp_gray_line=`wc -l $GRAY | awk '{print $1}'`
	if [[ $tmp_gray_line -gt 0 ]] ; then
		if [[ -e $LOGPID ]] ; then
			last_hour=`cat $LOGPID | awk '{print $1}'`

			if (( now_time_hour - last_now > 1 ))
			then
				cat $GRAY | sort | uniq -c | awk '{print($1,$2)}' | \
				while read num ip
				do
					if [[ $num -gt 1 ]]
					then
						echo "$ip"
					fi
				done > $LISTT

				if [[ -e $LISTT ]] ; then  
					logcount=`wc -l $LISTT | awk '{print($1)}'`

					if [[ $logcount -gt 0 ]] ; then
						#########发送邮件############
						env MAILRC=/dev/null charset="$CHARTSET" from="$MAIL_SENDER" smtp="$MAIL_SERVER" \
						nail -n -s "$HOST_NAME($IP)两次连续进入灰名单名单的IP" $MAIL_CLIENT < $LISTT

						echo "-------------以下字符是两次连续进入灰名单的------" >> $LIMITIPLOG
						cat $LISTT >> $LIMITIPLOG
						cat $LISTT > $GRAY
						############封灰名单##################
						cat $GRAY >> $BLACK
						cat $GRAY >>  $MROOT/logs/gray.log ####暂时存放灰名单IP用于观察
					fi
				fi

			fi   

		else
			echo $now_time_hour > $LOGPID
		fi
	fi
fi


rm -f $LISTT

echo "检查特殊文件的存在" >> $LIMITIPLOG

if [[ $# -eq 0 ]]
then
	echo "没有设置特殊连点文件,跳过特殊连点检查" >> $LIMITIPLOG
else
	graylinecount=`wc -l $GRAY | awk '{print $1}'`
	i=0
	if [[ $graylinecount -gt 1 ]] ; then
		cat $GRAY | awk '{print $1}' | \
		while read ip
		do
			if [[ "$ip" ]] ; then

				cat $1 | awk '{print($1)}' | \
				while read limitedUrl
				do
					tap=`cat $LIST | access.py FG -gr -h $ip | grep -E "$limitedYrl" | awk '{if( NR>2 && $1~/[0-9]+/&&"-"!~$2 && $3!="" && $1>0  )  print $3}' | wc -l | awk '{print $1}'`
					if [[ tap -gt 0 ]] ; then
						echo $ip >> $LISTT
					fi
				done

			fi
		done
		if [[ -e $LISTT ]] ; then
			cat $LISTT | uniq -c | awk '{print($2)}' > /tmp/.gray_tsld
			tsldEmailTap=`wc -l /tmp/.gray_tsld | awk '{print $1}'`
			if [[ $tsldEmailTap -gt 0 ]] ; then
				#########发送邮件############
				env MAILRC=/dev/null charset="$CHARTSET" from="$MAIL_SENDER" smtp="$MAIL_SERVER" \
				nail -n -s "$HOST_NAME ($IP)爬虫检查过程中在灰名单发现有特殊连接的IP" $MAIL_CLIENT < /tmp/.gray_tsld
				echo "----------------爬虫检查过程中在灰名单发现有特殊连接的IP-------------------" >> $LIMITIPLOG
				cat /tmp/.gray_tsld >> $LIMITIPLOG
				rm -f /tmp/.gray_tsld
			fi
		fi
	fi

fi


rm -f $LISTT
rm -f $LIST
cat $BLACK | awk ' BEGIN { FS="."} {printf "%d.%d.%d\n",$1,$2,$3}' | uniq -c | \
awk '{print($1,$2)}' | \
while read num ip
do
	if [[ $num -gt 1 ]]
	then
		echo "$ip"
		LINECOUNT=$(($LINECOUNT+$num))
	else
		LINECOUNT=$(($LINECOUNT+$num))
		head -n "$LINECOUNT" $BLACK | tail -1 >> $LIST
	fi
done > $LISTT



if [[ -e $LIST ]] ; then
	cat $LIST | awk '{print($1)}' | \
	# 封锁判断为异常的ip
	while read ip
	do
		continuetap=`iptables -nL | grep $ip | wc -l | awk '{print $1}'`
		if [[ $continuetap -gt 0 ]] ; then
			continue
		fi
		diptap=`echo $ip | awk '{print($1".0/24")}'`
		dtap=`iptables -nL | grep $diptap | wc -l | awk '{print $1}'`
		if [[ dtap -gt 0 ]] ; then
			continue
		fi
		#iptables -A INPUT -s $ip -p tcp --dport $PORT -j REJECT --reject-with tcp-reset
		iptables -A INPUT -s $ip -p tcp -j DROP
	done
fi


# 封锁异常ip段

if [[ -e $LISTT ]]  ; then
	cat $LISTT | awk '{print($1".0/24\n")}'  | \
	while read ip
	do
		if [[ "$ip" ]] ; then

			continuetap=`iptables -nL | grep $ip | wc -l | awk '{print $1}'`
			if [[ $continuetap -gt 0 ]] ; then
				continue
			fi
			#iptables -A INPUT -s $ip -p tcp --dport $PORT  -j REJECT --reject-with tcp-reset
			iptables -A INPUT -s $ip -p tcp -j DROP
		fi
	done
fi




if [[ $now_time_hour -eq 12 ]] ; then



	############排序iptables表###################
	#选出超过数量1从而作为网段处理
	iptables -nL | awk '/DROP/{print $4}' | awk -F"." '{print $1"."$2"."$3}' | sort | uniq -c | awk '{if($1>1) print $2}' > $SORT_D
	iptables -nL | awk '/DROP/{print $4}'  | \
	while read ip
	do
		ltmp=`echo $ip | awk -F"." '{print $1"."$2"."$3}'`
		if  cat $SORT_D | grep -q "$ltmp"
		then
			echo $ip > /dev/null
		else
			echo $ip >> $SORT_S
		fi
	done

	iptables -F

	if [[ -e $SORT_S ]] ; then
		cat $SORT_S | awk '{print($1)}' | \
		# 封锁判断为异常的ip
		while read ip
		do
			#iptables -A INPUT -s $ip -p tcp --dport $PORT -j REJECT --reject-with tcp-reset
			iptables -A INPUT -s $ip -p tcp -j DROP
		done
	fi


	# 封锁异常ip段

	if [[ -e $SORT_D ]]  ; then
		cat $SORT_D | awk '{print($1".0/24")}'  | \
		while read ip
		do
			#iptables -A INPUT -s $ip -p tcp --dport $PORT  -j REJECT --reject-with tcp-reset
			iptables -A INPUT -s $ip -p tcp -j DROP
		done
	fi




	rm -f $SORT_D
	rm -f $SORT_S

fi
