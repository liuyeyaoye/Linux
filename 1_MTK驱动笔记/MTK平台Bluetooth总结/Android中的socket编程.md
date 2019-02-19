

```c

平台：MT8321
Android版本：Android L

一、Android中的socket
1、在bt_socket.h中有2个接口：


bt_status_t (*listen)(btsock_type_t type, const char* service_name, const uint8_t* service_uuid, int channel, int* sock_fd, int flags);

bt_status_t (*connect)(const bt_bdaddr_t *bd_addr, btsock_type_t type, const uint8_t* uuid, int channel, int* sock_fd, int flags);


（1）先看listen接口：
这个接口用于通过socket接收数据。
listen会创建并监听一个socket，fd保存在指针sock_fd中。
如果有远程设备连接到了这个socket，read sock_fd时会读到远程设备的蓝牙地址。recvmsg sock_fd然后解析CMSG后会读到一个新的sockFd，此时read sockFd时会读到远程设备发来的数据。

（2）再看connect接口：
这个接口用于通过socket发送数据。
connect会连接指定的远程设备address，如果连接成功，会返回一个fd保存在sock_fd中。此时write 数据到sock_fd即可将数据传输到远程设备。

（3）在MTK-Android中，listen和connect接口都是在MTK的蓝牙协议栈bluetooth.blueangel.so中实现的。（不开源）

（4）在普通的socket编程中：
服务端有：socket()，bind()，listen()，accept()，recv()这些流程。
客户端有：socket()，connect()，send()这些流程。
但是在MTK-Android中，服务端只有listen，客户端只有connect，对socket的具体操作如accept()等都被这2个接口封装好了。

2、socket代码实现


BluetoothOppService.java
	startSocketListener
		mSocketListener.start(mHandler);
		
BluetoothOppRfcommListener.java
	public synchronized boolean start(Handler callback)
		 mBtServerSocket = mAdapter.listenUsingInsecureRfcommWithServiceRecord("OBEX Object Push", BluetoothUuid.ObexObjectPush.getUuid());
			createNewRfcommSocketAndRecord(name, uuid, false, false);
				socket.mSocket.bindListen();
					createSocketChannel(mType, mServiceName, mUuid, mPort, getSecurityFlags());
						com_android_bluetooth_btservice_AdapterService.cpp：createSocketChannelNative(type, serviceName, Utils.uuidToByteArray(uuid), port, flag);
		clientSocket = sSocket.accept();
			private BluetoothSocket acceptSocket(String RemoteAddr) throws IOException
			//注意这里并没有调用socket的accept函数。


3、关于读写数据的函数：read/write/send/recv/readv/writev/sendmsg/recvmsg
（1）read/write
int read(int fd, void *buf, int nbyte);
int write(int fd, const void *buf, int nbyte);

（2）send/recv
int send( SOCKET s, const char *buf, int len, int flags);
int recv( SOCKET s, char *buf, int len, int flags);

send/recv与read/write函数的区别在于第四个参数flags，如果flags=0，功能与read/write相同。
常用的flag为：MSG_NOSIGNAL，禁止向系统发送异常消息。

（3）readv/writev
用于一次读写多个缓冲区。
ssize_t readv(int filedes, const struct iovec *iov, int iovcnt);
ssize_t writev(int filedes, const struct iovec *iov, int iovcnt);

（4）sendto/recvfrom
指定地址发送/接收数据
无连接的数据报socket方式下，由于本地socket并没有与远端机器建立连接，所以在发送数据时应指明目的地址。
int sendto（int sockfd， const void *msg，int len, unsigned int flags， const struct sockaddr *to， int tolen）； 
int recvfrom（int sockfd，void *buf，int len，unsigned int flags，struct sockaddr *from，int *fromlen）； 

（5）sendmsg/recvmsg
代码位置：frameworks\base\core\jni\android_net_LocalSocketImpl.cpp
它可以发送/接收附属数据。

先看sendmsg：
int sendmsg(int s, const struct msghdr *msg, unsigned int flags);


struct msghdr {
    void         *msg_name;
    socklen_t    msg_namelen;
    struct iovec *msg_iov;
    size_t     msg_iovlen;
    void       *msg_control;
    size_t     msg_controllen;
    int        msg_flags;
};
（1）成员：
套接口地址成员：msg_name和msg_namelen。
I/O向量引用msg_iov与msg_iovlen。
附属数据缓冲区成员msg_control与msg_controllen。
接收信息标记位msg_flags。

（2）成员msg_name与msg_namelen：
这些成员只有当我们的套接口是一个数据报套接口时才需要。
msg_name成员指向我们要发送或是接收信息的套接口地址。
msg_namelen指明了这个套接口地址的长度。
蓝牙发送/接收数据不需要填充它。

（3）成员msg_iov与msg_iovlen：
这些成员指定了我们的I/O向量数组的位置以及他包含多少项。
msg_iov指向一个struct iovec数组。
msg_iovlen指明了在我们的I/O向量数组中有多少元素。
这个成员一般用来保存发送/接收的数据缓冲区和长度。

它一般这样赋值：
uint8  buffer=0;
struct iovec iv;
memset(&iv, 0, sizeof(iv));
iv.iov_base = &buffer;
iv.iov_len = 1;
msghdr.msg_iov = &iv;
msghdr.msg_iovlen = 1;

（4）成员msg_control与msg_controllen
这些成员指向了我们附属数据缓冲区并且表明了缓冲区大小。
msg_control指向附属数据缓冲区。
msg_controllen指明了缓冲区大小。

（5）成员msg_flags
MSG_EOR：当接收到记录结尾时会设置这一位。这通常对于SOCK_SEQPACKET套接口类型十分有用。
MSG_TRUNC：这个标记位表明数据的结尾被截短，因为接收缓冲区太小不足以接收全部的数据。
MSG_CTRUNC：这个标记位表明某些控制数据(附属数据)被截短，因为缓冲区太小。
MSG_OOB：这个标记位表明接收了带外数据。
MSG_ERRQUEUE：这个标记位表明没有接收到数据，但是返回一个扩展错误。

用的较多的msg_flags为MSG_NOSIGNAL，含义为：
Linux 下当连接断开，还发送数据的时候，不仅 send() 的返回值会有反映，而且还会向系统发送一个异常消息。
如果不作处理，程序会退出，这对于服务器提供稳定的服务将造成巨大的灾难。
为此，send() 函数的最后一个参数可以设置为 MSG_NOSIGNAL，禁止 send() 函数向系统发送常消息。


再看recvmsg：
int recvmsg(int s, struct msghdr *msg, unsigned int flags);
跟sendmsg类似。

（4）CMSG附属数据
访问辅助数据： CMSG_ALIGN, CMSG_SPACE, CMSG_NXTHDR, CMSG_FIRSTHDR

附属数据并不是套接字负载的一部分。这些控制消息可能包含接收数据包的接口、很少使用的各种报头字段、额外的错误描述、一组文件描述符和Unix凭据。
举例来说，控制消息可以用于发送额外的报头字段，诸如IP选项。辅助数据通过调用sendmsg发送，通过调用recvmsg接收。

CMSG操作：


#include <sys/socket.h>

struct cmsghdr *CMSG_FIRSTHDR(struct msghdr *msgh);
struct cmsghdr *CMSG_NXTHDR(struct msghdr *msgh, struct cmsghdr *cmsg);
size_t CMSG_ALIGN(size_t length);
size_t CMSG_SPACE(size_t length);
size_t CMSG_LEN(size_t length);
unsigned char *CMSG_DATA(struct cmsghdr *cmsg);

struct cmsghdr {
    socklen_t cmsg_len;    /* data byte count, including header */
    int       cmsg_level;  /* originating protocol */
    int       cmsg_type;   /* protocol-specific type */
    /* followed by unsigned char cmsg_data[]; */
};

CMSG_FIRSTHDR()：返回msghdr关联的辅助数据中的第一个cmsghdr的指针。
CMSG_NXTHDR()：返回参数指代的cmsghdr之后的下一个有效的cmsghdr。当缓冲区中没有足够的空间时，它返回NULL。
CMSG_ALIGN()：给定一个长度，返回它所必须的对齐边界。
CMSG_SPACE()：返回一个辅助数据所占用的字节数。
CMSG_DATA()：返回指向cmsghdr的数据部分的指针。
CMSG_LEN()：返回cmsghdr结构的cmsg_len成员的值，并考虑任何必要的对齐方式。 它将数据长度作为参数。


sendmsg使用CMSG辅助数据发送一组文件描述符及数据的例子：


int write_native(int fd, const void *buf, int nbyte)
{
    struct msghdr msg;
    unsigned char *buffer = (unsigned char *)buf;
    memset(&msg, 0, sizeof(msg));

    struct cmsghdr *cmsg;
    int countFds =3;
    int fds[countFds]={23,24,25};
    char msgbuf[CMSG_SPACE(countFds)];

    msg.msg_control = msgbuf;//通过附属数据发送文件符
    msg.msg_controllen = sizeof(msgbuf);
    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(fds));
    memcpy(CMSG_DATA(cmsg), fds, sizeof(fds));

    //仅仅只在第一次write的时候发送附属数据文件符
    while (nbyte> 0){
        struct iovec iv;//通过iv来发送数据
        memset(&iv, 0, sizeof(iv));
        iv.iov_base = buffer;
        iv.iov_len = len;
        msg.msg_iov = &iv;
        msg.msg_iovlen = 1;

        do{
            ret = sendmsg(fd, &msg, MSG_NOSIGNAL);
        } while (ret < 0 && errno == EINTR);

        buffer += ret;
        nbyte -= ret;

        //清空了msg，所以附属数据 msg.msg_control 只发送一次
        memset(&msg, 0, sizeof(msg));
    }
    return 0;
}


recvmsg使用CMSG接收一组文件描述符的例子：


这个函数先接收保存数据和文件描述符，然后解析出文件描述符。

int read_native(int fd, void *buffer, int nbyte)
{
    ssize_t ret;
    ssize_t bytesread = 0;
    struct msghdr msg;
    struct iovec iv;
    uint8_t *buf = (uint8_t *)buffer;
    // Enough buffer for a pile of fd's. We throw an exception if this buffer is too small.
    struct cmsghdr cmsgbuf[2*sizeof(cmsghdr) + 0x100];//用于保存辅助数据文件描述符

    memset(&msg, 0, sizeof(msg));
    memset(&iv, 0, sizeof(iv));

    iv.iov_base = buf;//数据保存在buf中
    iv.iov_len = nbyte;

    msg.msg_iov = &iv;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);

    do {
        ret = recvmsg(fd, &msg, MSG_NOSIGNAL);
    } while (ret < 0 && errno == EINTR);

    if (ret < 0 && errno == EPIPE) {
        // Treat this as an end of stream
        return 0;
    }

    if (ret < 0) {
        return -1;
    }

    if ((msg.msg_flags & (MSG_CTRUNC | MSG_OOB | MSG_ERRQUEUE)) != 0) {
        // To us, any of the above flags are a fatal error
        return -1;
    }

    if (ret >= 0) {
        socket_process_cmsg(&msg);
    }
    return ret;
}

//处理附属数据
static int socket_process_cmsg(struct msghdr * pMsg)
{
    struct cmsghdr *cmsgptr;

    for (cmsgptr = CMSG_FIRSTHDR(pMsg); 
          cmsgptr != NULL; cmsgptr = CMSG_NXTHDR(pMsg, cmsgptr)) {

        if (cmsgptr->cmsg_level != SOL_SOCKET) {
            continue;
        }

        if (cmsgptr->cmsg_type == SCM_RIGHTS) {
            int *pDescriptors = (int *)CMSG_DATA(cmsgptr);
            int count = ((cmsgptr->cmsg_len - CMSG_LEN(0)) / sizeof(int));
            if (count < 0) {
                return -1;
            }
			
	    int *fd = (int *)malloc(sizeof(int)*count);
            if (fd == NULL) {
                return -1;
            }

            for (int i = 0; i < count; i++){
				fd[i]=pDescriptors[i];
            }
        }
    }
    return 0;
}

```




