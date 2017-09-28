#include "stdio.h"
#include <fcntl.h>
#include <unistd.h>


#define BUFFER_LEN  4096

int main(int argc, char * argv[])
{
    int souce_fd;
    int dest_fd;
    char buffer[BUFFER_LEN];
    int n_chars;
    if (argc != 3)
    {
        printf("usage erro, usage:cp source dest \n");
        return -1;
    }
    
    if ((souce_fd = open(argv[1], O_RDONLY)) == -1)
    {
        printf("could not open %s\n",argv[1]);
        return -1;
    }
    //if argv[2] not exist ,create it. note that the permmision and the 3rd arg for O_CREAT. if not set the permmision, the file would be ---- and not rw_rw_rw_ (0666).why the file not 0666? 因为实际还会受到进程权限的影响。其实是umask。如果是0633 ==> 633 & ~022 ==> 611 & 755 = 611
    if ((dest_fd = open(argv[2], O_CREAT|O_RDWR ,0633)) == -1)
    {
        printf("could not open %s\n",argv[2]);
        return -1;
    }
    
    while ((n_chars =read(souce_fd, buffer , BUFFER_LEN)) > 0)
    {
        if(write(dest_fd, buffer, n_chars) != n_chars)
        {
            printf("write error to %s\n",argv[2]);
            break;
        }
    }
    if (n_chars == -1)
    {
        printf("read error from %s\n",argv[1]);
    }
    
    if (close(souce_fd) == -1 || close(dest_fd) == -1)
    {
        printf("close error\n");
        return -1;
    }
    
    return 0;
}
