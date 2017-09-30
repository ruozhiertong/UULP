#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int fd;
    int n_chars;
    char buf[100];
    fd = open("hello.txt",O_CREAT|O_RDWR,0777);
    if (fd == -1)
    {
        printf("open error\n");
        return -1;
    }
    
    printf("begin offset is %lld\n",lseek(fd, 0, SEEK_CUR));
    
    n_chars = write(fd, "hello world", 100);
    
    printf("write %d\n",n_chars);
    
    printf("now offset is %lld\n",lseek(fd, 200, SEEK_END)); //定位到文件尾的后200字节。 对于空洞文件，到文件尾的字节也包括空洞部分的。
    
    n_chars = write(fd, "fuck you", 100); //if write after lseek ,会出现空洞。即100字节到300字节共200字节的空洞，因为这个部分没有内容，字节数还是那么多，造成了空洞。 不过有空洞的文件在存储的时候，实际占据的磁盘块并不定很多。见AUPE第三章。
    
    printf("write %d\n",n_chars);
    
    printf("now offset is %lld\n",lseek(fd, 0, SEEK_END));//从文件的最后。
    
    printf("now offset is %lld\n",lseek(fd, 100, SEEK_END)); //if no write ,the size will not change
    
    printf("read %d\n",read(fd, buf, 100));
    
    printf("%s\n",buf);


    close(fd);
    
}
