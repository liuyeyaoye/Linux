

linux bus，driver，device总结

1、bus，driver，device的注册先后顺序为：

（1）对于platformBus：

首先是内核初始化注册一条platform_bus：

bus_register(&platform_bus_type)，这是bus；

然后是解析device_node，初始化platform_device：of_platform_default_populate_init()，这是device；

然后就是各种platform_driver的初始化，例如：platform_driver_register(&mt_i2c_driver)，这是driver。

（2）对于i2cBus(这里指的是i2c client，而不是i2c adapter，后者挂在platformBus上)：

首先在i2c-core.c中注册一条i2cBus：bus_register(&i2c_bus_type)，这是bus。

然后在i2c-vendor.c中，会调用i2c_register_adapter()，这个函数里面会调用of_i2c_register_devices()，在这个函数里面进行i2c client设备的注册，这是device。

最后是driver，分2种情况：

i2c总线driver基于platform_driver_register来实现：platform_driver_register(&mt_i2c_driver)。

i2c slave设备driver基于i2c_register_driver来实现：i2c_add_driver(&tpd_i2c_driver)。

2种情况下，它们都会调用driver_register来与device匹配。



2、bus，driver和device各自的结构体中，都会有一个私有数据结构，主要成员都是kset，kobject，klist。bus结构体包含kset，klist；driver结构体包含kobject，klist，klist_node。device结构体包含kobject，klist，klist_node。

3、关注一下driver_register()这个函数，它会循环遍历每一个device，然后调用drv->bus->match(dev, drv)来匹配它自己也就是driver和device。

如果它跟一个device匹配，就会调用drv->probe(dev)函数。当probe函数执行完成后，会继续寻找next_device()，如果还是能够匹配，继续调用drv->probe(dev)。

所以，一个driver可以对应多个device，一个driver的probe函数可以执行多次。

这个特性常用于i2c总线驱动中。例如在dts中i2c0—i2c3的匹配节点都一样：compatible = "vendor,i2c"，但是只注册了一个i2c总线driver：platform_driver_register(&mt_i2c_driver)，只不过这个driver的probe函数会被调用4次。


4、在device_register()函数中，会调用bus_add_device(dev)将这个device跟bus联系起来（将这个device的klist_node链表添加到bus的klist链表尾部）。

它还会调用bus_probe_device(dev)，试图匹配driver。但是这个时候driver还没有添加到bus上，所以这个操作没有作用。


5、在driver_register()中，也会调用 klist_add_tail 将这个driver跟bus联系起来（将这个driver的klist_node链表添加到bus的klist链表尾部）。

6、在driver_register()函数的really_probe()函数中：

首先判断执行的是：dev->bus->probe(dev);

如果上面的函数不存在，则执行：drv->probe(dev);

二者执行一个。

（1）在platformBus中：

dev->bus->probe为：NULL。

drv->probe为：platform_drv_probe()，它会调用：

platform_driver->probe(platform_device)，即我们定义的platform probe函数。

（2）在i2cBus中（指的是i2c client device和driver）：

dev->bus->probe为：i2c_device_probe()，它会调用：

i2c_driver->probe()，即我们定义的i2c_driver里面的probe函数。

drv->probe为：NULL。

7、注意

（1）调用driver_register()时，会执行迭代器：
```c
bus_for_each_dev(drv->bus, NULL, drv, __driver_attach);
```
遍历该bus上面的每一个device并试图与它匹配。

（2）调用device_register()时，会执行迭代器：
```c
bus_for_each_drv(dev->bus, NULL, &data,__device_attach_driver);
```
遍历该bus上面的每一个driver并试图跟它匹配。

