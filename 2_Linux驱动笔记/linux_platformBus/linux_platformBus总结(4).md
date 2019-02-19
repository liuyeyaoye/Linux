

Linux PlatformBus 总结


1、如何使用platform_driver？

在模块初始化函数中使用：platform_driver_register()注册一个platform_driver。

至少要填充它的成员probe，remove，driver.name，driver.owner，driver.of_match_table。

of_match_table里面的.compatible 和在dts中定义的要相同。


2、platform_driver_register这个函数有哪些功能？

（1）填充driver的成员，如bus、probe、remove

（2）调用driver_register()注册这个driver

（3）注册一个platform_driver，实际上仅仅把它和platformBus联系起来，然后调用driver_register()注册一个driver，核心还是在driver_register()。



3、platformBus怎么匹配platform_driver和platform_device的？

在driver_register()中，会调用drv->bus->match(dev, drv)来匹配device和driver，即调用platform_match函数，一般用到这个函数的2种匹配方式：

（1）基于of匹配：比较device和driver的compatible属性值是否相同。

（2）基于name匹配：比较platform_device.name 和 platform_driver.name是否相同。



4、platformBus这条总线在哪里定义和注册的？

在内核初始化的时候，会调用bus_register()注册一条platformBus。bus_register()最重要的功能是给bus的私有数据struct subsys_private分配内存并填充。



5、在基于of的匹配时，我们只定义了一个platform_driver，而没有定义platform_device，那么这个platform_device哪来的？

在基于of的匹配中，是通过 driver->of_match_table 和 device->of_node 来匹配的。

device的成员of_node的类型是struct device_node，它在内核初始化时的unflatten_dt_node()中分配内存并且将dtb的信息解析保存给它。

在优先级为3的模块初始化函数中，platform_device的资源和内存在这个函数中被统一分配：of_platform_populate()。



6、platformBus，platformDevice和platformDriver的初始化

在内核初始化时，platformBus的初始化在最前面：bus_register(&platform_bus_type)。

然后会进入 do_initcalls()：

（1）首先是platformDevice的初始化（arch_initcall_sync中，优先级为3s。sync表示同步，即等待，优先级为3s，比3低）：arch_initcall_sync(of_platform_default_populate_init)。

（2）然后是各种platformDriver的初始化（module_init中，优先级为6）。


