---
title: "升级OpenSSL及OpenSSH"
author: "Yu Nongxin"
date: "2024-02-05"
categories: ["Programming"]
tags: ["CentOS","OpenSSH"]
slug: "update-openssl-and-openssh"
thumbnailImagePosition: left
thumbnailImage: "banners/ssh.jpg"
---

近期负责的项目服务器扫描发现了较多关于OpenSSH的漏洞，经过查询资料大部分漏洞均在后续版本修复，因此决定通过升级来解决。根据扫描漏洞的基本情况，选择安装最新的OpenSSH-9.6p1，由于高版本OpenSSH对OpenSSL的版本也有要求，就一并升级。
<!--more-->

## 基本情况

服务器操作系统为CentOS7.6，OpenSSH和OpenSSL都是操作系统自带，通过yum无法跨版本升级，因此只能通过安装包进行升级。

||当前版本|升级版本|
|:---:|:---:|:---:|
|OpenSSL|1.0.2k-fips|3.0.12|
|OpenSSH|7.4p1|9.6p1|

## 准备工作

首先更新一下开发环境

```bash
yum install -y gcc gcc-c++ perl perl-IPC-Cmd pam pam-devel
yum install -y pam* zlib*
```

上传OpenSSH-9.6p1.tar.gz和openssl-3.0.12.tar.gz到服务器。

## 升级OpenSSL

解压安装包并编译安装

```bash
tar -zxvf openssl-3.0.12.tar.gz

# 安装位置为/usr/local/openssl-3.0.12
cd openssl-3.0.12
./config --prefix=/usr/local/openssl-3.0.12

make && make install
```

安装完毕后配置OpenSSL

```bash
# 将原有openssl文件备份
mv /usr/bin/openssl /usr/bin/openssl.bak
mv /usr/include/openssl /usr/include/openssl.bak
# 删除原有软链接
unlink /usr/lib64/libssl.so
# 建立新软链接
ln -s /usr/local/openssl-3.0.12/bin/openssl /usr/bin/openssl
ln -s /usr/local/openssl-3.0.12/include/openssl /usr/include/openssl
ln -s /usr/local/openssl-3.0.12/lib64/libssl.so /usr/lib64/libssl.so
echo '/usr/local/openssl-3.0.12/lib64' >> /etc/ld.so.conf
ldconfig -v

ln -s /usr/local/openssl-3.0.12/lib64/libssl.so.3 /usr/lib64/libssl.so.3
ln -s /usr/local/openssl-3.0.12/lib64/libcrypto.so.3 /usr/lib64/libcrypto.so.3


openssl version -a
```

出现如下信息即安装成功

```bash
OpenSSL 3.0.12 24 Oct 2023 (Library: OpenSSL 3.0.12 24 Oct 2023)
built on: Sun Feb  4 06:42:26 2024 UTC
platform: linux-x86_64
options:  bn(64,64)
compiler: gcc -fPIC -pthread -m64 -Wa,--noexecstack -Wall -O3 -DOPENSSL_USE_NODELETE -DL_ENDIAN -DOPENSSL_PIC -DOPENSSL_BUILDING_OPENSSL -DNDEBUG
OPENSSLDIR: "/usr/local/openssl-3.0.12/ssl"
ENGINESDIR: "/usr/local/openssl-3.0.12/lib64/engines-3"
MODULESDIR: "/usr/local/openssl-3.0.12/lib64/ossl-modules"
Seeding source: os-specific
CPUINFO: OPENSSL_ia32cap=0xfffa32035f8bffff:0x800d19e4fbb
```

## 升级OpenSSH

首先更新一下开发环境

```bash
yum install -y gcc pam-devel libselinux-devel zlib-devel openssl-devel
```

然后备份原ssh配置

```bash
mv /etc/ssh /etc/ssh_bak
mv /etc/pam.d/sshd /etc/pam.d/sshd.bak
# 删除本地openssh安装包
rpm -e --nodeps `rpm -qa | grep openssh`
```

开始安装

```bash
tar -zxvf openssh-9.6p1.tar.gz
cd openssh-9.6p1
# 指定ssl版本
./configure --prefix=/usr/local/openssh-9.6p1 --sysconfdir=/etc/ssh --with-md5-passwords --with-pam --with-zlib --with-tcp-wrappers --with-ssl-dir=/usr/local/openssl-3.0.12 --without-hardening

make && make install
```

安装完毕后将相关命令文件拷贝到PATH路径下

```bash
cp -arf /usr/local/openssh-9.6p1/bin/scp /usr/bin/
cp -arf /usr/local/openssh-9.6p1/bin/sftp /usr/bin/
cp -arf /usr/local/openssh-9.6p1/bin/ssh /usr/bin/
cp -arf /usr/local/openssh-9.6p1/bin/ssh-add /usr/bin/
cp -arf /usr/local/openssh-9.6p1/bin/ssh-agent /usr/bin/
cp -arf /usr/local/openssh-9.6p1/bin/ssh-keygen /usr/bin/
cp -arf /usr/local/openssh-9.6p1/bin/ssh-keyscan /usr/bin/
cp -arf /usr/local/openssh-9.6p1/sbin/sshd /usr/sbin/sshd
cp -a contrib/redhat/sshd.init /etc/init.d/sshd
# 赋权（按需执行）
chmod u+x /etc/init.d/sshd
chown root.root /etc/init.d/sshd
```

接下来还原配置文件

```bash
mv /etc/pam.d/sshd.bak /etc/pam.d/sshd
cp /etc/ssh_bak/sshd_config /etc/ssh/sshd_config
cp /etc/ssh_bak/ssh_config /etc/ssh/ssh_config
```

根据需要修改sshd_config

```bash
# 注释掉GSSAPI认证（不然启动报错）
#GSSAPIAuthentication yes
#GSSAPICleanupCredentials no
# 替换
Subsystem sftp /usr/local/openssh-9.6p1/libexec/sftp-server
# 新增兼容HostKey算法（解决堡垒机无法连接问题）
HostKeyAlgorithms ssh-rsa,ssh-ed25519,rsa-sha2-256,rsa-sha2-512
PubkeyAcceptedKeyTypes ssh-rsa,ssh-ed25519,rsa-sha2-256,rsa-sha2-512
```

添加开机启动并重启sshd服务

```bash
chkconfig --add sshd
chkconfig sshd on
# 重启
systemctl restart sshd
```

显示如下信息即代表安装成功

```bash
[root@XXX ~]# ssh -V
OpenSSH_9.6p1, OpenSSL 3.0.12 24 Oct 2023
[root@XXX ~]# systemctl status sshd
● sshd.service - SYSV: OpenSSH server daemon
   Loaded: loaded (/etc/rc.d/init.d/sshd; bad; vendor preset: enabled)
   Active: active (running) since Sun 2024-02-04 14:55:58 CST; 1 day 7h ago
     Docs: man:systemd-sysv-generator(8)
  Process: 8097 ExecStart=/etc/rc.d/init.d/sshd start (code=exited, status=0/SUCCESS)
 Main PID: 8105 (sshd)
   CGroup: /system.slice/sshd.service
           └─8105 sshd: /usr/sbin/sshd [listener] 0 of 10-100 startups
```
