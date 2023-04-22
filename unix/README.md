## 李慧琴 linux_c系统开发学习笔记

#### 注意事项：
- 1 弃用root用户
- 2 重构
- 3 代码积累，面试题，实验性lab，书籍课后习题

### I/O 
- stdio 标准IO
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

errno  
 /usr/include/asm-generic/errno-base.h 
 /usr/include/asm-generic/errno.h
现在版本unix已经从int类型转换成了宏  私有化数据？

perror() 
strerror()

- sysio 系统调用IO
**开发时两种io都可以使用时，一般使用c的标准IO（fopen，fclose）有两个好处 第一标准io封装了系统io调用，使得可移植；第二标准io会做一些读写加速等功能**
