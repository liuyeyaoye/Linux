


一、 regulator_register

```c
struct regulator_dev *
regulator_register(const struct regulator_desc *regulator_desc,
		   const struct regulator_config *config)
{
	const struct regulation_constraints *constraints = NULL;
	const struct regulator_init_data *init_data;
	static atomic_t regulator_no = ATOMIC_INIT(0);
	struct regulator_dev *rdev;
	struct device *dev;
	int ret, i;
	const char *supply = NULL;

	if (regulator_desc == NULL || config == NULL)
		return ERR_PTR(-EINVAL);

	dev = config->dev;
	WARN_ON(!dev);

	if (regulator_desc->name == NULL || regulator_desc->ops == NULL)
		return ERR_PTR(-EINVAL);

	if (regulator_desc->type != REGULATOR_VOLTAGE &&
	    regulator_desc->type != REGULATOR_CURRENT)
		return ERR_PTR(-EINVAL);

	/* Only one of each should be implemented */
	WARN_ON(regulator_desc->ops->get_voltage &&
		regulator_desc->ops->get_voltage_sel);
	WARN_ON(regulator_desc->ops->set_voltage &&
		regulator_desc->ops->set_voltage_sel);

	/* If we're using selectors we must implement list_voltage. */
	if (regulator_desc->ops->get_voltage_sel &&
	    !regulator_desc->ops->list_voltage) {
		return ERR_PTR(-EINVAL);
	}
	if (regulator_desc->ops->set_voltage_sel &&
	    !regulator_desc->ops->list_voltage) {
		return ERR_PTR(-EINVAL);
	}

	init_data = config->init_data;

	rdev = kzalloc(sizeof(struct regulator_dev), GFP_KERNEL);
	if (rdev == NULL)
		return ERR_PTR(-ENOMEM);

	mutex_lock(&regulator_list_mutex);

	mutex_init(&rdev->mutex);
	rdev->reg_data = config->driver_data;
	rdev->owner = regulator_desc->owner;
	rdev->desc = regulator_desc;
	if (config->regmap)
		rdev->regmap = config->regmap;
	else if (dev_get_regmap(dev, NULL))
		rdev->regmap = dev_get_regmap(dev, NULL);
	else if (dev->parent)
		rdev->regmap = dev_get_regmap(dev->parent, NULL);
	INIT_LIST_HEAD(&rdev->consumer_list);
	INIT_LIST_HEAD(&rdev->list);
	BLOCKING_INIT_NOTIFIER_HEAD(&rdev->notifier);
	INIT_DELAYED_WORK(&rdev->disable_work, regulator_disable_work);

	/* preform any regulator specific init */
	if (init_data && init_data->regulator_init) {
		ret = init_data->regulator_init(rdev->reg_data);
		if (ret < 0)
			goto clean;
	}

	/* register with sysfs */
	rdev->dev.class = &regulator_class;
	rdev->dev.of_node = config->of_node;
	rdev->dev.parent = dev;
	dev_set_name(&rdev->dev, "regulator.%d",
		     atomic_inc_return(&regulator_no) - 1);
	ret = device_register(&rdev->dev);
	if (ret != 0) {
		put_device(&rdev->dev);
		goto clean;
	}

	dev_set_drvdata(&rdev->dev, rdev);

	if (config->ena_gpio && gpio_is_valid(config->ena_gpio)) {
		ret = regulator_ena_gpio_request(rdev, config);
		if (ret != 0) {
			rdev_err(rdev, "Failed to request enable GPIO%d: %d\n",
				 config->ena_gpio, ret);
			goto wash;
		}

		if (config->ena_gpio_flags & GPIOF_OUT_INIT_HIGH)
			rdev->ena_gpio_state = 1;

		if (config->ena_gpio_invert)
			rdev->ena_gpio_state = !rdev->ena_gpio_state;
	}

	/* set regulator constraints */
	if (init_data)
		constraints = &init_data->constraints;

	ret = set_machine_constraints(rdev, constraints);
	if (ret < 0)
		goto scrub;

	/* add attributes supported by this regulator */
	ret = add_regulator_attributes(rdev);
	if (ret < 0)
		goto scrub;

	if (init_data && init_data->supply_regulator)
		supply = init_data->supply_regulator;
	else if (regulator_desc->supply_name)
		supply = regulator_desc->supply_name;

	if (supply) {
		struct regulator_dev *r;

		r = regulator_dev_lookup(dev, supply, &ret);
        //该函数没有作用，ret = -ENODEV ，return NULL;

		if (ret == -ENODEV) {
			/*
			 * No supply was specified for this regulator and
			 * there will never be one.
			 */
			ret = 0;
			goto add_dev;
		} else if (!r) {
			dev_err(dev, "Failed to find supply %s\n", supply);
			ret = -EPROBE_DEFER;
			goto scrub;
		}

		ret = set_supply(rdev, r);
		if (ret < 0)
			goto scrub;

		/* Enable supply if rail is enabled */
		if (_regulator_is_enabled(rdev)) {
			ret = regulator_enable(rdev->supply);
			if (ret < 0)
				goto scrub;
		}
	}

add_dev:
	/* add consumers devices */
	if (init_data) {
		for (i = 0; i < init_data->num_consumer_supplies; i++) {
			ret = set_consumer_device_supply(rdev,
				init_data->consumer_supplies[i].dev_name,
				init_data->consumer_supplies[i].supply);
			if (ret < 0) {
				dev_err(dev, "Failed to set supply %s\n",
					init_data->consumer_supplies[i].supply);
				goto unset_supplies;
			}
		}
	}

	//将 regulator_dev 添加到 regulator_list 链表。
	list_add(&rdev->list, &regulator_list);

	rdev_init_debugfs(rdev);
out:
	mutex_unlock(&regulator_list_mutex);
	return rdev;

unset_supplies:
	unset_regulator_supplies(rdev);

scrub:
	if (rdev->supply)
		_regulator_put(rdev->supply);
	regulator_ena_gpio_free(rdev);
	kfree(rdev->constraints);
wash:
	device_unregister(&rdev->dev);
	/* device core frees rdev */
	rdev = ERR_PTR(ret);
	goto out;

clean:
	kfree(rdev);
	rdev = ERR_PTR(ret);
	goto out;
}
```

二、 regulator_get

```c
struct regulator *regulator_get(struct device *dev, const char *id)
{
	return _regulator_get(dev, id, 0);
}

/* Internal regulator request function */
static struct regulator *_regulator_get(struct device *dev, const char *id,
					int exclusive)
{
	struct regulator_dev *rdev;
	struct regulator *regulator = ERR_PTR(-EPROBE_DEFER);
	const char *devname = NULL;
	int ret = 0;

	if (id == NULL) {
		pr_err("get() with no identifier\n");
		return regulator;
	}

	if (dev)
		devname = dev_name(dev);

	mutex_lock(&regulator_list_mutex);

	rdev = regulator_dev_lookup(dev, id, &ret);//根据 id 查找 regulator_dev
	if (rdev)
		goto found;

	/*
	 * If we have return value from dev_lookup fail, we do not expect to
	 * succeed, so, quit with appropriate error value
	 */
	if (ret) {
		regulator = ERR_PTR(ret);
		goto out;
	}

	if (board_wants_dummy_regulator) {
		rdev = dummy_regulator_rdev;
		goto found;
	}

#ifdef CONFIG_REGULATOR_DUMMY
	if (!devname)
		devname = "deviceless";

	/* If the board didn't flag that it was fully constrained then
	 * substitute in a dummy regulator so consumers can continue.
	 */
	if (!has_full_constraints) {
		pr_warn("%s supply %s not found, using dummy regulator\n",
			devname, id);
		rdev = dummy_regulator_rdev;
		goto found;
	}
#endif

	mutex_unlock(&regulator_list_mutex);
	return regulator;

found:
	if (rdev->exclusive) {
		regulator = ERR_PTR(-EPERM);
		goto out;
	}

	if (exclusive && rdev->open_count) {
		regulator = ERR_PTR(-EBUSY);
		goto out;
	}

	if (!try_module_get(rdev->owner))
		goto out;

	regulator = create_regulator(rdev, dev, id);
	if (regulator == NULL) {
		regulator = ERR_PTR(-ENOMEM);
		module_put(rdev->owner);
		goto out;
	}

	rdev->open_count++;
	if (exclusive) {
		rdev->exclusive = 1;

		ret = _regulator_is_enabled(rdev);
		if (ret > 0)
			rdev->use_count = 1;
		else
			rdev->use_count = 0;
	}

out:
	mutex_unlock(&regulator_list_mutex);

	return regulator;
}

static struct regulator_dev *regulator_dev_lookup(struct device *dev,
						  const char *supply,
						  int *ret)
{
	struct regulator_dev *r;
	struct device_node *node;
	struct regulator_map *map;
	const char *devname = NULL;

	/* first do a dt based lookup */
	if (dev && dev->of_node) {
		node = of_get_regulator(dev, supply);
		if (node) {
			list_for_each_entry(r, &regulator_list, list)
				if (r->dev.parent &&
					node == r->dev.of_node)
					return r;
		} else {
			/*
			 * If we couldn't even get the node then it's
			 * not just that the device didn't register
			 * yet, there's no node and we'll never
			 * succeed.
			 */
			*ret = -ENODEV;
		}
	}

	/* if not found, try doing it non-dt way */
	if (dev)
		devname = dev_name(dev);

	list_for_each_entry(r, &regulator_list, list)
		if (strcmp(rdev_get_name(r), supply) == 0)
			return r;

	list_for_each_entry(map, &regulator_map_list, list) {
		/* If the mapping has a device set up it must match */
		if (map->dev_name &&
		    (!devname || strcmp(map->dev_name, devname)))
			continue;

		if (strcmp(map->supply, supply) == 0)
			return map->regulator;
	}

	return NULL;
}

static struct device_node *of_get_regulator(struct device *dev, const char *supply)
{
	struct device_node *regnode = NULL;
	char prop_name[32]; /* 32 is max size of property name */

	dev_dbg(dev, "Looking up %s-supply from device tree\n", supply);

	snprintf(prop_name, 32, "%s-supply", supply);
	regnode = of_parse_phandle(dev->of_node, prop_name, 0);

	if (!regnode) {
		dev_dbg(dev, "Looking up %s property in node %s failed",
				prop_name, dev->of_node->full_name);
		return NULL;
	}
	return regnode;
}

static struct regulator *create_regulator(struct regulator_dev *rdev,
					  struct device *dev,
					  const char *supply_name)
{
	struct regulator *regulator;
	char buf[REG_STR_SIZE];
	int err, size;

	regulator = kzalloc(sizeof(*regulator), GFP_KERNEL);
	if (regulator == NULL)
		return NULL;

	mutex_lock(&rdev->mutex);
	regulator->rdev = rdev;
	list_add(&regulator->list, &rdev->consumer_list);

	if (dev) {
		regulator->dev = dev;

		/* Add a link to the device sysfs entry */
		size = scnprintf(buf, REG_STR_SIZE, "%s-%s",
				 dev->kobj.name, supply_name);
		if (size >= REG_STR_SIZE)
			goto overflow_err;

		regulator->supply_name = kstrdup(buf, GFP_KERNEL);
		if (regulator->supply_name == NULL)
			goto overflow_err;

		err = sysfs_create_link(&rdev->dev.kobj, &dev->kobj,
					buf);
		if (err) {
			rdev_warn(rdev, "could not add device link %s err %d\n",
				  dev->kobj.name, err);
			/* non-fatal */
		}
	} else {
		regulator->supply_name = kstrdup(supply_name, GFP_KERNEL);
		if (regulator->supply_name == NULL)
			goto overflow_err;
	}

	regulator->debugfs = debugfs_create_dir(regulator->supply_name,
						rdev->debugfs);
	if (!regulator->debugfs) {
		rdev_warn(rdev, "Failed to create debugfs directory\n");
	} else {
		debugfs_create_u32("uA_load", 0444, regulator->debugfs,
				   &regulator->uA_load);
		debugfs_create_u32("min_uV", 0444, regulator->debugfs,
				   &regulator->min_uV);
		debugfs_create_u32("max_uV", 0444, regulator->debugfs,
				   &regulator->max_uV);
	}

	/*
	 * Check now if the regulator is an always on regulator - if
	 * it is then we don't need to do nearly so much work for
	 * enable/disable calls.
	 */
	if (!_regulator_can_change_status(rdev) &&
	    _regulator_is_enabled(rdev))
		regulator->always_on = true;

	mutex_unlock(&rdev->mutex);
	return regulator;
overflow_err:
	list_del(&regulator->list);
	kfree(regulator);
	mutex_unlock(&rdev->mutex);
	return NULL;
}
```

三、 regulator_put

```c
void regulator_put(struct regulator *regulator)
{
	mutex_lock(&regulator_list_mutex);
	_regulator_put(regulator);
	mutex_unlock(&regulator_list_mutex);
}

static void _regulator_put(struct regulator *regulator)
{
	struct regulator_dev *rdev;

	if (regulator == NULL || IS_ERR(regulator))
		return;

	rdev = regulator->rdev;

	debugfs_remove_recursive(regulator->debugfs);

	/* remove any sysfs entries */
	if (regulator->dev)
		sysfs_remove_link(&rdev->dev.kobj, regulator->supply_name);
	mutex_lock(&rdev->mutex);
	kfree(regulator->supply_name);
	list_del(&regulator->list);
	kfree(regulator);

	rdev->open_count--;
	rdev->exclusive = 0;
	mutex_unlock(&rdev->mutex);

	module_put(rdev->owner);
}
```

四、 regulator_enable

1、只有 rdev->use_count == 0，才会真正的 enable regulator，否则只是简单的执行： rdev->use_count++。

2、调用此函数前，regulator可能已经被enable。

3、它实际上是调用： rdev->desc->ops->enable(rdev) 。

```c
int regulator_enable(struct regulator *regulator)
{
	struct regulator_dev *rdev = regulator->rdev;
	int ret = 0;

	if (regulator->always_on)
		return 0;

	if (rdev->supply) {
		ret = regulator_enable(rdev->supply);//rdev tree
		if (ret != 0)
			return ret;
	}

	mutex_lock(&rdev->mutex);
	ret = _regulator_enable(rdev);
	mutex_unlock(&rdev->mutex);

	if (ret != 0 && rdev->supply)
		regulator_disable(rdev->supply);

	return ret;
}

static int _regulator_enable(struct regulator_dev *rdev)
{
	int ret;

	/* check voltage and requested load before enabling */
	if (rdev->constraints &&
	    (rdev->constraints->valid_ops_mask & REGULATOR_CHANGE_DRMS))
		drms_uA_update(rdev);

	if (rdev->use_count == 0) {
		/* The regulator may on if it's not switchable or left on */
		ret = _regulator_is_enabled(rdev);
		if (ret == -EINVAL || ret == 0) {
			if (!_regulator_can_change_status(rdev))
				return -EPERM;

			ret = _regulator_do_enable(rdev);
			if (ret < 0)
				return ret;

		} else if (ret < 0) {
			rdev_err(rdev, "is_enabled() failed: %d\n", ret);
			return ret;
		}
		/* Fallthrough on positive return values - already enabled */
	}

	rdev->use_count++;

	return 0;
}

static int _regulator_do_enable(struct regulator_dev *rdev)
{
	int ret, delay;

	/* Query before enabling in case configuration dependent.  */
	ret = _regulator_get_enable_time(rdev);
	if (ret >= 0) {
		delay = ret;
	} else {
		rdev_warn(rdev, "enable_time() failed: %d\n", ret);
		delay = 0;
	}

	trace_regulator_enable(rdev_get_name(rdev));

	if (rdev->ena_pin) {
		ret = regulator_ena_gpio_ctrl(rdev, true);
		if (ret < 0)
			return ret;
		rdev->ena_gpio_state = 1;
	} else if (rdev->desc->ops->enable) {
		ret = rdev->desc->ops->enable(rdev);
		if (ret < 0)
			return ret;
	} else {
		return -EINVAL;
	}

	/* Allow the regulator to ramp; it would be useful to extend
	 * this for bulk operations so that the regulators can ramp
	 * together.  */
	trace_regulator_enable_delay(rdev_get_name(rdev));

	if (delay >= 1000) {
		mdelay(delay / 1000);
		udelay(delay % 1000);
	} else if (delay) {
		udelay(delay);
	}

	trace_regulator_enable_complete(rdev_get_name(rdev));

	return 0;
}
```

五、 regulator_is_enabled

```c
int regulator_is_enabled(struct regulator *regulator)
{
	int ret;

	if (regulator->always_on)
		return 1;

	mutex_lock(&regulator->rdev->mutex);
	ret = _regulator_is_enabled(regulator->rdev);
	mutex_unlock(&regulator->rdev->mutex);

	return ret;
}

static int _regulator_is_enabled(struct regulator_dev *rdev)
{
	/* A GPIO control always takes precedence */
	if (rdev->ena_pin)
		return rdev->ena_gpio_state;

	/* If we don't know then assume that the regulator is always on */
	if (!rdev->desc->ops->is_enabled)
		return 1;

	return rdev->desc->ops->is_enabled(rdev);
}
```

六、 regulator_disable

1、只有 rdev->use_count == 1，才会真正 disable regulator ，否则只是简单的自减：rdev->use_count--。

2、实际上会调用： rdev->desc->ops->disable(rdev)。

```c
int regulator_disable(struct regulator *regulator)
{
	struct regulator_dev *rdev = regulator->rdev;
	int ret = 0;

	if (regulator->always_on)
		return 0;

	mutex_lock(&rdev->mutex);
	ret = _regulator_disable(rdev);
	mutex_unlock(&rdev->mutex);

	if (ret == 0 && rdev->supply)
		regulator_disable(rdev->supply);

	return ret;
}

static int _regulator_disable(struct regulator_dev *rdev)
{
	int ret = 0;

	if (WARN(rdev->use_count <= 0,
		 "unbalanced disables for %s\n", rdev_get_name(rdev)))
		return -EIO;

	/* are we the last user and permitted to disable ? */
	if (rdev->use_count == 1 &&
	    (rdev->constraints && !rdev->constraints->always_on)) {

		/* we are last user */
		if (_regulator_can_change_status(rdev)) {
			ret = _regulator_do_disable(rdev);
			if (ret < 0) {
				rdev_err(rdev, "failed to disable\n");
				return ret;
			}
			_notifier_call_chain(rdev, REGULATOR_EVENT_DISABLE,
					NULL);
		}

		rdev->use_count = 0;
	} else if (rdev->use_count > 1) {

		if (rdev->constraints &&
			(rdev->constraints->valid_ops_mask &
			REGULATOR_CHANGE_DRMS))
			drms_uA_update(rdev);

		rdev->use_count--;
	}

	return ret;
}

static int _regulator_do_disable(struct regulator_dev *rdev)
{
	int ret;

	trace_regulator_disable(rdev_get_name(rdev));

	if (rdev->ena_pin) {
		ret = regulator_ena_gpio_ctrl(rdev, false);
		if (ret < 0)
			return ret;
		rdev->ena_gpio_state = 0;

	} else if (rdev->desc->ops->disable) {
		ret = rdev->desc->ops->disable(rdev);
		if (ret != 0)
			return ret;
	}

	trace_regulator_disable_complete(rdev_get_name(rdev));

	return 0;
}
```

七、 regulator_set_voltage

1、 真正的实现还是在 regulator 的 ops 中： rdev->desc->ops->set_voltage_sel(rdev, ret);

2、不同电压的引脚共用 regulator 时， regulator 为了保护低电压引脚不被被烧坏，会把低电压引脚的电压设置成 regulator 的最高电压，从而使高电压引脚无法上电。

对于共用 regulator ，但电压需求不同的引脚：

(1) 下电后，调用 regulator_put 函数，释放掉 regulator。

(2) 上电前，调用 regulator_get 函数，重新申请 regulator。

3、关注一下下面的注释。

```c
/**
 * If the regulator is enabled then the voltage will change to the new value
 * immediately otherwise if the regulator is disabled the regulator will
 * output at the new voltage when enabled.
 *
 * NOTE: If the regulator is shared between several devices then the lowest
 * request voltage that meets the system constraints will be used.
 *
 * Regulator system constraints must be set for this regulator before
 * calling this function otherwise this call will fail.
 */
int regulator_set_voltage(struct regulator *regulator, int min_uV, int max_uV)
{
	struct regulator_dev *rdev = regulator->rdev;
	int ret = 0;
	int old_min_uV, old_max_uV;

	mutex_lock(&rdev->mutex);

	/* If we're setting the same range as last time the change
	 * should be a noop (some cpufreq implementations use the same
	 * voltage for multiple frequencies, for example).
	 */
	if (regulator->min_uV == min_uV && regulator->max_uV == max_uV)
		goto out;

	/* sanity check */
	if (!rdev->desc->ops->set_voltage &&
	    !rdev->desc->ops->set_voltage_sel) {
		ret = -EINVAL;
		goto out;
	}

	/* constraints check */
	ret = regulator_check_voltage(rdev, &min_uV, &max_uV);
	if (ret < 0)
		goto out;
	
	/* restore original values in case of error */
	old_min_uV = regulator->min_uV;
	old_max_uV = regulator->max_uV;
	regulator->min_uV = min_uV;
	regulator->max_uV = max_uV;

	ret = regulator_check_consumers(rdev, &min_uV, &max_uV);
	if (ret < 0)
		goto out2;

	ret = _regulator_do_set_voltage(rdev, min_uV, max_uV);
	if (ret < 0)
		goto out2;
	
out:
	mutex_unlock(&rdev->mutex);
	return ret;
out2:
	regulator->min_uV = old_min_uV;
	regulator->max_uV = old_max_uV;
	mutex_unlock(&rdev->mutex);
	return ret;
}
```


