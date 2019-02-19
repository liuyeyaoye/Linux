

```c
一、蓝牙服务端读不到数据的问题：

1. 在客户端，调用：


//socke=23
socket = bt_status_t (*connect)(const bt_bdaddr_t *bd_addr, btsock_type_t type, const uint8_t* uuid, int channel, int* sock_fd, int flags);
lens=write(socket, (char *)sendbuffer, SEND);


2. 在服务端，调用：


sock = bt_status_t (*listen)(btsock_type_t type, const char* service_name, const uint8_t* service_uuid, int channel, int* sock_fd, int flags);
lens=read(sock, recvbuffer, MAX);  	读到了客户端的地址：10 00 11 11 B6 65 80 01 19
lens=read(27, recvbuffer, MAX); 	读到了客户端发来的数据：55 66 77 88 99


3. 查看log：


I/        (  236): btmtk_jsr82_int_compose_record: name=OBEX Object Push, channel=23 
E/Bluetooth(  236): accept socket3 fd = 23
I/        (  236): [JSR82][EXT]jbt_session_attach fd:27
I/        (  236): [JSR82] jbt_jsr82_channel_connected: channel: 23, session fd[24], subsessionfd[27]
I/        (  236): [session][stream]write_control_socket, fd 24, ctl 27, len 4, datalen 16, CMSG_SPACE(1)[16]


4.总结
(1)这个数据fd=27是变化的，它不一定就是ctl的那个fd。
(2)客户端在成功connect到服务端时，会给服务端发送一个sendmsg，其中就包含了数据fd。
(3)可以在服务端使用recvmsg来监听客户端发来的数据fd，然后read它，就可以得到客户端发给服务端的数据了。

```

