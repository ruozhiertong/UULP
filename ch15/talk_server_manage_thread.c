#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h> 
#include <pthread.h>

#define SERVER_IP	"127.0.0.1"
#define TIMEOUT_SEC	5
#define MAX_CONNECT	10

//extern int errno;  //最好用include <sys/errno.h>


/*
注意多线程要注意同步处理。对使用公用资源的冲突访问等。
在findAvailable 可能要避免冲突。不过这里只在主线程中处理，不会有问题。如果多个线程使用，可能会有问题的。
在其他线程中，可能会出现该socket fd 连接的 socket fd 正在销毁，而该socket fd 查找时还能找到连接的socket fd，
这样导致和一个已经销毁的fd通信。 所以这里应该同步处理。
不过，这个不用同步处理也不会大问题，因为就算处理，仍然可能造成和不存在的fd通信。不过这个没啥影响。 等下次查找的时候会发现，这个销毁了。所以影响不大。
*/

int clients_sock_fds[MAX_CONNECT]; // = {-1};
char clients_name[MAX_CONNECT][512];
int clients_sock_map[MAX_CONNECT]; //= {-1};
int client_num = 0;

int findPeople(char * name)
{
	printf("in findPeople:%s\n",name);
	for(int i = 0; i < MAX_CONNECT; i++)
	{
		printf("%s\n", clients_name[i]);

		if(clients_name[i][0] == '\0')
			continue;

		if(strcmp(name, clients_name[i]) == 0)
			return i;
	}

	return -1;
}

int findClient(int fd)
{
	printf("in findClient:%d\n",fd);
	for(int i =0; i < MAX_CONNECT; i++)
	{
		printf("%d\n",clients_sock_fds[i]);
		if(clients_sock_fds[i] == -1) //skip empty slot
			continue;

		if(clients_sock_fds[i] == fd)
			return i;
	}

	return -1;
}

int findAvailable()
{
	for(int i = 0; i < MAX_CONNECT; i++)
	{
		if(clients_sock_fds[i] == -1)
			return i;
	}

	return -1;
}


void * client_func(void *arg)
{
	int client_sockfd = *(int*)arg;
	free(arg);

	int fd_index = findClient(client_sockfd);
    //require name;
    char buf[512];
    int len;
    int nread;
    write(client_sockfd, "login name", 10);
    len = read(client_sockfd, buf , 512);
    buf[len] = '\0';
    strcpy(clients_name[fd_index],buf);

 	write(client_sockfd, "talk to who", 11);

    while(1)
    {
    	//一般是在select之后使用ioctl,可以用来检测缓冲区多少字节。 所以这里不是很适用。
    	//ioctl(client_sockfd, FIONREAD, &nread);//取得数据量交给nread
    	//printf("nread %d\n", nread);
   		 /*客户数据请求完毕，关闭套接字，从集合中清除相应描述符 */

    	len = read(client_sockfd, buf, 512);

	    if(len <= 0) 
	    { 
	    	int talk_to_fd;
	    	int talk_to_fd_index;
	    	talk_to_fd = clients_sock_map[fd_index];
	    	if(talk_to_fd != -1) //talking to somoone
	    	{	    	
	    		talk_to_fd_index = findClient(talk_to_fd);
	    		clients_sock_map[talk_to_fd_index] = -1;
	    		char msg[128];
	        	sprintf(msg, "%s is offline. choose other one to talk" , clients_name[fd_index]);
	        	write(talk_to_fd, msg, strlen(msg));//give message to someone who talk to
	    	}
	    	clients_sock_fds[fd_index] = -1;
	    	clients_sock_map[fd_index] = -1;
	    	clients_name[fd_index][0] = '\0';
	    	
	        close(client_sockfd); 
	        
	        client_num--;
	        printf("removing client on fd %d\n", client_sockfd);
	        break;
	    }
	    else
	    {
	    	//发送给通信的client
	    	if(clients_sock_map[fd_index] != -1)
	    	{   
	    		printf("%d resending %d...\n",fd_index,clients_sock_map[fd_index]);                 		
	            buf[len] = '\0';
	            char msg[512];
	            sprintf(msg, "%s says: %s", clients_name[fd_index] ,buf);

	            write(clients_sock_map[fd_index],msg,strlen(msg));
	    	}
	    	else
	    	{
	    		//提示用户通信的对象
	    		//talk to someone
	        	buf[len] = '\0';
	        	printf("%s\n",buf);
	        	int talk_to_fd_index = findPeople(buf);

	        	if(talk_to_fd_index < 0)
	        	{
	        		char msg[256];
	        		sprintf(msg, "%s is offline. choose other one to talk" , buf);
	        		write(client_sockfd, msg, strlen(msg));
	        	}
	        	else
	        	{
	        		clients_sock_map[fd_index] = clients_sock_fds[talk_to_fd_index];
	        		clients_sock_map[talk_to_fd_index] = client_sockfd;
	        		char msg[256];
	        		sprintf(msg, "connecting %s success.You can talk now!" , clients_name[talk_to_fd_index]);
	        		write(client_sockfd, msg, strlen(msg));
	        	}
	    	}
	    }
    } 
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("usage: talk_server port\n");
		return -1;
	}

	memset(clients_sock_fds, -1, sizeof(clients_sock_fds));
	memset(clients_sock_map, -1, sizeof(clients_sock_map));


	int server_sock_id;
	struct sockaddr_in server_addr;
	server_sock_id = socket(PF_INET, SOCK_STREAM, 0);

	if(server_sock_id < 0)
	{
		printf("socket error\n");
		return -1;
	}
		

	bzero(&server_addr, sizeof(server_addr));

	//server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));
	server_addr.sin_family = AF_INET;

	if(bind(server_sock_id , (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("bind error\n");
		return -1;
	}

	if(listen(server_sock_id , MAX_CONNECT) < 0)
	{
		printf("listen error\n");
		return -1;
	}

    while(1)
    {
    	printf("server waiting\n"); 
 		struct sockaddr_in client_address;
        int client_len = sizeof(client_address); 
 		int client_sockfd = accept(server_sock_id,(struct sockaddr *)&client_address, &client_len); 

        if(client_sockfd < 0)
        {
        	printf("accept error\n");
        	continue;
        }
        
        printf("accept client :%s %d  and adding client on fd %d\n", inet_ntoa(client_address.sin_addr),ntohs(client_address.sin_port),client_sockfd);

        int available_index = findAvailable();

        if(available_index < 0)
        {
        	printf("people full\n");
        	close(client_sockfd);
        	continue;
        }
        clients_sock_fds[available_index] = client_sockfd;
       
        client_num++;
        pthread_t tid;
        int *arg = (int*)malloc(sizeof(int));
        *arg = client_sockfd;
 		pthread_create(&tid, NULL, client_func, arg);
    } 

    close(server_sock_id);

	return 0;
}