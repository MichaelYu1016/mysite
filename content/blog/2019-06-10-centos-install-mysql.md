---
title: "CentOS7安装MySQL5.7"
author: "郁农欣"
date: 2019-06-10
categories: ["Programming"]
tags: ["MySQL"]
banner: "banners/mysql.png"
---

### 1. 下载并安装MySQL
考虑在CentOS7安装MySQL5.7，先下载官方的与CentOS7对应的Yum Repository
```bash
wget -i -c http://dev.mysql.com/get/mysql57-community-release-el7-10.noarch.rpm
```
使用上面的命令就直接下载了安装用的Yum Repository，然后就可以直接yum安装了。
```bash
yum -y install mysql57-community-release-el7-10.noarch.rpm
```
之后就开始安装MySQL服务器。
```bash
yum -y install mysql-community-server
```
安装完成后就会覆盖掉CentOS的mariadb。

### 2. MySQL数据库设置
首先启动MySQL
```bash
systemctl start mysqld.service
```
查看MySQL运行状态：
```bash
systemctl status mysqld.service
```
此时MySQL已经开始正常运行，不过要想进入MySQL还得先找出此时root用户的密码，通过如下命令可以在日志文件中找出密码：
```bash
grep "password" /var/log/mysqld.log
```
输入如下命令进入数据库：
```bash
mysql -uroot -p
```
输入初始密码，此时不能做任何事情，因为MySQL默认必须修改密码之后才能操作数据库：
```sql
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new password';
```
但此时还有一个问题，就是因为安装了Yum Repository，以后每次yum操作都会自动更新，需要把这个卸载掉：
```bash
yum -y remove mysql57-community-release-el7-10.noarch
```
完成上述步骤后，我们对MySQL进行一些配置：
#### (1) 设置远程用户登录
默认只允许root帐户在本地登录，如果要在其它机器上连接mysql，必须修改root允许远程连接，或者添加一个允许远程连接的帐户，为了安全起见，我添加一个新的帐户：
```sql
mysql> GRANT ALL PRIVILEGES ON *.* TO 'username'@'%' IDENTIFIED BY 'password' WITH GRANT OPTION;
```
#### (2) 配置默认编码为utf8
修改/etc/my.cnf配置文件，在[mysqld]下添加编码配置，如下所示：
```
[mysqld]
character_set_server=utf8
init_connect='SET NAMES utf8'
```
如果需要把客户端也设置成utf8，则需要加上：
```
[client]
default-character-set=utf8
```
重启MySQL后生效，可使用下面命令进行查看：
```sql
mysql> show variables like '%character%';
```

#### 默认配置文件路径：
``` 
配置文件：/etc/my.cnf 
日志文件：/var/log//var/log/mysqld.log 
服务启动脚本：/usr/lib/systemd/system/mysqld.service 
socket文件：/var/run/mysqld/mysqld.pid
```