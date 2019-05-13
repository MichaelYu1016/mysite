---
title: "Superset安装和部署"
author: "郁农欣"
date: 2019-04-16
categories: ["Python"]
tags: ["Superset"]
banner: "banners/superset.png"
---
最近公司需要开发一个报表系统,时间和人力都比较吃紧,最后选择了使用superset作为实现的工具.
### 安装superset注意事项
安装superset按照[官方文档](http://superset.apache.org/installation.html)进行操作,但是在具体操作过程中会遇到一些问题,本人在windows下的WSL中进行安装.
- python版本3.6
- superset版本0.28.1
- pandas要安装低版本0.23.4
- SQLAlchemy版本太高报错，安装1.2.18
- SQLAlchemy URI需要配置成MySQL

### 配置mysql
```bash
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install libmysqlclient-dev
pip3 install mysqlclient
```

### superset权限配置：
- 复制gamma权限: 
    - can_profile 去除个人页.
    - menu access保留dashboard,其他都可以删掉.
    - can_list dashboard保留,其他的dashboard操作可以删掉.
- 新建权限，授予访问表的权限
    - 保证一个dashboard中对应的sql lab生成的视图的权限都赋予了用户,不然会出现某个图表为空(特别是filter box图表).

### 前端页面调整
1. 修改首页，```__init__.py```中可修改myIndex的url,指向```/dashboard/list/```.但是有个bug需要调整，退出登录时会出现访问被拒绝的alert.
2. superset的大部分页面都是通过js生成的,所以要对页面进行改动需要对js文件进行修改.


### WSL开发环境调整
由于从Windows直接访问WSL的文件会存在各种权限问题,因此我选择将superset库挪到挂载到WSL的Windows盘中```/mnt/```,并在site-packages添加.pth文件,.pth文件中加入自己python库的路径```/mnt/XXX```,即可在Windows中使用IDE进行开发,在WSL中运行.