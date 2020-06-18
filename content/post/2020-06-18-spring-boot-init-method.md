---
title: "Spring Boot启动时执行初始化方法"
author: "Yu Nongxin"
date: "2020-06-18"
categories: ["Programming"]
tags: ["Spring Boot","Java"]
slug: "spring-boot-init-method"
thumbnailImage: "banners/spring.jpg"
summary: "使用ApplicationRunner和CommandLineRunner实现Spring Boot启动时自动执行特定方法"
---

在平时的开发中经常会遇到这样的问题，在Spring Boot容器启动之后需要执行特定的方法或者类。

Spring Boot给我们提供了两种“开机启动”某些方法的方式：ApplicationRunner和CommandLineRunner。这两种方法提供的目的是为了满足，在项目启动的时候立刻执行某些方法。他们都是在SpringApplication执行之后开始执行的。

这两个接口中有一个run方法，我们只需要实现这个方法即可。这两个接口的不同之处在于：ApplicationRunner中run方法的参数为ApplicationArguments，而CommandLineRunner接口中run方法的参数为String数组。下面通过两个简单的例子，来看一下这两个接口的实现。

### 1. CommandLineRunner

```java
import org.springframework.boot.CommandLineRunner;
import org.springframework.stereotype.Component;

@Component
public class InitCommandLineRunner implements CommandLineRunner{
    @Override
    public void run(String... args) throws Exception{
        System.out.println("InitCommandLineRunner class will be execute when the project was started!");
        System.out.println("原始参数："+Arrays.asList(args));
    }
}
```

### 2. ApplicationRunner

```java
import org.springframework.boot.ApplicationArguments;
import org.springframework.boot.ApplicationRunner;
import org.springframework.stereotype.Component;

@Component
public class InitApplicationRunner implements ApplicationRunner {
    @Override
    public void run(ApplicationArguments args) throws Exception{
        System.out.println("InitApplicationRunner class will be execute when the project was started!");
        System.out.println("原始参数："+Arrays.asList(args.getSourceArgs()));
    }
}
```

这两种方法的使用基本是一样的，区别在于传入的参数，CommandLineRunner是原始的命令行字符串，而ApplicationRunner则是解析之后的参数。如果同时实现了多个启动加载类，可以通过`@Order`注解来指定执行顺序。
