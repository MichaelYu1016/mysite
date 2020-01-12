---
title: "Json序列化和日志处理"
author: "Yu Nongxin"
date: "2019-07-06"
categories: ["Programming"]
tags: ["python","log","json"]
thumbnailImage: "banners/default.jpg"
---

最近在研究Flask项目时碰到了一些问题，一个是Decimal类型不能Json序列化，一个是如何控制日志的输出。
<!--more-->

## Json序列化

在MySQL中一般我们对数值有精度要求的话，都会设置成Decimal类型，而SQLACHEMY取出Decimal对象时Flask自带的json解码函数不能将其进行序列化，导致我在使用API读取数据时报错。问题其实很明确，如何快速解决是关键。从度娘上看了不少解决方法，从中选择了一种对自己代码改动较小的。核心思想是将Flask自带的json解码方法重写。

```python
from flask import Flask as _Flask
from flask.json import JSONEncoder as _JSONEncoder


class JSONEncoder(_JSONEncoder):
    def default(self, o):
        import decimal
        if isinstance(o, decimal.Decimal):
            return float(o)

        super(JSONEncoder, self).default(o)


class Flask(_Flask):
    json_encoder = JSONEncoder
```

在引入Flask类和JSONEncoder类时重命名，将我们继承的新类命名为原来的类名，这样之前写的代码就不需要进行修改了。重写的json解码方法是判断一下实例的类型，如果是Decimal类则转换成float类型。然后将新的解码方法替换Flask类中原有的方法即可。

## 日志处理

Python项目中的日志处理大部分都是通过logging模块来实现的。在Flask项目中可以把一下常量放在config中，例如日志的输出格式。

```python
# LOG_FORMAT = "%(asctime)s:%(levelname)s:%(name)s:%(message)s"
logging.basicConfig(format=app.config['LOG_FORMAT'])
    logging.getLogger().setLevel(logging.INFO)
```

通常在开发过程中使用上述配置就能够在控制台输出日志，日志等级可以通过```setLevel```来控制。但是如果在部署到生产环境，那么就需要写入到文件中。目前logging模块提供了多种输出方式，直接写入固定文件，或者使用handle根据日志文件大小或者时间将日志切割成多个文件。通常情况下日志文件以时间进行切割比较常见。

```python
if app.config['LOG_ENV_PROD']:
    logging.basicConfig(format=app.config['LOG_FORMAT'])
    logging.getLogger().setLevel(logging.WARNING)
    format = '%(asctime)s %(levelname)s %(module)s.%(funcName)s Line:%(lineno)d:%(message)s'
    fmt = logging.Formatter(format)
    handler = handlers.TimedRotatingFileHandler(
        app.config['FILENAME'],
        when=app.config['ROLLOVER'],
        interval=app.config['INTERVAL'],
        backupCount=app.config['BACKUP_COUNT'],
        encoding='utf-8')
    handler.setFormatter(fmt)
    logging.getLogger().addHandler(handler)
```

通过添加一个控制器handler我们可以将日志输出到指定路径下的文件中。这里为了减少部署时代码的改动量，我们使用一个变量`LOG_ENV_PROD`来判断是否启用该 log 配置。`TimeRotatingFileHandler`函数中需要设置文件路径，`when`设置基本时间单位，`interval`设置切割的时间区间，`backupCount`设置最大保留的日志文件数量，`encoding`设置日志文件的编码。`handler.setFormatter()`函数设置写入日志文件的日志格式。针对日志的处理还有很多内容，具体可以参考[logging](https://docs.python.org/3/library/logging.html).