---
title: "使用Nacos作为注册中心"
author: "Yu Nongxin"
date: "2020-05-16"
categories: ["Programming"]
tags: ["Nacos","Java","Spring Cloud"]
slug: "use-nacos-discovery"
thumbnailImage: "banners/nacos.jpg"
---
通常spring cloud的项目都是使用Eureka作为注册中心，不过阿里开源的Nacos集配置中心和注册中心于一体，正好最近接触的项目有用到，正好学习一下怎么用。
<!--more-->

## 1. 引入依赖

我们使用Nacos的服务发现，使用Feign作为服务调用工具。Spring Cloud也听过了相应的工具支持，引入对应依赖，

```xml
<dependency>
    <groupId>com.alibaba.cloud</groupId>
    <artifactId>spring-cloud-starter-alibaba-nacos-discovery</artifactId>
</dependency>
<dependency>
    <groupId>org.springframework.cloud</groupId>
    <artifactId>spring-cloud-starter-openfeign</artifactId>
</dependency>
```

## 2. 服务消费者

我们新建一个nacos-consumer的模块，建立一个Feign声明的调用接口，

```java
@FeignClient(value = "nacos-provider", contextId = "userProvider")
public interface UserService {
    @GetMapping("/${application.version}/{id}")
    String getUserInfo(@PathVariable String id);
}
```

`nacos-provider`是服务名称，和服务提供者的name保持一致。然后我们在建立一个Controller，

```java
@RestController
public class TestController {

    @Autowired
    UserService userService;

    @GetMapping("/user/{id}")
    public String getUserInfo(@PathVariable String id) {
        return userService.getUserInfo(id);
    }
}
```

这里将`UserService`注入，调用`getUserInfo`接口。至此一个简单的消费者服务就完成了。

## 3. 服务提供者

首先建立nacos-provider模块，然后将nacos-consumer模块引入，因为我们需要使用UserService接口。在通常项目开发中，一般将服务调用接口单独分离出一个jar引入到消费者和提供者的服务中，这里只是进行一个演示。

```xml
<dependency>
    <groupId>com.example</groupId>
    <artifactId>nacos-consumer</artifactId>
    <version>0.0.1-SNAPSHOT</version>
    <scope>compile</scope>
</dependency>
```

引入依赖后，我们实现一个接口类，

```java
@RestController
public class UserController implements UserService {

    @Override
    public String getUserInfo(String id) {
        if ("123456".equals(id)) {
            return "Hello, Nacos";
        } else {
            return "Not Allowed";
        }
    }
}
```

由于我们在`UserService`接口中已经定义了url，所以在实现类中可以删去。这样一个简单的服务提供者就实现了

## 4. 项目配置

由于使用了Nacos进行服务发现，所以在两个模块中都需要增加Nacos相关配置

```
spring.cloud.nacos.discovery.server-addr=127.0.0.1:8848
spring.application.name=nacos-provider
#spring.application.name=nacos-consumer

application.version=1.0 #接口Url中的变量
```

配置完上述参数后，provider服务在启动的时候依旧会报错，提示有相同的Bean存在，此时增加一个配置

```
spring.main.allow-bean-definition-overriding=true
```

这个配置允许后生成的Bean覆盖前一个同名Bean。这时在启动nacos-consumer和nacos-provider服务，打开`127.0.0.1:8848/nacos`，进入服务列表，能够看到
![nacos服务](/images/nacos_demo.png)说明服务已经启动并在nacos成功注册。打开http客户端，发送一个请求，你可以发现消费者能够调用提供者的接口获取到返回数据。
