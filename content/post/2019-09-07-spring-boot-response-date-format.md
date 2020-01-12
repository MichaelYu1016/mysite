---
title: "Spring Boot配置返回json日期格式失效"
author: "Yu Nongxin"
date: "2019-09-07"
categories: ["Programming"]
tags: ["Java", "Spring Boot"]
thumbnailImage: "banners/spring.jpg"
---

最近在使用Spring Boot重写一个系统，因为是前后端分离，所以后端都通过Restful接口对外提供服务，
在完成了基本的设计后，通过配置一个拦截器，在Response Body写入前统一返回体的格式。
<!--more-->

```java
@EnableWebMvc
@Configuration
public class ResponseConfig {

    @RestControllerAdvice
    static class CommonResultResponseAdvice implements ResponseBodyAdvice<Object> {
        @Override
        public boolean supports(MethodParameter methodParameter, Class<? extends HttpMessageConverter<?>> aClass) {
            return true;
        }

        @Override
        public Object beforeBodyWrite(Object body, MethodParameter methodParameter, MediaType mediaType, Class<? extends HttpMessageConverter<?>> aClass, ServerHttpRequest serverHttpRequest, ServerHttpResponse serverHttpResponse) { ... }
```
最初的实现是通过```@EnableWebMvc```注解来实现WebMvc的一些配置，但是在实际使用时发现一个问题，通过Mybatis从数据库取出的Date都转换为TimeStamp，在application.properties中配置时间格式，
```
spring.jackson.date-format=yyyy-MM-dd HH:mm:ss
spring.jackson.time-zone=GMT+8
```
然而发现并没有起作用。通过度娘发现是使用```@EnableWebMvc```注解导致配置失效，因此需要对拦截器做一些调整，即放弃```@EnableWebMvc```,直接```implements WebMvcConfigurer```来避免覆盖application.properties。
```java
@Configuration
public class ResponseConfig implements WebMvcConfigurer {

    @RestControllerAdvice
    static class CommonResultResponseAdvice implements ResponseBodyAdvice<Object> {
        @Override
        public boolean supports(MethodParameter methodParameter, Class<? extends HttpMessageConverter<?>> aClass) {
            return true;
        }

        @Override
        public Object beforeBodyWrite(Object body, MethodParameter methodParameter, MediaType mediaType, Class<? extends HttpMessageConverter<?>> aClass, ServerHttpRequest serverHttpRequest, ServerHttpResponse serverHttpResponse) { ... }
```
这样修改一些程序之后，Response中的TimeStamp就会变成我们设定的日期格式了。
