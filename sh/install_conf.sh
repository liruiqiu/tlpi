echo 20480 > /proc/sys/fs/file-max
echo fs.file-max = 65535 >> /etc/security/limits.conf
echo net.ipv4.tcp_fin_timeout = 30 >> /etc/security/limits.conf
echo net.ipv4.tcp_keepalive_time = 1800 >> /etc/security/limits.conf
echo net.ipv4.tcp_window_scaling = 0 >> /etc/security/limits.conf
echo net.ipv4.tcp_sack = 0 >> /etc/security/limits.conf
echo net.ipv4.tcp_timestamps = 0 >> /etc/security/limits.conf


echo HISTFILESIZE=2000 >> /etc/bashrc

echo HISTSIZE=2000 >> /etc/bashrc

echo HISTTIMEFORMAT="%Y%m%d-%H%M%S: " >> /etc/bashrc

echo export HISTTIMEFORMAT >> /etc/bashrc

echo ClientAliveInterval 300 >> /etc/ssh/sshd_config


echo JAVA_HOME=/usr/local/jdk >> /etc/profile
echo JRE_HOME=/usr/local/jdk/jre >> /etc/profile
echo CLASSPATH=.:$JAVA_HOME/lib/tools.jar >> /etc/profile
echo PATH=$JAVA_HOME/bin:$PATH >> /etc/profile
echo export JAVA_HOME CLASSPATH PATH JRE_HOME >> /etc/profile
echo export PATH=$PATH:/usr/local/mysql/bin >> /etc/profile
