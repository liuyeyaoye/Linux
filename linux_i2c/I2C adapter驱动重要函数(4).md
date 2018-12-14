

在i2c adapter 驱动程序中，会为每一条i2c总线都注册一个 adapter ： i2c_register_adapter ，i2c adapter中包含了i2c algorithm。

下面分析 i2c_register_adapter：

```c
static int i2c_register_adapter(struct i2c_adapter *adap)
{
	int res = 0;

	/* Can't register until after driver model init */
	if (unlikely(WARN_ON(!i2c_bus_type.p))) {
        //bus_register()会动态分配bus的struct subsys_private
		res = -EAGAIN;
		goto out_list;
	}

	/* Sanity checks */
	if (unlikely(adap->name[0] == '\0')) {
		pr_err("i2c-core: Attempt to register an adapter with ""no name!\n");
		return -EINVAL;
	}
	
	if (unlikely(!adap->algo)) {
		pr_err("i2c-core: Attempt to register adapter '%s' with ""no algo!\n", 
                  adap->name);
		return -EINVAL;
	}

	rt_mutex_init(&adap->bus_lock);
	mutex_init(&adap->userspace_clients_lock);
	INIT_LIST_HEAD(&adap->userspace_clients);

	/* Set default timeout to 1 second if not already set */
	if (adap->timeout == 0)
		adap->timeout = HZ;

	dev_set_name(&adap->dev, "i2c-%d", adap->nr);
	adap->dev.bus = &i2c_bus_type;
	adap->dev.type = &i2c_adapter_type;
	
	res = device_register(&adap->dev);
    //注册i2c总线adapter里面的device，将它与i2c_bus_type这条总线联系起来
	if (res)
		goto out_list;

	dev_dbg(&adap->dev, "adapter [%s] registered\n", adap->name);

	pm_runtime_no_callbacks(&adap->dev);

	/* bus recovery specific initialization */
	if (adap->bus_recovery_info) {
		struct i2c_bus_recovery_info *bri = adap->bus_recovery_info;
		if (!bri->recover_bus) {
			dev_err(&adap->dev, "No recover_bus() found, not using recovery\n");
			adap->bus_recovery_info = NULL;
			goto exit_recovery;
		}

		/* Generic GPIO recovery */
		if (bri->recover_bus == i2c_generic_gpio_recovery) {
			if (!gpio_is_valid(bri->scl_gpio)) {
				dev_err(&adap->dev, "Invalid SCL gpio, not using recovery\n");
				adap->bus_recovery_info = NULL;
				goto exit_recovery;
			}
			if (gpio_is_valid(bri->sda_gpio))
				bri->get_sda = get_sda_gpio_value;
			else
				bri->get_sda = NULL;

			bri->get_scl = get_scl_gpio_value;
			bri->set_scl = set_scl_gpio_value;
		} else if (!bri->set_scl || !bri->get_scl) {
			/* Generic SCL recovery */
			dev_err(&adap->dev, "No {get|set}_gpio() found, not using recovery\n");
			adap->bus_recovery_info = NULL;
		}
	}

exit_recovery:
	/* create pre-declared device nodes */
	of_i2c_register_devices(adap);//重要：i2c client device 初始化
	acpi_i2c_register_devices(adap);
	acpi_i2c_install_space_handler(adap);

	if (adap->nr < __i2c_first_dynamic_bus_num)
		i2c_scan_static_board_info(adap);

	mutex_lock(&core_lock);
	bus_for_each_drv(&i2c_bus_type, NULL, adap, __process_new_adapter);
//遍历总线上的i2c驱动，用于动态的检测i2c client device。
//它会调用 i2c client driver的detect函数，如果能检测到，就调用i2c_new_device 。
//该方法主要用于 i2c client driver先于 i2c adapter driver注册。
	mutex_unlock(&core_lock);
	return 0;

out_list:
	mutex_lock(&core_lock);
	idr_remove(&i2c_adapter_idr, adap->nr);
	mutex_unlock(&core_lock);
	return res;
}

static int __process_new_adapter(struct device_driver *d, void *data)
{
	return i2c_do_add_adapter(to_i2c_driver(d), data);
}

static int i2c_do_add_adapter(struct i2c_driver *driver,
			      struct i2c_adapter *adap)
{
	/* Detect supported devices on that bus, and instantiate them */
	i2c_detect(adap, driver);

	/* Let legacy drivers scan this bus for matching devices */
	if (driver->attach_adapter) {
		dev_warn(&adap->dev, "%s: attach_adapter method is deprecated\n",
			 driver->driver.name);
		dev_warn(&adap->dev, "Please use another way to instantiate "
			 "your i2c_client\n");
		/* We ignore the return code; if it fails, too bad */
		driver->attach_adapter(adap);
	}
	return 0;
}

static void of_i2c_register_devices(struct i2c_adapter *adap)
{
	struct device_node *bus, *node;
	struct i2c_client *client;

	/* Only register child devices if the adapter has a node pointer set */
	if (!adap->dev.of_node)
		return;

	dev_dbg(&adap->dev, "of_i2c: walking child nodes\n");

	bus = of_get_child_by_name(adap->dev.of_node, "i2c-bus");
	if (!bus)
		bus = of_node_get(adap->dev.of_node);//它为 i2c adapter 节点

    //循环遍历该i2c adapter节点的每一个i2c client 节点。
	for_each_available_child_of_node(bus, node) {
		if (of_node_test_and_set_flag(node, OF_POPULATED))
			continue;

		client = of_i2c_register_device(adap, node);//注册 i2c client device。
		if (IS_ERR(client)) {
			dev_warn(&adap->dev, "Failed to create I2C device for %s\n",
				      node->full_name);
			of_node_clear_flag(node, OF_POPULATED);
		}
	}
	of_node_put(bus);
}

static struct i2c_client *of_i2c_register_device(struct i2c_adapter *adap,
						 struct device_node *node)
{
	struct i2c_client *result;
	struct i2c_board_info info = {};
	struct dev_archdata dev_ad = {};
	const __be32 *addr_be;
	u32 addr;
	int len;

	dev_dbg(&adap->dev, "of_i2c: register %s\n", node->full_name);

	if (of_modalias_node(node, info.type, sizeof(info.type)) < 0) {
		dev_err(&adap->dev, "of_i2c: modalias failure on %s\n",
			node->full_name);
		return ERR_PTR(-EINVAL);
	}

	addr_be = of_get_property(node, "reg", &len);
	if (!addr_be || (len < sizeof(*addr_be))) {
		dev_err(&adap->dev, "of_i2c: invalid reg on %s\n",
			node->full_name);
		return ERR_PTR(-EINVAL);
	}

	addr = be32_to_cpup(addr_be);
	if (addr & I2C_TEN_BIT_ADDRESS) {
		addr &= ~I2C_TEN_BIT_ADDRESS;
		info.flags |= I2C_CLIENT_TEN;
	}

	if (addr & I2C_OWN_SLAVE_ADDRESS) {
		addr &= ~I2C_OWN_SLAVE_ADDRESS;
		info.flags |= I2C_CLIENT_SLAVE;
	}

	if (i2c_check_addr_validity(addr, info.flags)) {
		dev_err(&adap->dev, "of_i2c: invalid addr=%x on %s\n",
			addr, node->full_name);
		return ERR_PTR(-EINVAL);
	}

	info.addr = addr;
	info.of_node = of_node_get(node);
	info.archdata = &dev_ad;

	if (of_get_property(node, "wakeup-source", NULL))
		info.flags |= I2C_CLIENT_WAKE;

	result = i2c_new_device(adap, &info);
	if (result == NULL) {
		dev_err(&adap->dev, "of_i2c: Failure registering %s\n",
			node->full_name);
		of_node_put(node);
		return ERR_PTR(-EINVAL);
	}
	return result;
}

struct i2c_client * i2c_new_device(struct i2c_adapter *adap, struct i2c_board_info const *info)
{
	struct i2c_client	*client;
	int			status;

	client = kzalloc(sizeof *client, GFP_KERNEL);
	if (!client)
		return NULL;

	client->adapter = adap;
	client->dev.platform_data = info->platform_data;

	if (info->archdata)
		client->dev.archdata = *info->archdata;

	client->flags = info->flags;
	client->addr = info->addr;
	client->irq = info->irq;
	strlcpy(client->name, info->type, sizeof(client->name));

	status = i2c_check_addr_validity(client->addr, client->flags);
	if (status) {
		dev_err(&adap->dev, "Invalid %d-bit I2C address 0x%02hx\n",
			client->flags & I2C_CLIENT_TEN ? 10 : 7, client->addr);
		goto out_err_silent;
	}

	/* Check for address business */
	status = i2c_check_addr_busy(adap, i2c_encode_flags_to_addr(client));
	if (status)
		goto out_err;

	client->dev.parent = &client->adapter->dev;
	client->dev.bus = &i2c_bus_type;
	client->dev.type = &i2c_client_type;
	client->dev.of_node = info->of_node;
	client->dev.fwnode = info->fwnode;

	i2c_dev_set_name(adap, client);
	status = device_register(&client->dev);//注册i2c client的device。
	if (status)
		goto out_err;

	dev_dbg(&adap->dev, "client [%s] registered with bus id %s\n",
		client->name, dev_name(&client->dev));

	return client;

out_err:
	dev_err(&adap->dev,
		"Failed to register i2c client %s at 0x%02x (%d)\n",
		client->name, client->addr, status);
out_err_silent:
	kfree(client);
	return NULL;
}

```

