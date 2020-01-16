---
title: "Superset安装和部署"
author: "Yu Nongxin"
date: 2019-05-13
categories: ["Programming"]
tags: ["Superset","python"]
thumbnailImagePosition: left
thumbnailImage: "banners/default2.jpg"
---
最近公司需要开发一个报表系统,时间和人力都比较吃紧,最后选择了使用superset作为实现的工具.
<!--more-->
## 安装superset注意事项
安装superset按照[官方文档](http://superset.apache.org/installation.html)进行操作,但是在具体操作过程中会遇到一些问题,本人在windows下的WSL中进行安装.
- python版本3.6
- superset版本0.28.1
- pandas要安装低版本0.23.4
- SQLAlchemy版本太高报错，安装1.2.18
- SQLAlchemy URI需要配置成MySQL

## 配置mysql
```bash
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install libmysqlclient-dev
pip3 install mysqlclient
```

## superset权限配置：
- 复制gamma权限: 
    - can_profile 去除个人页.
    - menu access保留dashboard,其他都可以删掉.
    - can_list dashboard保留,其他的dashboard操作可以删掉.
- 新建权限，授予访问表的权限
    - 保证一个dashboard中对应的sql lab生成的视图的权限都赋予了用户,不然会出现某个图表为空(特别是filter box图表).

## 前端页面调整
1. 修改首页，~~```__init__.py```中可修改myIndex的url,指向```/dashboard/list/```.但是有个bug需要调整，退出登录时会出现访问被拒绝的alert.~~ 我们将登录后的首页指向仪表盘列表,在原基础上新建一个html页面,命名为welcome,继承```basic.html```模板,删去js渲染部分,直接打开```/dashboard/list/```.Python的视图部分可以修改成:
```python
    return self.render_template(
        'superset/welcome.html',
        entry='welcome',
        title='ChinaLife',
        bootstrap_data=json.dumps(payload, default=utils.json_iso_dttm_ser),
    )
```
2. superset的大部分页面都是通过js生成的,所以要对页面进行改动需要对js文件进行修改.

## 增加User模型并更换数据库
在flask_appbuilder中将User模型增加一个user_group字段.
在```jinja_context.py```文件中增加一个方法,同时在context中增加传递参数:
```python
# 新导入一些类和方法
from flask_appbuilder.security.sqla import models as ab_models
from superset import app, db
def current_group():
    """test the jinja context"""
    user = (
        db.session.query(ab_models.User)
            .filter_by(username=g.user.username)
            .one()
    )

    return user.user_group


self.context = {
            'url_param': url_param,
            'current_user_id': current_user_id,
            'current_username': current_username,
            'filter_values': filter_values,
            'form_data': {},
            'current_group': current_group, # 新增加的传递参数
        }
```
superset默认使用的数据库是sqlite,我们将数据库替换成MySQL.
```python
# The SQLAlchemy connection string.
# SQLALCHEMY_DATABASE_URI = 'sqlite:///' + os.path.join(DATA_DIR, 'superset.db')
SQLALCHEMY_DATABASE_URI = 'mysql+mysqldb://root:XXXXXXXXXX@127.0.0.1:3306/superset?charset=utf8'
```
使用```superset db upgrade```初始化数据库.对于版本低于5.7的MySQL数据库,在初始化的时候会出现1071错误,索引的字节长度超过767字节,解决的方法可以修改索引列的字符串长度,还可以将MySQL的字符从```utf8mb4```改成```urf8```.


## WSL开发环境调整
由于从Windows直接访问WSL的文件会存在各种权限问题,因此可以将superset库挪到挂载到WSL的Windows盘中```/mnt/```,并在site-packages添加.pth文件,.pth文件中加入自己python库的路径```/mnt/XXX```,即可在Windows中使用IDE进行开发,在WSL中运行.