#!/bin/sh
export PATH=$PATH/bin:/sbin:/usr/local/tool:/usr/local/mysql/bin:/usr/bin:usr/local/bin:/usr/sbin:/usr/local/sbin
INTE="eth0"
LAN="eth3"
IP="192.168.0."
TC=/sbin/tc
IPS="100" 
IPE="149"
speed="64kbps"
start(){
	#echo 1 > /proc/sys/net/ipv4/ip_forward
	#/sbin/iptables -t nat -A POSTROUTING -s 192.168.0.0/24 -j MASQUERADE
	#iptables -A FORWARD -j DROP 
	##############################Qos rule on eth0##########################
	if [ "$INTE" != "" ];then
		$TC qdisc del dev $INTE root 2>/dev/null
		$TC qdisc add dev $INTE root handle 10: htb default 0
		$TC class add dev $INTE parent 10: classid 10:1 htb rate 256kbps ceil 375kbps
		x=10
		y=49
		z=01
		while [ $z -le $y ]
		do                                                                                                  
			$TC class add dev $INTE parent 10:1 classid 10:$x htb rate 5kbps ceil $speed prio 0 
			$TC qdisc add dev $INTE parent 10:$x handle 1$z: pfifo
			$TC filter add dev $INTE parent 10: protocol ip prio 100 handle $x fw classid 10:$x

			z=` expr $z + 1 `
			x=` expr $x + 10 `
		done
		$TC class add dev $INTE parent 10:1 classid 10:510 htb rate 5kbps ceil 64kbps prio 0 
		$TC class add dev $INTE parent 10:1 classid 10:520 htb rate 5kbps ceil 64kbps prio 0 
		$TC class add dev $INTE parent 10:1 classid 10:530 htb rate 5kbps ceil 64kbps prio 0 

		$TC qdisc add dev $INTE parent 10:510 handle 151: pfifo
		$TC qdisc add dev $INTE parent 10:520 handle 152: pfifo
		$TC qdisc add dev $INTE parent 10:530 handle 153: pfifo

		$TC filter add dev $INTE parent 10: protocol ip prio 100 handle 510 fw classid 10:510
		$TC filter add dev $INTE parent 10: protocol ip prio 100 handle 520 fw classid 10:520
		$TC filter add dev $INTE parent 10: protocol ip prio 100 handle 530 fw classid 10:530

		COUNTER=$IPS
		x=10
		while [ $COUNTER -le $IPE ]
		do

			iptables -t mangle -A POSTROUTING -s $IP$COUNTER -j MARK --set-mark $x
			COUNTER=` expr $COUNTER + 1 `
			x=` expr $x + 10 `
		done
		iptables -t mangle -A POSTROUTING -s 192.168.0.18 -j MARK --set-mark 610
		iptables -t mangle -A POSTROUTING -s 192.168.0.19 -j MARK --set-mark 620
		iptables -t mangle -A POSTROUTING -s 192.168.0.20 -j MARK --set-mark 630
		echo ""
		echo "Qos rule on eth0 124.126.87.8 start..............ok!"
		echo ""
	fi
	#############################Qos rule on eth3############################
	if [ "$LAN" != "" ];then
		$TC qdisc del dev $LAN root 2>/dev/null
		$TC qdisc add dev $LAN root handle 10: htb default 0
		$TC class add dev $LAN parent 10: classid 10:1 htb rate 256kbps ceil 375kbps
		x=10
		y=49
		z=01
		while [ $z -le $y ]
		do                                                                                                  
			$TC class add dev $LAN parent 10:1 classid 10:$x htb rate 5kbps ceil $speed prio 0 
			$TC qdisc add dev $LAN parent 10:$x handle 1$z: pfifo
			$TC filter add dev $LAN parent 10: protocol ip prio 100 handle $x fw classid 10:$x

			z=` expr $z + 1 `
			x=` expr $x + 10 `
		done
		$TC class add dev $LAN parent 10:1 classid 10:510 htb rate 5kbps ceil 64kbps prio 0 
		$TC class add dev $LAN parent 10:1 classid 10:520 htb rate 5kbps ceil 64kbps prio 0 
		$TC class add dev $LAN parent 10:1 classid 10:530 htb rate 5kbps ceil 64kbps prio 0 
		$TC qdisc add dev $LAN parent 10:510 handle 151: pfifo
		$TC qdisc add dev $LAN parent 10:520 handle 152: pfifo
		$TC qdisc add dev $LAN parent 10:530 handle 153: pfifo

		$TC filter add dev $LAN parent 10: protocol ip prio 100 handle 510 fw classid 10:510
		$TC filter add dev $LAN parent 10: protocol ip prio 100 handle 520 fw classid 10:520
		$TC filter add dev $LAN parent 10: protocol ip prio 100 handle 530 fw classid 10:530

		COUNTER=$IPS
		x=10
		while [ $COUNTER -le $IPE ]
		do

			iptables -t mangle -A POSTROUTING -d $IP$COUNTER -j MARK --set-mark $x
			COUNTER=` expr $COUNTER + 1 `
			x=` expr $x + 10 `
		done
		iptables -t mangle -A POSTROUTING -d 192.168.0.18 -j MARK --set-mark 610
		iptables -t mangle -A POSTROUTING -d 192.168.0.19 -j MARK --set-mark 620
		iptables -t mangle -A POSTROUTING -d 192.168.0.20 -j MARK --set-mark 630
		echo ""
		echo "Qos rule on eth3 192.168.0.1 start...............ok!"
		echo ""
	fi
}
stop(){
	if [ "$INTE" != "" ];then
		$TC qdisc del dev $INTE root
	fi
	if [ "$LAN" != "" ];then
		$TC qdisc del dev $LAN root
	fi
	iptables -F -t mangle
	iptables -X -t mangle
	iptables -Z -t mangle
}
status(){
	echo "show qdisc ............ "
	echo ""
	echo ""
	echo ""
	$TC -d -s qdisc
	echo ""
	echo ""
	echo "show filter ............ "
	echo ""
	echo ""
	if [ "$LAN" != "" ];then
		$TC -d -s filter ls dev $LAN
	fi
	echo ""
	echo ""
	if [ "$INTE" != "" ];then
		$TC -d -s filter ls dev $INTE
	fi
	echo ""
	echo ""
	echo "show class ............ "
	echo ""
	echo ""
	if [ "$LAN" != "" ];then
		$TC -d -s class ls dev $LAN
	fi
	echo ""
	echo ""
	if [ "$INTE" != "" ];then
		$TC -d -s class ls dev $INTE
	fi
	echo ""
	echo ""

}

case "$1" in
	start)
	start
	;;
	stop)
	stop
	;;
	restart)
	stop
	start
	;;
	status)
	status
	;;
	*)
	echo $"Usage:$0 {start|stop|restart|status}"
	exit 1
esac
exit 0
