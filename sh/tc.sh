#!/bin/bash 
echo "1" > /proc/sys/net/ipv4/ip_forward 
iptables -P INPUT DROP 
iptables -P OUTPUT DROP 
iptables -P FORWARD DROP 
iptables -t nat -A POSTROUTING -s 192.168.1.0/255.255.255.0 -o eth0 -j SNAT --to-source 222.66.78.10

# start filters 
# TOS Minimum Delay (ssh, NOT scp) in 1:11: 
iptables -t mangle -A PREROUTING -m tos --tos Minimize-Delay -j MARK --set-mark 0x1 
iptables -t mangle -A PREROUTING -m tos --tos Minimize-Delay -j RETURN 

iptables -t mangle -A PREROUTING -m tos --tos Minimize-Cost -j MARK --set-mark 0x4 
iptables -t mangle -A PREROUTING -m tos --tos Minimize-Cost -j RETURN 

iptables -t mangle -A PREROUTING -m tos --tos Maximize-Throughput -j MARK --set-mark 0x5 
iptables -t mangle -A PREROUTING -m tos --tos Maximize-Throughput -j RETURN 

iptables -t mangle -A PREROUTING -p tcp -m tcp --sport 22 -j MARK --set-mark 0x1 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport 22 -j RETURN 

iptables -t mangle -I PREROUTING -p tcp -m tcp --tcp-flags SYN,RST,ACK SYN -j MARK --set-mark 0x1 
iptables -t mangle -I PREROUTING -p tcp -m tcp --tcp-flags SYN,RST,ACK SYN -j RETURN 

iptables -t mangle -A PREROUTING -p udp -m udp --dport 53 -j MARK --set-mark 0x1 
iptables -t mangle -A PREROUTING -p udp -m udp --dport 53 -j RETURN 

iptables -t mangle -A PREROUTING -p tcp -m tcp --dport ftp -j MARK --set-mark 0x1 
iptables -t mangle -A PREROUTING -p tcp -m tcp --dport ftp -j RETURN 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport ftp -j MARK --set-mark 0x1 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport ftp -j RETURN 

iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 80 -j MARK --set-mark 0x2 
iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 80 -j RETURN 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport 80 -j MARK --set-mark 0x2 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport 80 -j RETURN 

iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 443 -j MARK --set-mark 0x2 
iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 443 -j RETURN 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport 443 -j MARK --set-mark 0x2 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport 443 -j RETURN 

iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 8080 -j MARK --set-mark 0x2 
iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 8080 -j RETURN 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport 8080 -j MARK --set-mark 0x2 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport 8080 -j RETURN 


iptables -t mangle -A PREROUTING -p tcp -m tcp --dport ftp-data -j MARK --set-mark 0x3 
iptables -t mangle -A PREROUTING -p tcp -m tcp --dport ftp-data -j RETURN 

iptables -t mangle -A PREROUTING -p tcp -m tcp --sport ftp-data -j MARK --set-mark 0x3 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport ftp-data -j RETURN 

iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 25 -j MARK --set-mark 0x4 
iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 25 -j RETURN 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport 25 -j MARK --set-mark 0x4 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport 25 -j RETURN 

iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 110 -j MARK --set-mark 0x4 
iptables -t mangle -A PREROUTING -p tcp -m tcp --dport 110 -j RETURN 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport 110 -j MARK --set-mark 0x4 
iptables -t mangle -A PREROUTING -p tcp -m tcp --sport 110 -j RETURN 

iptables -t mangle -A PREROUTING -j MARK --set-mark 0x5 


iptables -t mangle -A OUTPUT -m tos --tos Minimize-Delay -j MARK --set-mark 0x1 
iptables -t mangle -A OUTPUT -m tos --tos Minimize-Delay -j RETURN 

iptables -t mangle -A OUTPUT -m tos --tos Minimize-Cost -j MARK --set-mark 0x4 
iptables -t mangle -A OUTPUT -m tos --tos Minimize-Cost -j RETURN 

iptables -t mangle -A OUTPUT -m tos --tos Maximize-Throughput -j MARK --set-mark 0x5 
iptables -t mangle -A OUTPUT -m tos --tos Maximize-Throughput -j RETURN 

iptables -t mangle -A OUTPUT -p tcp -m tcp --sport 29301 -j MARK --set-mark 0x1 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport 229301 -j RETURN 

iptables -t mangle -I OUTPUT -p tcp -m tcp --tcp-flags SYN,RST,ACK SYN -j MARK --set-mark 0x1 
iptables -t mangle -I OUTPUT -p tcp -m tcp --tcp-flags SYN,RST,ACK SYN -j RETURN 

iptables -t mangle -A OUTPUT -p udp -m udp --dport 53 -j MARK --set-mark 0x1 
iptables -t mangle -A OUTPUT -p udp -m udp --dport 53 -j RETURN 

iptables -t mangle -A OUTPUT -p tcp -m tcp --dport ftp -j MARK --set-mark 0x1 
iptables -t mangle -A OUTPUT -p tcp -m tcp --dport ftp -j RETURN 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport ftp -j MARK --set-mark 0x1 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport ftp -j RETURN 

iptables -t mangle -A OUTPUT -p tcp -m tcp --dport 80 -j MARK --set-mark 0x2 
iptables -t mangle -A OUTPUT -p tcp -m tcp --dport 80 -j RETURN 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport 80 -j MARK --set-mark 0x2 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport 80 -j RETURN 

iptables -t mangle -A OUTPUT -p tcp -m tcp --dport 443 -j MARK --set-mark 0x2 
iptables -t mangle -A OUTPUT -p tcp -m tcp --dport 443 -j RETURN 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport 443 -j MARK --set-mark 0x2 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport 443 -j RETURN 

iptables -t mangle -A OUTPUT -p tcp -m tcp --dport 8080 -j MARK --set-mark 0x2 
iptables -t mangle -A OUTPUT -p tcp -m tcp --dport 8080 -j RETURN 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport 8080 -j MARK --set-mark 0x2 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport 8080 -j RETURN 

iptables -t mangle -A OUTPUT -p tcp -m tcp --dport ftp-data -j MARK --set-mark 0x3 
iptables -t mangle -A OUTPUT -p tcp -m tcp --dport ftp-data -j RETURN 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport ftp-data -j MARK --set-mark 0x3 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport ftp-data -j RETURN 

iptables -t mangle -A OUTPUT -p tcp -m tcp --dport 25 -j MARK --set-mark 0x4 
iptables -t mangle -A OUTPUT -p tcp -m tcp --dport 25 -j RETURN 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport 25 -j MARK --set-mark 0x4 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport 25 -j RETURN 
iptables -t mangle -A OUTPUT -p tcp -m tcp --dport 110 -j MARK --set-mark 0x4 
iptables -t mangle -A OUTPUT -p tcp -m tcp --dport 110 -j RETURN 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport 110 -j MARK --set-mark 0x4 
iptables -t mangle -A OUTPUT -p tcp -m tcp --sport 110 -j RETURN 
iptables -t mangle -A OUTPUT -j MARK --set-mark 0x3 


iptables -A INPUT -p ALL -i eth1 -s 192.168.1.0/24 -j ACCEPT 
iptables -A INPUT -p ALL -i lo -j ACCEPT 
iptables -A INPUT -p ALL -d 222.66.78.10 -m state --state ESTABLISHED,RELATED -j ACCEPT 

iptables -A FORWARD -i eth1 -j ACCEPT 
iptables -A FORWARD -m state --state ESTABLISHED,RELATED -j ACCEPT 

iptables -A OUTPUT -p ALL -s 127.0.0.1 -j ACCEPT 
iptables -A OUTPUT -p ALL -s 192.168.1.1 -j ACCEPT 
iptables -A OUTPUT -p ALL -s 222.66.78.10 -j ACCEPT 

#iptables -t nat -A PREROUTING -i eth1 -p tcp --dport 80 -j REDIRECT --to-ports 3128 


#Set the following values to somewhat lesss than your actual download and uplink speed. 
DOWNLINK=10000
UPLINK=10000

#clean existing down and uplink qdiscs,put the errors to /var/log/htb_log 
tc qdisc del dev eth0 root 2> /dev/null > /dev/null 
tc qdisc del dev eth0 ingress 2> /dev/null > /dev/null 
tc qdisc del dev eth1 root 2> /dev/null > /dev/null 

########################################uplink########################################### 

#install root HTB,point default traffic to 1:15: 
tc qdisc add dev eth0 root handle 1: htb default 15 r2q 1 
#shape everythin at $UPLINK speed -this prevents huge queues in your DSL modem which destroy 

latency: 
# main class 
tc class add dev eth0 parent 1: classid 1:1 htb rate ${UPLINK}kbit ceil ${UPLINK}kbit 
#high prio class 1:11: 
tc class add dev eth0 parent 1:1 classid 1:11 htb rate 128kbit ceil 128kbit prio 0 
tc class add dev eth0 parent 1:1 classid 1:12 htb rate 128kbit ceil ${UPLINK}kbit prio 2 
tc class add dev eth0 parent 1:1 classid 1:13 htb rate 32kbit ceil ${UPLINK}kbit prio 1 
tc class add dev eth0 parent 1:1 classid 1:14 htb rate 32kbit ceil ${UPLINK}kbit prio 1 
#bulk & default class 1:15 -gets slightly less traffic,and a lower priority: 
tc class add dev eth0 parent 1:1 classid 1:15 htb rate 16kbit ceil ${UPLINK}kbit prio 3 

#bost get Stochastic Fairness: 
tc qdisc add dev eth0 parent 1:12 handle 12: sfq 
tc qdisc add dev eth0 parent 1:13 handle 13: sfq 
tc qdisc add dev eth0 parent 1:14 handle 14: sfq 
tc qdisc add dev eth0 parent 1:15 handle 15: sfq 

#TOS Mininum Delay (ssh,telnet) in 1:11: 
tc filter add dev eth0 parent 1:0 protocol ip prio 1 handle 1 fw classid 1:11 
tc filter add dev eth0 parent 1:0 protocol ip prio 2 handle 2 fw classid 1:12 
tc filter add dev eth0 parent 1:0 protocol ip prio 3 handle 3 fw classid 1:13 
tc filter add dev eth0 parent 1:0 protocol ip prio 4 handle 4 fw classid 1:14 
tc filter add dev eth0 parent 1:0 protocol ip prio 5 handle 5 fw classid 1:15 

# install the ingress qdisc on the ingress 
tc qdisc add dev eth0 handle ffff: ingress 
# DROP everything that's coming in too fast: 
tc filter add dev eth0 parent ffff: protocol ip prio 50 u32 match ip src 0.0.0.0/0 police rate ${DOWNLINK}kbit burst 15k drop flowid :1 


tc qdisc add dev eth1 root handle 2: htb 
tc class add dev eth1 parent 2: classid 2:1 htb rate 512kbit 
tc class add dev eth1 parent 2:1 classid 2:20 htb rate 128kbit ceil 384kbit burst 15k 
tc qdisc add dev eth1 parent 2:20 handle 20: sfq 
tc filter add dev eth2 parent 2:0 protocol ip prio 4 u32 match ip dst 192.168.1.0/24 flowid 2:20


