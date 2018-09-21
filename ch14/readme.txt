1.twebserver.c
gcc twebserver.c socklib.c -o twebserv -lpthread

./twebserv 12308

在浏览器中输入
或者
shell中发送http： 可以用wget,curl , telnet(telnet 是基于tcp连接的。 可以模拟多种协议的如http等)
https://zhidao.baidu.com/question/1964590922698291340.html
https://blog.csdn.net/xiongli880612/article/details/40300861
GET 请求：curl http://www.baidu.com/
POST 请求：curl -X POST -d '{"data":"test"}' http://www.baidu.com
指定IP的GET请求：curl -H "Host:www.baidu.com" http://14.215.177.38
如：注意这里用的是域名 rzet.local, 不能直接用127.0.0.1 或localhost。程序这么写的。
curl rzet.local:12308/
curl rzet.local:12308/socklib.c
curl rzet.local:12308/status



telnet rzet.local 12308  // 进入交互页面
GET /
/n
/n



telnet rzet.local 12308  // 进入交互页面
GET socklib.c
/n
/n

telnet rzet.local 12308  // 进入交互页面
GET /status HTTP/1.0
/n
/n
