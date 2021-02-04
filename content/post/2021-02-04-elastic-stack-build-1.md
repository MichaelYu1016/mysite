---
title: "Elastic Stack搭建日志收集系统（一）"
author: "Yu Nongxin"
date: "2021-02-04"
categories: ["Programming"]
tags: ["Elastic Stack"]
slug: "elastic-stack-build-1"
thumbnailImagePosition: left
thumbnailImage: "banners/elastic.jpg"
---

Elastic Stack，前身是ELK(Elasticsearch、Logstash、Kibana)，在发展的过程中，又有新成员Beats的加入，所以就形成了Elastic Stack。
<!--more-->

## 1. 介绍

传统的ELK系统是有Elasticsearch、Logstash和Kibana组成，其中Logstash作为数据收集引擎，将日志数据收集加工后传输到Elasticsearch中存储，用户使用Kibana进行可视化操作。考虑到Logstash收集和解析日志对服务器cpu和内存的压力较大，因此Elastic官方推出了Beats系列的轻量级日志采集器，其中包括6种工具：

- Packetbeat：网络数据（收集网络流量数据）
- Metricbeat：指标（收集系统、进程和文件系统级别的CPU和内存使用情况等数据）
- Filebeat：日志文件（收集文件数据）
- Winlogbeat：windows事件日志（收集Windows事件日志数据）
- Auditbeat：审计数据（收集审计日志）
- Heartbeat：运行时间监控（收集系统运行时的数据）

在日志收集系统，官方推荐使用Filebeat读取本地日志文件，传输给Elasticsearch或者Logstash进行后续处理的方案，我们计划使用Filebeat作为每个服务器上的客户端进行日志文件的增量读取，Logstash作为服务端收集每个Filebeat传输的数据，并进行统一的解析处理，最后推送到Elasticsearch。Logstash、Elasticsearch和Kibana服务计划单独部署在一台服务器上。

## 2. 下载

打开[https://www.elastic.co/start](https://www.elastic.co/start)，选择6.8.13版本的Elasticsearch、Logstash、Kibana和Filebeat下载，由于部署在Linux系统，下载tar.gz格式的文件。也可以考虑使用yum等方式进行下载和安装，具体可以参考官方文档。

```bash
curl -O https://artifacts.elastic.co/downloads/elasticsearch/elasticsearch-6.8.13.tar.gz
curl -O https://artifacts.elastic.co/downloads/kibana/kibana-6.8.13-linux-x86_64.tar.gz
curl -O https://artifacts.elastic.co/downloads/logstash/logstash-6.8.13.tar.gz
curl -O https://artifacts.elastic.co/downloads/beats/filebeat/filebeat-6.8.13-linux-x86_64.tar.gz
```

## 3. 安装

### 安装Elasticsearch

上传`elasticsearch-6.8.13.tar.gz`文件到目标服务器，

```bash
tar -zxvf elasticsearch-6.8.13.tar.gz # 解压文件
mv elasticsearch-6.8.13 /data/elasticsearch # 移动到目标文件夹
```

ES启动的时候会占用特别大的资源所以需要修改下系统参数，

```bash
vim /etc/security/limits.d/20-nproc.conf
```

然后在文件末尾增加以下内容

```
*          soft    nofile    65536
*          hard    nofile    65536
```

再编辑`/etc/sysctl.conf`文件，也是在文件末尾增加以下内容

```bash
vm.max_map_count=262144
```

然后执行`sysctl -p`命令，退出当前shell登录窗口，重新登录后，上述配置生效。

使用专门的用户来启动es，

```bash
useradd es
chown -R es.es /data/elasticsearch
```

修改Elasticsearch的相关配置`vim /data/elasticsearch/config/elasticsearch.yml`，

```bash
node.name: elasticsearch
network.host: 0.0.0.0
http.port: 9200
trasport.tcp.port: 9300
http.cors.enabled: true
http.cors.allow-origin: "*"
```

保存后，就可以启动服务

```bash
su - es # 使用es用户进行启动
/data/elasticsearch/bin/elasticsearch -d
```

通过浏览器访问`http://ip:9200`，如果出现下面返回即说明Elasticsearch启动成功

```json
{
  "name" : "elasticsearch",
  "cluster_name" : "elasticsearch",
  "cluster_uuid" : "JJBR-VplSFeYRDh0I2du8g",
  "version" : {
    "number" : "6.8.13",
    "build_flavor" : "default",
    "build_type" : "tar",
    "build_hash" : "be13c69",
    "build_date" : "2020-10-16T09:09:46.555371Z",
    "build_snapshot" : false,
    "lucene_version" : "7.7.3",
    "minimum_wire_compatibility_version" : "5.6.0",
    "minimum_index_compatibility_version" : "5.0.0"
  },
  "tagline" : "You Know, for Search"
}
```

### 安装Kibana

上传`kibana-6.8.13-linux-x86_64.tar.gz`文件到目标服务器，解压

```bash
tar -zxvf kibana-6.8.13-linux-x86_64.tar.gz
mv kibana-6.8.13-linux-x86_64 /data/kibana
```

然后调整Kibana的相关配置，

```bash
cd /data/kibana/config
vim kibana.yml
```

修改`server.host: "0.0.0.0"`，使得能够监听到所有ip的访问请求。打开浏览器，访问`http://ip:5601`，有Kibana图标界面即安装成功

### 安装Logstash

上传`logstash-6.8.13.tar.gz`文件到目标服务器，解压

```bash
tar -zxvf logstash-6.8.13.tar.gz
mv logstash-6.8.13 /data/logstash
```

### 安装Filebeat

上传`filebeat-6.8.13-linux-x86_64.tar.gz`文件到目标服务器，解压

```bash
tar -zxvf filebeat-6.8.13-linux-x86_64.tar.gz
mv filebeat-6.8.13 /data/filebeat
```

至此，基于Elastic Stack的日志收集系统的相关组件都已经安装完成，接下来就是配置Filebeat和Logstash，使Filebeat能正确读取日志文件，通过Logstash正确解析后传输至Elasticsearch了。
