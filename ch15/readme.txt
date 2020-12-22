1.
talk_server.c talk_client.c
功能：服务器和客户端进行通信。 
服务器在开启，等待客户端的链接。 当连接成功后，客户端和服务器端就可以进行对话。 
如果双方都没有任何应答，那么等待一段时间后会关闭连接。

这个功能的问题点：
一般的服务器端和客户端的交流都是一来一往。
只有接收到客户端/服务端 消息后，服务端/客户端再做出响应。而且如果现在正在响应，那么就无法处理发来的请求。就是说会阻塞等待请求或者响应结束之后才能进行下一步。
这个版本的talk_server / talk_client，不用阻塞在发请求和响应上。 用了select，当请求或响应中有一者准备好了，那么就可以进行处理。不用阻塞在请求或者响应上。服务器和客户端 不用等待对方，可以让自己的操作同时进行。

除了select方法，也可以使用非阻塞的方式。不过这个就是比较浪费cpu，见《Unilx Linux 编程实践》P465 15.2.1 15.2.2。


2.
talk_server_manage_select.c 和 talk_client_1.c  
talk_server_manage_thread.c 和 talk_client_1.c
运行：  ./talk_server_manage_select  12410
	   ./talk_client1 localhost 12410 或 ./talk_client1 127.0.0.1 12410 或 ./talk_client1 rzet.local 12410 (最后一个是域名，本机域名)

主要是客户端和服务端的socket的send 和receive 如何共同并发处理，即send 和 receive同时起作用，而不是两者串行。 a.select 的方式(多路复用机制)。b.多线程方式。（receive一直在一个线程中while，send 在需要的时候发送）。  以上方式原理就是让send 和 receive能同时起作用。
socket是全双工。有同时发送和接收的能力。 而程序中要同时用发送和接收，那么就要多线程／或多路复用。（因为单线程的话，接收和发送是串行的，没办法同时的接收，发送。一般双方通信，都是要求能同时接收，发送的，因此一般都是会用到多线程）。
select：https://www.cnblogs.com/alantu2018/p/8612722.html。
一个句柄都有可写 可读的状态。可写 表示该句柄可以被写入（句柄写处于空闲就表示可写，如果多个线程竞争使用写句柄，那么对于某个线程而言，这个句柄的写就处于不可用）， 可读 表示该句柄有内容可读（有内容就表示可读的状态）。


这个模型：
客户端和客户端通信。服务器端作为中间沟通的桥梁。客户端们都和服务端链接。
这样就可以避免两者通信时，需要一方以server形式存在，要一直监听连接，等待另一个客户端的连接。

方法A:服务器端使用select。  talk_server_manage_select.c
方法B：服务器端使用多线程。 talk_server_manage_thread.c
使用多线程更简单些。

某个客户端发送给所有客户端就是群聊/聊天室。
某个客户端只发送给连接的客户端就是单聊。


3.
websocket怎么做的？websocket也是用socket ，select 做的。群聊 给所有客户端广播。 单聊 给单个客户端发送。
web socket 主要还是利用socket实现。 客户端 服务器端 使用websocket 协议。 
这里服务器端使用php。使用的是php的API socket, select，使用原生的socket要处理websocket协议。 客户端是js，使用websock api。


//3.java 怎么做select。 Java用NIO: 单线程管理多个连接。 linux C中select 可用与非阻塞，也可用于阻塞。 java中的NIO是用于非阻塞的。 java中的selector是对channel进行检测。linux c中select是对fd进行检测。
//tomcat 一个连接就是一个线程，容器怎么管理application context 。单例模式，一个web应用就一个ServletContex，单例。 所有的请求都使用同一个ServletContex 实例。
//tomcat 对于多个请求 可能是BIO(用多线程) ，也有可能是NIO，
//集群和分布式。


AWS 上使用 cookie的，代码是客户端 还是服务器端的。代码是PHP，既有客户端也有服务端操作cookie的。 
cookie可以被服务端操作(如服务端的脚本jsp php Servlet)，也可以被客户端操作(如客户端的脚本js).
session 是服务端技术，由服务端操作。 客户端不能操作，客户端最多只能获得存在cookie中的sessionid。


