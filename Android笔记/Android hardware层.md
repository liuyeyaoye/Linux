Android HAL 层 API和数据结构


```c
1、 variant_keys 结构体
static const char *variant_keys[] = {
	"ro.hardware",
	"ro.product.board",
	"ro.board.platform",
	"ro.arch"
};
硬件抽象层的命令规范为： module_ID.variant.so ，如果不存在这些库文件，就会加载这个库： module_ID.default.so 



2、 struct hw_module_t 结构体
typedef struct hw_module_t {
	uint32_t tag;
	uint16_t version_major;
	uint16_t version_minor;
	const char *id;
	const char *name;
	const char *author;
	struct hw_module_methods_t *methods;
	void *dso; //保存使用 dlopen 的返回值句柄，该句柄给 dlsym 使用
	uint32_t reserved[32-7];
} hw_module_t; /* struct hw_module_t 这个结构体在代码中静态定义 */



3、 struct hw_module_methods_t 结构体
typedef struct hw_module_methods_t {
	int (*open) (const struct hw_module_t *module, const char *id, struct hw_device_t **device);
	/* 在 struct hw_module_methods_t 中的成员 open 中动态分配一个 struct hw_device_t 结构体 */
} hw_module_methods_t;



4、 struct hw_device_t 结构体
typedef struct hw_device_t {
	uint32_t tag;
	uint32_t version;
	struct hw_module_t *module;
	uint32_t reserved[12];
	int (*close) (struct hw_device_t *device);
} hw_device_t; /* struct hw_device_t 在 open 中动态分配 */



5、 硬件抽象层模块的加载流程
(1)
int hw_get_module(const char *id, const struct hw_module_t **module);
其中， module 是输出参数，如果加载成功， module 将会指向一个自定义的硬件抽象层模块结构体。
返回值：0——成功；小于0——失败

(2) 实现
#define HAL_LIBRARY_PATH1 "/system/lib/hw"
#define HAL_LIBRARY_PATH2 "/vendor/lib/hw"

static const char *variant_keys[] = {
	"ro.hardware",
	"ro.product.board",
	"ro.board.platform",
	"ro.arch"
};

static const int HAL_VARIANT_KEYS_COUNT = (sizeof(variant_keys)/sizeof(variant_keys[0]));

int hw_get_module(const char *id, const struct hw_module_t **module)
{
	int status;
	int i;
	const struct hw_module_t *hmi = NULL;
	char prop[PATH_MAX];
	char path[PATH_MAX];
	
	for(i=0; i<HAL_VARIANT_KEYS_COUNT+1; i++) {
		if (i< HAL_VARIANT_KEYS_COUNT) {
			if (property_get(variant_keys[i], prop, NULL) == 0)
				continue;
		
			snprint(path, sizeof(path), "%s/%s.%s.so", HAL_LIBRARY_PATH1, id, prop);
			if (access(path, R_OK) ==0)
				break;
			
			snprint(path, sizeof(path), "%s/%s.%s.so", HAL_LIBRARY_PATH2, id, prop);
			if (access(path, R_OK) ==0)
				break;
		} else {
			snprint(path, sizeof(path), "%s/%s.default.so", HAL_LIBRARY_PATH1, id);
			if (access(path,R_OK) ==0)
				break;
		}
	}
	
	status = -ENOENT;
	if (i< HAL_VARIANT_KEYS_COUNT+1) {
		status = load(id, path, module);
	}
	
	return status;
}



static int load(const char *id, const char *path, const struct hw_module_t **pHmi)
{
	int status;
	vodi *handle;
	struct hw_module_t *hmi;
	
	handle = dlopen(path, RTLD_NOW);
	if (handle == NULL) {
		char const * err_str = dlerror();
		LOGE("load: module=%s\n%s", path, err_str?err_str:"unknown");
		status = -EINVAL;
		goto done;
	}
	
	const char *sym = HAL_MODULE_INFO_SYM_AS_STR;	//"HMI"
	hmi = (struct hw_module_t *)dlsym(handle, sym);
	//dlsym 功能是根据动态链接库操作句柄与符号，返回符号对应的地址，不但可以获取函数地址，也可以获取变量地址。
	/*
		fingerprint_module_t HAL_MODULE_INFO_SYM = {
			.common = {
				.tag                = HARDWARE_MODULE_TAG,
				.module_api_version = FINGERPRINT_MODULE_API_VERSION_2_0,
				.hal_api_version    = HARDWARE_HAL_API_VERSION,
				.id                 = FINGERPRINT_HARDWARE_MODULE_ID,
				.name               = "Demo Fingerprint HAL",
				.author             = "The Android Open Source Project",
				.methods            = &fingerprint_module_methods,
			},
		};
		
		#define HAL_MODULE_INFO_SYM     HMI
		HMI 表示这个结构体的名称
	*/
	if (hmi == NULL) {
		LOGE("load: couldn't find symbol %s",sym);
		status = -EINVAL;
		goto done;
	}
	
	if (strcmp(id, hmi->id) != 0) {
		LOGE("load: id=%s != hmi->id=%s", id, hmi->id);
		status = -EINVAL;
		goto done;
	}
	
	hmi->dso = handle;
	
	status = 0;
	
	done:
	if(status != 0) {
		if (handle != NULL) {
			dlclose(handle);
			handle = NULL;
		}
	} else {
		LOGV("loaded HAL id=%s path=%s hmi=%p handle=%p", id, path, *pHmi, handle);
	}
	*pHmi = hmi;
	return status;
}
```

