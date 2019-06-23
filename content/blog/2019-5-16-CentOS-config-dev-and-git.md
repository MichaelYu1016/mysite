---
title: "CentOS配置git仓库和开发环境"
author: "Yu Nongxin"
date: 2019-05-16
categories: ["Programming"]
tags: ["CentOS","git","python"]
banner: "banners/CentOS.jpg"
---
项目开始后,需要有一个统一的代码仓库,同时还要有一个开发服务器.从阿里云申请了一台服务器,将测试环境和git仓库都放在该服务器上.

## 建立用户组和用户
首先通过ssh连接到服务器,由于是通过root进入的,所以先建立一个dev用户和一个git用户,同时建立dev和git用户组.
```bash
$ groupadd dev
$ useradd -d /home/dev -g dev -m dev
$ groupadd git
$ useradd -d /home/dev -g git -m git
```
通过```passwd```和```passwd 用户名```来修改密码
在Linux下查看用户组和用户可以使用
```bash
$ cat /etc/passwd #查看用户的passwd
$ cat /etc/shadow #查看用户名
$ cat /etc/group #查看用户组
```
## 搭建git仓库

### 1. 安装Git
```bash
$ yum install curl-devel expat-devel gettext-devel openssl-devel zlib-devel perl-devel
$ yum install git
```
### 2. 创建证书登录
收集所有需要登录的用户的公钥，公钥位于id_rsa.pub文件中，如果不存在,可以使用```ssh-keygen```命令来生成,生成的公钥会在```~/.ssh/```文件,把我们的公钥导入到/home/git/.ssh/authorized_keys文件里，一行一个。
如果没有该文件创建它：
```bash
$ cd /home/git/
$ mkdir .ssh
$ chmod 755 .ssh
$ touch .ssh/authorized_keys
$ chmod 644 .ssh/authorized_keys
```
然后打开RSA认证,在CentOS7下,```RSAAuthentication```选项已经被弃用,所以在```/etc/ssh/sshd_config```文件中将下列选项设置好
```bash
PubkeyAuthentication yes     
AuthorizedKeysFile  .ssh/authorized_keys
```
### 3. 初始化Git仓库
首先我们选定一个目录作为Git仓库,因为我们是一个superset项目,所以设置为/home/gitrepo/superset.git，创建的命令为：
```bash
$ cd /home
$ mkdir gitrepo
$ chown git:git gitrepo/
$ cd gitrepo
$ git init --bare superset.git
$ chown -R git:git superset.git 
```

### 4.禁用git用户的shell登录
出于安全考虑，创建的git用户不允许登录shell，这可以通过编辑```/etc/passwd```文件完成。找到类似下面的一行：
```bash
git:x:1001:1001:,,,:/home/git:/bin/bash  
```
最后一个冒号后改为：
```bash
git:x:1001:1001:,,,:/home/git:/usr/bin/git-shell  
```
这样，git用户可以正常通过ssh使用git，但无法登录shell，因为我们为git用户指定的git-shell每次一登录就自动退出。

### 5.使用git
先将初始化的git库clone到本地
```bash
$ git clone git@host:/home/gitrepo/superset.git
```
然后就可以在本地进行开发,在推送到远程代码库了.
在本地代码库提交代码使用
```bash
git add
git commit
```
向远程代码库提交
```bash
$ git remote add origin git@host:/home/gitrepo/superset.git # 把本地库与远程库关联
$ git push -u origin master # 第一次推送时
$ git push origin master # 第一次推送后，直接使用该命令即可推送修改
```
至此我们就在服务器上建立好了远程git仓库,和一个开发测试环境,作为一个小团队,在开发测试环境部署是也可以直接从git仓库clone代码到dev用户下进行部署.

## 开发环境安装python3
阿里云服务器使用的CentOS,自带的python2.7,但是我们在开发中使用的是python3,所以配置一下开发环境.
首先安装一下依赖包
```bash
$ yum -y install zlib-devel bzip2-devel openssl-devel ncurses-devel sqlite-devel readline-devel tk-devel gdbm-devel db4-devel libpcap-devel xz-devel
```
从官网下载python3.6
```bash
curl -O https://www.python.org/ftp/python/3.6.8/Python-3.6.8.tgz
```
最后一步，编译安装Python3，默认的安装目录是 /usr/local 如果你要改成其他目录可以在编译(make)前使用 configure 命令后面追加参数 “–prefix=/alternative/path” 来完成修改。
```bash
$ tar xf Python-3.6.8.tgz
$ cd Python-3.5.0
$ ./configure
$ make
$ sudo make install
```
接下来设置python3为默认python,用户自定义的配置放入```/etc/profile.d/```目录中,使用```vim /etc/profile.d/python.sh```打开后写入
```bash
alias python='/usr/local/bin/python3.6'
```
重启会话使配置生效
```bash
$ source /etc/profile.d/python.sh
```
此时CentOS下的python3就安装完成了.