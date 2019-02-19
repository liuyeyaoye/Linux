

```c

一. 相关的文件

1. i2c-core.c

(1) 这个文件里面有一个 postcore_initcall 调用，它的函数的主要作用是调用 bus_register() 注册一条 i2c bus 。

(2) 这个文件提供了各种i2c操作函数，如i2c读写函数，smbus i2c 系列函数，i2c of系列函数，i2c adapter系列函数。


2. i2c-vendor.c

这个文件是 i2c adapter 的注册和实现，跟 vendor 相关。

二. i2c-core.c

postcore_initcall(i2c_init);//优先级为2
static int __init i2c_init(void)
{
	int retval;
	retval = of_alias_get_highest_id("i2c");

	down_write(&__i2c_board_lock);
	if (retval >= __i2c_first_dynamic_bus_num)
		__i2c_first_dynamic_bus_num = retval + 1;
	up_write(&__i2c_board_lock);

	retval = bus_register(&i2c_bus_type);//注册一条 i2c bus
	if (retval)
		return retval;
		
	retval = i2c_add_driver(&dummy_driver);
    //添加一个 dummy i2c client driver
	if (retval)
		goto class_err;

	if (IS_ENABLED(CONFIG_OF_DYNAMIC))
		WARN_ON(of_reconfig_notifier_register(&i2c_of_notifier));

	return 0;
class_err:
	bus_unregister(&i2c_bus_type);
	return retval;
}

struct bus_type i2c_bus_type = {
	.name		= "i2c",
	.match		= i2c_device_match,
	.probe		= i2c_device_probe,
	.remove		= i2c_device_remove,
	.shutdown	= i2c_device_shutdown,
};

static int i2c_device_match(struct device *dev, struct device_driver *drv)
{
	struct i2c_client	*client = i2c_verify_client(dev);
	struct i2c_driver	*driver;

	if (!client)
		return 0;

	/* Attempt an OF style match */
	if (of_driver_match_device(dev, drv))
		return 1;

	/* Then ACPI style match */
	if (acpi_driver_match_device(dev, drv))
		return 1;

	driver = to_i2c_driver(drv);
	/* match on an id table if there is one */
	if (driver->id_table)
		return i2c_match_id(driver->id_table, client) != NULL;

	return 0;
}

static int i2c_device_probe(struct device *dev)
{
	struct i2c_client	*client = i2c_verify_client(dev);
	struct i2c_driver	*driver;
	int status;

	if (!client)
		return 0;

	if (!client->irq) {
		int irq = -ENOENT;
		if (dev->of_node) {
			irq = of_irq_get_byname(dev->of_node, "irq");
			if (irq == -EINVAL || irq == -ENODATA)
				irq = of_irq_get(dev->of_node, 0);
		} else if (ACPI_COMPANION(dev)) {
			irq = acpi_dev_gpio_irq_get(ACPI_COMPANION(dev), 0);
		}
		if (irq == -EPROBE_DEFER)
			return irq;
		if (irq < 0)
			irq = 0;

		client->irq = irq;
	}

	driver = to_i2c_driver(dev->driver);
	if (!driver->probe || !driver->id_table)
		return -ENODEV;

	if (client->flags & I2C_CLIENT_WAKE) {
		int wakeirq = -ENOENT;

		if (dev->of_node) {
			wakeirq = of_irq_get_byname(dev->of_node, "wakeup");
			if (wakeirq == -EPROBE_DEFER)
				return wakeirq;
		}

		device_init_wakeup(&client->dev, true);

		if (wakeirq > 0 && wakeirq != client->irq)
			status = dev_pm_set_dedicated_wake_irq(dev, wakeirq);
		else if (client->irq > 0)
			status = dev_pm_set_wake_irq(dev, client->irq);
		else
			status = 0;

		if (status)
			dev_warn(&client->dev, "failed to set up wakeup irq\n");
	}

	dev_dbg(dev, "probe\n");

	status = of_clk_set_defaults(dev->of_node, false);
	if (status < 0)
		goto err_clear_wakeup_irq;

	status = dev_pm_domain_attach(&client->dev, true);
	if (status == -EPROBE_DEFER)
		goto err_clear_wakeup_irq;

	status = driver->probe(client, i2c_match_id(driver->id_table, client));
	if (status)
		goto err_detach_pm_domain;

	return 0;

err_detach_pm_domain:
	dev_pm_domain_detach(&client->dev, true);
err_clear_wakeup_irq:
	dev_pm_clear_wake_irq(&client->dev);
	device_init_wakeup(&client->dev, false);
	return status;
}



三. i2c-vendor.c

module_init(vendor_i2c_init);

static s32 __init vendor_i2c_init(void)
{
	if (!map_dma_regs())
		pr_warn("Mapp dma regs successfully.\n");
	return platform_driver_register(&vendor_i2c_driver);
}

static struct platform_driver vendor_i2c_driver = {
	.probe = vendor_i2c_probe,
	.remove = vendor_i2c_remove,
	.driver = {
		.name = I2C_DRV_NAME,
		.owner = THIS_MODULE,
		.pm = &vendor_i2c_dev_pm_ops,
		.of_match_table = of_match_ptr(vendor_i2c_of_match),
	},
};

static const struct of_device_id vendor_i2c_of_match[] = {
	{ .compatible = "vendor,i2c", 
      .data = &vendor_compat 
    },
	{},
};

//有几条i2c总线，下面这个probe函数就会调用几次。一个probe driver对应了4个i2c总线device。
static int vendor_i2c_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct vendor_i2c *i2c;
	unsigned int clk_src_in_hz;
	struct resource *res;
	const struct of_device_id *of_id;

	i2c = devm_kzalloc(&pdev->dev, sizeof(struct vendor_i2c), GFP_KERNEL);
	if (i2c == NULL)
		return -ENOMEM;

	ret = vendor_i2c_parse_dt(pdev->dev.of_node, i2c);
    //从DTS中读取I2C总线的DTS信息，保存。
	if (ret)
		return -EINVAL;

/*
DTS中的I2C的地址为：
reg = <0x0 0x11007000 0x0 0x1000 0x0 0x11000100 0x0 0x80>;
有2个地址：
addr:0x0000000011007000   	size:0x00001000	这个地址为I2C0的总线地址
addr:0x0000000011000100		size:0x0080		这个地址为I2C0使用的DMA地址
*/
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);//i2c总线地址
	i2c->base = devm_ioremap_resource(&pdev->dev, res);//ioremap i2c总线地址
	if (IS_ERR(i2c->base))
		return PTR_ERR(i2c->base);

	if (i2c->id < I2C_MAX_CHANNEL)
		g_vendor_i2c[i2c->id] = i2c;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);//dma 地址
	i2c->pdmabase = devm_ioremap_resource(&pdev->dev, res);//ioremap dma地址
	if (IS_ERR(i2c->pdmabase))
		return PTR_ERR(i2c->pdmabase);

	i2c->irqnr = platform_get_irq(pdev, 0);
	if (i2c->irqnr <= 0)
		return -EINVAL;
		
	init_waitqueue_head(&i2c->wait);

	ret = devm_request_irq(&pdev->dev, i2c->irqnr, vendor_i2c_irq,
		IRQF_NO_SUSPEND | IRQF_TRIGGER_NONE, I2C_DRV_NAME, i2c);
	if (ret < 0) {
		dev_err(&pdev->dev,"Request I2C IRQ %d fail\n", i2c->irqnr);
		return ret;
	}
	
	of_id = of_match_node(vendor_i2c_of_match, pdev->dev.of_node);
	if (!of_id)
		return -EINVAL;

	i2c->dev_comp = of_id->data;
	i2c->adap.dev.of_node = pdev->dev.of_node;
	i2c->dev = &i2c->adap.dev;
	i2c->adap.dev.parent = &pdev->dev;
	i2c->adap.owner = THIS_MODULE;
	i2c->adap.algo = &vendor_i2c_algorithm;//i2c algorithm 由 vendor 实现。
	i2c->adap.algo_data = NULL;
	i2c->adap.timeout = 2 * HZ;
	i2c->adap.retries = 1;
	i2c->adap.nr = i2c->id;
	spin_lock_init(&i2c->cg_lock);

	if (i2c->dev_comp->dma_support == 2) {
		if (dma_set_mask(&pdev->dev, DMA_BIT_MASK(33))) {
			dev_err(&pdev->dev, "dma_set_mask return error.\n");
			return -EINVAL;
		}
	} else if (i2c->dev_comp->dma_support == 3) {
		if (dma_set_mask(&pdev->dev, DMA_BIT_MASK(36))) {
			dev_err(&pdev->dev, "dma_set_mask return error.\n");
			return -EINVAL;
		}
	}

	i2c->clk_main = devm_clk_get(&pdev->dev, "main");
	if (IS_ERR(i2c->clk_main)) {
		dev_err(&pdev->dev, "cannot get main clock\n");
		return PTR_ERR(i2c->clk_main);
	}
	
	i2c->clk_dma = devm_clk_get(&pdev->dev, "dma");
	if (IS_ERR(i2c->clk_dma)) {
		dev_err(&pdev->dev, "cannot get dma clock\n");
		return PTR_ERR(i2c->clk_dma);
	}
	
	i2c->clk_arb = devm_clk_get(&pdev->dev, "arb");
	if (IS_ERR(i2c->clk_arb))
		i2c->clk_arb = NULL;
	else
		dev_dbg(&pdev->dev, "i2c%d has the relevant arbitrator clk.\n", i2c->id);

	if (i2c->have_pmic) {
		i2c->clk_pmic = devm_clk_get(&pdev->dev, "pmic");
		if (IS_ERR(i2c->clk_pmic)) {
			dev_err(&pdev->dev, "cannot get pmic clock\n");
			return PTR_ERR(i2c->clk_pmic);
		}
		clk_src_in_hz = clk_get_rate(i2c->clk_pmic) / i2c->clk_src_div;
		i2c->main_clk = clk_src_in_hz;
	} else {
		clk_src_in_hz = clk_get_rate(i2c->clk_main) / i2c->clk_src_div;
		i2c->main_clk = clk_src_in_hz;
	}
	dev_warn(&pdev->dev, "i2c%d clock source %p,clock src frequency %d\n", 
                         i2c->id, i2c->clk_main, clk_src_in_hz);

	strlcpy(i2c->adap.name, I2C_DRV_NAME, sizeof(i2c->adap.name));
	mutex_init(&i2c->i2c_mutex);
	
	ret = i2c_set_speed(i2c, clk_src_in_hz);
	if (ret) {
		dev_err(&pdev->dev, "Failed to set the speed\n");
		return -EINVAL;
	}
	
	ret = vendor_i2c_clock_enable(i2c);
	if (ret) {
		dev_err(&pdev->dev, "clock enable failed!\n");
		return ret;
	}
	
	vendor_i2c_init_hw(i2c);//写vendor寄存器初始化I2C。
	vendor_i2c_clock_disable(i2c);
	
	if (i2c->ch_offset_default)//为I2C分配DMA
		i2c->dma_buf.vaddr = dma_alloc_coherent(&pdev->dev,
			PAGE_SIZE * 2, &i2c->dma_buf.paddr, GFP_KERNEL);
	else
		i2c->dma_buf.vaddr = dma_alloc_coherent(&pdev->dev,
			PAGE_SIZE, &i2c->dma_buf.paddr, GFP_KERNEL);
			
	if (i2c->dma_buf.vaddr == NULL) {
		dev_err(&pdev->dev, "dma_alloc_coherent fail\n");
		return -ENOMEM;
	}
	
	i2c_set_adapdata(&i2c->adap, i2c);
	
	ret = i2c_add_numbered_adapter(&i2c->adap);
	//注册一个I2C adapter驱动，它会调用 i2c_register_adapter()。
	if (ret) {
		dev_err(&pdev->dev, "Failed to add i2c bus to i2c core\n");
		free_i2c_dma_bufs(i2c);
		return ret;
	}
	
	platform_set_drvdata(pdev, i2c);

	if (!map_cg_regs(i2c))
		pr_warn("Map cg regs successfully.\n");

	if (!map_bus_regs(i2c, I2C_DUMP_INFRA_DBG))
		dev_info(i2c->dev, "Map infra regs successfully.\n");

	if (!map_bus_regs(i2c, I2C_DUMP_PERI_DBG))
		dev_info(i2c->dev, "Map peri regs successfully.\n");

	return 0;
}

```

