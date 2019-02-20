
```c
一、memset的一个错误例子

char main_info[1024];
char * info=main_info;

memset(main_info,0,sizeof(main_info));//清空数组，正确使用了memset。

info += sprintf(info, "%s%ds\n", "start camera test,test time:",test_time);
info += sprintf(info, "%s%ds\n","camera test pass, test time:", use_time);
info += sprintf(info, "%s%d\n","camera test count:", count);

memset(info,0,sizeof(char)*1024);
//程序运行时，这里会产生AEE错误，卡死。出错的原因是因为info指向的不是首地址了，它指向的size没有1024.

1、错误的第二个原因：
字符数组是可以被修改的，所以对它可以使用memset来清空。
字符串常量是只读的，它不能被修改，而memset又必须修改info，所以会产生错误。
经过测试，上面这个说法是错误的。因为snprintf进行的是拷贝操作，将字符串常量赋值给数组保存起来，memset是对数组进行操作，不会修改字符串常量。

（1）char str1[] = "mediatek_camera_test";
str1是一个字符数组，它保存的是一个个的字符，可以对str1使用memset操作。实际上是将常量区的字符串拷贝到全局/局部变量数组中保存起来。

（2） char *str2 = "mediatek_camera_test";
str2保存的是一个字符串常量，不能对str2使用memset，因为它保存的是一个字符串常量，只读的，不允许被修改。实际上str2保存的是常量区字符串的首地址，不允许修改它指向的内容。

二、sprintf函数
int sprintf( char *buffer, const char *format, [argument] …);
功能：把格式化的数据写入某个字符串缓冲区。
返回值：
错误：返回-1；
成功：返回成功写入buffer 的字节数，结束字符‘\0’不计入内。
使用方法：

char	main_info[1024];
char * info=main_info;
memset(main_info,0,sizeof(main_info));//清空数组，正确使用了memset。

info += sprintf(info, "%s%ds\n", "start camera test,test time:",test_time);
info += sprintf(info, "%s%ds\n","camera test pass, test time:", use_time);
info += sprintf(info, "%s%d\n","camera test count:", count);


三、关于字符串的 '\0'
1、字符数组和字符串常量都是以'\0'字符来结尾。

```



