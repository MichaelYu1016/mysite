---
title: "Maven项目引入本地jar包"
author: "Yu Nongxin"
date: "2020-03-27"
categories: ["Programming"]
tags: ["Spring Boot","Java","Maven"]
slug: "import-local-jar-package"
thumbnailImage: "banners/maven.jpg"
---

通常我们的java项目都是使用Maven进行管理，需要引入的包直接在pom文件中引入，在maven中心库或者私库中导入。
<!--more-->

但是，有时候我们会需要使用第三方提供的SDK进行开发，这些SDK往往是一个独立的jar包，不能直接从maven库引入。最简单直接的方法，就是在IDEA的项目设置中直接添加jar包，就能够解决开发时的依赖问题。但是这样在对项目打包时，会提示找不到相关依赖。此时我们就需要对maven项目进行一些配置。首先将本地jar包文件添加到项目目录下，可以建立一个lib文件夹存放。

然后在pom.xml文件中进行配置，在`<build>`标签中添加

```xml
<resource>
    <directory>lib</directory>
    <targetPath>/BOOT-INF/lib/</targetPath>
    <includes>
        <include>**/*.jar</include>
    </includes>
</resource>
```

在`<dependencies>`标签中添加依赖项

```xml
<dependency>
    <groupId>org.local</groupId>
    <artifactId>local-sdk</artifactId>
    <version>1.0</version>
    <scope>system</scope>
    <systemPath>${project.basedir}/lib/third-sdk-java-1.0.0.jar</systemPath>
</dependency>
```

依赖项中`groupId`,`artifactId`,`version`都可以自己命名，`scope`一定要写system，`systemPath`写项目中jar包的位置，配置好之后，打包的时候就不会报错了。

当打包完成，在Linux上运行时，我又发现了一个新问题，就是pom中添加了resource后，spring boot的配置文件没有生效，端口设置和日志设置都没有起效，于是开始查找问题。

最后在度娘的支持下，发现是打包的时候没有添加资源中的配置文件，在pom文件中添加

```xml
<resource>
    <directory>src/main/java</directory>
    <includes>
        <include>**/*.*</include>
    </includes>
</resource>
<resource>
    <directory>src/main/resources</directory>
    <includes>
        <include>**/*.*</include>
    </includes>
</resource>
```

此时，再次打包项目后运行，能够发现配置生效了。
