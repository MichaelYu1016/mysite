---
title: "Elastic Stack搭建日志收集系统（二）"
author: "Yu Nongxin"
date: "2021-08-30"
categories: ["Programming"]
tags: ["Elastic Stack"]
slug: "elastic-stack-build-1"
thumbnailImagePosition: left
thumbnailImage: "banners/elastic.jpg"
---

上一篇介绍了Elastic Stack相关组件的下载和安装，本篇主要介绍一下相关组件的配置和使用。
<!--more-->

## 1. Logstash配置

Logstash 是服务器端数据处理管道，能够从多个来源采集数据，转换数据，然后将数据发送到“存储库”中。Logstatsh使用`{}`定义区域，`input`区域定义了输入流，可以选择不同的输入插件来采集数据，下面例子中的`beats`就是支持Elastic Beats的采集插件。`output`是输出区域，可以指定我们希望将数据发送到的地方，这里我们选择发送到elasticsearch中，同时还可以在插件中指定相关配置，如索引命名方式。中间部分是`filter`过滤器，可以使用插件对采集到的数据进行过滤、解析等操作。

```conf
input {
    beats {
        port => "5044"
    }
}
# The filter part of this file is commented out to indicate that it is
# optional.
#filter {
#    grok {
#        match => { "message" => "%{COMBINEDAPACHELOG}"}
#    }
#}
output {
    elasticsearch {
        hosts => [ "localhost:9200" ]
        index => "springboot-%{+yyyy-MM-dd}"
    }
}
```

完成配置文件的编写后，通过指定该配置文件进行启动Logstash，`nohup ./logstash -f /data/logstash/config/springboot.conf >/dev/null &`，这个时候就等待采集器收集到数据后发送给Logstash了。

## 2. Filebeat配置

找到`filebeat.yml`文件进行编辑，

```yml
#=========================== Filebeat inputs =============================

filebeat.inputs:

# Each - is an input. Most options can be set at the input level, so
# you can use different inputs for various configurations.
# Below are the input specific configurations.

- type: log

  # Change to true to enable this input configuration.
  enabled: true

  # Paths that should be crawled and fetched. Glob based paths.
  paths:
    - /data/springboot1/log/springboot.out
    - /data/springboot2/log/springboot.out
```

我们选择input的类型是log，`enabled`选项设置为`true`,`paths`指定filebeat去读取的日志文件路径，可以指定多个。这样filebeat启动之后就会去采集相应路径的日志文件。

在输出配置部分，我们选择将采集结果发生到Logstash，所以找到对应配置，放开注释，

```yml
#----------------------------- Logstash output --------------------------------
output.logstash:
  # The Logstash hosts
  hosts: ["192.168.1.100:5044"]

  # Optional SSL. By default is off.
  # List of root certificates for HTTPS server verifications
  #ssl.certificate_authorities: ["/etc/pki/root/ca.pem"]

  # Certificate for SSL client authentication
  #ssl.certificate: "/etc/pki/client/cert.pem"

  # Client Certificate Key
  #ssl.key: "/etc/pki/client/cert.key"
```

指定好Logstash的地址，这时候filebeat就可以正常工作了，但是对于我们希望读取的java日志，还需要做一些特别处理，来解决异常日志的采集，避免出现一个异常被切分成多条记录。我们找到多行选项，由于打印java日志格式都是以时间戳开头，所以我们匹配模式设定为识别时间戳格式，开启匹配否定，`multiline.match:after`指匹配到指定模式的行之后所有不符合匹配模式的行都被合并到一个事件。

```yml
### Multiline options

  # Multiline can be used for log messages spanning multiple lines. This is common
  # for Java Stack Traces or C-Line Continuation

  # The regexp Pattern that has to be matched. The example pattern matches all lines starting with [
  #multiline.pattern: ^\[
  multiline.pattern: ^[0-9]{4}-[0-9]{2}-[0-9]{2}
  # Defines if the pattern set under pattern should be negated or not. Default is false.
  #multiline.negate: false
  multiline.negate: true
  # Match can be set to "after" or "before". It is used to define if lines should be append to a pattern
  # that was (not) matched before or after or as long as a pattern is not matched based on negate.
  # Note: After is the equivalent to previous and before is the equivalent to to next in Logstash
  #multiline.match: after
  multiline.match: after
```

除了时间戳的匹配模式处理多行数据，针对java的异常日志，还可以使用空格匹配法，因为java的异常堆栈日志除了第一行外都是空格缩进输出，

```yml
multiline.pattern: '^[[:space:]]+(at|\.{3})\b|^Caused by:'
multiline.negate: false
multiline.match: after
```

这样就能将异常日志合并到单个事件进行输出了。

## 3. Kibana和Elasticsearch基本使用

### 3.1 简介

在Kibana中，Management可以对Kibana和Elasticsearch进行相关配置，比如es的生命周期等。在Monitor中可以监控es和kibana的状态，包括健康度、磁盘占用、内存使用情况等。在Dev Tools中可以便捷的通过http访问es接口，对ES进行相关操作。Discover则是我们查看ES日志的主要地方，可以通过可视化界面对相关日志进行搜索。其余Tab则是Kibana提供的可视化工具和机器学习工具，这里就不深入介绍了。

### 3.2 ES查询基本语法

在使用Discover进行查询的时候，有一些基本语法需要知道，

1. 直接输入关键字。hellokitty 会分词，匹配每个单词，中文按字分词。
2. 完全匹配关键字词，不采用分词，使用双引号，例如"spring"。
3. 冒号指定字段包含关键字。例如：message：springboot，代表message字段包含关键字springboot,这里springboot会分词。
4. 冒号指定某字段包含某关键字,例如：message:"springboot",此时不使用分词。
5. 通配符模糊匹配。`?`匹配单个字符：message:spring?，`*`匹配0-多个字符：message:spring*。
6. 运算符AND，OR，NOT必须大写。例如：message:spring AND message:java。
7. 范围查找数字，例如：response:[200 TO 300]。
8. 特殊字符需要使用''\"转义（+ – && || ! ( ) { } [ ] ^ ” ~ * ? : \）。
