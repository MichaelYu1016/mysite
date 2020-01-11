---
title: "常用R包整理（持续更新）"
author: "Yu Nongxin"
date: 2017-10-01
categories: ["Programming"]
tags: ["R package"]

---
<!--more-->
CRAN上已有的R包数量众多，加上GitHub、R-Forge等平台上的R包，为我们提供了各种各样的选择，但是在数目众多的R包中，找到对自己有用的，相对而言要困难一些。在CRAN的task view中分类列举了很多领域的R包，我从中也发现了很多功能强大的拓展包，在这里就对我曾经用过的一些R包进行一些整理，挑出核心的一些进行说明。


#### 工程类
```
Rcpp:提供了R与C++进行交互的接口，同时以Rcpp为核心也拓展出了一系列R包，功能各不相同。

rJava:R和Java的接口，一些需要调用Java的R包的基础

XML,xml2:解析XML格式文件

doParallel:并行函数包

devtools:功能强大的开发工具包.
```



#### 数据处理
```
readr,readxl,haven:提供了对txt,csv,excel,sas,spss等文件格式数据的读写函数.
plyr,dplyr:Hadley Wickham编写的用来进行数据处理的R包，提供了一系列选择、过滤、变换、排序函数，功能强大，速度极快.
reshap2：对长宽数据进行变形操作的拓展包.
data.table:针对大数据集进行读取并提供了类似于dplyr的一系列处理函数.
magrittr:提供了更加丰富的pipe操作函数.
bigmemory：提供了针对内存不足情况下使用硬盘进行大数据处理的一套理论，目前已经发展为一个系列函数包，包括bigalgebra,biganalytics,biglm等.
RODBC:提供数据库接口
```
未完待续...