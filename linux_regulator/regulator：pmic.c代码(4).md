


一、 pmic.c

由vendor实现，主要是调用 regulator_register() 注册 regulator 。

```c
fs_initcall(pmic_init);//fs_initcall()的优先级为 5

static int __init pmic_init(void)
{
    int ret;

    ret = platform_device_register(&pmic_device);
    if (ret)
        return ret;

    ret = platform_driver_register(&pmic_driver);
    if (ret) 
        return ret;

    return 0;
}

struct platform_device pmic_device = {
    .name   = "vendor-pmic",
    .id        = -1,
};

static struct platform_driver pmic_driver = {
    .probe        = pmic_probe,
    .remove       = pmic_remove,
    .shutdown     = pmic_shutdown,
    .suspend      = pmic_suspend,
    .resume       = pmic_resume,
    .driver       = {
        .name = "vendor-pmic",
    },
};

static int pmic_probe(struct platform_device *dev)
{   
    pmic_thread_handle = kthread_create(pmic_thread_kthread, (void *) NULL, "pmic_thread");
    if (IS_ERR(pmic_thread_handle)) 
        pmic_thread_handle = NULL;     

    else
        wake_up_process(pmic_thread_handle);

    vendor_regulator_init(dev);

    return 0;
}

void vendor_regulator_init(struct platform_device *dev)
{
    int i=0;
    int ret=0;
    int isEn=0;

    for (i = 0; i < ARRAY_SIZE(vendor_ldos); i++) 
    {
        if (vendor_ldos[i].isUsedable==1)
        {
            vendor_ldos[i].config.dev=&(dev->dev);
            vendor_ldos[i].config.init_data=&vendor_ldos[i].init_data;
            if(vendor_ldos[i].desc.n_voltages!=1)
            {
                const int *pVoltage;
                
                if(vendor_ldos[i].vol_reg!=0)
                {
                    if(vendor_ldos[i].pvoltages!=NULL)
                    {
                        pVoltage=(const int*)vendor_ldos[i].pvoltages;
                        vendor_ldos[i].init_data.constraints.max_uV=pVoltage[vendor_ldos[i].desc.n_voltages-1];
                        vendor_ldos[i].init_data.constraints.min_uV=pVoltage[0];       
                    }
                    else
                    {
                        vendor_ldos[i].init_data.constraints.max_uV=(vendor_ldos[i].desc.n_voltages-1)*vendor_ldos[i].desc.uV_step+vendor_ldos[i].desc.min_uV;
                        vendor_ldos[i].init_data.constraints.min_uV=vendor_ldos[i].desc.min_uV;
                    }
                }
            }

            vendor_ldos[i].desc.owner=THIS_MODULE;

            vendor_ldos[i].rdev= regulator_register(&vendor_ldos[i].desc,&vendor_ldos[i].config);
            if ( IS_ERR(vendor_ldos[i].rdev) )                                
                ret = PTR_ERR(vendor_ldos[i].rdev);
			
            vendor_ldos[i].reg=regulator_get(&(dev->dev), vendor_ldos[i].desc.name);
            isEn=regulator_is_enabled(vendor_ldos[i].reg); 
            if(isEn!=0)
                printk("[regulator] %s is default on\n", vendor_ldos[i].desc.name);      
        }
    }
}
```

