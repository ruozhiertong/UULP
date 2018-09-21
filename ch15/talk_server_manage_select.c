#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <sys/ioctl.h> 
 
#define SERVER_IP	"127.0.0.1"
#define TIMEOUT_SEC	5
#define MAX_CONNECT	10

//extern int errno;  //最好用include <sys/errno.h>

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


int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("usage: talk_server port\n");
		return -1;
	}

	

	memset(clients_sock_fds, -1, sizeof(clients_sock_fds));
	memset(clients_sock_map, -1, sizeof(clients_sock_map));

	int maxfd = 0;
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

	fd_set client_fds_set, tmp_fds_set;
	int result;
	int nread;
	char buf[512];
	int len;

	FD_ZERO(&client_fds_set);
	FD_SET(server_sock_id,&client_fds_set);
	maxfd = maxfd > server_sock_id? maxfd : server_sock_id;

    while(1)
    {

 
 		tmp_fds_set = client_fds_set;//将需要监视的描述符集copy到select查询队列中，select会对其修改，所以一定要分开使用变量 
        printf("server waiting\n"); 
        /*无限期阻塞，并测试文件描述符变动 */
        //FD_SETSIZE：系统默认的最大文件描述符
        result = select(maxfd + 1, &tmp_fds_set, (fd_set *)0,(fd_set *)0, (struct timeval *) 0); 

        if(result < 0)
        {
        	printf("select error\n");
        	return -1;
        }

        /*扫描所有的文件描述符*/
        for(int fd = 0; fd < maxfd + 1; fd++) 
        {
            /*找到相关文件描述符*/
            if(FD_ISSET(fd,&tmp_fds_set)) 
            { 
              /*判断是否为服务器套接字，是则表示为客户请求连接。*/
                if(fd == server_sock_id) 
                { 
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
                    	continue;
                    }




                    clients_sock_fds[available_index] = client_sockfd;
                    
                    //require name;
                    write(client_sockfd, "login name", 10);
                    len = read(client_sockfd, buf , 512);
                    buf[len] = '\0';
                    strcpy(clients_name[available_index],buf);

                    write(client_sockfd, "talk to who", 11);

                    client_num++;
                    FD_SET(client_sockfd, &client_fds_set);//将客户端socket加入到集合中
                    maxfd = maxfd > client_sockfd? maxfd : client_sockfd;

                } 
                /*客户端socket中有数据请求时*/
                else 
                { 
                	int fd_index = findClient(fd);
                	printf("index %d, fd %d\n",fd_index, fd);
                    ioctl(fd, FIONREAD, &nread);//取得数据量交给nread
                    
                    /*客户数据请求完毕，关闭套接字，从集合中清除相应描述符 */
                    if(nread == 0) 
                    { 
                    	int talk_to_fd = clients_sock_map[fd_index];
                    	int talk_to_fd_index = findClient(talk_to_fd);//talk to
                    	if(talk_to_fd_index >=0)
                    		clients_sock_map[talk_to_fd_index] = -1;

						char msg[128];
                        sprintf(msg, "%s is offline. choose other one to talk" , clients_name[fd_index]);
                    	clients_sock_fds[fd_index] = -1;
                    	clients_sock_map[fd_index] = -1;
                    	clients_name[fd_index][0] = '\0';

                        close(fd); 
                        FD_CLR(fd, &client_fds_set); //去掉关闭的fd
                        
                        client_num--;
                        printf("removing client on fd %d\n", fd); 

                        write(talk_to_fd, msg, strlen(msg));//give message to someone who talk to

                    } 
                    /*处理客户数据请求*/
                    else 
                    {
                    	//发送给通信的client
                    	if(clients_sock_map[fd_index] != -1)
                    	{   
                    		printf("%d resending %d...\n",fd_index,clients_sock_map[fd_index]);                 		
	                        len = read(fd, buf, 512);
	                        buf[len] = '\0';
	                        char msg[512];
	                        sprintf(msg, "%s says: %s", clients_name[fd_index] ,buf);

	                        write(clients_sock_map[fd_index],msg,strlen(msg));
                    	}
                    	else
                    	{
                    		//提示用户通信的对象
                    		//talk to someone
	                    	len = read(fd, buf , 512);
	                    	buf[len] = '\0';
	                    	printf("%s\n",buf);
	                    	int talk_to_fd_index = findPeople(buf);

	                    	if(talk_to_fd_index < 0)
	                    	{
	                    		char msg[256];
                        		sprintf(msg, "%s is offline. choose other one to talk" , buf);
	                    		write(fd, msg, strlen(msg));
	                    	}
	                    	else
	                    	{
	                    		clients_sock_map[fd_index] = clients_sock_fds[talk_to_fd_index];
	                    		clients_sock_map[talk_to_fd_index] = fd;
	                    		char msg[256];
                        		sprintf(msg, "connecting %s success.You can talk now!" , clients_name[talk_to_fd_index]);
	                    		write(fd, msg, strlen(msg));
	                    	}
	                    
                    	}
                    } 
                } 
            } 
        }
    } 

    close(server_sock_id);

	return 0;
}