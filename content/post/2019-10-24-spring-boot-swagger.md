---
title: "Spring Boot集成Swagger2"
author: "Yu Nongxin"
date: "2019-10-24"
categories: ["Programming"]
tags: ["Java", "Spring Boot"]
thumbnailImagePosition: left
thumbnailImage: "banners/swagger.jpg"
---

在spring boot中使用swagger 2可以通过官方提供的Swagger 2和Swagger UI包来实现。
<!--more-->

## 集成Swagger 2

首先在POM文件中引入依赖：

```
    <!-- swagger -->
    <dependency>
        <groupId>io.springfox</groupId>
        <artifactId>springfox-swagger2</artifactId>
        <version>2.9.2</version>
    </dependency>
    <!-- swagger-ui -->
    <dependency>
        <groupId>io.springfox</groupId>
        <artifactId>springfox-swagger-ui</artifactId>
        <version>2.9.2</version>
    </dependency>
```
然后我们创建一个Swagger的java配置类SwaggerConfig.java，
```java
@Configuration
@EnableSwagger2
@Profile({"dev","test"}) //只在开发和测试环境暴露Api
public class SwaggerConfig {

    @Bean
    public Docket createRestApi() {

        return new Docket(DocumentationType.SWAGGER_2)
                .apiInfo(apiInfo())
                .select()
                //为当前包路径
                .apis(RequestHandlerSelectors.withMethodAnnotation(ApiOperation.class))
                .paths(PathSelectors.any())
                .build();
    }

    //构建 api文档的详细信息函数,注意这里的注解引用的是哪个
    private ApiInfo apiInfo() {
        return new ApiInfoBuilder()
                //页面标题
                .title("Swagger API")
                //创建人
                .contact(new Contact("us", "www.example.com", "example@163.com"))
                //版本号
                .version("1.0")
                //描述
                .description("Swagger API说明文档")
                .build();
    }
}
```
在配置类中我们要加上```@EnableSwagger2```注解来启用Swagger2，使用```@Profile({"dev","test"})```注解只在开发和测试环境启用Swagger，通过上述配置我们就可以在项目中使用注解来构建一个Swagger API文档。

## 增加安全认证

虽然我们上一节的配置中设置了Swagger API文档的启用环境，但是我们的API如果是需要使用认证进行访问的话，就需要进行一些额外的配置。以token访问为例，我们如果不设置一个统一的配置，就需要在每一个API的注解上加上认证，这显然比较繁琐，而且每次使用Swagger UI界面的接口都需要输入token。这里我们通过在SwaggerConfig.java中配置ApiKey来实现对所有接口的认证配置，同时放开login接口用来获取token，在获取token后只需要在Swagger UI界面的Authorize按钮弹出框进行登录，就能够解锁其他API接口，并不需要再次输入token，能够节省大量重复工作。新增配置如下：
```java
    @Bean
    public Docket createRestApi() {

        return new Docket(DocumentationType.SWAGGER_2)
                .apiInfo(apiInfo())
                .select()
                //为当前包路径
                .apis(RequestHandlerSelectors.withMethodAnnotation(ApiOperation.class))
                .paths(PathSelectors.any())
                .build()
                .securitySchemes(securitySchemes())
                .securityContexts(securityContexts());
    }
    
    // Header中配置全局Authorization
    private List<ApiKey> securitySchemes() {
        List<ApiKey> apiKeys = new ArrayList<>();
        apiKeys.add(new ApiKey("Authorization", "Authorization", "header"));
        return apiKeys;
    }

    private List<SecurityContext> securityContexts() {
        List<SecurityContext> securityContexts = new ArrayList<>();
        securityContexts.add(SecurityContext.builder()
                .securityReferences(defaultAuth())
                .forPaths(PathSelectors.regex("^(?!/login).*$")) // 排除登录接口
                .build());
        return securityContexts;
    }

    private List<SecurityReference> defaultAuth() {
        AuthorizationScope authorizationScope = new AuthorizationScope("global", "accessEverything");
        AuthorizationScope[] authorizationScopes = new AuthorizationScope[1];
        authorizationScopes[0] = authorizationScope;
        List<SecurityReference> securityReferences = new ArrayList<>();
        securityReferences.add(new SecurityReference("Authorization", authorizationScopes));
        return securityReferences;
    }
```

## 额外需求

由于项目的特殊性，我们在接收参数时很多时候都使用Map和String类型，这就和Swagger直接扫描Model生成参数模型有了冲突，因此需要想办法解决。网上有博客给出了使用javassis动态生成含有Swagger注解的javaBean的方法来解决这个问题。
首先引入javassis依赖。
```
    <!-- https://mvnrepository.com/artifact/org.javassist/javassist -->
    <dependency>
        <groupId>org.javassist</groupId>
        <artifactId>javassist</artifactId>
        <version>3.26.0-GA</version>
    </dependency>
```
然后我们根据需要创建两个注解类，
```java
@Target({ElementType.PARAMETER, ElementType.FIELD, ElementType.METHOD})
@Retention(RetentionPolicy.RUNTIME)
public @interface ApiJsonObject {

    ApiJsonProperty[] value(); //对象属性值

    String name();  //对象名称

    String notes(); // 说明

}

@Target(ElementType.ANNOTATION_TYPE)
@Retention(RetentionPolicy.RUNTIME)
public @interface ApiJsonProperty {

    String key();  //key

    String example() default "";

    String type() default "string";  //支持string, int, double

    String description() default "";

    boolean required() default false; // 是否必填

}
```
这两个注解的作用是构建我们接受Map和String类型参数时的模型类和属性，接下来我们需要一个插件类来实现对注解参数的读取和装配到JavaBean中。建立一个新的插件类MapApiReader.java，这个类将通过@ApiJsonProperty和@ApiJsonObject生成动态类，并将属性值增加到新的ApiModelProperty，最终实现在Swagger UI界面的展示。
```java
@Component
@Order   //plugin加载顺序，默认是最后加载
public class MapApiReader implements ParameterBuilderPlugin {
    @Autowired
    private TypeResolver typeResolver;

    private static final String basePackage = "com.example.swagger.model.";  //动态生成的Class名

    @Override
    public void apply(ParameterContext parameterContext) {
        ResolvedMethodParameter methodParameter = parameterContext.resolvedMethodParameter();

        if (methodParameter.getParameterType().canCreateSubtype(Map.class) || methodParameter.getParameterType().canCreateSubtype(String.class)) { //判断是否需要修改对象ModelRef,这里我判断的是Map类型和String类型需要重新修改ModelRef对象
            Optional<ApiJsonObject> optional = methodParameter.findAnnotation(ApiJsonObject.class);  //根据参数上的ApiJsonObject注解中的参数动态生成Class
            if (optional.isPresent()) {
                String name = optional.get().name();  //model 名称
                String notes = optional.get().notes(); // model 描述
                ApiJsonProperty[] properties = optional.get().value();

                parameterContext.getDocumentationContext().getAdditionalModels().add(typeResolver.resolve(createRefModel(properties, name)));  //像documentContext的Models中添加我们新生成的Class

                parameterContext.parameterBuilder()  //修改Map参数的ModelRef为我们动态生成的class
                        .parameterType("body")
                        .modelRef(new ModelRef(name))
                        .description(notes)
                        .name(name);
            }
        }

    }

    /**
     * 根据propertys中的值动态生成含有Swagger注解的javaBean
     */
    private Class createRefModel(ApiJsonProperty[] propertys, String name) {
        ClassPool pool = ClassPool.getDefault();
        CtClass ctClass = pool.makeClass(basePackage + name);

        try {
            for (ApiJsonProperty property : propertys) {
                ctClass.addField(createField(property, ctClass));
            }
            return ctClass.toClass();
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * 根据property的值生成含有swagger apiModelProperty注解的属性
     */
    private CtField createField(ApiJsonProperty property, CtClass ctClass) throws NotFoundException, CannotCompileException {
        CtField ctField = new CtField(getFieldType(property.type()), property.key(), ctClass);
        ctField.setModifiers(Modifier.PUBLIC);

        ConstPool constPool = ctClass.getClassFile().getConstPool();

        AnnotationsAttribute attr = new AnnotationsAttribute(constPool, AnnotationsAttribute.visibleTag);
        Annotation ann = new Annotation("io.swagger.annotations.ApiModelProperty", constPool);
        ann.addMemberValue("value", new StringMemberValue(property.description(), constPool));
        // 设置注解属性的dataType
        if (ctField.getType().subclassOf(ClassPool.getDefault().get(String.class.getName()))) {
            ann.addMemberValue("dataType", new StringMemberValue("string", constPool));
        }
        if (ctField.getType().subclassOf(ClassPool.getDefault().get(Integer.class.getName()))) {
            ann.addMemberValue("dataType", new StringMemberValue("integer",constPool));
        }
        if (ctField.getType().subclassOf(ClassPool.getDefault().get(Double.class.getName()))) {
            ann.addMemberValue("dataType",new StringMemberValue("double",constPool));
        }
        // 设置是否必填
        if (property.required()) {
            ann.addMemberValue("required", new BooleanMemberValue(true,constPool));
        }else {
            ann.addMemberValue("required", new BooleanMemberValue(false,constPool));
        }
        // 设置example的值
        ann.addMemberValue("example",new StringMemberValue(property.example(),constPool));
        attr.addAnnotation(ann);
        ctField.getFieldInfo().addAttribute(attr);

        return ctField;
    }

    private CtClass getFieldType(String type) throws NotFoundException {
        CtClass fileType = null;
        switch (type) {
            case "string":
                fileType = ClassPool.getDefault().get(String.class.getName());
                break;
            case "int":
                fileType = ClassPool.getDefault().get(Integer.class.getName());
                break;
            case "double":
                fileType = ClassPool.getDefault().get(Double.class.getName());
                break;
        }
        return fileType;
    }

    @Override
    public boolean supports(DocumentationType delimiter) {
        return true;
    }
}
```

示例可以参考[https://github.com/MichaelYu1016/swagger2example](https://github.com/MichaelYu1016/swagger2example)

## 可能会遇到的坑

### 1. 整型的默认值出现空值异常

**解决方法**：引入高版本的swagger-model覆盖swagger2中的
```
    <dependency>
        <groupId>io.swagger</groupId>
        <artifactId>swagger-models</artifactId>
        <version>1.5.21</version>
    </dependency>
```

### 2. 集成shiro时放行swagger资源

这个问题并不一定会出现，如果出现swagger ui界面无法显示，可以考虑试一下，在shiro的配置类中放行下来uri，
```java
filterRuleMap.put("/swagger-ui.html", "anon");
filterRuleMap.put("/swagger-resources", "anon");
filterRuleMap.put("/swagger-resources/configuration/security", "anon");
filterRuleMap.put("/swagger-resources/configuration/ui", "anon");
filterRuleMap.put("/v2/api-docs", "anon");
filterRuleMap.put("/webjars/springfox-swagger-ui/**", "anon");
```
