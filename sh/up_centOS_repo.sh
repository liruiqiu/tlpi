#! /bin/sh
#1. 进入/etc/yum.repos.d/ 备份老的配置文件 
#cd /etc/yum.repos.d/ &&  mkdir  -p back && mv *.repo back 
mv /etc/yum.repos.d/CentOS-Base.repo /etc/yum.repos.d/CentOS-Base.repo.backup
 
#2.下载配置 
wget -O /etc/yum.repos.d/CentOS-Base.repo http://mirrors.aliyun.com/repo/Centos-7.repo 
#curl -o /etc/yum.repos.d/CentOS-Base.repo http://mirrors.aliyun.com/repo/Centos-7.repo
#如果是阿里云服务器可以把域名替换成内网域名，这样访问源的时候不占有公网带宽 
#sed -i 's/aliyun.com/aliyuncs.com/'  /etc/yum.repos.d/CentOS-Base.repo 
 
#3. 更新索引 
yum clean all && yum makecache 
 
#4. 使用yum安装软件,以安装apache为例： 
 
#yum search httpd 
#yum install httpd -y 
 
