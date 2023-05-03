# <center>**李慧琴 linux_c系统开发学习笔记**

### 注意事项：
- 1 弃用root用户
- 2 重构
- 3 代码积累，面试题，实验性lab，书籍课后习题

### 代码风格  
- 可读性
- 正确
- 效率

### I/O 

- sysio 系统调用IO
**开发时两种io都可以使用时，一般使用c的标准IO（fopen，fclose）有两个好处 第一标准io封装了系统io调用，使得可移植；第二标准io会做一些读写加速等功能**


#### std I/O 标准I/O  

**FILE**

##### 基础函数
```c
  fopen() fclose()
  fgetc() fputc() 
  fgets() fputs() 
  fread() fwrite()
  printf() scanf()
  fseek() ftell() rewind()
  fflush()
```

man 3 fopen

##### errno  
```
 /usr/include/asm-generic/errno-base.h 
 /usr/include/asm-generic/errno.h
现在版本unix已经从int类型转换成了宏  私有化数据？

perror() 
strerror() 
```

每个文件只要不为空，都会携带有换行符 LF/CRLF
每个字符串都有结束'\0'
在使用fgets读取时要注意上面两个问题


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
返回读到的快的数量nmemb
如果文件字节足够时，
fread(buf, 1, 10, fp)         // 返回10  10*1字节
fread(buf, 10, 1, fp)         // 返回1   1*10字节
返回都是一次读取到的字节数

但是如果文件字节不够，比如只有5字节时
fread(buf, 1, 10, fp)         // 返回5  5*1字节
fread(buf, 10, 1, fp)         // 返回0  ？？？字节

所以一般习惯用第一种形式，一个一个读写，当成一个fgetc使用，比较安全

权限： ~(0666 & umask)

##### 格式化输出
```
printf  
fprintf  
dprintf  
sprintf  
snprintf  
scanf
fscanf
sscanf
```
输出格式由%开始 以一个转换字符结束  如果%后面不是转换说明，则此行为未定义
```
%[flags][fldwidth][precision][lenmodifier]convtype

负号        指定被转换参数左对齐
数          指定最小字段宽度  可以为* 通过参数（int）来计算
小数点       将字段宽度和精度区分
数          小数点后的数指定打印精度 可以为*
字母 h/l/z  参数长度

转换格式： d/u/x/c/s/f等
```


##### fseek 产生空洞文件
用处：比如下载，下载一个2G字节大小的文件，需要先校验本地磁盘是否有足够大小
     此时下载任务第一个要做的，就是先占位
     先“w”打开一个文件，fseek将其设置为2G大小的空洞文件占位磁盘，如果磁盘不够直接报错
     不会下到一半再报错磁盘不够
     后续下载的字节填充这个占位文件即可（下载过程中可能涉及多线程，将空洞文件某一块分配给某一个线程处理）

##### 缓冲区作用：  
合并系统调用
- 行缓冲： 换行时候刷新，满了的时候刷新，强制刷新（stdout是这样的，终端设备）  
- 全缓冲： 换行时候刷新，满了的时候刷新（默认，只要不是终端设备）
- 无缓冲： stderr 需要立即输出的内容  

setvbuf修改缓冲模式，但是基本不需要程序员去修改  


getline会出现可控的内存泄露  

##### 临时文件：  
1）如何不冲突
2）及时销毁  
tmpnam()
tmpfile()
tmpnam()函数存在bug：在返回唯一路径名和用该名字创建文件之间，存在一个时间窗口，可能会被别的进程/线程用相同文件名创建文件 操作不原子
推荐使用  
tmpfile()
mkstemp()

#### sys I/O （文件I/O 系统调用I/O）  

**fd**  
文件描述符 fd 
          （int  文件系统的某个数组结构的下标 即从0开始
          某个进程空间最大可用一般1024  ulimit -c ::open files）
          0, 1, 2 (stdin, stdout, stderr) 为默认打开
          之后再次获取fd，将为当前可用的最小的文件描述符
- 文件I/O的操作
```
open
close
read
write
lseek
```

映射：
r  -> O_RDONLY
r+ -> O_RDWR
w  -> 0_WRONLY | O_CREAT | O_TRUNK
w+ -> O_RDWR | O_CREAT | O_TRUNK
a  -> 0_WRONLY | O_CREAT | O_TRUNK | O_APPEND
a+ -> O_RDWR | O_CREAT | O_TRUNK | O_APPEND

##### 文件I/O和标准I/O的区别
  标准io有缓冲区概念，会在缓冲区满了或者fflush时执行系统调用，而文件io立刻会执行进入到内核态执行系统调用
  吞吐量&响应速度
  标准io和文件io不可以混用，必须配套使用  但是可以使用  int fileno(FILE *stream); 转换
##### I/O的效率问题
  mycp1 测试
##### 文件共享  
  多进程同时打开同一个文件时，读操作OK，写操作需要原子执行或者加锁 （可以O_APPEND属性，是原子的）

##### 原子操作
##### 重定向：dup() dup2()
##### 同步： 
```
  sync() 
  fsync()
  fdatasync() 
  fcntl()    // 比如可以设置fd的性质 阻塞非阻塞
  ioctl()    // 设备相关的设置
```
##### /dev/fd 目录 虚目录
  当前进程的文件描述符信息

##### 无缓冲IO
- 所有的磁盘IO都要经过内核的块缓冲区（内核的缓冲区高速缓存），read/write的数据都要被内核缓冲，术语**不带缓冲的I/O**指在用户的进程中对这两个函数不会自动缓冲，每次调用read/write都需要进行一次系统调用
- 
- 

### 文件系统  
##### 目录和文件
##### 获取文件属性 
```
stat
fstat
lstat 
``` 
##### 文件访问权限
##### umask
##### 文件权限的管理 chmod()  chown()
##### 粘住位
##### 文件系统 FAT  UFS
##### 硬链接，符号链接
##### utime()
##### 目录的创建和销毁
##### cd
##### 分析目录，读取目录内容
#### 系统数据文件和信息
#### 进程环境

