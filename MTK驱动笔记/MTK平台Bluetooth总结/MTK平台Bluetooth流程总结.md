

```c

平台：MT8321
Android版本：Android L
蓝牙协议栈：blueangel

一、xxxx项目蓝牙流程(参考MTK-Android原生版本流程实现)
1、客户端（通过socket发送数据）


int OSSocketInitClientBt(void)//xxxx
{
	int ret;

	OSSockCtl(0, BT_HW_ON, 0, 0);
	OSSockCtl(0, BT_INQUIRY_START, 0, 0);

    //服务端蓝牙地址
	uint8_t addr[6]={0x22,0x22,0xa9,0x65,0x80,0x01};

	TW_SockAddr TwBdAddr;
	TwBdAddr.Addr.bdAddr.BD_ADDR0=addr[0];
	TwBdAddr.Addr.bdAddr.BD_ADDR1=addr[1];
	TwBdAddr.Addr.bdAddr.BD_ADDR2=addr[2];
	TwBdAddr.Addr.bdAddr.BD_ADDR3=addr[3];
	TwBdAddr.Addr.bdAddr.BD_ADDR4=addr[4];
	TwBdAddr.Addr.bdAddr.BD_ADDR5=addr[5];

	ret = OSConnect (0, &TwBdAddr, -1);
	if (ret < 0) {
		ALOGE("creat file descriptor on socket fd Fail: ret=%d",ret);
		return -1;
	}
	else{
		ALOGE("creat file descriptor on socket fd Sucess: ret=%d",ret);
	}

	#define SEND 10
	int len;
	uint8_t sendbuffer[SEND]={0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee};
	len = OSSend (0, sendbuffer, SEND, OSSocketFlagsNone);
	if(len<0){
		ALOGE("fail to send data : len=%d",len);
		return -1;
	}else if(len == SEND){
		ALOGE("Sucess to send data : len=%d",len);
	}
	sleep(1);
	return 0;
}


2、服务端（接收客户端发来的数据）


int OSSocketInitServerBt(void)//printer
{
	int ret,i;

	OSSockCtl(0, BT_HW_ON, 0, 0);

	int sock;
	sock = OSSocket (OSSocketBT, OSSocketStream, OSSocketBTRFCOMM);

	OSSockCtl(0, BT_INQUIRY_START, 0, 0);

	ALOGW("accept sock , server create socket fd:%d\n",sock);

	int sock2=-1;
	OSListen(sock,10);
	sock2 = OSAccept (sock, 0);
	if(sock2==-1)    return -1;

	#define RECV 10
	int len,j;
	uint8_t revcbuf[RECV];
	len = OSRecv (sock2, revcbuf, RECV, OSSocketFlagsNone);
	for(j=0;j<RECV;j++){
		ALOGE("accept sock, read socket data: %x",revcbuf[j]);
	}
	return 0;
}


二、MTK-Android蓝牙流程

1、蓝牙配对流程


classInitNative();
initNative();
getAdapterPropertyNative(adapter_name);
getAdapterPropertyNative(adapter_address);
configHciSnoopLogNative(0);
enableNative();
setAdapterPropertyNative(scan_mode);//2
setAdapterPropertyNative(discovery_timeout);//120s
createSocketChannelNative(000eSMS/MMS);
startDiscoveryNative();
createSocketChannelNative(OBEX Phonebook Access Server);
createSocketChannelNative(OBEX Object Push);
sBluetoothInterface->remove_bond(address);
sBluetoothInterface->cancel_discovery();
sBluetoothInterface->create_bond(address, transport);
sspReplyNative();


2、蓝牙发送数据流程
在蓝牙配对之后，每次发送文件，调用：
startDiscoveryNative();//搜索发现设备
cancelDiscoveryNative();//停止搜索
connectSocketNative();//连接设备
即可。

3、蓝牙接收数据流程
（1）在MTK-Android中，本设备在蓝牙打开后，就会调用createSocketChannelNative()创建socket，然后listen它。
当有其他设备向它发送文件时，本设备接收文件即可。

（2）所以在MTK-Android蓝牙中，不存在客户端和服务端，每个机器在打开蓝牙后，就是一个服务端，等待其他设备的连接。
相反，当它想要给其他设备传输文件时，调用connect()即可。
每传输一次文件，都需要调用一次connect()接口。

（3）蓝牙传输文件使用的socket服务是：OBEX Object Push

（4）Android蓝牙只创建了3个socket服务，通过uuid来区分：
000eSMS/MMS：通过蓝牙传输短信和彩信。uuid：0x1132
OBEX Phonebook Access Server：电话号码簿访问协议PBAP，通过蓝牙共享电话号码。uuid:0x112f
OBEX Object Push：对象推送协议，它用于资源在设备间的共享传输，比如蓝牙传输文件和音乐。uuid:0x1105

```


