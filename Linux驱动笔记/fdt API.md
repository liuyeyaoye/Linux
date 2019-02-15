
```c
〇，疑问
	struct device_node *node = NULL;
	node = of_find_compatible_node(NULL, NULL, "mediatek,lcd-backlight");

在内存中是怎么保存这个 compitible 字符串的？
在编译时，dts文件通过dct工具编译成dtb文件，在kernel初始化时会解析这个dtb文件，将其中的内容以字符串的形式保存在内存中。
	


一， kernel 中 的fdt API：


1， of_get_flat_dt_prop ：从 node 节点中查找 name 的字符串，返回它的首地址。
原型： 
const void *__init of_get_flat_dt_prop(unsigned long node, const char *name,int *size)
示例：
struct tag_videolfb *videolfb_tag = NULL;
videolfb_tag = (struct tag_videolfb *)of_get_flat_dt_prop(node, "atag,videolfb", (int *)&size);



2， fb_early_init_dt_get_chosen ：
原型：
static int fb_early_init_dt_get_chosen(unsigned long node, const char *uname, int depth, void *data)
{
	if (depth != 1 || (strcmp(uname, "chosen") != 0 && strcmp(uname, "chosen@0") != 0))
		return 0;

	*(unsigned long *)data = node;
	return 1;
}
含义：
depth 深度要为1，表示在根节点下(一般根节点/ 的depth为0)
检查节点名是否为chosen节点



3， 
of_scan_flat_dt(fb_early_init_dt_get_chosen, &node);
含义：查找根节点 / 下的 chosen 节点，将它的首地址保存在 node 中。

int __init of_scan_flat_dt(int (*it)(unsigned long node, const char *uname, int depth,  void *data),  void *data)
/* of_scan_flat_dt : scan flattened tree blob and call callback on each.*/



二， lk 中的 fdt API：
1， 大小端：
cpu_to_fdt32()


2， fdt_property
const struct fdt_property *fdt_get_property(const void *fdt, int nodeoffset,  const char *name, int *lenp)
示例：
/* Becuase the model is at the begin of device tree.
 * use nodeoffset=0
*/
	nodeoffset = 0;
	char *prop_name = "model";
	const char *str = "MT6735"
	
	prop = fdt_get_property(fdt, nodeoffset, prop_name, &len);
/* 含义：从 fdt 中获得字符串为 prop_name 的属性，返回它的首地址。 */

	if(prop) {
		fdt_setprop_string(fdt, nodeoffset, prop_name, str);
		
/*原型为：
#define fdt_setprop_string(fdt, nodeoffset, name, str) 	fdt_setprop((fdt), (nodeoffset), (name), (str), strlen(str)+1)
含义：为属性 name 添加一个value str。如果在 fdt 中没有 name 这个属性，则添加一个属性。
*/
		prop = fdt_get_property(fdt, nodeoffset, prop_name, &len);
		namestroff = fdt32_to_cpu(prop->nameoff);
		dprintf(CRITICAL, "%s=%s\n", fdt_string(fdt, namestroff), (char *)prop->dat
	}

3， fdt_path_offset
原型：
int fdt_path_offset(const void *fdt, const char *path)
{
	return fdt_path_offset_namelen(fdt, path, strlen(path));
}

示例：
	dt_dram_info mem_reg_property[128];
	offset = fdt_path_offset(fdt, "/memory");
/*含义：从 fdt 中 获得 字符串  "memory" 的偏移，返回  "memory" 的首地址。*/	
	
	ret = fdt_setprop(fdt, offset, "reg", mem_reg_property,3 * sizeof(dt_dram_info));
/*在 memory 节点中查找属性reg，为它添加一个value为 mem_reg_property。如果在 fdt 中没有这个属性，则添加这个属性及 value*/


4， 代码分析

	offset = fdt_path_offset(fdt, "/chosen");
/*从地址 fdt 中查找 chosen 节点，返回它的首地址*/	
	
	ret = fdt_setprop_cell(fdt, offset, "linux,initrd-start",(unsigned int) ramdisk);
	ret = fdt_setprop_cell(fdt, offset, "linux,initrd-end", (unsigned int)ramdisk + ramdisk_size);
/* set a property to a single cell value*/	
	
static inline int fdt_setprop_cell(void *fdt, int nodeoffset, const char *name,  uint32_t val)
{
	return fdt_setprop_u32(fdt, nodeoffset, name, val);
}


5， 代码分析
	offset = fdt_path_offset(fdt, "/chosen");
	char buf[FDT_BUFF_SIZE+8], *ptr;

	ptr = (char *)target_atag_boot((unsigned *)buf);
/*为数组 buf 赋值，ptr指向 buf 的尾部，返回 ptr 指针*/	
	
	ret = fdt_setprop(fdt, offset, "atag,boot", buf, ptr - buf);
/*从地址 fdt 中的 chosen 节点中查找 属性名为 "atag,boot" 的并且追加 buf 字符串，如果没有这个属性，则创建再追加。*/	
	
```

