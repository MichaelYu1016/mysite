---
title: "Python虚拟环境和包管理工具——Pipenv"
author: "Yu Nongxin"
date: 2019-06-23
categories: ["Programming"]
tags: ["packaging tool","python"]
banner: "banners/python.jpg"
---

# 前言
最近准备自己从头写一个项目，但是之前也没有系统研究过管理自己的开发过程。在学习别人的源码时，认真看了一下好的项目的文件结构，突然发现了一个新的python开发利器[Pipenv](https://pypi.org/project/pipenv/)。在开发前端时有npm，yarn这样的包管理工具，能够生成项目中使用的包和依赖关系，而在python中，过去一直使用的是requirement.txt，经常需要手动维护，相对而言并不是特别方便。而Pipenv的出现则带来了很大变化，使得Python开发过程中虚拟环境的管理和包的管理都有了更好的选择。

# 安装
根据个人喜好，可以选择先把pip的下载源更换成国内的以加快下载速度。
### 1.更换国内源
Windows下找到%APPDATA%文件夹，并建立pip文件夹，新建一个```pip.ini```的文件，如果换成清华源的话写入
```
[global]
index-url = https://pypi.tuna.tsinghua.edu.cn/simple
[install]
trusted-host=pypi.tuna.tsinghua.edu.cn
```

Linux下在用户根目录新建一个.pip文件夹，创建~/.pip/pip.conf文件，内容和windows下一样即可。

### 2.安装Pipenv
Pipenv的安装非常简单，在命令行输入
```bash
pip install pipenv
```
安装完毕后就可以开始使用了。

# 使用Pipenv
新建一个准备当环境的文件夹venv，并cd进入该文件夹：
```pipenv --three```   会使用当前系统的Python3创建环境

```pipenv --python``` 3.6 指定某一Python版本创建环境

```pipenv shell``` 激活虚拟环境

```pipenv --where```  显示目录信息```E:\Documents\PythonScript\learn_fab```

```pipenv --venv```  显示虚拟环境信息```E:\Documents\PythonScript\venv\learn_fab```

```pipenv --py```  显示Python解释器信息

在使用pipenv时，我们需要忘记pip，安装包都使用```pipenv install <package>```命令，如果想安装只有开发环境使用的包，则可以使用```pipenv install --dev <package>```来进行区分。
初次使用时，cd进项目文件夹，使用```pipenv install```进行初始化，会创建Pipfile和Pipfile.lock两个文件，Pipfile会记录安装的包，而Pipfile.lock则会把安装的子依赖记录下来。
在使用pipenv安装包的时候，如果感觉下载速度慢，可以在Pipfile文件中的[source]下将url改为国内源。
如果有人从git库中下载了Pipfile文件，使用```pipenv install```会根据Pipfile的内容创建一个新的虚拟环境并安装依赖的软件包。而开发环境则使用```pipenv install -dev```来安装开发环境需要的软件包。

# 参数及命令一览
```bash
$ pipenv
Usage: pipenv [OPTIONS] COMMAND [ARGS]...

Options:
  --where          Output project home information.
  --venv           Output virtualenv information.
  --py             Output Python interpreter information.
  --envs           Output Environment Variable options.
  --rm             Remove the virtualenv.
  --bare           Minimal output.
  --completion     Output completion (to be eval'd).
  --man            Display manpage.
  --three / --two  Use Python 3/2 when creating virtualenv.
  --python TEXT    Specify which version of Python virtualenv should use.
  --site-packages  Enable site-packages for the virtualenv.
  --version        Show the version and exit.
  -h, --help       Show this message and exit.


Usage Examples:
   Create a new project using Python 3.7, specifically:
   $ pipenv --python 3.7

   Remove project virtualenv (inferred from current directory):
   $ pipenv --rm

   Install all dependencies for a project (including dev):
   $ pipenv install --dev

   Create a lockfile containing pre-releases:
   $ pipenv lock --pre

   Show a graph of your installed dependencies:
   $ pipenv graph

   Check your installed dependencies for security vulnerabilities:
   $ pipenv check

   Install a local setup.py into your virtual environment/Pipfile:
   $ pipenv install -e .

   Use a lower-level pip command:
   $ pipenv run pip freeze

Commands:
  check      Checks for security vulnerabilities and against PEP 508 markers
             provided in Pipfile.
  clean      Uninstalls all packages not specified in Pipfile.lock.
  graph      Displays currently–installed dependency graph information.
  install    Installs provided packages and adds them to Pipfile, or (if no
             packages are given), installs all packages from Pipfile.
  lock       Generates Pipfile.lock.
  open       View a given module in your editor.
  run        Spawns a command installed into the virtualenv.
  shell      Spawns a shell within the virtualenv.
  sync       Installs all packages specified in Pipfile.lock.
  uninstall  Un-installs a provided package and removes it from Pipfile.
```
