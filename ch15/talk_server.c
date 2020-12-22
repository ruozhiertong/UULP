#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>



#define SERVER_IP	"127.0.0.1"
#define TIMEOUT_SEC	5

extern int errno;

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("usage: talk_server port\n");
		return -1;
	}

	int sock_id;
	struct sockaddr_in server_addr;

	int client_connect_fd;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);



	sock_id = socket(PF_INET, SOCK_STREAM, 0);

	if(sock_id < 0)
	{
		printf("socket error\n");
		return -1;
	}
		

	bzero(&server_addr, sizeof(server_addr));

	//server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_family = AF_INET;

	if(bind(sock_id , (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("bind error\n");
		return -1;
	}

	if(listen(sock_id , 5) < 0)
	{
		printf("listen error\n");
		return -1;
	}
    
    while(1)
    {
    	client_connect_fd = accept(sock_id, (struct sockaddr *)&client_addr, &client_addr_len);

    	if(client_connect_fd < 0)
    	{
    		printf("accept error %d\n", errno);
    		return -1;
    	}


    	struct timeval timeout;
    	timeout.tv_sec = TIMEOUT_SEC;
    	timeout.tv_usec = 0;
		int count = 0;
    	
    	fd_set read_fds;

    	//这里客户端的并发性会有点问题。 每次都只能处理某一个client。 只有处理完一个client才能accept新的client。
    	//这个readme.txt 中也有说明了。
    	while(1)
    	{
    		FD_ZERO(&read_fds);
	    	FD_SET(client_connect_fd,&read_fds);
	    	FD_SET(STDIN_FILENO, &read_fds);
	    	int ret = select(client_connect_fd+1,&read_fds,NULL,NULL,&timeout);

	    	if(ret == -1)
	    	{
	    		printf("select error\n");
	    		return -1;
	    	}
	    	else if(ret > 0)
	    	{
	    		if(FD_ISSET(STDIN_FILENO , &read_fds))
	    		{
	    			//send message
	    			char buf[512];
	    			int len = read(STDIN_FILENO , buf, 512);
	    			buf[len] = '\0';
	    			write(client_connect_fd, buf, len);
	    		}
	    		if(FD_ISSET(client_connect_fd , &read_fds))
	    		{
	    			//show message
	    			char buf[512];
	    			int len = read(client_connect_fd, buf, 512);
	    			if(len == 0)
	    			{
	    				printf("client close\n");
	    				break;
	    			}
	    			buf[len] = '\0';
	    			printf("from %s:%d: %s\n",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),buf);
	    		}
	    	}
	    	else
	    	{
	    		if(count++ > 5)
	    			break;
	    		printf("nothing happen after %d seconds\n",TIMEOUT_SEC);
	    	}
    	}  
    	close(client_connect_fd);

    }
    close(sock_id);

	return 0;
}