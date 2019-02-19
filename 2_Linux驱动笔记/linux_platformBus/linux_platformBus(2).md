

Linux PlatformBus 知识详解

一、PlatformBus的注册
```c
在 arch/arm/kernel/head-common.S 中：b	start_kernel 
——>rest_init();
——>kernel_thread(kernel_init, NULL, CLONE_FS);
——>kernel_init_freeable();
——>do_basic_setup();
        ——>driver_init();
                 ——>platform_bus_init();//这个函数注册一条platformBus
        ——>do_initcalls();
```

```c
int __init platform_bus_init(void)
{
    int error;
    early_platform_cleanup();
    error = device_register(&platform_bus);
//platform_bus这个device是所有platformDevice的parent
    if (error)
        return error;
    error =  bus_register(&platform_bus_type);//注册一条platformBus
    if (error)
        device_unregister(&platform_bus);
    of_platform_register_reconfig_notifier();
    return error;
}

struct device platform_bus = {
    .init_name	= "platform",
};

struct bus_type platform_bus_type = {
    .name	= "platform",
    .dev_groups	= platform_dev_groups,
    .match	= platform_match,
    .uevent	= platform_uevent,
    .pm	= &platform_dev_pm_ops,
};
```


二、一个典型的 platformDriver
```c
static struct platform_driver i2c_driver = {
    .probe = i2c_probe,
    .remove = i2c_remove,
    .driver = {
        .name = I2C_DRV_NAME,
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(i2c_of_match),
    },
};

static const struct of_device_id i2c_of_match[] = {
    { .compatible = "vendor,XXXX-i2c" },
    {},
};

static int i2c_probe(struct platform_device *pdev)
{
    gpdev = pdev;
    .......
}

static s32 __init i2c_init(void)
{
    platform_driver_register(&i2c_driver);
    return 0;
}
```


三、分析 platform_driver_register()
```c
#define platform_driver_register(drv) \
        __platform_driver_register(drv, THIS_MODULE)

int __platform_driver_register(struct platform_driver *drv, struct module *owner)
{
    drv->driver.owner = owner;
    drv->driver.bus = &platform_bus_type;
    drv->driver.probe = platform_drv_probe;//实际上是调用：drv->probe(dev)
    drv->driver.remove = platform_drv_remove;//实际上是调用：drv->remove(dev);
    drv->driver.shutdown = platform_drv_shutdown;//实际上是调用：drv->shutdown(dev);
    return driver_register(&drv->driver);
}

struct bus_type platform_bus_type = {
    .name	= "platform",
    .dev_groups	= platform_dev_groups,
    .match	= platform_match,
    .uevent	= platform_uevent,
    .pm	= &platform_dev_pm_ops,
};
```
```c
static int platform_match(struct device *dev, struct device_driver *drv)
{
    struct platform_device *pdev = to_platform_device(dev);
    struct platform_driver *pdrv = to_platform_driver(drv);

/* When driver_override is set, only bind to the matching driver */
    if (pdev->driver_override)
        return !strcmp(pdev->driver_override, drv->name);

/* Attempt an OF style match first */
    if (of_driver_match_device(dev, drv))
        return 1;

/* Then try ACPI style match */
    if (acpi_driver_match_device(dev, drv))
        return 1;

/* Then try to match against the id table */
    if (pdrv->id_table)
        return platform_match_id(pdrv->id_table, pdev) != NULL;

/* fall-back to driver name match */
    return (strcmp(pdev->name, drv->name) == 0);
}
```

