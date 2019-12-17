---
title: "Spring Boot配置多数据源"
author: "Yu Nongxin"
date: "2019-12-17"
categories: ["Programming"]
tags: ["Java", "Spring Boot"]
banner: "banners/mysql.png"
---
通常使用spring boot创建一个项目都是使用单一数据源，但是有些时候我们会需要配置多个数据源来满足不同的需求。目前针对自定义查询较多的情况，一般会选择使用Mybatis作为数据库操作框架。本文以Mybatis为例介绍如何在Spring boot中配置多个数据源。

## 准备工作
在application.properties文件中配置好数据源的基本信息
```
spring.datasource.test1.jdbc-url=jdbc:mysql://127.0.0.1:3306/test1?useUnicode=true&characterEncoding=UTF-8&useJDBCCompliantTimezoneShift=true&useLegacyDatetimeCode=false&serverTimezone=Hongkong&allowMultiQueries=true
spring.datasource.test1.username=root
spring.datasource.test1.password=root
spring.datasource.test2.jdbc-url=jdbc:mysql://127.0.0.1:3306/test2?useUnicode=true&characterEncoding=UTF-8&useJDBCCompliantTimezoneShift=true&useLegacyDatetimeCode=false&serverTimezone=Hongkong&allowMultiQueries=true
spring.datasource.test2.username=root
spring.datasource.test2.password=root
```
特别需要注意的是数据库的uri，在单数据源情况下我们写成`datasource.url`，但是在多数据源情况下一定要写成`datasource.test1.jdbc-url`，不然会报错。

同时在pom.xml中配置依赖，这里我的数据源都是MySQL所以加上MySQL的依赖和Mybatis依赖。
```
<dependency>
    <groupId>mysql</groupId>
    <artifactId>mysql-connector-java</artifactId>
    <scope>runtime</scope>
    <version>8.0.17</version>
</dependency>
<dependency>
    <groupId>org.mybatis.generator</groupId>
    <artifactId>mybatis-generator-core</artifactId>
    <version>1.3.7</version>
</dependency>
```

## Spring配置

完成了准备工作，我们需要在代码中添加数据源的配置。先配置test1数据源
```java
import org.apache.ibatis.session.SqlSessionFactory;
import org.mybatis.spring.SqlSessionFactoryBean;
import org.mybatis.spring.SqlSessionTemplate;
import org.mybatis.spring.annotation.MapperScan;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.boot.jdbc.DataSourceBuilder;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Primary;
import org.springframework.core.io.support.PathMatchingResourcePatternResolver;

import javax.sql.DataSource;

/**
 * 配置test1数据源
 */
@Configuration
@MapperScan(basePackages = "com.example.mapper.test1", sqlSessionFactoryRef = "test1SqlSessionFactory")
public class DataSource1Config {
    /**
     * Bean 将这个对象放入Spring容器中
     * Primary 表示这个数据源是默认数据源(虽然去掉好像也不会影响什么~)
     * 读取application.properties中的配置参数映射成为一个对象
     * prefix表示参数的前缀
     */
    @Bean(name = "test1DataSource")
    @Primary
    @ConfigurationProperties(prefix = "spring.datasource.test1")
    public DataSource getTest1DataSource() {
        return DataSourceBuilder.create().build();
    }

    /**
     * Qualifier表示查找Spring容器中名字为test1DataSource的对象
     */
    @Bean(name = "test1SqlSessionFactory")
    @Primary
    public SqlSessionFactory test1SqlSessionFactory(@Qualifier("test1DataSource") DataSource datasource)
            throws Exception {
        SqlSessionFactoryBean bean = new SqlSessionFactoryBean();
        bean.setDataSource(datasource);
        bean.setMapperLocations(
                // 设置mybatis的xml所在位置
                new PathMatchingResourcePatternResolver().getResources("classpath*:mapper/test1/*.xml"));
        return bean.getObject();
    }

    @Bean("test1SqlSessionTemplate")
    @Primary
    public SqlSessionTemplate test1SqlSessionTemplate(
            @Qualifier("test1SqlSessionFactory") SqlSessionFactory sessionFactory) {
        return new SqlSessionTemplate(sessionFactory);
    }
}
```
这里配置中我们可以看到我配置的数据源test1扫描的包是`com.example.mapper.test1`,xml文件的资源路径是`classpath*:mapper/test1/*.xml`，都是在mapper文件下单独用test1文件夹管理，之后的数据源也会用同样方式处理。

接下来我们配置数据源test2：
```java
import org.apache.ibatis.session.SqlSessionFactory;
import org.mybatis.spring.SqlSessionFactoryBean;
import org.mybatis.spring.SqlSessionTemplate;
import org.mybatis.spring.annotation.MapperScan;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.boot.jdbc.DataSourceBuilder;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.core.io.support.PathMatchingResourcePatternResolver;

import javax.sql.DataSource;

/**
 * 配置test2数据源
 */
@Configuration
@MapperScan(basePackages = "com.example.mapper.test2", sqlSessionFactoryRef = "test2SqlSessionFactory")
public class DataSource1Config {
    /**
     * Bean 将这个对象放入Spring容器中
     * 读取application.properties中的配置参数映射成为一个对象
     * prefix表示参数的前缀
     */
    @Bean(name = "test2DataSource")
    @ConfigurationProperties(prefix = "spring.datasource.test2")
    public DataSource getTest2DataSource() {
        return DataSourceBuilder.create().build();
    }

    /**
     * Qualifier表示查找Spring容器中名字为test2DataSource的对象
     */
    @Bean(name = "test2SqlSessionFactory")
    public SqlSessionFactory test2SqlSessionFactory(@Qualifier("test2DataSource") DataSource datasource)
            throws Exception {
        SqlSessionFactoryBean bean = new SqlSessionFactoryBean();
        bean.setDataSource(datasource);
        bean.setMapperLocations(
                // 设置mybatis的xml所在位置
                new PathMatchingResourcePatternResolver().getResources("classpath*:mapper/test2/*.xml"));
        return bean.getObject();
    }

    @Bean("test2SqlSessionTemplate")
    public SqlSessionTemplate test2SqlSessionTemplate(
            @Qualifier("test2SqlSessionFactory") SqlSessionFactory sessionFactory) {
        return new SqlSessionTemplate(sessionFactory);
    }
}
```
完成这些之后就可以写Mapper文件和xml文件并放入对应的文件夹中。

在运行前我们还需要关闭Spring自动配置数据源
```java
@SpringBootApplication(exclude = {DataSourceAutoConfiguration.class})
```
在启动函数上排除掉自动配置数据源就可以，测试结果这里就省略了。