---
title: "Docker初尝试"
author: "Yu Nongxin"
date: "2019-08-25"
categories: ["Programming"]
tags: ["Docker"]
thumbnailImagePosition: left
thumbnailImage: "banners/docker.jpg"
summary: "Docker的一些基本使用方法"
---

# 下载和安装

最近随着公司发展的需要，开始学习Java，特别是在开发Web项目使用的主流框架Spring。在学习过程中，使用到了一些应用，其中Docker是一个目前非常流行的用来辅助开发的程序。它能够快速的搭建起开发所需要的环境，并且不受主机环境的影响。作为第一次接触Docker的小白，首先自然是看过教程后再度娘上再搜索一番，注册一个Docker账号之后，下载Docker Desktop。

要在Windows上运行Docker，首先需要开启BIOS中的虚拟化，在打开Hype-V。然后按照Docker，启动。Docker启动之后，简单配置一下Docker的下载镜像源`https://9cok3rbr.mirror.aliyuncs.com`，配置一下挂载的硬盘，这些都能够在度娘上查到，就不一一赘述了。

# 使用

安装和配置好后，在PowerShell中输入```docker info```就能看到当前docker的信息。

接下来我们就需要下载在Docker中运行的应用的镜像。常用命令有
```bash
docker pull <image>
docker search <image>
```
根据需求，我下载mongodb的image
```bash
docker pull mongo
```
然后等待完成。下载完成后，我们开始启动一个docker容器运行mongo。
```bash
docker run --name mongo -p 27017:27017 -v mongo:/data/db -e MONGO_INITDB_ROOT_USERNAME=admin -e MONGO_INITDB_ROOT_PASSWORD=admin -d mongo
```
这里有一个小坑需要注意，在使用```-v```命令挂载文件时，mongo必须要使用```docker volume create <volumename>```建立一个卷，不能直接使用主机的文件路径，会报各种权限错误。
启动之后可以使用```docker ps```查看
```bash
CONTAINER ID        IMAGE               COMMAND                  CREATED             STATUS              PORTS                      NAMES
be2b3fbdd11d        mongo               "docker-entrypoint.s…"   6 hours ago         Up 6 hours          0.0.0.0:27017->27017/tcp   mongo
```
可以看到mongo容器已经运行了，我们通过命令行登录到MongoDB容器中去，
```
docker exec -it mongo bash
```
在使用Shell连接MongoDB
```shell
mongo -u admin -p admin
```
之后就能够操作MongoDB了。

# 命令
相对来说Docker的配置操作还是比较简单的，比较常用的命令都可以通过```-h```来获取，
```
容器相关
docker run [OPTION] IMAGE [COMMAND]
    -d  后台运行容器
    -e  设置环境变量
    --expose/-p 宿主端口:容器端口
    --name  指定容器名称
    --link  链接不同容器
    -v  宿主目录:容器目录，挂载磁盘卷
docker start/stop <容器名>
docker ps <容器名>
docker logs <容器名>
```
