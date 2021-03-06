#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define SERVER_IP	"127.0.0.1"

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		printf("usage:talc_client domain port\n");
		return -1;
	}

	int sock_id;
	struct sockaddr_in server_addr;

	struct	hostent *hp;

	sock_id = socket(AF_INET, SOCK_STREAM, 0);

	if(sock_id < 0)
	{
		printf("socket error\n");
		return -1;
	}

	bzero((void*)&server_addr, sizeof(server_addr));
	
	hp = gethostbyname(argv[1]);           /* get info about host  */                                        /* fill in host part    */
	bcopy((void *)hp->h_addr, (void *)&server_addr.sin_addr, hp->h_length);

	//server_addr.sin_addr.s_addr  = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));        /* fill in socket port  */
	server_addr.sin_family = AF_INET; 

	//client 端 不需要进行bind操作。 如果想bind操作，也是可以的。


	if (connect(sock_id,(struct sockaddr *)&server_addr, sizeof(server_addr)) !=0)
	{
		printf("connect error\n");
		return -1;
	}

	printf("connect success!\n");

	char buf[512];
	int len;
	
	len = read(sock_id , buf, 512);
	buf[len] = '\0';
	printf("%s\n", buf);
	scanf("%s",buf);
	write(sock_id, buf, strlen(buf));

	fd_set read_fds;
	struct timeval timeout;
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	int count = 0;

	

	while(1)
	{

		FD_ZERO(&read_fds);   
		FD_SET(sock_id, &read_fds);   
		FD_SET(STDIN_FILENO, &read_fds);
		//int ret = select(sock_id+1,&read_fds,NULL,NULL,&timeout);
		int ret = select(sock_id+1,&read_fds,NULL,NULL,NULL);
    	
    	if(ret == -1)
    	{
    		printf("select error\n");
    		close(sock_id);
    		return -1;
    	}
    	else if(ret > 0)
    	{
    		if(FD_ISSET(STDIN_FILENO , &read_fds))
    		{
    			//send message
    			len = read(STDIN_FILENO , buf, 512);
    			if(len == 0)
    				break;
    			//\n 也被读进来了。
    			buf[len - 1] = '\0';
    			write(sock_id, buf, len);
    		}
    		if(FD_ISSET(sock_id , &read_fds))
    		{
    			//show message
    			len = read(sock_id, buf, 512);
    			if (len == 0)
    				break;
    			buf[len] = '\0';
    			printf("from %s:%d: %s\n",inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port),buf);
    		}
    	}
    	else //timeout
    	{
    		/*
    		if(count++ > 5)
    			break;
    		printf("nothing happen after 3 seconds\n");
    		*/
    	}

	}

	close(sock_id);
	return 0;

}