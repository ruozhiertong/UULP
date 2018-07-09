1. 习题5.23。关于mesg命令 原理： 通过tty的是否可写入的权限来实现的。 如果mesg允许其他人写入，那么write的消息就能写进来。
所以，通过设置tty的写权限来达到write是否可写入。 tty设备，同文件类似，通过fstat 获得 struct stat settings; 更改settings中的st_mode权限，就能达到目的。
强哥：https://blog.csdn.net/lwqxmu/article/details/8915369
强哥博客中其实不用知道tty的名称，直接用fstat通过fd来设置tty，这样就解决了无法知道tty名称的问题。
