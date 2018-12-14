


regulator 总结

1、因为在 vendor 的 camera 驱动中， 前后摄的VCAMA，VCAMD，VCAMIO，VCAMAF都是通过LDO regulator供电的，所以才会关注一下regulator。

2、在 vendor 的 pmic 驱动中注册 regulator ： regulator_register()。

3、在imgsensor驱动操作regulator，如regulator_get()，regulator_put()，regulator_enable()，regulator_voltage()，regulator_disable()，regulator_is_enabled()。

4、If the regulator is shared between several devices then the lowest request voltage that meets the system constraints will be used.

（1）不同电压的引脚共用 regulator 时， regulator 为了保护低电压引脚不被被烧坏，

会把低电压引脚的电压设置成 regulator 的最高电压，从而使高电压引脚无法上电。代码中实现的地方为 regulator_check_consumers() ：

```c
if (*max_uV > regulator->max_uV)	
  *max_uV = regulator->max_uV;
if (*min_uV < regulator->min_uV)	
  *min_uV = regulator->min_uV;
```

（2）实例：假如后摄VCAMD=1.0V，前摄VCAMD=1.2V，那么打开后摄时， regulator->min_uV = 1000000 ， regulator->max_uV = 1000000 ， 

然后是后摄切到前摄， 要设置的电压为： *min_uV = 1200000 ， *max_uV = 1200000 。

经过上面代码的判断后， *min_uV = 1200000 ， *max_uV = 1000000 。引发错误。

（3）解决：

对于共用 regulator ，但电压需求不同的引脚：

(1) 下电后，调用 regulator_put 函数，释放掉 regulator。

(2) 上电前，调用 regulator_get 函数，重新申请 regulator。

5、如果还是设置regulator失败，则需要查看pmic驱动，查看它支持设置的电压是多少。例如：

```c
const static int VCAMD_voltages[] = {
	1000000,
    1200000,
    1300000,    
    1500000,
    1800000,        
};
```

6、在调用 regulator_set_voltage() 时，有2个最大/最小电压，它们在哪里赋值的？

（1）
regulator->min_uV; 

regulator->max_uV;

这个，即我们要设置的最小/最大电压， regulator consumer device 电压。

（2）
rdev->constraints->max_uV; 

rdev->constraints->min_uV;

这个物理限制，在pmic的regulator中初始化，即我们自己定义的最小/最大电压，上面VCAMD的最小、最大电压为：1000000和1800000。也可以在dts中定义。

7、后续遇到bug再补充。


