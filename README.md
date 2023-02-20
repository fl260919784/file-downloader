# 概要说明

## 概要思路

**当前方案强依赖Content-Length + Range: bytes**

1. 通过发送HTTP HEAD请求，分析Content-Length来获取总文件大小
   1. **如果文件大于int64长度，则当前方案不支持（libcurl通过Content-Length获取长度存在溢出）**
2. 通过Range: bytes来实现分段下载任务拆分

## 多任务下载说明

1. Executor表达的是无状态的线程池，启动后持续的从TaskGenerator中获取任务，当获取任务为空时，表达所有任务结束
2. 下载任务切分，以及下载状态控制均为TaskGenerator的实现类DownloaderTaskGenerator控制

## 文件写入相关

1. 并发写入依赖lseek/lseek64，通过计算好偏移量直接写入文件，各下载任务间无需相互等待或感知
2. **好处:** 文件不用合并，下载的各任务间无需互相感知


# 环境与依赖说明

## 操作系统

ubuntu 18.04

## 编译器

GCC 9.4

## C++标准

C++17

## libcurl

+ 安装说明

```
apt install libcurl4-openssl-dev
引入背景： 依赖libcurl发起http等协议请求
补充说明：动态链接没问题，即-lcurl
```

+ version

```
7.68.0
```

## boost

+ 安装说明

```
apt install libboost-program-options-dev
引入背景： 解析命令行参数
```

+ version

```
1.71.0
```