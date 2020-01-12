---
title: "Spring Boot项目中的单元测试"
author: "Yu Nongxin"
date: "2020-01-12"
categories: ["Programming"]
tags: ["Spring Boot","Java","Unit Test"]
slug: "spring-boot-unit-test"
---
单元测试是我们修改代码时能够正确实现需要功能的保证，在TDD开发中第一件事就是写好单元测试，在进行具体方法代码的编写，所以写好单元测试对开发者来说是一项非常重要的技能。
<!--more-->
在Spring Boot搭建的Web项目中，我们通常会将项目结构划分为Controller、Service和Dao层。一般来说Dao层直接访问数据库，单元测试相对简单，这里接不介绍了。Service层因为存放了业务逻辑，相对复杂，如何做到每个单元测试都能独立执行，就需要引入一些测试工具类。Controller层是访问层，这里主要需要模拟出Web访问，同时实现一些权限控制。

# Service单元测试

Service层的测试相对而言情况会复杂一些，可能会有较多的依赖项，这个时候如果要用真实数据测试无疑会有较多的问题，我们可以mock数据来使测试能够执行。

```java
@MockBean
private OrderMapper orderMapper;
@MockBean
private UserService userService;
```

这里使用@MockBean注解来解决依赖问题，需要注意的是该注解只能 mock 本地的代码——或者说是自己写的代码，对于储存在库中而且又是以 Bean 的形式装配到代码中的类无能为力。
@SpyBean 解决了 SpringBoot 的单元测试中 @MockBean 不能 mock 库中自动装配的 Bean 的局限，但是目前没使用过。

```java
@Test
public void getOrder() {
    Order testCase = new Order();
    testCase.setId(1);
    testCase.setCustomName("测试买家");
    testCase.setSalesChannels("有赞");
    testCase.setSellerName("测试销售");
    log.info("初始化测试用例");
    // 设置预期接口调用返回值
    when(userService.getFullName(1)).thenReturn("测试销售");
    when(orderMapper.selectByPrimaryKey(1)).thenReturn(testCase);
    Map result = orderService.getOrder(1);
    // 断言
    assertEquals(result.get("id"),"1");
    assertThat(result.get("id"),equalTo("1")); //推荐使用
    verify(orderMapper).selectByPrimaryKey(1);
    // 验证调用userService.getFullName方法2次
    verify(userService,times(2)).getFullName(1);
}
```

上面的程序中可以看到，orderService依赖orderMapper和userService，我们设置了这两个接口方法的预期返回值`when(methodCall).thenReturn(value)`，它能够不用调用真实方法就能够让单元测试执行，并返回我们设定的返回值。`verify(mock)`则用来验证行为是否发生，上面程序中我们验证了`orderMapper.selectByprimaryKey(1)`调用了1次，`userService.getFullName(1)`调用了2次，和我们的代码逻辑一致。通过mock来进行单元测试还有很多方法，需要慢慢学习。

# Controller单元测试

```java
@Test
public void getOrder() throws Exception {
    MvcResult mvcResult = mockMvc.perform(MockMvcRequestBuilders.get("/api/v1/order/1")
            .contentType(MediaType.APPLICATION_JSON_UTF8).header("Authorization",token))
            .andExpect(MockMvcResultMatchers.status().isOk())
            .andDo(MockMvcResultHandlers.print())
            .andReturn();

    logger.info(mvcResult.getResponse().getContentAsString());
}
```

1. `mockMvc.perform`执行一个请求
2. `MockMvcRequestBuilders.get("/api/v1/order/1")`构造一个请求，Post请求就用`.post`方法
3. `contentType(MediaType.APPLICATION_JSON_UTF8)`代表发送端发送的数据格式是`application/json;charset=UTF-8`
4. `accept(MediaType.APPLICATION_JSON_UTF8)`代表客户端希望接受的数据类型为`application/json;charset=UTF-8`
5. `ResultActions.andExpect`添加执行完成后的断言
6. `ResultActions.andExpect(MockMvcResultMatchers.status().isOk())`方法看请求的状态响应码是否为200如果不是则抛异常，测试不通过
7. `andExpect(MockMvcResultMatchers.jsonPath("$.XXX").value(""))`这里jsonPath用来获取XXX字段比对是否为value中的值,不是就测试不通过
8. `ResultActions.andDo`添加一个结果处理器，表示要对结果做点什么事情，比如此处使用`MockMvcResultHandlers.print()`输出整个响应结果信息

对于大部分Controller层来说，都会有一个登录权限的问题，我们可以创建一个基类，在测试前模拟登录，这里以JWT认证为例。

```java
@Autowired
SecurityManager securityManager;
@Autowired
UserService userService;
@Autowired
protected WebApplicationContext context;

protected String token = "Bearer ";
protected MockMvc mockMvc;

@Before
public void loginByMock() {
    //完成shiro认证
    MockHttpServletRequest mockHttpServletRequest = new MockHttpServletRequest(context.getServletContext());
    MockHttpServletResponse mockHttpServletResponse = new MockHttpServletResponse();

    SecurityUtils.setSecurityManager(securityManager);
    mockMvc = MockMvcBuilders.webAppContextSetup(context).build();
    Subject subject = new WebSubject
            .Builder(mockHttpServletRequest, mockHttpServletResponse)
            .buildWebSubject();
    User user = new User();
    user.setUsername("admin");
    user.setPassword("123456");
    User userForBase=userService.findByUsername(user.getUsername());
    AuthenticationToken auth = new JWTToken(TokenUtils.createToken(userForBase));
    subject.login(auth);
    ThreadContext.bind(subject);
    token = token + auth.getPrincipal().toString();
}
```

这里登录后权限控制由Shiro控制，可以使用类似Service单元测试中介绍的mock方法，模拟Shiro验证结构，也可以将用户和权限直接生成一些测试用例写入sql文件，在运行测试时写入H2数据库，个人感觉使用mock的话会更麻烦，不过考虑隔离性的话使用mock更好。

# 断言assertThat的使用

`assertThat` 使用了 Hamcrest 的 Matcher 匹配符，用户可以使用匹配符规定的匹配准则精确的指定一些想设定满足的条件,具有很强的易读性，而且使用起来更加灵活。

```java
字符相关匹配符
/**
 * equalTo匹配符断言被测的testedValue等于expectedValue，
 * equalTo可以断言数值之间，字符串之间和对象之间是否相等，相当于Object的equals方法
 */
assertThat(testedValue, equalTo(expectedValue));
/**
 * equalToIgnoringCase匹配符断言被测的字符串testedString
 * 在忽略大小写的情况下等于expectedString
 */
assertThat(testedString, equalToIgnoringCase(expectedString));
/**
 * equalToIgnoringWhiteSpace匹配符断言被测的字符串testedString
 * 在忽略头尾的任意个空格的情况下等于expectedString，
 * 注意：字符串中的空格不能被忽略
 */
assertThat(testedString, equalToIgnoringWhiteSpace(expectedString);
/**containsString匹配符断言被测的字符串testedString包含子字符串subString**/
assertThat(testedString, containsString(subString) );
/**endsWith匹配符断言被测的字符串testedString以子字符串suffix结尾*/
assertThat(testedString, endsWith(suffix));
/**startsWith匹配符断言被测的字符串testedString以子字符串prefix开始*/
assertThat(testedString, startsWith(prefix));
一般匹配符
/**nullValue()匹配符断言被测object的值为null*/
assertThat(object,nullValue());
/**notNullValue()匹配符断言被测object的值不为null*/
assertThat(object,notNullValue());
/**is匹配符断言被测的object等于后面给出匹配表达式*/
assertThat(testedString, is(equalTo(expectedValue)));
/**
 * is匹配符简写应用之一，is(equalTo(x))的简写，断言testedValue等于expectedValue
 */
assertThat(testedValue, is(expectedValue));
/**
 * is匹配符简写应用之二，is(instanceOf(SomeClass.class))的简写，
 * 断言testedObject为Cheddar的实例
 */
assertThat(testedObject, is(Cheddar.class));
/**not匹配符和is匹配符正好相反，断言被测的object不等于后面给出的object*/
assertThat(testedString, not(expectedString));
/**allOf匹配符断言符合所有条件，相当于“与”（&&）*/
assertThat(testedNumber, allOf( greaterThan(8), lessThan(16) ) );
/**anyOf匹配符断言符合条件之一，相当于“或”（||）*/
assertThat(testedNumber, anyOf( greaterThan(16), lessThan(8) ) );
数值相关匹配符
/**closeTo匹配符断言被测的浮点型数testedDouble在20.0¡À0.5范围之内*/
assertThat(testedDouble, closeTo( 20.0, 0.5 ));
/**greaterThan匹配符断言被测的数值testedNumber大于16.0*/
assertThat(testedNumber, greaterThan(16.0));
/** lessThan匹配符断言被测的数值testedNumber小于16.0*/
assertThat(testedNumber, lessThan (16.0));
/** greaterThanOrEqualTo匹配符断言被测的数值testedNumber大于等于16.0*/
assertThat(testedNumber, greaterThanOrEqualTo (16.0));
/** lessThanOrEqualTo匹配符断言被测的testedNumber小于等于16.0*/
assertThat(testedNumber, lessThanOrEqualTo (16.0));
集合相关匹配符
/**hasEntry匹配符断言被测的Map对象mapObject含有一个键值为"key"对应元素值为"value"的Entry项*/
assertThat(mapObject, hasEntry("key", "value" ) );
/**hasItem匹配符表明被测的迭代对象iterableObject含有元素element项则测试通过*/
assertThat(iterableObject, hasItem (element));
/** hasKey匹配符断言被测的Map对象mapObject含有键值“key”*/
assertThat(mapObject, hasKey ("key"));
/** hasValue匹配符断言被测的Map对象mapObject含有元素值value*/
assertThat(mapObject, hasValue(value));
```
