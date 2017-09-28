#include <stdio.h>

int main()
{
    //test for goto
    
    int i;
    scanf("%d",&i);
    if (i ==0)
    {
        printf("to goto\n");
        goto test1; //goto 直接跳到test1处执行。test1前不管。
    }
    else
    {
        printf("not to goto\n");
    }
    
    printf("before goto\n");
    
test1:  //不管是不是goto过来的，只要运行到这，正常执行。 这里label只是为了goto直接跳过来执行，而不是说没有goto过来的就不执行。
    printf("in goto label\n");
    
    
    return 0;
}
