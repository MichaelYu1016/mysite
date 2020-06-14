---
title: "Spring国际化"
author: "Yu Nongxin"
date: "2020-06-14"
categories: ["Programming"]
tags: ["Spring","Java","i18n"]
slug: "spring-use-i18n"
thumbnailImage: "banners/spring.jpg"
summary: "使用MessageSource实现Spring的国际化"
---

Spring提供了MessageSource接口来提供消息处理功能，用来支持信息的国际化和包含参数的信息的替换。这次在一个项目中使用到了相关信息替换功能，进而扩展到对国际化的支持，发现Spring在对国际化的支持上非常友好，上手很简单，接下来简单介绍一下如何使用。

## 1. 基本设置

消息处理功能的配置可以使用配置文件和注解配置(`@Bean`)，这里我们在application.properties中设置message资源包的路径，

```properties
spring.messages.basename=i18n/message
```

如果有多个资源包，可以使用逗号做分隔符。然后我们在resources文件夹下创建一个新文件夹，命名为i18n。在i18n文件夹中创建资源文件message.properties，这个文件就是我们的消息资源的基类，在里面添加一些消息对应关系，

```properties
E-0000=success
E-0001=fail
E-0002=unknown
```

然后，创建再创建两个文件message_zh_CN.properties和message_en_US.properties。分别在两个文件中填入消息对应关系，

```properties
# message_zh_CN.properties
E-0000=成功
E-0001=失败
E-0002=未知信息

# message_en_US.properties
E-0000=success
E-0001=fail
```

在US的消息资源文件中我们没有给出`E-0002`的映射关系。

## 2. 基本使用方法

MessageSource的使用方法比较简单，直接使用`@Autowired`注入，然后调用`getMessage()`方法来获取相应的消息文本。

```java
String us = messageSource.getMessage("E-0000",null, Locale.US);
String cn = messageSource.getMessage("E-0000",null, Locale.CHINA);
log.info("US message is {}", us);
log.info("China message is {}", cn);
String unknown = messageSource.getMessage("E-0002",null,null);
log.info("不指定Locale时的信息: {}",unknown);
String unknown_us = messageSource.getMessage("E-0002",null,Locale.US);
log.info("指定Locale时的信息: {}",unknown_us);
String unknown_jp = messageSource.getMessage("E-0002",null,Locale.JAPAN);
log.info("指定Locale的资源文件不存在时的信息: {}",unknown_jp);

# US message is success
# China message is 成功
# 不指定Locale时的信息: 未知信息
# 指定Locale时的信息: unknown
# 指定Locale的资源文件不存在时的信息: 未知信息
```

从结果可以看到，`E-0000`消息在指定`Locale`为US时获取的文本时`success`，在指定CHINA时获取到的是`成功`，也就是说实现了同一个code的消息国际化。继续往下看，在不指定`Locale`的情况下获取`E-0002`对应消息，输出的是zh_CN文件中的文本，这是因为在没有指定`Locale`的情况下默认使用系统的`Locale`。当指定`Locale`为US时，由于我们没有在`message_en_US.properties`给出`E-0002`的相关属性，此时会在其基类`message.properties`中去寻找相应属性，所以输出的是`unknown`。而当我们指定了一个没有对应资源文件的`Locale`JAPAN时，会因为`fallbackToSystemLocale`默认为`true`而获取到系统`Locale`的对应资源，所以输出结果为`未知信息`。如果在配置时将`fallbackToSystemLocale`设置为`false`，则会从基类文件获取结果，此时应该输出`unknown`。

在使用MessageSource时还可以传入参数使得消息动态变化，在`message_zh_CN.properties`中我们增加一个属性，

```properties
E-0003=模块{0}发生{1}异常
```

然后进行测试，

```java
String[] params = {"message","读写"};
String dynamic = messageSource.getMessage("E-0003",params,Locale.CHINA);
log.info("获取动态信息:{}",dynamic);

# 获取动态信息:模块message发生读写异常
```

可以看到动态传入的参数在输出时填充到了预设好的文本中，通过`{0}`占位符和动态参数，能够提供更丰富的消息展示。

## 3. 小结

通过上面的代码及输出结果，我们可以将MessageSource的处理流程简单归纳一下：

1. 检测是否有对应`Locale`的配置，有的话则从当前`Locale`配置中读取信息，当相关属性不存在时可以追踪到基类文件中获取。
2. 若没有对应`Locale`的配置，如果`fallbackToSystemLocale`为`true`,则获取默认`defaultLocale`的配置。
3. 如果`fallbackToSystemLocale`为`false`，则直接到基类文件中获取信息。
