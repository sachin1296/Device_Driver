#include <linux/module.h>

int __init my_module_init(void)
{
	printk("Module init done\n");
	return 0;
}
void __exit my_module_exit(void)
{
	printk("Bye world\n");
}

module_init(my_module_init);
module_exit(my_module_exit);


MODULE_LICENSE("GPL");
