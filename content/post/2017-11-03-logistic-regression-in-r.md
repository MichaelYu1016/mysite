---
title: Logistic回归的R实现
author: "Yu Nongxin"
date: 2017-11-03
slug: logistic-regression-in-r
categories: ["Machine Learning"]
tags: ["R","Logistic Regression"]
thumbnailImage: "banners/default.jpg"
---

在统计学习和机器学习中，logistic回归是应用最广泛的一种分类方法，其模型通常针对二分类问题，并且具有较好的解释能力。logistic回归模型可以有条件概率分布$P(Y|X)$表示，此时随机变量$X$取值为实数，随机变量$Y$取值为1或0.我们可以通过监督学校的方法来估计模型参数。
<!--more-->
*定义*
$$
P(Y=1|x)=\frac{exp(\omega *x+b)}{1+exp(\omega *x+b)}
$$
$$
P(Y=0|x)=\frac{1}{1+exp(\omega *x+b)}
$$
对于给定的输入实例$x$，按照上式可以求出$P(Y=1|x)$和$P(Y=0|x)$。logistic回归将比较两个条件概率值得大小，将实例$x$分到概率值较大的那一类。

一个事件的几率（odds）是指该事件发生的概率与该事件不发生的概率的比值，如果事件发生的概率是p，那么该事件的几率是$\frac{p}{1-p}$，该事件的对数几率（log odds）或logit函数是
$$
logit(p)=log\frac{p}{1-p}
$$
对logistic回归而言，即有：
$$
log\frac{P(Y=1|x)}{1-P(Y=1|x)}=\omega \cdot x
$$
这就是说，在logistic回归模型中，输出$Y=1$的对数几率是输入$x$的线性函数。因此我们也把logistic回归归为广义线性模型（GLM）的一种。

下面我们用Titanic数据集在R中演示logistic回归模型。

首先读入Titanic数据集

```r
Titanic <- read.table('C:/Users/Lovely/Documents/R/titanic.txt')
```

然后我们先看一下数据集的构成

```r
str(Titanic)

# 'data.frame':    2201 obs. of  4 variables:
#  $ V1: int  1 1 1 1 1 1 1 1 1 1 ...
#  $ V2: int  1 1 1 1 1 1 1 1 1 1 ...
#  $ V3: int  1 1 1 1 1 1 1 1 1 1 ...
#  $ V4: int  1 1 1 1 1 1 1 1 1 1 ...

summary(Titanic)

#        V1              V2               V3               V4       
#  Min.   :0.000   Min.   :0.0000   Min.   :0.0000   Min.   :0.000  
#  1st Qu.:0.000   1st Qu.:1.0000   1st Qu.:1.0000   1st Qu.:0.000  
#  Median :1.000   Median :1.0000   Median :1.0000   Median :0.000  
#  Mean   :1.369   Mean   :0.9505   Mean   :0.7865   Mean   :0.323  
#  3rd Qu.:3.000   3rd Qu.:1.0000   3rd Qu.:1.0000   3rd Qu.:1.000  
#  Max.   :3.000   Max.   :1.0000   Max.   :1.0000   Max.   :1.000
```

根据数据源对应的介绍，四个变量分别是Class，Sex，Age，Survived。我们给原始数据添加对应名称。

```r
Titanic <- `colnames<-`(Titanic,c('Class','Sex','Age','Survived'))
```

然后我们使用`glm()`函数建立logistic回归模型

```r
fit <- glm(Survived~.,data = Titanic, family = binomial(link = 'logit'))
summary(fit)

# 
# Call:
# glm(formula = Survived ~ ., family = binomial(link = &quot;logit&quot;), 
#     data = Titanic)
# 
# Deviance Residuals: 
#     Min       1Q   Median       3Q      Max  
# -1.8989  -0.7879  -0.5877   0.7022   2.0615  
# 
# Coefficients:
#             Estimate Std. Error z value Pr(&gt;|z|)    
# (Intercept)  2.60985    0.29360   8.889  &lt; 2e-16 ***
# Class       -0.32904    0.04648  -7.079 1.45e-12 ***
# Sex         -1.00627    0.24565  -4.096 4.20e-05 ***
# Age         -2.61420    0.13329 -19.613  &lt; 2e-16 ***
# ---
# Signif. codes:  0 '***' 0.001 '**' 0.01 '*' 0.05 '.' 0.1 ' ' 1
# 
# (Dispersion parameter for binomial family taken to be 1)
# 
#     Null deviance: 2769.5  on 2200  degrees of freedom
# Residual deviance: 2274.9  on 2197  degrees of freedom
# AIC: 2282.9
# 
# Number of Fisher Scoring iterations: 4

summary(fit$fitted.values)

#    Min. 1st Qu.  Median    Mean 3rd Qu.    Max. 
#  0.1194  0.1586  0.2669  0.3230  0.2669  0.9073
```

此时我们可以看到模型的各个参数都显著，因此我们认为模型拟合的是很好的，而根据对拟合值分布的观察情况可以看出其都是分布在$[0,1]$之间的。我们在通过判断概率值大小来比较模型分类的正确率。

```r
fit_value <- fit$fitted.values
library(dplyr)
fit_value <- if_else(fit_value < 0.5, 0 , 1)
table(fit_value,Titanic$Survived)

#          
# fit_value    0    1
#         0 1364  367
#         1  126  344
```

可以看出模型分类的准确率在77.6%，整体表现还行，但是通过一些其他方法还能够进一步提升模型的准确率。
