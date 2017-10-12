#include <stdio.h>

/*
 测试结构化内容写入文件。1.能不能直接打开文件查看？ 无法直接查看，结构化写入的是二进制。可以用Sublime 转成十六进制查看。
                     2.可以再次结构化读入。可以。
*/
 
typedef struct student
{
    char name[81];
    int age;
    int score;
}Student;

int main()
{
    
    printf("%d\n",sizeof(int));
    printf("%d\n",sizeof(char));
    
    printf("%d\n",sizeof(Student)); //92， 写入3个，文件大小92*3 = 276
    FILE *fp;
    fp = fopen("student.txt", "w+");
    if (fp == NULL)
    {
        printf("open error\n");
        return -1;
    }
    Student stu[3];
    strcpy(stu[0].name,"test");
    stu[0].age = 18;
    stu[0].score = 60;
    
    strcpy(stu[1].name,"hello");
    stu[1].age = 20;
    stu[1].score = 70;
    
    
    strcpy(stu[2].name,"world");
    stu[2].age = 19;
    stu[2].score = 80;

//    for (int i = 0; i < 3; i++)
//    {
//        fwrite((void*)&stu[i], sizeof(Student), 1, fp);
//    }
    
    fwrite(stu, sizeof(Student), 3, fp); //也可以一下子写入。
    
    fseek(fp, 0, SEEK_SET);
    
    Student st;
    
    printf("%d\n",sizeof(st.age));
    printf("%d\n",sizeof(st.name));
    
    printf("%d\n",sizeof(st.score));
    
    printf("%d\n",sizeof(st)); //结构体大小比各成员大小和大， 因为编译器做了优化。结构体对齐。
    
    for (int i = 0; i< 3; i ++)
    {
        fread(&st, sizeof(Student), 1, fp);

        printf("%s %d %d\n", st.name,st.age,st.score);
    }
    
    
    fclose(fp);
    
    
    
    
    
    
    
    
    
    
    
    
    
}