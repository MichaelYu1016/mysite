---
title: "Go初尝试"
author: "Yu Nongxin"
date: "2021-01-23"
categories: ["Programming"]
tags: ["Go"]
slug: "first-try-on-go"
thumbnailImagePosition: left
thumbnailImage: "banners/golang.jpg"
---

Golang是一门诞生于2009年的年轻的编程语言，背靠Google这颗大树，近年来发展非常迅猛。
<!--more-->
GO语言开发者来说非常易学，而且据说在性能和并发编程上有非常出色的表现，因此我决定尝试一下。

## 1. 安装

了解一门新语言，最好的就是上官网，国内同学打开[https://golang.google.cn](https://golang.google.cn)。在Download界面选择对应的操作系统，我在windows下使用，就选择下载.msi文件。接下来的就是双击安装就好了。

安装完成后，打开cmd或者powershell，输入
```bash
$ go version
go version go1.15.7 windows/amd64
```
如果显示上面的安装版本，就说明成功了。

接着选择一款IDE方便开发，考虑到free和轻量化，我选择了VS Code。在插件列表搜索Go，安装有Go核心团队开发的插件Go，就具备了一个基本的本地开发环境。

## 2. Hello World

尝试一门新语言的第一个程序必然是“Hello World”了。按照Go的tutorial，建立一个新文件夹，创建一个`hello.go`文件，输入

```go
package main

import "fmt"

func main() {
    fmt.Println("Hello, World!")
}
```

保存好文件，在终端输入

```bash
$ go run .
Hello, World!
```

能输出“Hello, World!”说明代码运行成功了。

## 3. 配置代理

Go语言提供了`go get command`命令来获取modules，不过由于墙的存在，很多modules不能正常下载，因此需要使用代理。
在终端输入`go env`可以查看目前go的环境变量，其中GOPROXY就是代理。我们需要将它改成一个国内代理网址，最简单的就是在windows的环境变量中进行设置。

新增一个windows环境变量

```md
变量名：GOPROXY
变量值: https://goproxy.cn
```

我选择了中国的go代理地址，此时我们在VS Code中下载相关modules就都能成功了。

## 4. 使用外部包

跟着Go的官方tutorial尝试调用外部Package。首先在[https://pkg.go.dev/](https://pkg.go.dev/)网站查找package，我们找到`rsc.io/quote`,然后在原来的`hello.go`文件中做如下修改

```go
package main

import (
	"fmt"

	"rsc.io/quote"
)

func main() {
	fmt.Println(quote.Go())
}
```

在保存文件时，Go还会进行自动的代码格式化，使大家开发的风格保持一致。保存好代码，由于引入了外部包，所以为了方便追踪依赖包的版本，我们使用`go mod init command`命令生成一个`go.mod`文件。

```bash
$ go mod init hello
go: creating new go.mod: module hello
```

这时就可以在文件夹内看到一个`go.mod`文件。接下来我们运行代码

```bash
$ go run .
go: finding module for package rsc.io/quote
go: downloading rsc.io/quote v1.5.2
go: found rsc.io/quote in rsc.io/quote v1.5.2
go: downloading rsc.io/sampler v1.3.0
go: downloading golang.org/x/text v0.0.0-20170915032832-14c0d48ead0c
Don't communicate by sharing memory, share memory by communicating.
```

根据console的输出可以看到Go根据引入的module，自动去下载了相关包，然后输出了我们调用的`quote.Go()`函数的内容。

## 5. 总结

整体来说，Go的安装、配置和运行都非常简单、易上手，语法也比较友好，各种库比较友好，类似自动代码格式化、自动下载依赖等等都能很好的提升生产力，而且大部分你需要的东西都在标准库中存在，确实是一门非常有现代编程语言特色的语言。接下来就需要更深入地去学习和了解Go的使用了。
