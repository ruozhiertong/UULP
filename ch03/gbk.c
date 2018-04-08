#include <stdio.h>

/**
 测试字符在内存中的编码格式。
 **/
int main(int argc, const char * argv[]) {
    // insert code here...
    char  a[10] = "hello";
    char b [10] = "你好";
    printf("%x\n",&a);
    printf("%s\n",a);
    
    printf("%llx\n",&b);
    printf("%s\n",b);
    
    //打印内存值。二进制数。 输出的是十六进制。
    for (int i = 0; i < 10; i++)
    {
        printf("%d a = %llx b =%llx\n", i ,a[i],b[i]);
    }
    
    return 0;
}
