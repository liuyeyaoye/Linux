Makefile

```c
一、基础
1、 =	:=	?=	+=	的区别
=		：	赋值(将整个Makefile文件展开后，再决定变量的值)
:=		：	赋值(变量的值决定于它在Makefile文件中的位置，而不是整个Makefile文件展开后的最终值)
?=		：	如果没有赋值过，就把 = 之后的值赋给它
+=		：	追加
=	和  :=  的区别：

x=foo
y=$(x) bar
x=xyz
经过上面， y=xyz bar

x:=foo
y:=$(x) bar
x:=xyz
经过上面， y=foo bar

一般使用	:=



二、函数
1、 filter 函数
用法： $(filter	 PATTERN……   ,		TEXT)
作用： 返回 TEXT 中符合 PATTERN 格式的字符串。如果没有符合的，则返回空。

2、 word 函数
用法：$(word  <n> ,    <text> )
作用：取字符串 text  中的第 n  个字符， n  从1开始。

3、 sort 函数
用法：$(sort LIST)
作用：将 LIST 中的字符串按照升序排序，去掉重复的字符串。(以首字母为准比较，首字母相同，以次字母为准)
例子：$(sort 	foo	bar	lose	   foo)
返回值为：	"bar  foo  lose"

4、 strip 函数
用法： $(strip <string> )
作用： 去掉 <string> 字串中开头、中间、结尾的空字符。 

5、 wildcard  函数 ：用来判断存在还是不存在某一路径下的文件
用法： $(wildcard PATTERN...)
作用： 在Makefile中，它被展开为已经存在的、使用空格分开的、匹配此模式的所有文件列表。如果不存在任何符合此模式的文件，函数会忽略模式字符并返回空。
例子： $(wildcard 	$(IMGSENSOR_DRIVER_PATH)/$(MTK_PLATFORM)/camera_project/$(ARCH_MTK_PROJECT)/$(1))	，由于不存在此文件，所以返回空。

6、 eval  函数
用法： $(eval text)
作用： text 的内容将作为makefile的一部分而被make解析和执行

7、 foreach  函数
用法： $( foreach  <var>,  <list>,  <text> )
作用： 
把参数  <list>   中的单词逐一取出放到参数  <var>  所指定的变量中，然后再执行  < text>  所包含的表达式。
每一次  <text>  会返回一个字符串，循环过程中，<text>   的所返回的每个字符串会以空格分隔。
最后当整个循环结束时，  <text>  所返回的每个字符串所组成的整个字符串（以空格分隔）将会是   foreach    函数的返回值。

$( foreach drv, gc2355_mipi_raw gc0310_mipi_yuv gc5025s_mipi_raw  ,  $(eval $(call FILTER_DRV,$(drv)))  )

8、 call  函数
用法： $(  call   <expression>,  <parm1>,  <parm2>,  <parm3>...  )
作用： 
当make执行这个函数时， <expression>  参数中的变量，如$(1)，$(2)，$(3)等，会被参 数 <parm1>，<parm2>，<parm3>依次取代。
<expression>  的返回值就是  call  函数的返回值。

9、 subset 函数
用法： $(subst FROM,	 TO,  TEXT) 
作用： 把字串  TEXT  中的  FROM  字符替换为 TO  
例子： $(subst $\",  ,   $(CONFIG_CUSTOM_KERNEL_IMGSENSOR)  )

10、 define 宏定义
用法： 
define FILTER_DRV
endef
作用： 定义一个操作集合，它不会自动执行，一般通过 call 调用。

define FILTER_DRV
ifeq ($(wildcard $(IMGSENSOR_DRIVER_PATH)/$(MTK_PLATFORM)/camera_project/$(ARCH_MTK_PROJECT)/$(1)), )

ifeq ($(wildcard $(IMGSENSOR_DRIVER_PATH)/$(MTK_PLATFORM)/$(1)), )
common_drv += ../common/$(COMMON_VERSION)/$(1)/					/* 这里是 common 目录下的 camera 配置 */
else
platform_drv += $(1)/				/* 这里是 mt6765 目录下的 camera 配置 */	
endif

else
project_drv += ./camera_project/$(ARCH_MTK_PROJECT)/$(1)/       /* 这里是 camera_project 目录下的 camera 配置 */
endif

endef



三、例子(mt6739平台 imgsensor 驱动)

define FILTER_DRV
ifeq ($(wildcard $(IMGSENSOR_DRIVER_PATH)/$(MTK_PLATFORM)/camera_project/$(ARCH_MTK_PROJECT)/$(1)),)

ifeq ($(wildcard $(IMGSENSOR_DRIVER_PATH)/$(MTK_PLATFORM)/$(1)),)
common_drv += ../common/$(COMMON_VERSION)/$(1)/
else
platform_drv += $(1)/
endif

endif
endef


$(foreach drv,$(subst $\",,$(CONFIG_CUSTOM_KERNEL_IMGSENSOR)),$(eval $(call FILTER_DRV,$(drv))))

 ——>  $( foreach drv, gc2355_mipi_raw gc0310_mipi_yuv gc5025s_mipi_raw  ,  $(eval $(call FILTER_DRV,$(drv)))  )

含义：
如果在 imgsensor/src/mt6739/  这个目录没有配置的 imgsensor ，
则使用 imgsensor/src/common/v1/  中的 imgsensor 。
在 imgsensor/src/common/v1/  中，使用的是与 platform 无关的 imgsensor 。
在 imgsensor/src/mt6739/ 中，使用的是与 platform 相关的 imgsensor 。
```


