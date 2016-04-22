#! /bin/sh

# Source function library
. /etc/rc.d/init.d/functions

this_name=${0##*/}
app_dir=/opt/tomcatoperator
log_dir=$app_dir/logs
monitor_dir=$log_dir/monitor
bin_dir=$app_dir/bin
pid_file=$monitor_dir/monitor_boss_cpu.pid
sleep=3

start (){
[ ! -d $monitor_dir ] && {
	mkdir $monitor_dir
}

echo $$ > $pid_file

while (true)
do

	day=`date +"%Y-%m-%d"`
	log_file=$monitor_dir/boss_cpu_"$day".log
	[ ! -f $log_file ] && {
	echo  "TIME			USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME" >> $log_file
}
out=` ps axu|grep tomcatoperator|grep -v " grep "|awk -F"$JAVA_HOME" '{print $1}'`
echo "$(date +"%Y-%m-%d %H:%M:%S")     $out" >> $log_file
sleep $sleep

done
}

stop () {

	[ -f $pid_file ] && pid=$(head $pid_file) && [ ! -z "$pid" ]  && pid=$(ps aux|grep $pid|grep $this_name|awk '{print $2}') && echo $pid &&  [ ! -z "$pid" ]&& {
	/bin/kill -9 $pid
}

}

case "$1" in
	start)
	stop
	start
	;;
	stop)
	stop
	;;
	restart)
	stop
	start
	;;
	condrestart)
	if [ -f /var/lock/subsys/$prog ]; then
		stop
		start
	fi
	;;
	reload)
	reload
	;;
	status)
	status $lighttpd
	RETVAL=$?
	;;
	*)
	echo $"Usage: $0 {start|stop|restart|condrestart|reload|status}"
	RETVAL=1
esac

exit $RETVAL
