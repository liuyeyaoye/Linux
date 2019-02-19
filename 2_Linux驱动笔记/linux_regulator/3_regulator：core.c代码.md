


core.c中的模块初始化函数

```c
core_initcall(regulator_init);//core_initcall优先级为1
late_initcall(regulator_init_complete);//late_initcall的优先级为7
```

一、 regulator_init

```c
static int __init regulator_init(void)
{
	int ret;

	ret = class_register(&regulator_class);

	debugfs_root = debugfs_create_dir("regulator", NULL);
	if (!debugfs_root)
		pr_warn("regulator: Failed to create debugfs directory\n");

	debugfs_create_file("supply_map", 0444, debugfs_root, NULL, &supply_map_fops);

	regulator_dummy_init();//注册一个 dummy_regulator

	return ret;
}
```

二、 regulator_init_complete

```c
static int __init regulator_init_complete(void)
{
	struct regulator_dev *rdev;
	struct regulator_ops *ops;
	struct regulation_constraints *c;
	int enabled, ret;

	if (of_have_populated_dt())
		has_full_constraints = true;

	mutex_lock(&regulator_list_mutex);

	/* If we have a full configuration then disable any regulators
	 * which are not in use or always_on.  This will become the
	 * default behaviour in the future.
	 */
	list_for_each_entry(rdev, &regulator_list, list) {
		ops = rdev->desc->ops;
		c = rdev->constraints;

		if (c && c->always_on)
			continue;

		mutex_lock(&rdev->mutex);

		if (rdev->use_count)
			goto unlock;

		/* If we can't read the status assume it's on. */
		if (ops->is_enabled)
			enabled = ops->is_enabled(rdev);
		else
			enabled = 1;

		if (!enabled)
			goto unlock;

		if (has_full_constraints) {
			/* We log since this may kill the system if it
			 * goes wrong. */
			rdev_info(rdev, "disabling\n");
			ret = _regulator_do_disable(rdev);
			if (ret != 0) {
				rdev_err(rdev, "couldn't disable: %d\n", ret);
			}
		} else {
			/* The intention is that in future we will
			 * assume that full constraints are provided
			 * so warn even if we aren't going to do
			 * anything here.
			 */
			rdev_warn(rdev, "incomplete constraints, leaving on\n");
		}

unlock:
		mutex_unlock(&rdev->mutex);
	}

	mutex_unlock(&regulator_list_mutex);

	return 0;
}
```


