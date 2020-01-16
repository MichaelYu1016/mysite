---
title: "Spring Boot实现异步调用"
author: "Yu Nongxin"
date: "2020-01-16"
categories: ["Programming"]
tags: ["Spring Boot","Java","Async"]
slug: "spring-boot-async"
thumbnailImage: "banners/spring.jpg"
summary: "本文将介绍如何在Spring Boot中实现异步调用,通过异步编程提高应用程序的效率。"
---

在Java应用中，绝大多数情况下都是通过同步的方式来实现交互处理的，即程序按照代码的顺序一行一行的逐步往下执行，每一行代码都必须等待上一行代码执行完毕才能开始执行；但是在处理与第三方系统交互的时候，容易造成响应迟缓的情况，所以在一些情景下，通过异步编程可以提高效率，提升接口的吞吐量。本文将介绍如何在Spring Boot中实现异步调用。

# 开启异步功能

新建一个Spring Boot项目，版本为2.1.7.RELEASE，引入`spring-boot-starter-web`依赖。

要开启异步支持，首先得在Spring Boot启动类上加上`@EnableAsync`注解：

```java
@SpringBootApplication
@EnableAsync
public class SpringBootAsyncApplication {
	public static void main(String[] args) {
		SpringApplication.run(SpringBootAsyncApplication.class, args);
	}
}
```

# 编写异步方法

开启异步功能后，我们需要编写异步方法，在`service`包中建立`AsyncService.java`接口类，定义两个方法`asyncMethod`和`syncMethod`。

```java
public interface AsyncService {
    /**
     * 异步处理方法
     */
    void asyncMethod();

    /**
     * 同步处理方法
     */
    void syncMethod();
}
```

然后在`service`包中建立`impl`包作为存放实现类的地方，新建`AsyncServiceImpl.java`类。

```java
@Service
public class AsyncServiceImpl implements AsyncService {
    private Logger logger = LoggerFactory.getLogger(this.getClass());

    @Async
    @Override
    public void asyncMethod() {
        long start = System.currentTimeMillis();
        logger.info("异步方法开始");
        sleep();
        logger.info("异步方法内部线程名称：{}", Thread.currentThread().getName());
        long end = System.currentTimeMillis();
        logger.info("异步方法结束, 总耗时：{} ms", end - start);
    }

    @Override
    public void syncMethod() {
        long start = System.currentTimeMillis();
        logger.info("同步方法开始");
        sleep();
        long end = System.currentTimeMillis();
        logger.info("同步方法线程名称：{}", Thread.currentThread().getName());
        logger.info("同步方法结束, 总耗时：{} ms", end - start);
    }

    private void sleep(){
        try {
            TimeUnit.SECONDS.sleep(2);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}
```

这个实现类中用`sleep()`模拟处理时间，`@Async`注解表示该方法是异步调用，通过日志输出可以看到方法被调用后执行时间。然后我们在`controller`包下新建`AsyncController.java`用来测试异步调用和同步调用的区别。

```java
@RestController
public class AsyncController {

    private Logger logger = LoggerFactory.getLogger(this.getClass());

    @Autowired
    private AsyncService asyncService;

    @GetMapping("async")
    public void testAsync() {
        long start = System.currentTimeMillis();
        logger.info("接收请求，开始处理");
        asyncService.asyncMethod();
        long end = System.currentTimeMillis();
        logger.info("异步处理结束，耗时：{} ms", end - start);
    }

    @GetMapping("sync")
    public void testSync() {
        long start = System.currentTimeMillis();
        logger.info("接收请求，开始处理");
        asyncService.syncMethod();
        long end = System.currentTimeMillis();
        logger.info("同步处理结束，耗时：{} ms", end - start);
    }
}
```

通过`AsyncServiceImpl.java`中日志输出的方法处理时间和`AsyncController.java`中日志输出的方法处理时间，可以对比异步同步调用在时间上的差别。我们启动项目，先访问`http://localhost:8080/sync`，看一下同步调用，控制台输出如下：

```bash
[nio-8080-exec-1] c.e.y.s.controller.AsyncController       : 接收请求，开始处理
[nio-8080-exec-1] c.e.y.s.service.impl.AsyncServiceImpl    : 同步方法开始
[nio-8080-exec-1] c.e.y.s.service.impl.AsyncServiceImpl    : 同步方法线程名称：http-nio-8080-exec-1
[nio-8080-exec-1] c.e.y.s.service.impl.AsyncServiceImpl    : 同步方法结束, 总耗时：2002 ms
[nio-8080-exec-1] c.e.y.s.controller.AsyncController       : 同步处理结束，耗时：2008 ms
```

接着我们访问`http://localhost:8080/async`看一下异步调用的输出：

```bash
[nio-8080-exec-3] c.e.y.s.controller.AsyncController       : 接收请求，开始处理
[nio-8080-exec-3] c.e.y.s.controller.AsyncController       : 异步处理结束，耗时：18 ms
[         task-1] c.e.y.s.service.impl.AsyncServiceImpl    : 异步方法开始
[         task-1] c.e.y.s.service.impl.AsyncServiceImpl    : 异步方法内部线程名称：task-1
[         task-1] c.e.y.s.service.impl.AsyncServiceImpl    : 异步方法结束, 总耗时：2000 ms
```

可以看到异步调用在`controller`方法层面处理非常快，只用了18ms，`service`层处理用时2000ms，说明异步方法确实在新线程中去执行，没有对主线程造成阻塞。而同步调用的结果则是顺序执行，`service`层方法执行完毕后才继续执行`controller`层后续代码，耗时2008ms。这里充分展示了异步调用在响应速度上的优势。

# 处理异步回调

异步回调是指异步方法有返回值时，需要获取返回值。在java中使用`Future`来接收回调值。在`service`中新增异步回调的方法，

```java
@Async
@Override
public Future<String> asyncCallback() {
    long start = System.currentTimeMillis();
    logger.info("异步回调方法开始");
    sleep();
    logger.info("异步回调方法内部线程名称：{}", Thread.currentThread().getName());
    long end = System.currentTimeMillis();
    logger.info("异步回调方法结束, 总耗时：{} ms", end - start);
    return new AsyncResult<>("I have done!");
}
```

在`controller`中增加访问入口，

```java
@GetMapping("asyncCallback")
public String testAsyncCallback() throws Exception {
    long start = System.currentTimeMillis();
    logger.info("接收请求，开始处理");
    Future<String> stringFuture = asyncService.asyncCallback();
    String result = stringFuture.get();
    logger.info("异步回调方法返回值：{}", result);
    long end = System.currentTimeMillis();
    logger.info("异步回调结束，总耗时：{} ms", end - start);
    return stringFuture.get();
}
```

向`http://localhost:8080/asyncCallback`发起请求，控制台输出如下：

```bash
[nio-8080-exec-2] c.e.y.s.controller.AsyncController       : 接收请求，开始处理
[   asyncThread1] c.e.y.s.service.impl.AsyncServiceImpl    : 异步回调方法开始
[   asyncThread1] c.e.y.s.service.impl.AsyncServiceImpl    : 异步回调方法内部线程名称：asyncThread1
[   asyncThread1] c.e.y.s.service.impl.AsyncServiceImpl    : 异步回调方法结束, 总耗时：2004 ms
[nio-8080-exec-2] c.e.y.s.controller.AsyncController       : 异步回调方法返回值：I have done!
[nio-8080-exec-2] c.e.y.s.controller.AsyncController       : 异步回调结束，总耗时：2034 ms
```

可以从日志结果发现，`Future`的`get`方法是阻塞的，即顺序执行，所以通常将异步回调方法用在流程顺序不冲突的执行过程。

# 建立自定义线程池

在阿里的java开发手册中推荐手动设置线程池，在spring boot中我们可以通过配置类来对线程池进行设置。建立`config`包，新建`AsyncPoolConfig.java`类，

```java
@Configuration
public class AsyncPoolConfig {

    @Bean("asyncThread")
    public ThreadPoolTaskExecutor asyncThreadPoolTaskExecutor(){
        ThreadPoolTaskExecutor executor = new ThreadPoolTaskExecutor();
        executor.setCorePoolSize(20);
        executor.setMaxPoolSize(200);
        executor.setQueueCapacity(25);
        executor.setKeepAliveSeconds(200);
        executor.setThreadNamePrefix("asyncThread");
        executor.setWaitForTasksToCompleteOnShutdown(true);
        executor.setAwaitTerminationSeconds(60);

        executor.setRejectedExecutionHandler(new ThreadPoolExecutor.CallerRunsPolicy());

        executor.initialize();
        return executor;
    }
}
```

上面我们通过ThreadPoolTaskExecutor的一些方法自定义了一个线程池，这些方法的含义如下所示：

- `corePoolSize`：线程池核心线程的数量，默认值为1（这就是默认情况下的异步线程池配置使得线程不能被重用的原因）。
- `maxPoolSize`：线程池维护的线程的最大数量，只有当核心线程都被用完并且缓冲队列满后，才会开始申超过请核心线程数的线程，默认值为`Integer.MAX_VALUE`。
- `queueCapacity`：缓冲队列。
- `keepAliveSeconds`：超出核心线程数外的线程在空闲时候的最大存活时间，默认为60秒。
- `threadNamePrefix`：线程名前缀。
- `waitForTasksToCompleteOnShutdown`：是否等待所有线程执行完毕才关闭线程池，默认值为`false`。
- `awaitTerminationSeconds`：`waitForTasksToCompleteOnShutdown`的等待的时长，默认值为0，即不等待。
- `rejectedExecutionHandler`：当没有线程可以被使用时的处理策略（拒绝任务），默认策略为`abortPolicy`，包含下面四种策略：
    - `callerRunsPolicy`：用于被拒绝任务的处理程序，它直接在 `execute` 方法的调用线程中运行被拒绝的任务；如果执行程序已关闭，则会丢弃该任务。
    - `abortPolicy`：直接抛出`java.util.concurrent.RejectedExecutionException`异常。
    - `discardOldestPolicy`：当线程池中的数量等于最大线程数时、抛弃线程池中最后一个要执行的任务，并执行新传入的任务。
    - `discardPolicy`：当线程池中的数量等于最大线程数时，不做任何动作。

从最后的处理策略可以看出和`java.util.concurrent`包的`ThreadPoolExecutor`相似，实际上`ThreadPoolTaskExecutor`是Spring对JDK中`ThreadPoolExecutor`的封装。

在异步回调的例子中，因为已经配置了自定义线程池，同时给`Bean`命名了，所有异步回调的线程显示为`asyncThread1`，这里因为我只设置了一个线程池，所以`@Async`默认使用了该线程池，如果配置了多个线程池，则在`@Async`中指定使用的线程池，

```java
@Service
public class TestService {
    ...

    @Async("asyncThread")
    public void asyncMethod() {
       ...
    }
    ...
}
```
