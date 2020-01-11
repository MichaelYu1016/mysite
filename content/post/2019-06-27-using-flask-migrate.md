---
title: "使用Flask-Migrate包进行数据库迁移"
author: "Yu Nongxin"
date: "2019-06-27"
categories: ["Programming"]
tags: ["python","Flask"]
thumbnailImagePosition: left
thumbnailImage: "banners/flask.jpg"
---

<!--more-->

## 前言
在使用开源项目进行二次开发的时候，大部分依赖库都由前辈们配置完成，不需要我们考虑太多。当自己开始从零写一个项目时，就会发现有很多东西需要去考虑。如果使用Django框架，大部分工具都集成好了，可以减少很多麻烦，但是会出现功能冗余的情况。而使用Flask则相对灵活，对应的就是需要自己配置需要的第三方库。本文主要介绍在Flask框架下使用Flask-Migrate进行数据库迁移。

## 安装和使用
数据库是搭建Web应用必不可少的一部分，我们在开发的过程中建立的数据模型是时刻在变化的，通常我们会使用SQLAlchemy来进行ORM操作，但是这个有一个缺陷就是建好表之后无法在保留数据的情况下对表结构进行修改。这时候我们就需要Flask-Migrate帮我们实现这一功能。
```bash
pip install flask-migrate
```
安装好之后，我们就可以使用了
```python
from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_migrate import Migrate

app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///app.db'

db = SQLAlchemy(app)
migrate = Migrate(app, db)

class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(128))
```
这个例子来自Flask-Migrate官方文档。一般情况下，我们只需要引入Migrate就可以了。
之后再命令行输入
```
flask db init
```
就能够建立一个应用的迁移文件夹```migrations```，之后我们需要生成迁移文件
```
flask db migrate
```
通过命令生成迁移脚本，特别注意：Flask-Migrate使用的Alembic不能检测到表名、列名等变化，这部分迁移命令需要我们手动添加到版本控制中。
```
flask db upgrade
```
在完成了迁移脚本的，使用upgrade命令来同步数据库。

## 命令一览
```
Usage: flask db [OPTIONS] COMMAND [ARGS]...

  Perform database migrations.

Options:
  --help  Show this message and exit.

Commands:
  branches   Show current branch points
  current    Display the current revision for each database.
  downgrade  Revert to a previous version
  edit       Edit a revision file
  heads      Show current available heads in the script directory
  history    List changeset scripts in chronological order.
  init       Creates a new migration repository.
  merge      Merge two revisions together, creating a new revision file
  migrate    Autogenerate a new revision file (Alias for 'revision...
  revision   Create a new revision file.
  show       Show the revision denoted by the given symbol.
  stamp      'stamp' the revision table with the given revision; don't run...
  upgrade    Upgrade to a later version
```
