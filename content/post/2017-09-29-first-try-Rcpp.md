---
title: "Rcpp的初次尝试"
author: "郁农欣"
date: 2017-09-29T21:00:00-02:00
categories: ["Programming"]
tags: ["R","R Markdown", "Rcpp"]
thumbnailImage: "banners/default.jpg"
---

Rcpp包作为连接R和C++的利器，是广大R语言使用者使用最广泛的拓展包。R语言给我们提供了丰富多样的统计计算函数和最新的统计模型包，但是受制于自身语言运算速度，R语言在计算循环时耗时巨大，因此需要引入更快的底层语言对其进行加速，C++作为一种计算速度非常快的中级语言，非常好的契合了这个需求，而Rcpp包则提供了两种语言互相沟通的完美结合方式。
<!--more-->
作为曾经学习过C++的人，对于单个的.cpp文件源代码更熟悉，而inline形式的C++代码插入并不利于我编写函数，因此我们首先从调用.cpp文件中的函数开始学习Rcpp包。

我们首先用C++写一个斐波那契函数：
```r
#include <Rcpp.h>
using namespace Rcpp;
// [[Rcpp::export]]
int fib_cpp_1(int n)
{
  if(n==1||n==2) return 1;
  return fib_cpp_1(n-1)+fib_cpp_1(n-2);
}
```
此时我们将其保存为`fib_cpp_1.cpp`文件，同时我们希望在其中引入R代码进行比较的话，可以使用
```r
/*** R
library(microbenchmark)
fib <- function(n){
  if(n==1|n==2) return(1)
  return(fib(n-1)+fib(n-2))
}
microbenchmark(fib_cpp_1(15),fib(15))
*/
```
将这部分代码插入.cpp文件，并在R语言中使用` Rcpp::sourceCpp()`函数可以得到下面的结果：
```r
Rcpp::sourceCpp('fib_cpp_1.cpp')

# 
# > library(microbenchmark)
# 
# > fib <- function(n) {
# +     if (n == 1 | n == 2) 
# +         return(1)
# +     return(fib(n - 1) + fib(n - 2))
# + }
# 
# > microbenchmark(fib_cpp_1(15), fib(15))
# Unit: microseconds
#           expr     min       lq      mean  median      uq      max neval
#  fib_cpp_1(15)   1.643   2.4640  12.11992   3.285   4.106  772.653   100
#        fib(15) 696.291 712.5075 834.15744 739.604 794.823 4616.621   100
```



