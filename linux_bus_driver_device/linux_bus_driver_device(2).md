

linux bus_register，device_register，driver_register的实现



一、bus_register
```c
int bus_register(struct bus_type *bus)
{
    int retval;
    struct subsys_private *priv;
    struct lock_class_key *key = &bus->lock_key;
    priv = kzalloc(sizeof(struct subsys_private), GFP_KERNEL);
    if (!priv)
        return -ENOMEM;
    priv->bus = bus;
    bus->p = priv;
    BLOCKING_INIT_NOTIFIER_HEAD(&priv->bus_notifier);
    retval = kobject_set_name(&priv->subsys.kobj, "%s", bus->name);
    if (retval)
        goto out;
    priv->subsys.kobj.kset = bus_kset;
    priv->subsys.kobj.ktype = &bus_ktype;
    priv->drivers_autoprobe = 1;
    retval = kset_register(&priv->subsys);
    if (retval)
        goto out;
    retval = bus_create_file(bus, &bus_attr_uevent);
    if (retval)
        goto bus_uevent_fail;
    priv->devices_kset = kset_create_and_add("devices", NULL, &priv->subsys.kobj);
    if (!priv->devices_kset) {
        retval = -ENOMEM;
        goto bus_devices_fail;
    }
    priv->drivers_kset = kset_create_and_add("drivers", NULL,&priv->subsys.kobj);
    if (!priv->drivers_kset) {
        retval = -ENOMEM;
        goto bus_drivers_fail;
    }
    INIT_LIST_HEAD(&priv->interfaces);
    __mutex_init(&priv->mutex, "subsys mutex", key);
    klist_init(&priv->klist_devices, klist_devices_get, klist_devices_put);
    klist_init(&priv->klist_drivers, NULL, NULL);
    retval = add_probe_files(bus);
    if (retval)
        goto bus_probe_files_fail;
    retval = bus_add_groups(bus, bus->bus_groups);
    if (retval)
        goto bus_groups_fail;
    pr_debug("bus: '%s': registered\n", bus->name);
    return 0;
bus_groups_fail:
    remove_probe_files(bus);
bus_probe_files_fail:
    kset_unregister(bus->p->drivers_kset);
bus_drivers_fail:
    kset_unregister(bus->p->devices_kset);
bus_devices_fail:
    bus_remove_file(bus, &bus_attr_uevent);
bus_uevent_fail:
    kset_unregister(&bus->p->subsys);
out:
    kfree(bus->p);
    bus->p = NULL;
    return retval;
}
```
2、注意

（1）bus_register主要用于注册一条bus，例如注册一条platformBus，注册一条i2cBus。

（2）bus_register会分配和初始化该bus的私有数据结构体，然后是一些kset、klist的初始化。

二、device_register
```c
int device_register(struct device *dev)
{
    device_initialize(dev);//初始化device一些成员
    return device_add(dev);
}
```
```c
int device_add(struct device *dev)
{
    struct device *parent = NULL;
    struct kobject *kobj;
    struct class_interface *class_intf;
    int error = -EINVAL;
    struct kobject *glue_dir = NULL;
    dev = get_device(dev);//增加dev的kobject的kref计数
    if (!dev)
        goto done;
    if (!dev->p) {
        error = device_private_init(dev);//初始化dev的私有数据struct device_private
        if (error)
        goto done;
    }
    if (dev->init_name) {
        dev_set_name(dev, "%s", dev->init_name);
        dev->init_name = NULL;
    }
    if (!dev_name(dev) && dev->bus && dev->bus->dev_name)
        dev_set_name(dev, "%s%u", dev->bus->dev_name, dev->id);
    if (!dev_name(dev)) {
        error = -EINVAL;
        goto name_error;
    }
    pr_debug("device: '%s': %s\n", dev_name(dev), __func__);
    parent = get_device(dev->parent);
    kobj = get_device_parent(dev, parent);
    if (kobj)
        dev->kobj.parent = kobj;
    if (parent && (dev_to_node(dev) == NUMA_NO_NODE))
        set_dev_node(dev, dev_to_node(parent));
    error = kobject_add(&dev->kobj, dev->kobj.parent, NULL);
    if (error) {
        glue_dir = get_glue_dir(dev);
        goto Error;
    }
    if (platform_notify)
        platform_notify(dev);
    error = device_create_file(dev, &dev_attr_uevent);
    if (error)
        goto attrError;
    error = device_add_class_symlinks(dev);
    if (error)
        goto SymlinkError;
    error = device_add_attrs(dev);
    if (error)
        goto AttrsError;
    error = bus_add_device(dev);
    if (error)
        goto BusError;
    error = dpm_sysfs_add(dev);
    if (error)
        goto DPMError;
    device_pm_add(dev);
    if (MAJOR(dev->devt)) {
        error = device_create_file(dev, &dev_attr_dev);
        if (error)
        goto DevAttrError;
        error = device_create_sys_dev_entry(dev);
        if (error)
            goto SysEntryError;
        devtmpfs_create_node(dev);
    }
    if (dev->bus)
        blocking_notifier_call_chain(&dev->bus->p->bus_notifier,BUS_NOTIFY_ADD_DEVICE, dev);
    kobject_uevent(&dev->kobj, KOBJ_ADD);
    bus_probe_device(dev);
    if (parent)
        klist_add_tail(&dev->p->knode_parent,&parent->p->klist_children);
    if (dev->class) {
        mutex_lock(&dev->class->p->mutex);
        klist_add_tail(&dev->knode_class,&dev->class->p->klist_devices);
        list_for_each_entry(class_intf,&dev->class->p->interfaces, node)
        if (class_intf->add_dev)
            class_intf->add_dev(dev, class_intf);
        mutex_unlock(&dev->class->p->mutex);
    }
done:
    put_device(dev);
    return error;
SysEntryError:
    if (MAJOR(dev->devt))
        device_remove_file(dev, &dev_attr_dev);
DevAttrError:
    device_pm_remove(dev);
    dpm_sysfs_remove(dev);
DPMError:
    bus_remove_device(dev);
BusError:
    device_remove_attrs(dev);
AttrsError:
    device_remove_class_symlinks(dev);
SymlinkError:
    device_remove_file(dev, &dev_attr_uevent);
attrError:
    kobject_uevent(&dev->kobj, KOBJ_REMOVE);
    glue_dir = get_glue_dir(dev);
    kobject_del(&dev->kobj);
Error:
    cleanup_glue_dir(dev, glue_dir);
    put_device(parent);
name_error:
    kfree(dev->p);
    dev->p = NULL;
    goto done;
}
```

```c
int bus_add_device(struct device *dev)
{
  struct bus_type *bus = bus_get(dev->bus);
  int error = 0;
  if (bus) {
    pr_debug("bus: '%s': add device %s\n", bus->name, dev_name(dev));
    error = device_add_attrs(bus, dev);
    if (error)
      goto out_put;
    error = device_add_groups(dev, bus->dev_groups);
    if (error)
      goto out_id;
    error = sysfs_create_link(&bus->p->devices_kset->kobj,
                          &dev->kobj, dev_name(dev));
    if (error)
      goto out_groups;
    error = sysfs_create_link(&dev->kobj,
                        &dev->bus->p->subsys.kobj, "subsystem");
    if (error)
      goto out_subsys;
    klist_add_tail(&dev->p->knode_bus, &bus->p->klist_devices);
  }
  return 0;
out_subsys:
  sysfs_remove_link(&bus->p->devices_kset->kobj, dev_name(dev));
out_groups:
  device_remove_groups(dev, bus->dev_groups);
out_id:
  device_remove_attrs(bus, dev);
out_put:
  bus_put(dev->bus);
  return error;
}

```
```c
void bus_probe_device(struct device *dev)
{
  struct bus_type *bus = dev->bus;
  struct subsys_interface *sif;
  if (!bus)
    return;
  if (bus->p->drivers_autoprobe)
    device_initial_probe(dev);
  mutex_lock(&bus->p->mutex);
  list_for_each_entry(sif, &bus->p->interfaces, node)
  if (sif->add_dev)
    sif->add_dev(dev, sif);
  mutex_unlock(&bus->p->mutex);
}
```


2、注意

（1）device_register主要完成device的成员初始化，并且为device的私有数据分配内存。

（2）它会调用bus_add_device()将device和bus联系起来。

（3）它会调用bus_probe_device()，但是此时bus上面还没有driver，所以这个函数无作用。



三、driver_register
```c
int driver_register(struct device_driver *drv)
{
    int ret;
    struct device_driver *other;
    BUG_ON(!drv->bus->p);
    //bus的私有数据已经在 bus_register()中分配。
    //bus->probe 为 null
    if ((drv->bus->probe && drv->probe) ||
        (drv->bus->remove && drv->remove) ||
        (drv->bus->shutdown && drv->shutdown))
        printk(KERN_WARNING "Driver '%s' needs updating - please use "
                                                        "bus_type methods\n", drv->name);
    other = driver_find(drv->name, drv->bus);
    if (other) {
        printk(KERN_ERR "Error: Driver '%s' is already registered, ""aborting...\n", drv->name);
        return -EBUSY;
    }
    ret = bus_add_driver(drv);
    if (ret)
        return ret;
    ret = driver_add_groups(drv, drv->groups);
    if (ret) {
        bus_remove_driver(drv);
        return ret;
    }
    kobject_uevent(&drv->p->kobj, KOBJ_ADD);
    return ret;
}
```

```c
int bus_add_driver(struct device_driver *drv)
{
    struct bus_type *bus;
    struct driver_private *priv;
    int error = 0;
    bus = bus_get(drv->bus);
    if (!bus)
        return -EINVAL;
    pr_debug("bus: '%s': add driver %s\n", bus->name, drv->name);
    priv = kzalloc(sizeof(*priv), GFP_KERNEL);
//为driver的私有数据struct driver_private分配内存
    if (!priv) {
        error = -ENOMEM;
        goto out_put_bus;
    }
    klist_init(&priv->klist_devices, NULL, NULL);
    priv->driver = drv;
    drv->p = priv;
    priv->kobj.kset = bus->p->drivers_kset;
    error = kobject_init_and_add(&priv->kobj, &driver_ktype, NULL, "%s", drv->name);
    if (error)
        goto out_unregister;
    klist_add_tail(&priv->knode_bus, &bus->p->klist_drivers);
    if (drv->bus->p->drivers_autoprobe) {
//drivers_autoprobe=1，在bus_register中定义
        if (driver_allows_async_probing(drv)) {
            pr_debug("bus: '%s': probing driver %s asynchronously\n",drv->bus->name, drv->name);
            async_schedule(driver_attach_async, drv);
        } else {
            error = driver_attach(drv);
            if (error)
                goto out_unregister;
          }
    }
    module_add_driver(drv->owner, drv);
    error = driver_create_file(drv, &driver_attr_uevent);
    if (error) {
        printk(KERN_ERR "%s: uevent attr (%s) failed\n",__func__, drv->name);
    }
    error = driver_add_groups(drv, bus->drv_groups);
    if (error) {
        printk(KERN_ERR "%s: driver_create_groups(%s) failed\n",__func__, drv->name);
    }
    if (!drv->suppress_bind_attrs) {
        error = add_bind_files(drv);
        if (error) {
            printk(KERN_ERR "%s: add_bind_files(%s) failed\n",__func__, drv->name);
        }
    }
    return 0;
out_unregister:
    kobject_put(&priv->kobj);
    drv->p = NULL;
out_put_bus:
    bus_put(bus);
    return error;
}
```

```c
int driver_attach(struct device_driver *drv)

{
    return bus_for_each_dev(drv->bus, NULL, drv, __driver_attach);
}
```

```c
int bus_for_each_dev(struct bus_type *bus, struct device *start,void *data, 
int (*fn)(struct device *, void *))
{
    struct klist_iter i;
    struct device *dev;
    int error = 0;
    if (!bus || !bus->p)
        return -EINVAL;
    klist_iter_init_node(&bus->p->klist_devices, &i, (start ? &start->p->knode_bus : NULL));
    while ((dev = next_device(&i)) && !error)
        error = fn(dev, data);
    klist_iter_exit(&i);
    return error;
}
```

```c
static int __driver_attach(struct device *dev, void *data)
{
    struct device_driver *drv = data;
    if (!driver_match_device(drv, dev))
        return 0;
    if (dev->parent)
        device_lock(dev->parent);
    device_lock(dev);
    if (!dev->driver)
        driver_probe_device(drv, dev);
    device_unlock(dev);
    if (dev->parent)
        device_unlock(dev->parent);
    return 0;
}
```

```c
int driver_probe_device(struct device_driver *drv, struct device *dev)
{
    int ret = 0;
    if (!device_is_registered(dev))
        return -ENODEV;
    pr_debug("bus: '%s': %s: matched device %s with driver %s\n",
                drv->bus->name, __func__, dev_name(dev), drv->name);
    if (dev->parent)
        pm_runtime_get_sync(dev->parent);
    pm_runtime_barrier(dev);
    ret = really_probe(dev, drv);
    pm_request_idle(dev);
    if (dev->parent)
        pm_runtime_put(dev->parent);
    return ret;
}
```

```c
static int really_probe(struct device *dev, struct device_driver *drv)
{
    int ret = 0;
    int local_trigger_count = atomic_read(&deferred_trigger_count);
#ifdef CONFIG_MTPROF
    unsigned long long ts = 0;
#endif
    atomic_inc(&probe_count);
    pr_debug("bus: '%s': %s: probing driver %s with device %s\n",
            drv->bus->name, __func__, drv->name, dev_name(dev));
    WARN_ON(!list_empty(&dev->devres_head));
    dev->driver = drv;
/* If using pinctrl, bind pins now before probing */
    ret = pinctrl_bind_pins(dev);
    if (ret)
        goto probe_failed;
    if (driver_sysfs_add(dev)) {
        printk(KERN_ERR "%s: driver_sysfs_add(%s) failed\n",__func__, dev_name(dev));
        goto probe_failed;
    }
    if (dev->pm_domain && dev->pm_domain->activate) {
        ret = dev->pm_domain->activate(dev);
        if (ret)
        goto probe_failed;
    }
    devices_kset_move_last(dev);
    if (dev->bus->probe) {//bus->probe is null
        TIME_LOG_START();
        ret = dev->bus->probe(dev);
        TIME_LOG_END();
        bootprof_probe(ts, dev, drv, (unsigned long)dev->bus->probe);
        if (ret)
        goto probe_failed;
    } else if (drv->probe) {
        TIME_LOG_START();
        ret = drv->probe(dev);//这里会调用 platform_drv_probe
        TIME_LOG_END();
        bootprof_probe(ts, dev, drv, (unsigned long)drv->probe);
        if (ret)
            goto probe_failed;
    }
    pinctrl_init_done(dev);
    if (dev->pm_domain && dev->pm_domain->sync)
        dev->pm_domain->sync(dev);
    driver_bound(dev);
    ret = 1;
    pr_debug("bus: '%s': %s: bound device %s to driver %s\n",
        drv->bus->name, __func__, dev_name(dev), drv->name);
    goto done;
probe_failed:
    devres_release_all(dev);
    driver_sysfs_remove(dev);
    dev->driver = NULL;
    dev_set_drvdata(dev, NULL);
    if (dev->pm_domain && dev->pm_domain->dismiss)
        dev->pm_domain->dismiss(dev);
    switch (ret) {
        case -EPROBE_DEFER:
        /* Driver requested deferred probing */
        dev_dbg(dev, "Driver %s requests probe deferral\n", drv->name);
        driver_deferred_probe_add(dev);
        /* Did a trigger occur while probing? Need to re-trigger if yes */
        if (local_trigger_count != atomic_read(&deferred_trigger_count))
            driver_deferred_probe_trigger();
        break;
        case -ENODEV:
        case -ENXIO:
        pr_debug("%s: probe of %s rejects match %d\n",drv->name, dev_name(dev), ret);
        break;
        default:
        /* driver matched but the probe failed */
        printk(KERN_WARNING "%s: probe of %s failed with error %d\n",
              drv->name, dev_name(dev), ret);
    }
    ret = 0;
done:
    atomic_dec(&probe_count);
    wake_up(&probe_waitqueue);
    return ret;
}
```
2、注意

（1）driver_register添加driver到bus上，然后与device进行match，自动调用driver的probe函数。

（2）driver_register会给driver的私有数据分配内存。

（3）driver_register会循环遍历每一个device，进行match，如果match上了，会调用driver的probe函数。

