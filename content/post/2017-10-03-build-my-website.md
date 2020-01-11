---
title: "用blogdown拓展包搭建个人主页"
author: "Yu Nongxin"
date: 2017-10-03
thumbnailImagePosition: left
thumbnailImage: "banners/website.jpg"
categories: ["Programming"]
tags: ["R package","hugo"]
---
<!--more-->
谢易辉在bookdown的基础上又推出了blogdown拓展包，提供了一套以hugo为基础的个人主页搭建方式，将网站托管在netlify上，关联GitHub库后可以实现自动更新网站。按照[blogdown](https://bookdown.org/yihui/blogdown/)的说明文档，我重新搭建了自己的个人主页。

首先在GitHub上建立了一个repository-mysite，我将这个库作为我存放静态网站的地方。然后打开Rstudio，在console中输入
```r
install.packages(blogdown)
```
安装完拓展包后，安装说明安装Hugo网站生成器.
```r
library(blogdown)
install_hugo()
```
安装好Hugo后，需要开始进行一些基本设置，
```r
setwd("dir") #设置一个文件夹放置生成的静态网站文件
```
或者直接建立一个project来管理静态网站的代码和文档.

在开始搭建静态网站之前，我们需要给自己的主页设置theme，作为对网页设置不太了解的人，我果断在[https://themes.gohugo.io]( https://themes.gohugo.io)中选择了一个主题.

选择主题后可以直接运行下面代码进行静态网站的建立：
```r
new_site(theme = "yihui/hugo-lithium-theme") #yihui/hugo-lithium-theme改成你选择的主题的GitHub仓库即可
```
此时我们可以在自己的`dir`文件夹下看到生成了很多静态网站的文档，其中`public`文件夹是生成静态网站的结果，需要同步到GitHub；`content`则是我们放置.md或者.Rmd文件的地方。一般导入了主题会有一个简单的示例出现，根据示例你可以进行修改，将模板修改为自己的样式.

需要注意的是，使用blogdown包的好处在于支持编译.Rmd文件，不过其需要增加一步
```r
build_site()
```

这一步会编译.Rmd文件，之后在运行
```r
hugo_build()
```
即可生成新的静态网站。使用GitHub进行同步，netlify会自动将更新进行推送，及时更新网站内容。一些具体的细节问题可以通过[Hugo](http://gohugo.io/)的说明文档和[blogdown](https://bookdown.org/yihui/blogdown/)包的文档中寻找解决方法.
