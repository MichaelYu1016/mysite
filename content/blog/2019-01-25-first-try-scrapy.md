---
title: "使用Scrapy库爬取网页初体验"
author: "Yu Nongxin"
date: 2019-01-25
categories: ["Programming"]
tags: ["爬虫","Scrapy"]
banner: "banners/scrapy.jpg"
---

从前自己鼓捣爬虫的目的都是为了抓取一些数据，尤其是股票数据，因为没有涉及到太复杂的网页，所以自己简单写写就能把东西抓取下来。这次因为工作需要，开始了解一下常用的爬虫框架，首先就想到了Scrapy。说干就干，在使用`pip install scrapy`后，发现在安装Twisted库时报错了，这就有问题了，赶紧百度一下，发现大家的解决方法就是下载.whl文件安装，推荐上[UCI的python库](https://www.lfd.uci.edu/~gohlke/pythonlibs/)找合适的版本。本地安装好Twisted库后，在安装Scrapy库，一切就那么简单。在终端上输入
```
scrapy -h
```
出现下面结果就是安装成功了。
```
Scrapy 1.5.2 - no active project

Usage:
  scrapy <command> [options] [args]

Available commands:
  bench         Run quick benchmark test
  fetch         Fetch a URL using the Scrapy downloader
  genspider     Generate new spider using pre-defined templates
  runspider     Run a self-contained spider (without creating a project)
  settings      Get settings values
  shell         Interactive scraping console
  startproject  Create new project
  version       Print Scrapy version
  view          Open URL in browser, as seen by Scrapy

  [ more ]      More commands available when run from project directory

Use "scrapy <command> -h" to see more info about a command
```
之后就可以开始自己的第一个爬虫实验了。本次爬取的目标定在CSDN博客的首页。先建立一个scrapy项目。

1、创建工程
在终端进入到想创建项目的文件夹后输入
```
scrapy startproject blog
```
之后一个叫blog的scrapy项目文件夹就建立了。

2、创建爬虫
```
cd blog
scrapy genspider csdn csdn.net
```
这时候就会生成编写爬虫的python文件。

3、文件说明(来源网络)
- scrapy.cfg  项目的配置信息，主要为Scrapy命令行工具提供一个基础的配置信息。（真正爬虫相关的配置信息在settings.py文件中）
- items.py    设置数据存储模板，用于结构化数据，如：Django的Model
- pipelines    数据处理行为，如：一般结构化的数据持久化
- settings.py 配置文件，如：递归的层数、并发数，延迟下载等
- spiders      爬虫目录，如：创建文件，编写爬虫规则

**注意：一般创建爬虫文件时，以网站域名命名**

4、具体代码
CSDN的博客首页加载的文章目录是采用动态加载，所有直接访问首页网址是无法获取内容的，打开开发者模式，通过Network找到访问数据的api地址，`https://blog.csdn.net/api/articles?type=more&category=home&shown_offset=`等号后面跟着的是时间戳。然后我们在preview中看一下返回的json包的内容，挑选一下我们想要的，接下来就可以开始写代码了。

先来看一下我们需要存储的数据有哪些，在`items.py`文件中进行设置：
```python
import scrapy


class BlogItem(scrapy.Item):
    # define the fields for your item here like:
    # name = scrapy.Field()
    # 作品分类
    article_category = scrapy.Field()
    # 发布时间
    created_time = scrapy.Field()
    # 作者
    name = scrapy.Field()
    # 文章标题
    article_title = scrapy.Field()
    # 文章链接
    article_link = scrapy.Field()
```
接下来就是爬虫文件的编写了,在`csdn.py`文件中将要爬取的网址和内容进行设置，
```python
import scrapy
import time
import json
from blog.items import BlogItem

class CsdnSpider(scrapy.Spider):
    name = 'csdn'
    allowed_domains = ['https://blog.csdn.net']

    def start_requests(self):
        for i in range(0,50):
            url_time = int(time.time()*1000000)
            #通过开发者模式找到动态页面的api地址进行访问
            url = "https://blog.csdn.net/api/articles?type=more&category=home&shown_offset="+str(url_time)
            yield scrapy.Request(url,callback=self.parse)

    def parse(self, response):
        datas = json.loads(response.text)["articles"]
        for data in datas:
            item = BlogItem()
            item['article_category'] = data['category']
            item['created_time'] = data['created_at']
            item['name'] = data['nickname']
            item['article_title'] = data['title']
            item['article_link'] = data['url']
            yield item
```
因为只是为了测试一下爬虫的效果，所有我限制了一下爬取动态页面的数量。然后在配置文件`settings.py`中将默认数据传输设置取消注释即可
```python
ITEM_PIPELINES = {
   'blog.pipelines.BlogPipeline': 300,
}
```
最后是设置一下数据存储的脚本`pipelines.py`，我们将获取的数据保存成json文件，
```python
import json

class BlogPipeline(object):
    def __init__(self):
        #初始化文件
        self.file_name = open('CSDN博客首页文章.json','w',encoding='utf-8')

    def process_item(self, item, spider):
        #将item转换成字典，在转字符串，json.dumps转换中文默认ascii编码，需要设置一下
        text = json.dumps(dict(item),ensure_ascii=False)+'\n'
        self.file_name.write(text)
        return item

    def close_spider(self):
        self.file_name.close()
```
一切搞定之后，就可以在终端运行爬虫了，执行
```
cd blog
scrapy crawl csdn
```
运行之后得到的json文件如下：
```json
{"article_category": "其它", "created_time": "2天前", "name": "weixin_43932460", "article_title": "适合教孩子编码的 7 款免费编程语言", "article_link": "https://blog.csdn.net/weixin_43932460/article/details/86592099"}
{"article_category": "其它", "created_time": "12月05日", "name": "张高伟", "article_title": "mysql的sql_mode 模式修改 my.cnf", "article_link": "https://blog.csdn.net/qq_36663951/article/details/78720091"}
{"article_category": "其它", "created_time": "5天前", "name": "小姐的感觉", "article_title": "我要带徒弟学JAVA架构   写架构，非用架构", "article_link": "https://blog.csdn.net/fswhwd/article/details/86551852"}
{"article_category": "其它", "created_time": "01月04日", "name": "吴烜", "article_title": "编程语言试验之Antlr4+JavaScript实现&quot;圈4&quot;", "article_link": "https://blog.csdn.net/wuxuanecios/article/details/85748721"}
{"article_category": "其它", "created_time": "3天前", "name": "六年鹿苑", "article_title": "mybatis架构浅析", "article_link": "https://blog.csdn.net/weixin_41477980/article/details/86570603"}
{"article_category": "其它", "created_time": "01月01日", "name": "longji", "article_title": "微软基于F#的 Liqui|&gt; 量子编程语言", "article_link": "https://blog.csdn.net/longji/article/details/85530666"}
{"article_category": "其它", "created_time": "05月30日", "name": "冰 河", "article_title": "Hadoop之&mdash;&mdash; WARN util.NativeCodeLoader: Unable to load native-hadoop library for your platform...", "article_link": "https://blog.csdn.net/l1028386804/article/details/51538611"}
{"article_category": "其它", "created_time": "2天前", "name": "weixin_43932460", "article_title": "多种编程语言的优缺点梳理", "article_link": "https://blog.csdn.net/weixin_43932460/article/details/86575118"}
{"article_category": "其它", "created_time": "07月11日", "name": "意外金喜", "article_title": "nodejs模块nodemailer基本使用-邮件发送(支持附件)", "article_link": "https://blog.csdn.net/zzwwjjdj1/article/details/51878392"}
```
整体而言，使用scrapy爬取网页，先要对爬取的对象有一个比较清楚的认识，在去构建爬虫，本次尝试主要是基于网络上的一些前辈分享的经验进行的，后续的使用还需要再对整个框架和web知识进行深入一些的了解。
