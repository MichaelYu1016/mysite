---
title: "Spring Boot自定义配置Json序列化"
author: "Yu Nongxin"
date: "2020-01-18"
categories: ["Programming"]
tags: ["Spring Boot","Java","json"]
slug: "spring-boot-json-config"
thumbnailImage: "banners/json.jpg"
---

在写项目时，会有大量需要使用json格式传递数据的情况，而有时候会需要对json的格式做一些自定义，本文将简单介绍在Spring Boot中对json序列化和反序列化做自定义配置。
<!--more-->

# 准备工作

首先我们用Spring Boot创建一个简单的web应用，在`dao`包中创建一个`User`类，

```java
public class User implements Serializable {
    private static final long serialVersionUID = 6222176558369919436L;

    private String userName;
    private int age;
    private String password;
    private LocalDate birthday;
    private LocalDateTime createTime;
    private String mail;

    ... // getter and setter
}
```

在`controller`包中创建一个访问控制类`JsonController.java`,

```java
@RestController
public class JsonController {

    @GetMapping("/user")
    public User getUser() {
        User user = new User();
        user.setUserName("张三");
        user.setAge(26);
        user.setBirthday(LocalDate.now());
        user.setCreateTime(LocalDateTime.now());
        user.setMail("12345679887@qq.com");
        return user;
    }

    @PostMapping("/user")
    public Object addUser(@RequestBody User user) {
        System.out.println(user.toString());
        return null;
    }
}
```

创建完实体类和控制器，我们就可以开始配置自定义的json格式了，这里我们使用的Spring Boot集成的Jackson工具包来处理json。

# LocalDate类型的序列化及反序列化

过去，在java中对时间类型都是使用`Date`类，但是在java 8中新增了`LocalDate, LocalTime, LocalDateTime`用来表示日期和时间，相比较`Date`类，新的`LocalDate`是不可变的，所以线程安全，因此现在都推荐使用`LocalDate`来表示日期。但是，在序列化时如果不做特殊处理`LocalDate`返回的将是`"birthday":{...}`，`LocalDate`并不会序列化成我们希望的`2020-01-01`这种格式，这时候就需要对json序列化做一些修改了。首先需要确保你的依赖中有，

```
<dependency>
    <groupId>com.fasterxml.jackson.datatype</groupId>
    <artifactId>jackson-datatype-jsr310</artifactId>
</dependency>
```

然后在`config`包中创建一个`JsonConfig.java`类，

```java
@Configuration
public class JsonConfig {
    /**
     * Date格式化字符串
     */
    private static final String DATE_FORMAT = "yyyy-MM-dd";
    /**
     * DateTime格式化字符串
     */
    private static final String DATETIME_FORMAT = "yyyy-MM-dd HH:mm:ss";
    /**
     * Time格式化字符串
     */
    private static final String TIME_FORMAT = "HH:mm:ss";

    /**
     * 自定义Bean
     * 可以配置jackson映射到objectMapper的各种规则，特别是使用LocalDate，LocalDateTime，LocalTime后的序列化和反序列化。
     */
    @Bean
    @Primary
    public Jackson2ObjectMapperBuilderCustomizer jackson2ObjectMapperBuilderCustomizer() {
        return builder -> builder.serializerByType(LocalDateTime.class, new LocalDateTimeSerializer(DateTimeFormatter.ofPattern(DATETIME_FORMAT)))
                .serializerByType(LocalDate.class, new LocalDateSerializer(DateTimeFormatter.ofPattern(DATE_FORMAT)))
                .serializerByType(LocalTime.class, new LocalTimeSerializer(DateTimeFormatter.ofPattern(TIME_FORMAT)))
                .deserializerByType(LocalDateTime.class, new LocalDateTimeDeserializer(DateTimeFormatter.ofPattern(DATETIME_FORMAT)))
                .deserializerByType(LocalDate.class, new LocalDateDeserializer(DateTimeFormatter.ofPattern(DATE_FORMAT)))
                .deserializerByType(LocalTime.class, new LocalTimeDeserializer(DateTimeFormatter.ofPattern(TIME_FORMAT)));
    }
}
```

我们可以通过`Jackson2ObjectMapperBuilderCustomizer`来实现对json序列化的自定义，

```java
@FunctionalInterface
public interface Jackson2ObjectMapperBuilderCustomizer {
    void customize(Jackson2ObjectMapperBuilder jacksonObjectMapperBuilder);
}
```

查看源码可知，`Jackson2ObjectMapperBuilderCustomizer`是一个函数式接口，所以我们使用Lambda表达式来简化代码。通过设置`Jackson2ObjectMapperBuilder`来控制ObjectMapper在序列化和反序列化json时的操作。从上面的代码中可以看到，我们设置了`LocalDate, LocalTime, LocalDateTime`的序列化和反序列化方法，并指定了转换成日期的格式。这时候我们通过访问`http://localhost:8080/user`可以得到，

```json
{
    "userName": "张三",
    "password": null,
    "age": 26,
    "birthday": "2020-01-17",
    "createTime": "2020-01-17 23:17:39",
    "mail": "12345679887@qq.com"
}
```

# 自定义json格式

在上面返回的json中，`password`参数为`null`，这是因为在控制器中我们并没有设置`password`的值，如果我们不想在返回值中显示值为`null`的属性，可以在`JsonConfig`中做一些配置，

```java
/**
    * 自定义Bean
    * 可以配置jackson映射到objectMapper的各种规则，特别是使用LocalDate，LocalDateTime，LocalTime后的序列化和反序列化。
    */
@Bean
@Primary
public Jackson2ObjectMapperBuilderCustomizer jackson2ObjectMapperBuilderCustomizer() {
    return builder -> builder.serializerByType(LocalDateTime.class, new LocalDateTimeSerializer(DateTimeFormatter.ofPattern(DATETIME_FORMAT)))
            .serializerByType(LocalDate.class, new LocalDateSerializer(DateTimeFormatter.ofPattern(DATE_FORMAT)))
            .serializerByType(LocalTime.class, new LocalTimeSerializer(DateTimeFormatter.ofPattern(TIME_FORMAT)))
            .deserializerByType(LocalDateTime.class, new LocalDateTimeDeserializer(DateTimeFormatter.ofPattern(DATETIME_FORMAT)))
            .deserializerByType(LocalDate.class, new LocalDateDeserializer(DateTimeFormatter.ofPattern(DATE_FORMAT)))
            .deserializerByType(LocalTime.class, new LocalTimeDeserializer(DateTimeFormatter.ofPattern(TIME_FORMAT)))
            // 配置Date类序列化格式，也可以通过在application.properties中设置
            .dateFormat(new SimpleDateFormat(DATETIME_FORMAT))
            // 是否包括取值为null的字段
            .serializationInclusion(JsonInclude.Include.NON_NULL)
            // json序列化是对名称的处理策略（userName->user_name)
            .propertyNamingStrategy(PropertyNamingStrategy.SNAKE_CASE)
            // 反序列化设置多余字段处理
            .failOnUnknownProperties(false);
}
```

这里我们设置了`Date`类型序列化时的格式，返回值属性必须非`null`，属性名字的命名策略以及在反序列化时对未知字段的处理方式。这些配置都是jackson已经定义好了，我们可以自己选择的，除了在`Config`类中指定，还可以通过`application.properties`文件配置。

在上面的配置中，我选择了属性的命名策略为`SNAKE_CASE`，即将`userName`映射为`user_name`，在序列化时是`userName->user_name`，在反序列化时是`user_name->userName`。这时访问`http://localhost:8080/user`得到，

```json
{
    "user_name": "张三",
    "age": 26,
    "birthday": "2020-01-17",
    "create_time": "2020-01-17 23:18:22",
    "mail": "12345679887@qq.com"
}
```

通过`Config`类配置的是属于全局配置，如果只是某几个属性需要指定序列化的命名，则可以使用`@JsonProperty`来实现，

```java
public class User implements Serializable {
    private static final long serialVersionUID = 6222176558369919436L;
//    可以指定property也可以通过config类设置property命名策略
    @JsonProperty(value = "user_name")
    private String userName;
    private int age;
    private String password;
    private LocalDate birthday;
    private LocalDateTime createTime;
    private String mail;

    ... // getter and setter
}
```

此时如果不在配置类中设置`propertyNamingStrategy`，返回的结果会是：

```json
{
    "user_name": "张三",
    "age": 26,
    "birthday": "2020-01-17",
    "createTime": "2020-01-17 23:19:52",
    "mail": "12345679887@qq.com"
}
```

可以看到`userName`序列化后变成了`user_name`，而`createTime`并没有发生改变。如果设置了全局命名策略，但是有些对于有些字段需要更换命名方式，可以通过注解的形式指定，如果是整个实体类都改变，还可以通过`@JsonNaming`注解实现，

```java
@JsonNaming(PropertyNamingStrategy.SnakeCaseStrategy.class)
public class User implements Serializable {
    ...
}
```

Jackson包中可使用的命名策略有5种，
策略|示例
:--:|:--:
*LOWER_CAMEL_CASE(default)* | `userName`
*KEBAB_CASE* | `user-name`
*LOWER_CASE* | `username`
*SNAKE_CASE* | `user_name`
*UPPER_CAMEL_CASE* | `UserName`

在使用Jackson包进行json处理时，还可以通过注解来实现很多事情，了解更多可以戳[这里](https://www.baeldung.com/jackson)。